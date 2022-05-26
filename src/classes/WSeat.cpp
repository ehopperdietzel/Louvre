#include "WSeat.h"

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

#include <WWayland.h>
#include <WCursor.h>
#include <WToplevelRole.h>
#include <WSurface.h>
#include <WCompositor.h>
#include <WTime.h>
#include <WOutput.h>

using namespace Wpp;


/****************************** Virtual mehtods ******************************/

void WSeat::pointerMoveEvent(float, float)
{

    // Resizing
    if(updateResizingToplevelSize())
        return;


    // Moving surface
    if(updateMovingTopLevelPos())
    {
        // Limits the top position
        Int32 topbarHeight = (WPP_TB_H+2)/compositor()->outputs().front()->getOutputScale();

        if(movingTopLevel()->surface()->pos().y() < topbarHeight)
            movingTopLevel()->surface()->setY(topbarHeight);

        compositor()->repaintAllOutputs();
        return;
    }


    // Draggin surface
    if(draggingSurface())
    {
        sendPointerMoveEvent();
        return;
    }


    // Find the surface at cursor positon
    WSurface *surface = surfaceAt(cursor()->position(),true);

    // If no surface was found
    if(!surface)
    {
        setPointerFocus(nullptr);
        cursor()->setCursor("arrow");
    }
    else
    {
        if(pointerFocusSurface() == surface)
            sendPointerMoveEvent();
        else
            setPointerFocus(surface);
    }

}


void WSeat::pointerClickEvent(UInt32 button, UInt32 state)
{

    if(!pointerFocusSurface())
    {
        WSurface *surface = surfaceAt(cursor()->position());

        if(surface)
        {
            setKeyboardFocus(surface);
            setPointerFocus(surface);
            sendPointerButtonEvent(button,state);
        }
        return;
    }

    sendPointerButtonEvent(button,state);

    if(button != LEFT_BUTTON)
        return;

    if(state == LIBINPUT_BUTTON_STATE_PRESSED)
    {
        setDragginSurface(pointerFocusSurface());

        setKeyboardFocus(pointerFocusSurface());

        if(pointerFocusSurface()->type() == WSurface::Toplevel)
            pointerFocusSurface()->toplevel()->configure(pointerFocusSurface()->toplevel()->state() | WToplevelRole::Activated);

        // Raise view
        if(pointerFocusSurface()->parent())
            compositor()->riseSurface(pointerFocusSurface()->topParent());
        else
            compositor()->riseSurface(pointerFocusSurface());

    }
    else
    {

        // Stop Resizing TopLevel
        stopResizingToplevel();

        // Stop Moving TopLevel
        stopMovingTopLevel();

        // Set dragging surface to nullptr
        setDragginSurface(nullptr);

        // Send a mouse leave event if the surface was being dragged
        if(!pointerFocusSurface()->inputRegionContainsPoint(pointerFocusSurface()->pos(),cursor()->position()))
        {
            setPointerFocus(nullptr);
            cursor()->setCursor("arrow");
        }

    }

}

void WSeat::keyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group)
{
    sendKeyboardModifiersEvent(depressed,latched,locked,group);
}

void WSeat::keyEvent(UInt32 keyCode, UInt32 keyState)
{
    xkb_keysym_t sym = keySymbol(keyCode);

    sendKeyboardKeyEvent(keyCode,keyState);

    if(keyState == LIBINPUT_KEY_STATE_RELEASED)
    {
        // Ends compositor if ESC is pressed
        if(sym == XKB_KEY_F7)
        {
            WWayland::terminateDisplay();
            exit(0);
        }
        else if(sym == XKB_KEY_F1)
        {
            if (fork()==0)
            {
                setsid();
                char *const envp[] = {"XDG_RUNTIME_DIR=/run/user/1000",0};
                const char *argv[64] = {"/usr/bin/weston-terminal" , NULL, NULL , NULL};
                execve(argv[0], (char **)argv, envp);
                exit(0);
            }
        }
    }
}

void WSeat::setCursorRequest(WSurface *cursorSurface, Int32 hotspotX, Int32 hotspotY)
{
    if(cursorSurface)
        cursor()->setTexture(cursorSurface->texture(),WPointF(hotspotX,hotspotY));
    else
        cursor()->setCursor("arrow");

    setCursorSurface(cursorSurface);
}

void WSeat::sendPointerLeaveEvent(WSurface *surface)
{
    // If surface is nullptr
    if(!surface)
        return;

    // If do not have a wl_pointer
    if(!surface->client()->pointerResource())
        return;

    // Send the unset focus event
    wl_pointer_send_leave(surface->client()->pointerResource(),WWayland::nextSerial(),surface->resource());

    // Version 5+
    if(wl_resource_get_version(surface->client()->pointerResource()) >= 5)
        wl_pointer_send_frame(surface->client()->pointerResource());

}

void WSeat::sendPointerEnterEvent(WSurface *surface, const WPoint &point)
{
    // If surface is nullptr
    if(!surface)
        return;

    // If do not have a wl_pointer
    if(!surface->client()->pointerResource())
        return;

    // Send focus event
    wl_pointer_send_enter(surface->client()->pointerResource(),WWayland::nextSerial(),surface->resource(),wl_fixed_from_double(point.x()),wl_fixed_from_double(point.y()));

    // Version 5+
    if(wl_resource_get_version(surface->client()->pointerResource()) >= 5)
        wl_pointer_send_frame(surface->client()->pointerResource());

}

/****************************** Non virtual mehtods ******************************/

WSeat::WSeat(WCompositor *compositor)
{
    p_compositor = compositor;

    // Create null keys
    wl_array_init(&p_keys);

    // Create XKB context
    p_xkbContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);

    // Set the default keymap
    setKeymap();

    // Setup libinput
    p_udev = udev_new();
    p_libinputInterface.open_restricted = &Wpp::WSeat::openRestricted;
    p_libinputInterface.close_restricted = &Wpp::WSeat::closeRestricted;
    p_li = libinput_udev_create_context(&p_libinputInterface, NULL, p_udev);
    libinput_udev_assign_seat(p_li, "seat0");
    libinput_dispatch(p_li);
}

WSeat::~WSeat()
{
    wl_array_release(&p_keys);
}

void WSeat::setKeymap(const char *rules, const char *model, const char *layout, const char *variant, const char *options)
{
    char *keymapString,*xdgRuntimeDir,*map;

    p_xkbKeymapName.rules = rules;
    p_xkbKeymapName.model = model;
    p_xkbKeymapName.layout = "latam";
    p_xkbKeymapName.variant = variant;
    p_xkbKeymapName.options = options;

    // Find a keymap matching suggestions
    p_xkbKeymap = xkb_keymap_new_from_names(p_xkbContext, &p_xkbKeymapName, XKB_KEYMAP_COMPILE_NO_FLAGS);

    // Get the keymap string
    keymapString = xkb_keymap_get_as_string(p_xkbKeymap, XKB_KEYMAP_FORMAT_TEXT_V1);

    // Store the keymap size
    p_xkbKeymapSize = strlen(keymapString) + 1;

    // Get the XDG_RUNTIME_DIR env
    xdgRuntimeDir = getenv("XDG_RUNTIME_DIR");

    // Open and store the file descritor
    p_xkbKeymapFd = open(xdgRuntimeDir, O_TMPFILE|O_RDWR|O_EXCL, 0600);

    if(p_xkbKeymapFd < 0)
    {
        printf("Error creating shared memory for keyboard layout.\n");
        exit(-1);
    }

    // Write the keymap string
    ftruncate(p_xkbKeymapFd, p_xkbKeymapSize);
    map = (char*)mmap(NULL, p_xkbKeymapSize, PROT_READ|PROT_WRITE, MAP_SHARED, p_xkbKeymapFd, 0);
    //strcpy(map, keymapString);
    memcpy(map,keymapString,p_xkbKeymapSize);
    munmap(map, p_xkbKeymapSize);

    // Keymap string not needed anymore
    free(keymapString);

    // Create a xkb keyboard state to handle modifiers
    p_xkbKeymapState = xkb_state_new(p_xkbKeymap);
}

Int32 WSeat::libinputFd() const
{
    return libinput_get_fd(p_li);
}

WCompositor *WSeat::compositor() const
{
    return p_compositor;
}

WCursor *WSeat::cursor() const
{
    return compositor()->cursor();
}

void WSeat::setPointerFocus(WSurface *surface)
{
    // If surface is not nullptr
    if(surface)
    {
        // If already has focus
        if(pointerFocusSurface() == surface)
            return;

        // Remove focus from focused surface
        sendPointerLeaveEvent(pointerFocusSurface());

        // Set focus
        if(surface->client()->pointerResource())
        {
            // Send focus event
            sendPointerEnterEvent(surface,cursor()->position()-surface->pos(true));
            p_pointerFocusSurface = surface;
        }
        else
            p_pointerFocusSurface = nullptr;
    }

    // If surface is nullptr
    else
    {
        // Remove focus from focused surface
        sendPointerLeaveEvent(pointerFocusSurface());
        p_pointerFocusSurface = nullptr;
    }
}

void WSeat::sendPointerMoveEvent()
{
    // If no surface has focus surface
    if(!pointerFocusSurface())
        return;

    // If do not have a wl_pointer
    if(!pointerFocusSurface()->client()->pointerResource())
        return;

    // Calculate local cursor position
    WPoint pos = cursor()->position()-pointerFocusSurface()->pos(true);

    // Send pointer move event to the focused surface
    wl_pointer_send_motion(pointerFocusSurface()->client()->pointerResource(),WTime::ms(),wl_fixed_from_int(pos.x()),wl_fixed_from_int(pos.y()));

    // Version 5+
    if(wl_resource_get_version(pointerFocusSurface()->client()->pointerResource()) >= 5)
        wl_pointer_send_frame(pointerFocusSurface()->client()->pointerResource());
}

void WSeat::sendPointerButtonEvent(UInt32 button, UInt32 state)
{
    // If no surface has focus
    if(!pointerFocusSurface())
        return;

    // If do not have a wl_pointer
    if(!pointerFocusSurface()->client()->pointerResource())
        return;

    // Send pointer button event
    wl_pointer_send_button(pointerFocusSurface()->client()->pointerResource(),WWayland::nextSerial(),WTime::ms(),button,state);

    // Version 5+
    if(wl_resource_get_version(pointerFocusSurface()->client()->pointerResource()) >= 5)
        wl_pointer_send_frame(pointerFocusSurface()->client()->pointerResource());
}

void WSeat::setKeyboardFocus(WSurface *surface)
{
    // If surface is not nullptr
    if(surface)
    {
        // If already has focus
        if(keyboardFocusSurface() == surface)
            return;
        else
        {
            // If another surface has focus
            if(keyboardFocusSurface() && keyboardFocusSurface()->client()->keyboardResource())
                wl_keyboard_send_leave(keyboardFocusSurface()->client()->keyboardResource(),WWayland::nextSerial(),keyboardFocusSurface()->resource());

            if(surface->client()->keyboardResource())
            {
                wl_keyboard_send_enter(surface->client()->keyboardResource(),WWayland::nextSerial(),surface->resource(),&p_keys);
                p_keyboardFocusSurface = surface;
                sendKeyboardModifiersEvent();
            }
            else
                p_keyboardFocusSurface = nullptr;
        }

    }
    else
    {
        // If a surface has focus
        if(keyboardFocusSurface() && keyboardFocusSurface()->client()->keyboardResource())
            wl_keyboard_send_leave(keyboardFocusSurface()->client()->keyboardResource(),WWayland::nextSerial(),keyboardFocusSurface()->resource());

        p_keyboardFocusSurface = nullptr;
    }
}

void WSeat::sendKeyboardKeyEvent(UInt32 keyCode, UInt32 keyState) const
{
    // If no surface has focus
    if(!keyboardFocusSurface())
        return;

    // If do not have a wl_keyboard
    if(!keyboardFocusSurface()->client()->keyboardResource())
        return;

    wl_keyboard_send_key(keyboardFocusSurface()->client()->keyboardResource(),WWayland::nextSerial(),WTime::ms(),keyCode,keyState);
}

void WSeat::sendKeyboardModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group) const
{
    // If no surface has focus
    if(!keyboardFocusSurface())
        return;

    // If do not have a wl_keyboard
    if(!keyboardFocusSurface()->client()->keyboardResource())
        return;

    wl_keyboard_send_modifiers(keyboardFocusSurface()->client()->keyboardResource(),WWayland::nextSerial(),depressed,latched,locked,group);
}

void WSeat::sendKeyboardModifiersEvent() const
{
    sendKeyboardModifiersEvent(p_modifiersState.depressed, p_modifiersState.latched, p_modifiersState.locked, p_modifiersState.group);
}

void WSeat::startResizingToplevel(WToplevelRole *topLevel, WToplevelRole::Edge edge)
{
    p_resizingToplevel = topLevel;
    p_resizingToplevelEdge = edge;
    p_resizingToplevelInitSize = topLevel->surface()->size();
    p_resizingToplevelInitWindowSize = topLevel->windowGeometry().bottomRight();
    p_resizingToplevelInitCursorPos = cursor()->position();
    p_resizingToplevelInitPos = topLevel->surface()->pos();
}

bool WSeat::updateResizingToplevelSize()
{
    if(resizingToplevel())
    {
        WSize newSize = resizingToplevel()->calculateResizeRect(resizingToplevelInitCursorPos()-cursor()->position(),
                                                                p_resizingToplevelInitWindowSize,
                                                                resizingToplevelEdge());

        resizingToplevel()->configure(newSize ,WToplevelRole::Activated | WToplevelRole::Resizing);
        return true;
    }
    return false;
}

void WSeat::updateResizingToplevelPos()
{

    if(resizingToplevel())
    {
        WSize s = resizingToplevelInitSize();
        WPoint p = resizingToplevelInitPos();
        Int32 bs = resizingToplevel()->surface()->bufferScale();
        WToplevelRole::Edge edge =  resizingToplevelEdge();

        if(edge ==  WToplevelRole::Edge::Top || edge ==  WToplevelRole::TopLeft || edge ==  WToplevelRole::TopRight)
            resizingToplevel()->surface()->setY(p.y() + (s.h() - resizingToplevel()->surface()->size().h())/bs);

        if(edge ==  WToplevelRole::Edge::Left || edge ==  WToplevelRole::Edge::TopLeft || edge ==  WToplevelRole::Edge::BottomLeft)
            resizingToplevel()->surface()->setX(p.x() + (s.w() - resizingToplevel()->surface()->size().w())/bs);
    }
}

void WSeat::stopResizingToplevel()
{
    if(resizingToplevel())
        resizingToplevel()->configure(WToplevelRole::Activated);

    p_resizingToplevel = nullptr;
}

void WSeat::startMovingTopLevel(WToplevelRole *topLevel)
{
    p_movingTopLevelInitPos = topLevel->surface()->pos();
    p_movingTopLevelInitCursorPos = cursor()->position();
    p_movingTopLevel = topLevel;
}

bool WSeat::updateMovingTopLevelPos()
{
    if(movingTopLevel())
    {
        movingTopLevel()->surface()->setPos(movingTopLevelInitPos() - movingTopLevelInitCursorPos() + cursor()->position());
        return true;
    }
    return false;
}

void WSeat::stopMovingTopLevel()
{
    p_movingTopLevel = nullptr;
}

void WSeat::setDragginSurface(WSurface *surface)
{
    p_draggingSurface = surface;
}

void WSeat::setCursorSurface(WSurface *surface)
{
    p_cursorSurface = surface;
}

WSurface *WSeat::pointerFocusSurface() const
{
    return p_pointerFocusSurface;
}

WSurface *WSeat::keyboardFocusSurface() const
{
    return p_keyboardFocusSurface;
}

WSurface *WSeat::touchFocusSurface() const
{
    return p_touchFocusSurface;
}

WSurface *WSeat::draggingSurface() const
{
    return p_draggingSurface;
}

WSurface *WSeat::cursorSurface() const
{
    return p_cursorSurface;
}

WToplevelRole *WSeat::resizingToplevel() const
{
    return p_resizingToplevel;
}

WToplevelRole *WSeat::movingTopLevel() const
{
    return p_movingTopLevel;
}

WToplevelRole *WSeat::activeTopLevel() const
{
    return p_activeTopLevel;
}

const WPoint &WSeat::movingTopLevelInitPos() const
{
    return p_movingTopLevelInitPos;
}

const WPoint &WSeat::movingTopLevelInitCursorPos() const
{
    return p_movingTopLevelInitCursorPos;
}

const WPoint &WSeat::resizingToplevelInitPos() const
{
    return p_resizingToplevelInitPos;
}

const WPoint &WSeat::resizingToplevelInitCursorPos() const
{
    return p_resizingToplevelInitCursorPos;
}

const WSize &WSeat::resizingToplevelInitSize() const
{
    return p_resizingToplevelInitSize;
}

WToplevelRole::Edge WSeat::resizingToplevelEdge() const
{
    return p_resizingToplevelEdge;
}

WSurface *WSeat::surfaceAt(const WPoint &point, bool useRolePos)
{
    for(list<WSurface*>::const_reverse_iterator s = compositor()->surfaces().rbegin(); s != compositor()->surfaces().rend(); s++)
        if((*s)->type() != WSurface::Undefined && (*s)->type() != WSurface::Cursor && !(*s)->minimized())
            if((*s)->inputRegionContainsPoint((*s)->pos(useRolePos),point))
                return *s;
        
    return nullptr;
}

xkb_keysym_t WSeat::keySymbol(UInt32 keyCode)
{
    return xkb_state_key_get_one_sym(p_xkbKeymapState,keyCode+8);
}

void WSeat::processInput()
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
            this->pointerMoveEvent(x,y);
            if(compositor()->cursor())
                compositor()->cursor()->move(x,y);
        }
        else if(eventType == LIBINPUT_EVENT_POINTER_BUTTON)
        {
            libinput_event_pointer *pointerEvent = libinput_event_get_pointer_event(ev);
            uint32_t button = libinput_event_pointer_get_button(pointerEvent);
            libinput_button_state state = libinput_event_pointer_get_button_state(pointerEvent);
            this->pointerClickEvent(button,state);
        }
        else if(eventType == LIBINPUT_EVENT_KEYBOARD_KEY)
        {
            libinput_event_keyboard *keyEv = libinput_event_get_keyboard_event(ev);
            libinput_key_state keyState = libinput_event_keyboard_get_key_state(keyEv);
            int keyCode = libinput_event_keyboard_get_key(keyEv);
            xkb_state_update_key(p_xkbKeymapState,keyCode+8,(xkb_key_direction)keyState);
            keyEvent(keyCode,keyState);
            updateModifiers();
        }


        libinputEvent(ev);
        libinput_event_destroy(ev);
        libinput_dispatch(p_li);
    }


}

int WSeat::openRestricted(const char *path, int flags, void *)
{
    return open(path, flags);
}

void WSeat::closeRestricted(int fd, void *)
{
    close(fd);
}

void WSeat::updateModifiers()
{
    p_modifiersState.depressed = xkb_state_serialize_mods(p_xkbKeymapState, XKB_STATE_MODS_DEPRESSED);
    p_modifiersState.latched = xkb_state_serialize_mods(p_xkbKeymapState, XKB_STATE_MODS_LATCHED);
    p_modifiersState.locked = xkb_state_serialize_mods(p_xkbKeymapState, XKB_STATE_MODS_LOCKED);
    p_modifiersState.group = xkb_state_serialize_layout(p_xkbKeymapState, XKB_STATE_LAYOUT_EFFECTIVE);
    keyModifiersEvent(p_modifiersState.depressed,p_modifiersState.latched,p_modifiersState.locked,p_modifiersState.group);
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
