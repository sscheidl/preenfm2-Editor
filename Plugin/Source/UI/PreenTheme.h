/* Copyright 2026 tAUREON. SPDX-License-Identifier: GPL-3.0-or-later */
#pragma once
#include "JuceHeader.h"
#include <array>

namespace PreenTheme
{
// UI-only colours: never parameters, MIDI messages or hardware patch state.
struct Palette
{
    Colour background, surface, surfaceRaised, outline;
    Colour textPrimary, textMuted, accent, accentBright;
    Colour warning;
};

inline constexpr int count = 6;
inline const std::array<const char*, count> names {
    "Classic", "tAUREON", "Graphite Rose", "Arctic", "Forest", "Plum"
};

inline Palette palette(int index)
{
    using C = Colour;
    switch (index)
    {
        case 1: return { C(0xff090b0d), C(0xff191a19), C(0xff302d27), C(0xff69604e),
                         C(0xfff0ece3), C(0xffc0b8a7), C(0xffc8a15a), C(0xffe2bd75), C(0xffffc46b) };
        case 2: return { C(0xff181a1d), C(0xff25282d), C(0xff30343a), C(0xff5a5f67),
                         C(0xfff3eee4), C(0xffb8b9bd), C(0xffbd7e7c), C(0xffd7a09b), C(0xffffc46b) };
        case 3: return { C(0xffe9eef2), C(0xfff8fafc), C(0xffdce5eb), C(0xff647789),
                         C(0xff172b3a), C(0xff4e6373), C(0xff356d94), C(0xff285d83), C(0xff865000) };
        case 4: return { C(0xff101916), C(0xff1b2b24), C(0xff263a30), C(0xff536f5f),
                         C(0xfff0f1e7), C(0xffafc4b7), C(0xff9bc6a5), C(0xffb5dfbe), C(0xffffc46b) };
        case 5: return { C(0xff1c1722), C(0xff2d2535), C(0xff3b3045), C(0xff78677f),
                         C(0xfff6edf4), C(0xffc6b4ca), C(0xffd59aad), C(0xffefb5c9), C(0xffffc46b) };
        default: return { C(0xff0b1117), C(0xff111a24), C(0xff182532), C(0xff314353),
                          C(0xffedf4f7), C(0xff9db0bd), C(0xff35c2c8), C(0xff5edbe0), C(0xffffc46b) };
    }
}

// Inherited custom colour IDs also work for dynamically created children.
enum ColourIds
{
    backgroundId = 0x2100000, surfaceId, raisedId, outlineId,
    textId, mutedId, accentId, brightId, warningId
};
inline Colour colour(const Component& c, int id)
{
    // Hidden tab contents and teardown may have no theme LookAndFeel attached.
    // Never query an unspecified custom ID in JUCE's default LookAndFeel.
    return c.getLookAndFeel().isColourSpecified(id) ? c.findColour(id, true) : Colour();
}
inline Colour carrierOutline(const Component& c)
{
    return colour(c, backgroundId).getBrightness() > 0.6f
        ? Colour(0xff17636a) : Colour(0xff5edbe0);
}
}
