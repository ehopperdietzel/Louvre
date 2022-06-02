
<img style="position:relative;margin:0px;padding:0;top:40px" src="https://i.imgur.com/cCT9KwN.png" width="104"/>
<h1 style="margin-top:0px;padding-top:0px">Louvre</h1>

A free and open source C++ library to create Wayland compositors.

<hr>

> 🔨 **Currently under development !!**

<hr>

## 📚 Dependencies
```
sudo apt install libwayland-dev libdrm-dev libgbm-dev libevdev-dev libxcursor-dev libxfixes-dev libinput-dev libegl-dev mesa-common-dev libgles2-mesa-dev libxkbcommon-dev libsoil-dev weston
```

## ✍️ Example

1. Add your user to the **input** and **video** groups and then reboot.
2. Compile the library and examples (the X11 backend is used by default).

```
$ cd Wpp/src
$ mkdir build
$ meson build
$ cd build
$ meson compile
```
3. Run the 'Minimal' example.
```
$ cd examples/Minimal
$ ./Minimal
```
4. Press F1 to launch weston-terminal.

![Example](screenshots/Screenshot-01.png)

5. Press F7 to quit.

## 💻 Backends

* X11 :white_check_mark:
* DRM/KMS :white_check_mark:

## 🧩 Protocols

* Wayland :white_check_mark:
* XDG Shell :white_check_mark:
* Decoration Manager
* Presentation Time
* Viewporter
* Qt Shell

## 💬 Buffer Sharing Mechanisms
* Shared Memory :white_check_mark:
* EGL :white_check_mark:
* KMS 

## 🕹️ Input Support
* Pointer :white_check_mark:
* Keyboard :white_check_mark:
* Touch

## 🖌️ Supported Rendering APIs
* OpenGL ES 2 :white_check_mark:
* OpenGL ES 3
* OpenGL 3.x
* OpenGL 4.x
* Vulkan

## ⏲️ Performance
* Multithreading :white_check_mark:
* DRM Hardware Cursor :white_check_mark:
* DRM Planes

## 👴 Compatibility
* GTK Clients :white_check_mark:
* Qt 5.x Clients :white_check_mark:
* SDL :white_check_mark:
* EFL :white_check_mark:
* XWayland


