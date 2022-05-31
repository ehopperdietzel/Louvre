
<img style="position:relative;margin:0;padding:0;top:40px" src="https://i.imgur.com/cCT9KwN.png" width="112"/>
<h1 style="margin-top:0;padding-top:0">Louvre</h1>

A free and open source C++ library to create Wayland compositors.

<hr>

> 🔨 **Currently under development !!**

<hr>

## 📚 Dependencies
```
sudo apt install libwayland-dev libdrm-dev libgbm-dev libevdev-dev libxcursor-dev 
libxfixes-dev libinput-dev libegl-dev mesa-common-dev libgles2-mesa-dev 
libxkbcommon-dev libsoil-dev
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

* X11 <span style="padding-left:3px;font-size:8px;vertical-align:middle">🟢</span>
* DRM/KMS <span style="padding-left:3px;font-size:8px;vertical-align:middle">🟢</span>

## 🧩 Protocols

* Wayland <span style="padding-left:3px;font-size:8px;vertical-align:middle">🟢</span>
* XDG Shell <span style="padding-left:3px;font-size:8px;vertical-align:middle">🟠</span>
* Decoration Manager <span style="padding-left:3px;font-size:8px;vertical-align:middle">🟠</span>
* Presentation Time <span style="padding-left:3px;font-size:8px;vertical-align:middle">
🔴</span>
* Viewporter <span style="padding-left:3px;font-size:8px;vertical-align:middle">
🔴</span>
* Qt Shell <span style="padding-left:3px;font-size:8px;vertical-align:middle">
🔴</span>

## 💬 Buffer Sharing Mechanisms
* Shared Memory <span style="padding-left:3px;font-size:8px;vertical-align:middle">🟢</span>
* EGL <span style="padding-left:3px;font-size:8px;vertical-align:middle">🟢</span>
* KMS <span style="padding-left:3px;font-size:8px;vertical-align:middle">
🔴</span>

## 🕹️ Input Support
* Pointer <span style="padding-left:3px;font-size:8px;vertical-align:middle">🟢</span>
* Keyboard <span style="padding-left:3px;font-size:8px;vertical-align:middle">🟢</span>
* Touch <span style="padding-left:3px;font-size:8px;vertical-align:middle">
🔴</span>

## 🖌️ Supported Rendering APIs
* OpenGL ES 2 <span style="padding-left:3px;font-size:8px;vertical-align:middle">🟢</span>
* OpenGL ES 3 <span style="padding-left:3px;font-size:8px;vertical-align:middle">
🟠</span>
* OpenGL 3.x <span style="padding-left:3px;font-size:8px;vertical-align:middle">
🔴</span>
* OpenGL 4.x <span style="padding-left:3px;font-size:8px;vertical-align:middle">
🔴</span>
* Vulkan <span style="padding-left:3px;font-size:8px;vertical-align:middle">
🔴</span>

## ⏲️ Performance
* Multithreading <span style="padding-left:3px;font-size:8px;vertical-align:middle">🟢</span>
* DRM Hardware Cursor <span style="padding-left:3px;font-size:8px;vertical-align:middle">🟢</span>
* DRM Planes <span style="padding-left:3px;font-size:8px;vertical-align:middle">
🔴</span>


## 👴 Compatibility
* XWayland Support <span style="padding-left:3px;font-size:8px;vertical-align:middle">
🔴</span>


