/* Copyright 2026 tAUREON. SPDX-License-Identifier: GPL-3.0-or-later */
#include "../Source/MidiPerformanceRouter.h"
#include <iostream>
#include <vector>

namespace {
int checks = 0;
int failures = 0;
void check(bool condition, const char* name)
{
    ++checks;
    if (!condition) { ++failures; std::cerr << "FAIL: " << name << '\n'; }
}
using Message = std::vector<uint8_t>;
struct Capture {
    std::vector<Message> messages;
    bool reject = false;
    bool operator()(const uint8_t* data, int size, int channel) {
        if (reject) return false;
        Message message(data, data + size);
        if (channel > 0 && message[0] >= 0x80 && message[0] < 0xf0)
            message[0] = static_cast<uint8_t>((message[0] & 0xf0) | (channel - 1));
        messages.push_back(message);
        return true;
    }
};
}

int main()
{
    // The same note on different members must retain independently addressed expression.
    for (bool mpe : { false, true }) {
        MidiPerformanceRouter router;
        Capture capture;
        check(router.configure(mpe, 7, 1, capture), "initial configuration");
        for (int channel = 0; channel < 16; ++channel) {
            const std::vector<Message> messages {
                { static_cast<uint8_t>(0x90 | channel), 60, 100 },
                { static_cast<uint8_t>(0xa0 | channel), 60, 91 }, // PolyAT
                { static_cast<uint8_t>(0xd0 | channel), 92 },    // MPE Press
                { static_cast<uint8_t>(0xe0 | channel), 1, 70 }, // Glide
                { static_cast<uint8_t>(0xb0 | channel), 74, 93 },// Slide
                { static_cast<uint8_t>(0xb0 | channel), 101, 0 },
                { static_cast<uint8_t>(0xb0 | channel), 100, 0 },
                { static_cast<uint8_t>(0xb0 | channel), 6, 48 }, // Bend RPN
                { static_cast<uint8_t>(0xb0 | channel), 38, 0 },
                { static_cast<uint8_t>(0xb0 | channel), 101, 127 },
                { static_cast<uint8_t>(0xb0 | channel), 100, 127 },
                { static_cast<uint8_t>(0x80 | channel), 60, 25 }
            };
            for (const auto& original : messages) {
                check(router.forward(original.data(), static_cast<int>(original.size()), capture), "forward succeeds");
                auto expected = original;
                if (!mpe) expected[0] = static_cast<uint8_t>((expected[0] & 0xf0) | 6);
                check(capture.messages.back() == expected, "exact performance bytes and order");
            }
        }
        for (const Message& original : { Message{0xf8}, Message{0xf0, 0x7d, 1, 0xf7} }) {
            check(router.forward(original.data(), static_cast<int>(original.size()), capture), "system forwarding");
            check(capture.messages.back() == original, "system bytes not channel remapped");
        }
        const auto before = capture.messages.size();
        check(router.release(capture), "release after balanced notes");
        check(capture.messages.size() == before, "no unsolicited panic for balanced notes");
    }

    MidiPerformanceRouter router;
    Capture capture;
    router.configure(true, 1, 10, capture);
    const uint8_t on2[] = {0x91, 60, 100}, on3[] = {0x92, 60, 100}, pedal[] = {0xb1, 64, 127};
    router.forward(on2, 3, capture);
    router.forward(on3, 3, capture);
    router.forward(pedal, 3, capture);
    capture.messages.clear();
    check(router.configure(false, 7, 10, capture), "MPE to normal change");
    check(capture.messages == std::vector<Message>{{0x81,60,0},{0xb1,64,0},{0x82,60,0}}, "release only owned member notes and sustain");
    router.forward(on2, 3, capture);
    capture.messages.clear();
    check(router.configure(false, 8, 10, capture), "normal channel change");
    check(capture.messages == std::vector<Message>{{0x86,60,0}}, "old normal channel receives note off");
    router.forward(on2, 3, capture);
    capture.messages.clear();
    check(router.configure(true, 8, 11, capture), "device replacement");
    check(capture.messages.empty(), "no old-device cleanup sent to replacement");

    router.forward(on2, 3, capture);
    capture.reject = true;
    check(!router.configure(false, 1, 11, capture), "full queue defers routing change");
    capture.reject = false;
    capture.messages.clear();
    check(router.configure(false, 1, 11, capture), "cleanup retry");
    check(capture.messages == std::vector<Message>{{0x81,60,0}}, "rejected release retained for retry");
    capture.reject = true;
    check(!router.forward(on2, 3, capture), "rejected note on");
    capture.reject = false;
    capture.messages.clear();
    router.release(capture);
    check(capture.messages.empty(), "rejected note not tracked");

    const uint8_t zeroOff[] = {0x91,60,0};
    router.forward(on2, 3, capture);
    router.forward(on2, 3, capture);
    router.forward(zeroOff, 3, capture);
    capture.messages.clear();
    router.release(capture);
    check(capture.messages == std::vector<Message>{{0x80,60,0}}, "overlap and velocity-zero note off count");

    std::vector<std::array<int,3>> controllers;
    emitEditorNrpn(1, 514, 129, [&](int channel,int cc,int value) {
        controllers.push_back({channel,cc,value});
    });
    const std::vector<std::array<int,3>> legacy {{1,99,4},{1,98,2},{1,6,1},{1,38,1}};
    check(controllers == legacy, "editor NRPN never emits unsafe arpeggiator CC100/101");
    for (int channel = 0; channel < 16; ++channel) {
        for (int cc = 0; cc < 128; ++cc) {
            const uint8_t msg[] = {static_cast<uint8_t>(0xb0 | channel), static_cast<uint8_t>(cc), 0};
            const bool configuration = cc == 6 || cc == 38 || (cc >= 96 && cc <= 101);
            check(isPreenfmConfigurationMessage(msg, 3) == configuration, "configuration guard on every channel/controller");
        }
    }
    router.configure(true, 1, 20, capture);
    router.forward(on2, 3, capture);
    capture.reject = true;
    check(!router.forward(zeroOff, 3, capture), "queue rejects velocity-zero release");
    capture.reject = false;
    capture.messages.clear();
    check(router.configure(true, 1, 20, capture), "retry release without changing mode");
    check(capture.messages == std::vector<Message>{{0x81,60,0}}, "rejected note off is retried next block");
    capture.messages.clear();
    router.release(capture);
    check(capture.messages.empty(), "retried note off not duplicated at deactivation");
    router.forward(pedal, 3, capture);
    const uint8_t pedalOff[] = {0xb1,64,0};
    capture.reject = true;
    check(!router.forward(pedalOff, 3, capture), "queue rejects sustain off");
    capture.reject = false;
    capture.messages.clear();
    check(router.configure(true, 1, 20, capture), "retry sustain without changing mode");
    check(capture.messages == std::vector<Message>{{0xb1,64,0}}, "rejected sustain off retried next callback");
    std::cout << checks << " checks, " << failures << " failures\n";
    return failures == 0 ? 0 : 1;
}
