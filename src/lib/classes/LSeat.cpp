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
    m_compositor = compositor;

    // Setup libinput
    m_udev = udev_new();
    m_libinputInterface.open_restricted = &Louvre::LSeat::openRestricted;
    m_libinputInterface.close_restricted = &Louvre::LSeat::closeRestricted;
    m_li = libinput_udev_create_context(&m_libinputInterface, NULL, m_udev);
    libinput_udev_assign_seat(m_li, "seat0");
    libinput_dispatch(m_li);

    m_pointer = compositor->createPointerRequest(this);
    m_keyboard = compositor->createKeyboardRequest(this);
}

LSeat::~LSeat()
{

}

Int32 LSeat::libinputFd() const
{
    return libinput_get_fd(m_li);
}

LCompositor *LSeat::compositor() const
{
    return m_compositor;
}

LCursor *LSeat::cursor() const
{
    return compositor()->cursor();
}

UInt32 LSeat::capabilities() const
{
    return m_capabilities;
}

void LSeat::setCapabilities(UInt32 capabilitiesFlags)
{
    m_capabilities = capabilitiesFlags;

    for(LClient *lClient : compositor()->clients())
    {
        if(lClient->seatResource())
            wl_seat_send_capabilities(lClient->seatResource(),m_capabilities);
    }
}

LSurface *LSeat::touchFocusSurface() const
{
    return m_touchFocusSurface;
}

LToplevelRole *LSeat::activeTopLevel() const
{
    return m_activeTopLevel;
}


LPointer *LSeat::pointer() const
{
    return m_pointer;
}

LKeyboard *LSeat::keyboard() const
{
    return m_keyboard;
}

void LSeat::libinputEvent(libinput_event */*ev*/)
{

}

void LSeat::processInput()
{
    int ret = libinput_dispatch(m_li);

    if (ret != 0)
    {
        printf("Failed to dispatch libinput: %s\n", strerror(-ret));
        return;
    }

    libinput_event *ev;

    while ((ev = libinput_get_event(m_li)) != NULL)
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
                compositor()->cursor()->move(x*(1.+abs(x*0.025)),y*(1.+abs(y*0.025)));
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
                xkb_state_update_key(keyboard()->m_xkbKeymapState,keyCode+8,(xkb_key_direction)keyState);
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
        libinput_dispatch(m_li);
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
