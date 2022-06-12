#include "LSeat.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <libudev.h>
#include <libinput.h>
#include <signal.h>


#include <xkbcommon/xkbcommon-compat.h>
#include <xkbcommon/xkbcommon-compose.h>
#include <xkbcommon/xkbcommon-keysyms.h>
#include <xkbcommon/xkbcommon-names.h>
#include <sys/mman.h>

#include <sys/poll.h>
#include <sys/eventfd.h>

#include <LWayland.h>
#include <LCursor.h>
#include <LToplevelRole.h>
#include <LSurface.h>
#include <LCompositor.h>
#include <LTime.h>
#include <LOutput.h>
#include <LPopupRole.h>

#include <LPointer.h>
#include <LKeyboard.h>

using namespace Louvre;


/****************************** Virtual mehtods ******************************/




/****************************** Non virtual mehtods ******************************/

LSeat::LSeat(LCompositor *compositor)
{
    p_compositor = compositor;

    // Setup libinput
    p_udev = udev_new();
    p_libinputInterface.open_restricted = &Louvre::LSeat::openRestricted;
    p_libinputInterface.close_restricted = &Louvre::LSeat::closeRestricted;
    p_li = libinput_udev_create_context(&p_libinputInterface, NULL, p_udev);
    libinput_udev_assign_seat(p_li, "seat0");
    libinput_dispatch(p_li);

    p_pointer = compositor->createPointerRequest(this);
    p_keyboard = compositor->createKeyboardRequest(this);
}

LSeat::~LSeat()
{

}

Int32 LSeat::libinputFd() const
{
    return libinput_get_fd(p_li);
}

LCompositor *LSeat::compositor() const
{
    return p_compositor;
}

LCursor *LSeat::cursor() const
{
    return compositor()->cursor();
}

UInt32 LSeat::capabilities() const
{
    return p_capabilities;
}

void LSeat::setCapabilities(UInt32 capabilitiesFlags)
{
    p_capabilities = capabilitiesFlags;

    for(LClient *lClient : compositor()->clients())
    {
        if(lClient->seatResource())
            wl_seat_send_capabilities(lClient->seatResource(),p_capabilities);
    }
}

LSurface *LSeat::touchFocusSurface() const
{
    return p_touchFocusSurface;
}

LToplevelRole *LSeat::activeTopLevel() const
{
    return p_activeTopLevel;
}


LPointer *LSeat::pointer() const
{
    return p_pointer;
}

LKeyboard *LSeat::keyboard() const
{
    return p_keyboard;
}

void LSeat::processInput()
{
    int ret = libinput_dispatch(p_li);

    if (ret != 0)
    {
        printf("Failed to dispatch libinput: %s\n", strerror(-ret));
        return;
    }

    libinput_event *ev;

    while ((ev = libinput_get_event(p_li)) != NULL)
    {

        libinput_event_type eventType = libinput_event_get_type(ev);


        if(eventType == LIBINPUT_EVENT_POINTER_MOTION)
        {
            libinput_event_pointer *pointerEvent = libinput_event_get_pointer_event(ev);

            double x = libinput_event_pointer_get_dx(pointerEvent);
            double y = libinput_event_pointer_get_dy(pointerEvent);

            if(pointer())
                pointer()->pointerMoveEvent(x,y);

            if(compositor()->cursor())
                compositor()->cursor()->move(x,y);
        }
        else if(eventType == LIBINPUT_EVENT_POINTER_BUTTON)
        {
            libinput_event_pointer *pointerEvent = libinput_event_get_pointer_event(ev);
            uint32_t button = libinput_event_pointer_get_button(pointerEvent);
            libinput_button_state state = libinput_event_pointer_get_button_state(pointerEvent);

            if(pointer())
                pointer()->pointerButtonEvent(button,state);
        }
        else if(eventType == LIBINPUT_EVENT_KEYBOARD_KEY)
        {
            if(keyboard())
            {
                libinput_event_keyboard *keyEv = libinput_event_get_keyboard_event(ev);
                libinput_key_state keyState = libinput_event_keyboard_get_key_state(keyEv);
                int keyCode = libinput_event_keyboard_get_key(keyEv);
                xkb_state_update_key(keyboard()->p_xkbKeymapState,keyCode+8,(xkb_key_direction)keyState);
                keyboard()->keyEvent(keyCode,keyState);
                keyboard()->updateModifiers();
            }
        }
        else if(eventType == LIBINPUT_EVENT_POINTER_AXIS)
        {
            if(pointer())
            {
                libinput_event_pointer *axisEvent = libinput_event_get_pointer_event(ev);

                double x = 0.0;
                double y = 0.0;

                if(libinput_event_pointer_has_axis(axisEvent,LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL))
                    x = libinput_event_pointer_get_axis_value(axisEvent,LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL);

                if(libinput_event_pointer_has_axis(axisEvent,LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL))
                    y = libinput_event_pointer_get_axis_value(axisEvent,LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL);

            #if LOUVRE_SEAT_VERSION >= 5
                UInt32 source = libinput_event_pointer_get_axis_source(axisEvent);
                pointer()->pointerAxisEvent(x,y,source);
            #else
                pointer()->pointerAxisEvent(x,y);
            #endif
            }
        }



        libinputEvent(ev);
        libinput_event_destroy(ev);
        libinput_dispatch(p_li);
    }
}

int LSeat::openRestricted(const char *path, int flags, void *)
{
    return open(path, flags);
}

void LSeat::closeRestricted(int fd, void *)
{
    close(fd);
}


/*
 * static void init_cursors() {
    s_cursors[CursorStyle_Arrow] = XcursorLibraryLoadCursor(s_display, "arrow");
    s_cursors[CursorStyle_Ibeam] = XcursorLibraryLoadCursor(s_display, "xterm");
    s_cursors[CursorStyle_Crosshair] = XcursorLibraryLoadCursor(s_display, "crosshair");
    s_cursors[CursorStyle_ClosedHand] = XcursorLibraryLoadCursor(s_display, "hand2");
    s_cursors[CursorStyle_OpenHand] = XcursorLibraryLoadCursor(s_display, "hand2");
    s_cursors[CursorStyle_ResizeLeftRight] = XcursorLibraryLoadCursor(s_display, "sb_h_double_arrow");
    s_cursors[CursorStyle_ResizeUpDown] = XcursorLibraryLoadCursor(s_display, "sb_v_double_arrow");
    s_cursors[CursorStyle_SizeAll] = XcursorLibraryLoadCursor(s_display, "diamond_cross");
}
*/
