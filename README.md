# PreenFM+

You'll find here the source code of the software editor for the preenfm sound module (version 2 & 3). <br />
It uses the [JUCE framework](https://juce.com/).

> [!IMPORTANT]
> **Experimental AI-assisted development.** PreenFM+ 4.x is a tAUREON
> research project exploring the practical benefits and limits of AI-assisted
> coding for audio applications and synthesizer software. Substantial parts of
> the 4.x modernization were developed through human-directed collaboration
> with [OpenAI Codex](https://openai.com/codex/) and
> [Anthropic Claude Code](https://www.anthropic.com/claude-code). Treat 4.x
> builds as experimental software and verify important behaviour with your own
> host, MIDI setup and hardware. See the
> [tAUREON AI development notice](docs/AI_DEVELOPMENT_NOTICE.md) for details.

## Credits and project lineage

PreenFM+ stands on work generously made available by other developers:

- **[Xavier Hosxe (`Ixox`)](https://github.com/Ixox)** created the original
  [preenfm2Controller](https://github.com/Ixox/preenfm2Controller) editor and
  the PreenFM synthesizer ecosystem on which this fork is based.
- **[Patrice Vigouroux (`pvig`)](https://github.com/pvig)** developed the 2024
  [PreenFM2 VOSIM firmware extension](https://github.com/pvig/preenfm2/commit/9da152d14fe1946b9926645d1179404facab8eb6),
  which provides the algorithms and firmware behaviour targeted by PreenFM+
  4.x.
- The editor uses the [JUCE](https://juce.com/) application and plug-in
  framework.

Many thanks to Xavier and Patrice for their original work and for publishing
it for the community. The 4.x modernization, integration, testing and project
direction are maintained as a tAUREON experimental project with assistance
from Codex and Claude Code. The AI services are development tools and are not
the upstream authors or maintainers of PreenFM.

## Modern CMake build

PreenFM+ 4.0.3 targets JUCE 9.0.0, C++17, VST3 and a standalone plugin host.
JUCE is fetched automatically and pinned to a release tag, so no sibling `JUCE_6.0`
directory or generated Projucer project is required.

Version 4.x assumes PreenFM firmware with the VOSIM extension (algorithms 29-32
and LFO shapes 6-8). Use a 3.x editor with older firmware.

Requirements on Windows:

- CMake 4.4 or newer
- Visual Studio 2026 Build Tools with the C++ workload
- Windows 10/11 SDK

```powershell
cmake --preset windows-vs2026-x64
cmake --build --preset windows-vs2026-x64-release
```

The release artefacts are written below
`build/windows-vs2026-x64/PreenfmEditor_artefacts/Release`.

The `windows-vs2022-x64` presets remain available as a fallback.

Only the CMake-based Windows x64 build is maintained and tested for 4.0.3. The
checked-in Projucer, Visual Studio 2019, Linux and macOS projects are legacy
references and are not release build paths.

See [docs/MODERNIZATION.md](docs/MODERNIZATION.md) for migration details,
validation status, corrected legacy bugs and remaining technical debt. Version
4.x includes substantial correctness work inherited from the original editor in
addition to its new firmware and GUI support.

There are compiled versions for macOS and Windows [here](https://github.com/Ixox/preenfm2Controller/releases).  

To start, follow these steps : 

<br />
<img src="docs/toStart.PNG" width="600" />
<br />
<br />
If you have any questions or suggestions, please use the preenfm forum.  
This thread should be the one :   
http://ixox.fr/forum/index.php?topic=69349.0
<br />
<br />


# Screenshots

<br />
<img src="docs/pfmEditor3_Engine.PNG"  />
<br />
<br />
<img src="docs/pfmEditor3_Modulation.PNG"/>
<br />
<br />
<img src="docs/pfmEditor3_Arp.PNG"  />
<br />
<br />
<img src="docs/pfmEditor3_Organize.PNG" />
<br />
<br />
<br />
