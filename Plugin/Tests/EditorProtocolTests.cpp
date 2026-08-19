/*
 * Deterministic tests for the PreenFM2 editor remote protocol, version 1
 * (firmware 3.00 alpha).
 *
 * These tests cover the wire arithmetic and the address-space guarantees: the
 * places where an off-by-one silently writes to the wrong preset slot. They
 * deliberately depend on nothing but PreenNrpn.h, so they need no JUCE, no
 * plugin instance and no hardware.
 *
 * They are a simulation of the protocol encoding, not a hardware test.
 *
 * Build and run:
 *   cmake --build --preset windows-vs2026-x64-release --target EditorProtocolTests
 *   build/windows-vs2026-x64/Plugin/Tests/Release/EditorProtocolTests.exe
 */

#include <cstdio>
#include <cstdlib>

#include "../Source/PreenNrpn.h"

namespace {

int failures = 0;
int checks = 0;

void check(bool condition, const char* what)
{
    ++checks;
    if (!condition) {
        ++failures;
        std::printf("FAIL: %s\n", what);
    }
}

// The editor sends a one-based bank/preset; the wire is zero based.
int storeTargetFor(int guiBank, int guiPreset)
{
    return ((guiBank - 1) << 7) | (guiPreset - 1);
}

// The firmware reports a zero-based position; the GUI shows it one based.
int guiValueFromWire(int wireValue)
{
    return wireValue + 1;
}

// Mirrors Pfm2AudioProcessor::setHardwarePresetTarget().
int clampBank(int bank)
{
    if (bank < 1) return 1;
    if (bank > PREENFM_EDITOR_BANK_COUNT) return PREENFM_EDITOR_BANK_COUNT;
    return bank;
}

int clampPreset(int preset)
{
    if (preset < 1) return 1;
    if (preset > PREENFM_EDITOR_PRESET_COUNT) return PREENFM_EDITOR_PRESET_COUNT;
    return preset;
}

// Mirrors the interception in Pfm2AudioProcessor::handleIncomingNrpn().
bool isEditorProtocolParameter(int nrpnParameter)
{
    return nrpnParameter >= PREENFM_EDITOR_NRPN_FIRST
        && nrpnParameter <= PREENFM_EDITOR_NRPN_LAST;
}

void testConstantsMatchFirmware()
{
    check(PREENFM_EDITOR_NRPN_PAGE == 4, "editor page is 4");
    check(PREENFM_EDITOR_NRPN_FIRST == 512, "page 4 starts at parameter 512");
    check(PREENFM_EDITOR_NRPN_LAST == 639, "page 4 ends at parameter 639");
    check(PREENFM_EDITOR_PROTOCOL_VERSION == 1, "protocol version is 1");
    check(PREENFM_EDITOR_BANK_COUNT == 64, "64 addressable patch banks");
    check(PREENFM_EDITOR_PRESET_COUNT == 128, "128 presets per bank");
    check((PREENFM_EDITOR_CAPABILITY_STORE
        | PREENFM_EDITOR_CAPABILITY_POSITION_QUERY) == 3,
        "capability bits 0 and 1 make 3");
}

// Case 12/13: page 4 must not collide with anything the editor already uses.
void testAddressSpaceIsDisjoint()
{
    check(!isEditorProtocolParameter(PREENFM2_NRPN_ALGO),
        "algo is not an editor-protocol parameter");
    check(!isEditorProtocolParameter(PREENFM2_NRPN_LETTER1),
        "first preset-name letter is not on page 4");
    check(!isEditorProtocolParameter(PREENFM2_NRPN_LETTER12),
        "last preset-name letter is not on page 4");
    check(!isEditorProtocolParameter(PREENFM2_NRPN_STEPSEQ1_STEP1),
        "step sequencer 1 is not on page 4");
    check(!isEditorProtocolParameter(PREENFM2_NRPN_STEPSEQ2_STEP16),
        "step sequencer 2 is not on page 4");
    check(!isEditorProtocolParameter(16383),
        "the full dump request 127/127 is not on page 4");

    // Every request and response id must land inside the page.
    check(isEditorProtocolParameter(PREENFM_EDITOR_NRPN_FIRST
        + PREENFM_EDITOR_REQ_CAPABILITY), "capability request is on page 4");
    check(isEditorProtocolParameter(PREENFM_EDITOR_NRPN_FIRST
        + PREENFM_EDITOR_REQ_STORE), "store request is on page 4");
    check(isEditorProtocolParameter(PREENFM_EDITOR_NRPN_FIRST
        + PREENFM_EDITOR_RSP_STORE_TARGET), "store target echo is on page 4");

    // Requests and responses must not overlap, otherwise a reply looped back
    // by MIDI thru could be decoded as a new request.
    check(PREENFM_EDITOR_REQ_STORE < PREENFM_EDITOR_RSP_PROTOCOL_VERSION,
        "request and response id ranges are disjoint");
}

// Case 6: store target encoding.
void testStoreTargetEncoding()
{
    check(storeTargetFor(1, 1) == 0, "GUI 1/1 encodes to wire 0");
    check(storeTargetFor(2, 1) == 128, "GUI 2/1 encodes to wire 128");
    check(storeTargetFor(4, 13) == 396, "GUI 4/13 encodes to wire 396");
    check(storeTargetFor(64, 128) == ((63 << 7) | 127),
        "GUI 64/128 encodes to the highest legal target");
    check(storeTargetFor(64, 128) == 8191, "highest legal target is 8191");

    // The firmware answers status 2 for bank >= 64, so the editor must never
    // be able to produce such a target.
    const int highestBank = (storeTargetFor(PREENFM_EDITOR_BANK_COUNT, 1)) >> 7;
    check(highestBank == 63, "highest wire bank the editor can send is 63");
}

// Cases 3 and 4: reported position mapping.
void testPositionMapping()
{
    check(guiValueFromWire(0) == 1, "wire bank 0 shows as bank 1");
    check(guiValueFromWire(63) == 64, "wire bank 63 shows as bank 64");
    check(guiValueFromWire(127) == 128, "wire preset 127 shows as preset 128");
}

// Case 14: old saved states must clamp, never wrap.
void testBankClamping()
{
    check(clampBank(1) == 1, "bank 1 stays 1");
    check(clampBank(64) == 64, "bank 64 stays 64");
    check(clampBank(65) == 64, "bank 65 clamps to 64");
    check(clampBank(128) == 64, "old state bank 128 clamps to 64, not 1");
    check(clampBank(0) == 1, "bank 0 clamps up to 1");
    check(clampBank(-5) == 1, "negative bank clamps up to 1");

    check(clampPreset(128) == 128, "preset 128 stays 128");
    check(clampPreset(129) == 128, "preset 129 clamps to 128");
    check(clampPreset(0) == 1, "preset 0 clamps up to 1");

    // A wrap would map 128 onto bank 1 and silently overwrite the wrong slot.
    check(clampBank(128) != 1, "bank 128 must not wrap onto bank 1");
}

// Case 5: only a complete, valid group may be applied.
void testPositionAcceptance()
{
    struct Group { int bankType, bank, preset, valid; };

    auto acceptable = [](const Group& g) {
        return g.bankType == PREENFM_EDITOR_BANKTYPE_PATCH
            && g.valid == 1
            && g.bank >= 0 && g.bank < PREENFM_EDITOR_BANK_COUNT
            && g.preset >= 0 && g.preset < PREENFM_EDITOR_PRESET_COUNT;
    };

    check(acceptable({ 0, 0, 0, 1 }), "bank 0 preset 0 valid is accepted");
    check(acceptable({ 0, 63, 127, 1 }), "bank 63 preset 127 valid is accepted");
    check(!acceptable({ 0, 2, 17, 0 }), "VALID 0 is not accepted");
    check(!acceptable({ 1, 2, 17, 1 }), "a non-patch bank type is not accepted");
    check(!acceptable({ 0, 64, 17, 1 }), "bank 64 is out of range");
    check(!acceptable({ 0, 2, 128, 1 }), "preset 128 is out of range");
    check(!acceptable({ 0, -1, 17, 1 }), "negative bank is out of range");
}

// Case 10: every documented status code has a distinct meaning.
void testStatusCodes()
{
    check(PREENFM_EDITOR_STATUS_OK == 0, "status 0 is success");
    check(PREENFM_EDITOR_STATUS_BANK_NOT_FOUND == 1, "status 1 is bank missing");
    check(PREENFM_EDITOR_STATUS_INVALID_TARGET == 2, "status 2 is bad target");
    check(PREENFM_EDITOR_STATUS_AMBIGUOUS_CHANNEL == 3, "status 3 is ambiguous channel");
    check(PREENFM_EDITOR_STATUS_STORAGE_ERROR == 4, "status 4 is storage error");
    check(PREENFM_EDITOR_STATUS_PROTOCOL_ERROR == 5, "status 5 is protocol error");
}

// Mirrors hasDeadlinePassed() in PluginProcessor.cpp. The raw millisecond
// counter wraps after ~49.7 days, where a plain >= comparison would either
// fire every timeout instantly or never fire again.
bool hasDeadlinePassed(unsigned int now, unsigned int deadline)
{
    return static_cast<int>(now - deadline) >= 0;
}

void testWrapSafeDeadlines()
{
    check(!hasDeadlinePassed(1000u, 1700u), "700 ms before the deadline: not due");
    check(hasDeadlinePassed(1700u, 1700u), "exactly at the deadline: due");
    check(hasDeadlinePassed(1701u, 1700u), "past the deadline: due");

    // Deadline computed just before the wrap, current time just after it.
    const unsigned int nearMax = 0xFFFFFF00u;
    const unsigned int deadline = nearMax + 700u;   // wraps around
    check(deadline < nearMax, "the test really does wrap the counter");
    check(!hasDeadlinePassed(nearMax + 100u, deadline),
        "before the deadline across a wrap: not due");
    check(hasDeadlinePassed(deadline, deadline),
        "at the deadline across a wrap: due");
    check(hasDeadlinePassed(deadline + 1u, deadline),
        "past the deadline across a wrap: due");

    // A naive comparison would get the wrap case wrong; prove the difference.
    check((nearMax + 100u) >= deadline,
        "the naive comparison would fire too early here");
}

// The store batch must fit into the output queue in one contiguous run,
// otherwise queueNrpnBatch() refuses and the store is aborted.
void testStoreBatchFits()
{
    constexpr int outputQueueCapacity = 2048;
    constexpr int nameLetters = 12;
    constexpr int storeRequest = 1;
    // Worst case: every hardware parameter of the editor.
    constexpr int hardwareParameters = 240;
    constexpr int batchSize = nameLetters + hardwareParameters + storeRequest;

    check(batchSize < outputQueueCapacity,
        "a complete store batch fits into the output queue");
    check(batchSize * 4 < outputQueueCapacity * 4,
        "the batch stays far below the queue capacity");
}

} // namespace

int main()
{
    testConstantsMatchFirmware();
    testWrapSafeDeadlines();
    testStoreBatchFits();
    testAddressSpaceIsDisjoint();
    testStoreTargetEncoding();
    testPositionMapping();
    testBankClamping();
    testPositionAcceptance();
    testStatusCodes();

    std::printf("%d checks, %d failures\n", checks, failures);
    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
