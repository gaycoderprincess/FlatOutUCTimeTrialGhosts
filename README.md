# FlatOut UC Time Trial Ghosts

Plugin to add a time trial mode with laptime replay ghosts to FlatOut: Ultimate Carnage

![preview 1](https://i.imgur.com/0NFTCiL.png)

## Installation

- Make sure you have the Steam GFWL version of the game, as this is the only version this plugin is compatible with. (exe size of 4242504 bytes)
- Plop the files into your game folder, edit `FlatOutUCTimeTrialGhosts_gcp.toml` to change the options to your liking.
- Start the game and launch any race, you will now have a PB ghost to race against instead of AI opponents.
- Enjoy, nya~ :3

## Building

Building is done on an Arch Linux system with CLion and vcpkg being used for the build process. 

Before you begin, clone [nya-common](https://github.com/gaycoderprincess/nya-common) and [nya-common-fouc](https://github.com/gaycoderprincess/nya-common-fouc) to folders next to this one, so they can be found.

Required packages: `mingw-w64-gcc vcpkg`

To install all dependencies, use:
```console
vcpkg install tomlplusplus:x86-mingw-static
```

Once installed, copy files from `~/.vcpkg/vcpkg/installed/x86-mingw-static/`:

- `include` dir to `nya-common/3rdparty`
- `lib` dir to `nya-common/lib32`

You should be able to build the project now in CLion.
