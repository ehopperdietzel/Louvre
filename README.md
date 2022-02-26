
# Wayland++
A friendly C++ library to create Wayland compositors that run directly on top of DRM or X11 (useful while developing).

Wayland++ uses **libinput** and **evdev** for input listening and devices discovering, **xkbcommon** for keyboard mapping and any version of **Open GL** or **Vulkan** for drawing (any **EGL** - compatible API).

## Libraries
`sudo apt-get update & sudo apt install libwayland-dev libdrm-dev libgbm-dev libevdev-dev libinput-dev libegl-dev mesa-common-dev libgles2-mesa-dev libxkbcommon-dev`

## Example

1. Add your user to the **input** and **video** groups and then reboot.
2. Compile the ***MyCompositor*** example. (Easily buildable with QtCreator).
3. Run it like a regular program when using the X11 backend and on a new tty when using the DRM backend.
4. Press 1 to launch weston-terminal.
5. Press ESC to quit.

## Todo

* Globals Implementation
	* Compositor
		* create_region ✅
		* create_surface ✅
	* Region
		* add ✅
		* destroy ✅
		* subtract ✅
	* Surface
		* attach ✅
		* commit ✅
		* damage ✅
		* damage_buffer
		* destroy ✅
		* frame ✅
		* set_buffer_scale ✅
		* set_buffer_transform
		* set_input_region
		* set_opaque_region
	* Seat
		* get_keyboard ✅
		* get_pointer ✅
		* get_touch
		* release ✅
	* Pointer
		* set_cursor ✅
		* release ✅
	* Keyboard
		* release ✅
	* Output
		* release ✅
	* Data Device Manager
		* create_data_source
		* get_data_device

* Extensions Implementation
	* Xdg wm-base
		* create_positioner
		* destroy ✅
		* get_xdg_surface ✅
		* pong
	* Xdg surface
		* ack_configure
		* destroy
		* get_popup ✅
		* get_toplevel ✅
		* set_window_geometry
	* Xdg toplevel
		* destroy
		* move ✅
		* resize
		* set_app_id ✅
		* set_fullscreen
		* set_max_size ✅
		* set_maximized
		* set_min_size ✅
		* set_minimized
		* set_parent
		* set_title ✅
		* show_window_menu
		* unset_fullscreen
		* unset_maximized
	* Xdg popup
		* destroy
		* grab
		* reposition
* Shared Memory Textures  ✅
* EGL Texture Sharing (share textures already stored in GPU) ✅
* XWayland Support

