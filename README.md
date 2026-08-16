# PreenFM+

You'll find here the source code of the software editor for the preenfm sound module (version 2 & 3). <br />
It uses the [JUCE framework](https://juce.com/).

## Modern CMake build

PreenFM+ 4.0.0 targets JUCE 9.0.0, C++17, VST3 and a standalone plugin host.
JUCE is fetched automatically and pinned to a release tag, so no sibling `JUCE_6.0`
directory or generated Projucer project is required.

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

See [docs/MODERNIZATION.md](docs/MODERNIZATION.md) for migration details,
validation status and remaining technical debt.

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

