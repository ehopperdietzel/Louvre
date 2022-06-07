A free and open source C++ library to create Wayland compositors.

<hr>

> 🔨 **Currently under development !!**

<hr>

## 📚 Dependencies
```
$ sudo apt install libwayland-dev libdrm-dev libgbm-dev libevdev-dev libxcursor-dev libxfixes-dev libinput-dev libegl-dev mesa-common-dev libgles2-mesa-dev libxkbcommon-dev libsoil-dev weston
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

![Example](Screenshot-01.png)

5. Press F7 to quit.

## 💻 Backends

* X11
* DRM/KMS

## 🧩 Protocols

* Wayland
* XDG Shell
* Decoration Manager
* Presentation Time
* Viewporter
* Qt Shell

## 💬 Buffer Sharing Mechanisms
* Shared Memory
* EGL
* KMS 

## 🕹️ Input Support
* Pointer
* Keyboard
* Touch

## 🖌️ Supported Rendering APIs
* OpenGL ES 2
* OpenGL ES 3
* OpenGL 3.x
* OpenGL 4.x
* Vulkan

## ⏲️ Performance
* Multithreading
* DRM Hardware Cursor
* DRM Planes

## 👴 Compatibility
* GTK Clients 
* Qt 5.x Clients 
* SDL
* EFL
* XWayland


