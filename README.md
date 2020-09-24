# Chooter

A cute little 2D space shooter game written in C using the [SDL](https://www.libsdl.org "SDL's Homepage") library

![Chooter Showcase](res/image/showcase.gif)

## Dependencies

+ [premake](https://premake.github.io/) 
+ SDL2
+ SDL2_image
+ SDL2_mixer

<generic package [manager](https://i.imgflip.com/3so0ng.jpg)> `install premake sdl2 sdl2_image sdl2_mixer`

## Build

The building process is really complicated, so please make sure you are executing everything correctly and thoughtfully:

```
premake5 gmake2 && make [config=debug|release]
```

The binary named `chooter` will be located in the `bin/Debug` directory (or `bin/Release`).

## Controls

+ WASD to move up, left, down and right respectively.
+ Spacebar to fire a bullet.
+ `[A-Z]` to type your name when you reach the high score table.
