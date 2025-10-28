# Building and Installation

## Prerequisites

Specialised software is required in order to compile this source code. If one or more prerequisites are not installed, the final executable may not build or lack certain features.

### cglm

1. Clone [cglm](https://github.com/recp/cglm) with a [git](https://git-scm.com/) client of your choosing.
2. Build and install by following instructions in `cglm/BUILDING.md`

### CMake

Download [CMake](https://cmake.org/) from the website or via package manager.

- Arch Linux: `sudo pacman -Syu cmake`

### libtvrender

[libtvrender](https://github.com/igni-project/libtvrender) is not currently available on any major package repositories and must be built and installed from source.

1. Clone libtvrender with a Git client of your choice. With the [official git command-line client](https://git-scm.com/downloads/linux), this will be `git clone https://github.com/igni-project/libtvrender.git`.

2. Follow build and installation instructions outlined by libtvrender/INSTALL.md. 

### GLAD

1. Install the `glad` package with your system's package manager

On Arch Linux, this is the command '`sudo pacman -Syu glad`.'

2. Run the `./glad-gen` script in this repository's root directory.

### GLFW

1. Install the `glfw` package with your system's package manager

On Arch Linux, this is the command '`sudo pacman -Syu glfw`.'

### libdrm

1. Install the `libdrm` package with your system's package manager

On Arch Linux, this is the command '`sudo pacman -Syu libdrm`.'

## Building

1. Create a new, empty 'build' folder.
2. Open CMake by clicking its icon or entering `cmake-gui` in a command prompt.
3. Set the source code directory to this repositories root folder.
4. Set the build directory to the 'build' folder created in step 1.
5. Click on the `Configure` button to configure project settings.
6. When done configuring, click `Generate`, then `Open Project`.
7. Build the project with the IDE of your choice.

## Installation

Before TVrender can run, it needs to be installed. This section walks through the process of installing this program.

1. Open up a terminal and enter the command `cmake --install .` in the build directory created in the building process. Read the __Building__ section for more info on this directory.

2. TVrender is now installed and ready for use.

By default, this software installs to /usr/local. To change the install location, set the CMAKE\_INSTALL\_PREFIX variable via the CMake GUI or by entering the command `cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr .` and rebuilding the project.


