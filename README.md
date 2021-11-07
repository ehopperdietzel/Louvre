# Wayland++
A friendly C++ library to create Wayland compositors that runs directly on top of DRM.

Wayland++ uses **libinput** and **evdev** for input listening and **OpenGL ES 2.0** for drawing.

## Libraries
`sudo apt install libwayland-dev libdrm-dev libgbm-dev libevdev-dev libinput-dev libegl-dev mesa-common-dev libgles2-mesa-dev`

## Example

1. Add your user to the **input** and **video** groups.
2. Compile the ***MyCompositor*** example with QtCreator.
3. Run it in a new tty.
4. Press Q to quit.


