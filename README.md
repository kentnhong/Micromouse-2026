# MM Micromouse Project



## Installing Prerequisites

# MM Micromouse Project (STM32F411)

This repository contains the firmware and supporting code for a Micromouse robot, targeting the STM32F411 microcontroller.

## Installing Prerequisites
Install the ARM toolchain for STM32F411:
```
sudo apt-get install gcc-arm-none-eabi
```
On Windows, download from [ARM Developer](https://developer.arm.com/downloads/-/gnu-rm) and add it to your PATH. Verify installation:
```
arm-none-eabi-gcc --version
```

Install CMake:
```
sudo apt-get install cmake
```
On Windows, download from [CMake](https://cmake.org/download/), add to PATH, and verify:
```
cmake --version
```

Install Ninja:
```
sudo apt-get install ninja-build
```
On Windows, download from [Ninja Releases](https://github.com/ninja-build/ninja/releases), add to PATH, and verify:
```
ninja --version
```

Pull in external dependencies:
```
git submodule update --init --recursive
```

## Building (STM32F411 Only)
To build for STM32F411:
On Windows, use the `.ps1` script in PowerShell. On Linux, use the `.sh` script:
```
./make.ps1 -t stm32f411
```
or
```
./make.sh -t stm32f411
```

For a clean build:
```
./make.ps1 -t stm32f411 -c
```

To build in Release mode:
```
./make.ps1 -t stm32f411 -r
```

## Debugging
Install OpenOCD:
```
sudo apt-get install openocd
```
On Windows, download from [OpenOCD](https://openocd.org/pages/getting-openocd.html) and add to PATH.
Install the Cortex-Debug extension for VS Code.
Reference launch.json files are found under `.vscode` in this repository.

## Developing
Install clang-format for auto-formatting:
On Windows:
```
python -m pip install clang-format
```
On Linux:
```
sudo apt install clang-format
```
In VS Code, enable Format On Save in settings.

## Building
To build:
On Windows, use the `.ps1` script in PowerShell. On Linux, use the `.sh` script. Minimum parameters:
```./make.ps1 -t <name of preset>```
Example:
```./make.ps1 -t stm32f411 -c```
(See CMakePresets.json for available presets.)

To build a specific application:
```./make.ps1 -t stm32f411 -a cli_app```

For a clean build:
```./make.ps1 -t <name of preset> -c```

To build in Release mode:
```./make.ps1 -t stm32f746 -r```

## Debugging
Install OpenOCD:
```sudo apt-get install openocd```
On Windows, download from [OpenOCD](https://openocd.org/pages/getting-openocd.html) and add to PATH.
Install the Cortex-Debug extension for VS Code.
Reference launch.json files are found under `.vscode` in this repository.

## Developing
Install clang-format for auto-formatting:
On Windows:
```python -m pip install clang-format```
On Linux:
```sudo apt install clang-format```
In VS Code, enable Format On Save in settings.

To run native unit tests, build for native, then `cd build/native` and run:
```ctest```
