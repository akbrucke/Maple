![Static Badge](https://img.shields.io/badge/Version-1.10-red)  
# Maple
VST3 Harmonic Mastering Console with 3-stage cascading saturation circuit and re-routable 2-band Baxandall EQ, using JUCE Framework, vibe-coded and then refined with a human touch

## Overview & Design Philosophy
Rather than relying on a single static saturation curve, Maple cascades three
independent analog-modeled gain stages in series and allows the equalizer to be inserted at any point
before, between, or after those stages.  

All nonlinear waveshaping and shelving filters operate inside a selectable 2x, 4x, or 8x linear-phase FIR
oversampling domain. Dry and wet signals are blended sample-accurately inside the oversampled loop,
ensuring zero phase shift or comb filtering at any parallel mix setting.  

## Features

### Saturation Models
Maple features four selectable saturation models, one per stage: **Tube, 73 Class-A, Vintage Tape, and FET.**  
They aren't generic ```std::tanh(x)``` waveshapers - each mode combines custom asymmetric transfer functions with frequency-dependent filtering and state memory tailored to the physical behavior of the four analog  
topologies, kept lightweight enough to cascade three stages in series inside an 8x linear-phase FIR oversampling loop.

### Baxandall Equalizer
A 2-band Baxandall mastering-grade EQ is included, which can be routed to four selectable positions in the saturation circuit. Placing the EQ before each saturation model results in unique breakup textures. For example, it can be used as a pre-emphasis EQ before the Vintage Tape model. The EQ is completely free of Nyquist cramping.

### Parallel Blend & 2X Drive
The plugin features a phase-exact Mix control for dialing in more aggressive settings to use in parallel, as well as a 2X Drive mode.

## Screenshots  

<img width="911" height="540" alt="Maple" src="https://github.com/user-attachments/assets/f8dd6b17-c881-4d55-bed2-87f9374e5ca1" />

## Installation
Currently, Maple is released for Windows 10/11 only, but Mac and Linux releases are coming soon. To grab the latest prebuilt release, head to the [releases page.](https://github.com/akbrucke/Maple/releases) Included with the release download is a thorough user manual, a collection of presets, and installation instructions.

## Building from Source (Windows)

Maple uses CMake and automatically fetches JUCE (`9.0.5`) during configuration. No manual SDK downloads or git submodules are required.

### Prerequisites
* **Git**
* **CMake 3.22 or newer**
* **Visual Studio 2022** (Community Edition or newer) with the **Desktop development with C++** workload installed

### Build Steps

1. Clone the repository
   
2. Open a terminal in the root folder and run  
`cmake -B build -G "Visual Studio 17 2022" -A x64`  
And then compile the release build:  
`cmake --build build --config Release`

3. Locate & Install the VST3 Plugin  
The finished VST3 bundle will be located in `build\Maple_artefacts\Release\VST3\Maple.vst3`  
Copy `Maple.vst3` into your system VST3 folder so your DAW can detect it.  
On Windows, this would be: `C:\Program Files\Common Files\VST3\`

## Building from Source (macOS, Apple silicon)

### Prerequisites
* **Xcode Command Line Tools**  
* **Homebrew**  
* **CMake 3.22 or newer**  
* **Ninja**  
(Full Xcode is not required)  

### Build Steps

1. Clone the repository  

2. Open a terminal in the root folder and run  
`cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release`   
then `cmake --build build`  

3. Locate & Install the VST3 Plugin  
The finished VST3 bundle will be located in `build\Maple_artefacts\Release\VST3\Maple.vst3`  
Copy `Maple.vst3` into your system VST3 folder so your DAW can detect it.  
On macOS, this would be: `~/Library/Audio/Plug-Ins/VST3/`  

- *Note that Logic Pro and GarageBand only load Audio Units, so they won't see the VST3*  

## Building from Source (Linux)  

### Prerequisites  
* JUCE 8 requires standard C++ build tools along with X11, FreeType, ALSA, and WebKit/GTK development headers to compile VST3 targets on Linux.  

### Build Steps

1. Clone the repository  

2. Open a terminal in the root folder and run  
`cmake -B build -DCMAKE_BUILD_TYPE=Release`  
And then compile the release build:  
`cmake --build build --config Release`  

3. Locate & Install the VST3 Plugin
The finished VST3 bundle will be located in `build\Follower_artefacts\Release\VST3\Follower.vst3`  
Copy `Follower.vst3` into your system VST3 folder so your DAW can detect it.  
On Linux, this would be: `~/.vst3`  

## Changelog
MAPLE V1.10 (November 22, 2026)
-Updated font rendering in preparation for cross-platform support

MAPLE V1.00 (November 18th, 2026)
-Released

