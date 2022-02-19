
# Wayland++
A friendly C++ library to create Wayland compositors that runs directly on top of DRM (or X11 for testing).

Wayland++ uses **libinput** and **evdev** for input listening, **xkbcommon** for keyboard mapping and **OpenGL ES 2.0** for drawing.

## Libraries
`sudo apt-get update & sudo apt install libwayland-dev libdrm-dev libgbm-dev libevdev-dev libinput-dev libegl-dev mesa-common-dev libgles2-mesa-dev libxkbcommon-dev`

## Example

1. Add your user to the **input** and **video** groups and then reboot.
2. Compile the ***MyCompositor*** example. (Easily buildable with QtCreator).
3. Run it like a normal program when using the X11 backend and on a new tty when using the DRM backend.
5. Press Q to quit.

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
		* set_cursor
		* release ✅
	* Keyboard
		* release ✅
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
		* move
		* resize
		* set_app_id
		* set_fullscreen
		* set_max_size
		* set_maximized
		* set_min_size
		* set_minimized
		* set_parent
		* set_title
		* show_window_menu
		* unset_fullscreen
		* unset_maximized
	* Xdg popup
		* destroy
		* grab
		* reposition

