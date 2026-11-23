![Static Badge](https://img.shields.io/badge/Version-1.10-red)  
# Maple
VST3 Harmonic Mastering Console with 3-stage saturation circuit and 2-band Baxandall EQ  

## Overview & Design Philosophy
Rather than relying on a single static saturation curve, Maple cascades three
independent analog-modeled gain stages in series and allows the equalizer to be inserted at any point
before, between, or after those stages.  

All nonlinear waveshaping and shelving filters operate inside a selectable 2x, 4x, or 8x linear-phase FIR
oversampling domain. Dry and wet signals are blended sample-accurately inside the oversampled loop,
ensuring zero phase shift or comb filtering at any parallel mix setting.  

## Features

### Four Selectable Saturation Models
The plugin features four selectable saturation models, one per stage: Tube, 73 Class-A, Vintage Tape, and FET.  

## Screenshots  



## Building from Source (Windows)

Maple uses CMake and automatically fetches JUCE (`9.0.5`) during configuration. No manual SDK downloads or git submodules are required.

### Prerequisites
* **Git**
* **CMake 3.22 or newer**
* **Visual Studio 2022** (Community Edition or newer) with the **Desktop development with C++** workload installed

### Build Steps

1. Clone the repository
   
2. Open a terminal in the root folder and run  
```cmake -B build -G "Visual Studio 17 2022" -A x64```  
And then compile the release build:  
```cmake --build build --config Release```

3. Locate & Install the VST3 Plugin
The finished VST3 bundle will be located in ```build\Maple_artefacts\Release\VST3\Maple.vst3```  
Copy ```Maple.vst3``` into your system VST3 folder so your DAW can detect it.  
On Windows, this would be: ```C:\Program Files\Common Files\VST3\```

## Changelog
MAPLE V1.10 (November 22, 2026)
-Updated font rendering in preparation for cross-platform support

MAPLE V1.00 (November 18th, 2026)
-Released

