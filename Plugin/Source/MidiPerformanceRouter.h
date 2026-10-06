/* Copyright 2026 tAUREON. SPDX-License-Identifier: GPL-3.0-or-later */
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>

// Audio-thread-owned state. The caller supplies an allocation-free output sink.
// Editor NRPNs do not pass through this router.
class MidiPerformanceRouter
{
public:
    template <typename Sink>
    bool configure(bool preserveChannels, int controlChannel,
                   uint32_t deviceGeneration, Sink&& send) noexcept
    {
        const int channel = controlChannel < 1 ? 1 : (controlChannel > 16 ? 16 : controlChannel);
        if (generation != deviceGeneration)
        {
            // Old notes belong to the disconnected device, not its replacement.
            notes = {};
            sustain = {};
            pendingOffs = {};
            pendingSustainOff = {};
            hasPendingReleases = false;
            generation = deviceGeneration;
        }
        if (hasPendingReleases && !retryPending(send))
            return false;
        if (preserve != preserveChannels || (!preserveChannels && outputChannel != channel))
        {
            if (!release(send))
                return false;
        }
        preserve = preserveChannels;
        outputChannel = channel;
        return true;
    }

    template <typename Sink>
    bool forward(const uint8_t* data, int size, Sink&& send) noexcept
    {
        if (data == nullptr || size <= 0)
            return false;
        const bool channelEvent = data[0] >= 0x80 && data[0] < 0xf0;
        const int channel = preserve ? (data[0] & 0x0f) : outputChannel - 1;
        const int overrideChannel = channelEvent && !preserve ? outputChannel : 0;
        if (!send(data, size, overrideChannel)) {
            // Retry safety-critical releases on subsequent blocks, not only
            // when the user changes routing or deactivates the instance.
            if (channelEvent && size >= 3 && data[1] < 128) {
                const int type = data[0] & 0xf0;
                auto& pending = pendingOffs[static_cast<size_t>(channel)][data[1]];
                if ((type == 0x80 || (type == 0x90 && data[2] == 0))
                    && pending < notes[static_cast<size_t>(channel)][data[1]]) {
                    ++pending;
                    hasPendingReleases = true;
                }
                if (type == 0xb0 && data[1] == 64 && data[2] < 64) {
                    pendingSustainOff[static_cast<size_t>(channel)] = sustain[static_cast<size_t>(channel)];
                    hasPendingReleases = hasPendingReleases || sustain[static_cast<size_t>(channel)];
                }
            }
            return false;
        }
        if (!channelEvent || size < 3 || data[1] > 127 || data[2] > 127)
            return true;
        const int type = data[0] & 0xf0;
        auto& count = notes[static_cast<size_t>(channel)][data[1]];
        if (type == 0x90 && data[2] != 0)
        {
            if (count != std::numeric_limits<uint16_t>::max())
                ++count;
        }
        else if (type == 0x80 || (type == 0x90 && data[2] == 0))
        {
            if (count > 0)
                --count;
        }
        else if (type == 0xb0 && data[1] == 64)
            sustain[static_cast<size_t>(channel)] = data[2] >= 64;
        else if (type == 0xb0 && (data[1] == 120 || data[1] == 123)) {
            notes[static_cast<size_t>(channel)] = {};
            pendingOffs[static_cast<size_t>(channel)] = {};
        }
        return true;
    }

    template <typename Sink>
    bool retryPending(Sink&& send) noexcept
    {
        for (size_t channel = 0; channel < notes.size(); ++channel) {
            for (size_t note = 0; note < notes[channel].size(); ++note) {
                while (pendingOffs[channel][note] > 0) {
                    const uint8_t message[] = {static_cast<uint8_t>(0x80 | channel),
                        static_cast<uint8_t>(note), 0};
                    if (!send(message, 3, 0)) return false;
                    --pendingOffs[channel][note];
                    if (notes[channel][note] > 0) --notes[channel][note];
                }
            }
            if (pendingSustainOff[channel]) {
                const uint8_t message[] = {static_cast<uint8_t>(0xb0 | channel), 64, 0};
                if (!send(message, 3, 0)) return false;
                pendingSustainOff[channel] = false;
                sustain[channel] = false;
            }
        }
        hasPendingReleases = false;
        return true;
    }

    template <typename Sink>
    bool release(Sink&& send) noexcept
    {
        for (size_t channel = 0; channel < notes.size(); ++channel)
        {
            for (size_t note = 0; note < notes[channel].size(); ++note)
            {
                while (notes[channel][note] > 0)
                {
                    const uint8_t message[] = { static_cast<uint8_t>(0x80 | channel),
                                                static_cast<uint8_t>(note), 0 };
                    if (!send(message, 3, 0))
                        return false;
                    --notes[channel][note];
                    if (pendingOffs[channel][note] > 0) --pendingOffs[channel][note];
                }
            }
            if (sustain[channel])
            {
                const uint8_t message[] = { static_cast<uint8_t>(0xb0 | channel), 64, 0 };
                if (!send(message, 3, 0))
                    return false;
                sustain[channel] = false;
                pendingSustainOff[channel] = false;
            }
        }
        hasPendingReleases = false;
        return true;
    }

private:
    bool preserve = false;
    bool hasPendingReleases = false;
    int outputChannel = 1;
    uint32_t generation = 0;
    std::array<std::array<uint16_t, 128>, 16> notes {};
    std::array<std::array<uint16_t, 128>, 16> pendingOffs {};
    std::array<bool, 16> sustain {};
    std::array<bool, 16> pendingSustainOff {};
};

// Never emit RPN Null here: PreenFM2 uses CC100/101 for the arpeggiator
// outside an active MPE zone. Its active zone cannot be queried by protocol v1.
template <typename Sink>
void emitEditorNrpn(int channel, int parameter, int value, Sink&& send)
{
    send(channel, 99, parameter >> 7);
    send(channel, 98, parameter & 0x7f);
    send(channel, 6, value >> 7);
    send(channel, 38, value & 0x7f);
}

inline bool isPreenfmConfigurationMessage(const uint8_t* data, int size) noexcept
{
    if (size < 3 || data == nullptr || (data[0] & 0xf0) != 0xb0) return false;
    switch (data[1]) {
        case 6: case 38: case 96: case 97:
        case 98: case 99: case 100: case 101: return true;
        default: return false;
    }
}
