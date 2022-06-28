#include "LKeyboard.h"

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <LTime.h>
#include <LWayland.h>
#include <LCompositor.h>
#include <LSeat.h>

#include <LClient.h>
#include <LClientPrivate.h>
#include <LDataDevicePrivate.h>
#include <LDataOfferPrivate.h>
#include <DataOffer.h>
#include <LDataSourcePrivate.h>
#include <LCursor.h>
#include <LOutput.h>

using namespace Louvre;

struct wl_data_offer_interface dataOffer_implementation =
{
   .accept = &Globals::DataOffer::accept,
   .receive = &Globals::DataOffer::receive,
   .destroy = &Globals::DataOffer::destroy,
   .finish = &Globals::DataOffer::finish,
   .set_actions = &Globals::DataOffer::set_actions
};

LKeyboard::LKeyboard(LSeat *seat)
{
    m_seat = seat;

    // Create null keys
    wl_array_init(&m_keys);

    // Create XKB context
    m_xkbContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);

    // Set the default keymap
    setKeymap();
}

LKeyboard::~LKeyboard()
{
    wl_array_release(&m_keys);
}

LSeat *LKeyboard::seat() const
{
    return m_seat;
}

LCompositor *LKeyboard::compositor() const
{
    return seat()->compositor();
}

void LKeyboard::setKeymap(const char *rules, const char *model, const char *layout, const char *variant, const char *options)
{
    if(m_xkbKeymapFd != -1)
    {
         close(m_xkbKeymapFd);
    }

    char *keymapString,*map;

    //const char *xdgRuntimeDir = "/run/user/1000";

    m_xkbKeymapName.rules = rules;
    m_xkbKeymapName.model = model;
    m_xkbKeymapName.layout = layout;
    m_xkbKeymapName.variant = variant;
    m_xkbKeymapName.options = options;

    // Find a keymap matching suggestions
    m_xkbKeymap = xkb_keymap_new_from_names(m_xkbContext, &m_xkbKeymapName, XKB_KEYMAP_COMPILE_NO_FLAGS);

    // Get the keymap string
    keymapString = xkb_keymap_get_as_string(m_xkbKeymap, XKB_KEYMAP_FORMAT_TEXT_V1);

    // Store the keymap size
    m_xkbKeymapSize = strlen(keymapString) + 1;

    // Get the XDG_RUNTIME_DIR env
    char *xdgRuntimeDir = getenv("XDG_RUNTIME_DIR");

    // Open and store the file descritor
    m_xkbKeymapFd = open(xdgRuntimeDir, O_TMPFILE|O_RDWR|O_EXCL, 0600);

    if(m_xkbKeymapFd < 0)
    {
        printf("Error creating shared memory for keyboard layout.\n");
        exit(-1);
    }

    // Write the keymap string
    ftruncate(m_xkbKeymapFd, m_xkbKeymapSize);
    map = (char*)mmap(NULL, m_xkbKeymapSize, PROT_READ|PROT_WRITE, MAP_SHARED, m_xkbKeymapFd, 0);
    //strcpy(map, keymapString);
    memcpy(map,keymapString,m_xkbKeymapSize);
    munmap(map, m_xkbKeymapSize);

    // Keymap string not needed anymore
    free(keymapString);

    // Create a xkb keyboard state to handle modifiers
    m_xkbKeymapState = xkb_state_new(m_xkbKeymap);

    for(LClient *lClient : compositor()->clients())
    {
        if(lClient->keyboardResource())
            wl_keyboard_send_keymap(lClient->keyboardResource(),WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1, keymapFd(), keymapSize());
    }
}

Int32 LKeyboard::keymapFd() const
{
    return m_xkbKeymapFd;
}

Int32 LKeyboard::keymapSize() const
{
    return m_xkbKeymapSize;
}

LSurface *LKeyboard::focusSurface() const
{
    return m_keyboardFocusSurface;
}

const LKeyboard::LKeyboardModifiersState &LKeyboard::modifiersState() const
{
    return m_modifiersState;
}

void LKeyboard::setFocus(LSurface *surface)
{
    // If surface is not nullptr
    if(surface)
    {
        // If already has focus
        if(focusSurface() == surface)
            return;
        else
        {
            // If another surface has focus
            if(focusSurface() && focusSurface()->client()->keyboardResource())
            {
                focusSurface()->client()->imp()->m_lastKeyboardLeaveEventSerial = LWayland::nextSerial();
                wl_keyboard_send_leave(focusSurface()->client()->keyboardResource(),focusSurface()->client()->lastKeyboardLeaveEventSerial(),focusSurface()->resource());
            }

            if(surface->client()->keyboardResource())
            {
                // Send data device selection first
                if(compositor()->dataSelection() && surface->client()->dataDevice())
                {
                    LDataDevice *lDevice = surface->client()->dataDevice();
                    wl_resource *dataOffer = wl_resource_create(surface->client()->client(),&wl_data_offer_interface,1,0);

                    if(dataOffer != NULL)
                    {
                        LDataOffer *lDataOffer = new LDataOffer(dataOffer,lDevice);
                        lDataOffer->imp()->m_dataSource = compositor()->dataSelection();
                        wl_resource_set_implementation(dataOffer, &dataOffer_implementation, lDataOffer, &Globals::DataOffer::resource_destroy);

                        // Save the data offer reference to que data selection
                        compositor()->dataSelection()->imp()->m_dataOffers.push_back(lDataOffer);

                        wl_data_device_send_data_offer(lDevice->resource(),dataOffer);

                        for(const char *mime : compositor()->dataSelection()->imp()->m_mimeTypes)
                        {
                            printf("SEND MIME %s\n",mime);
                            wl_data_offer_send_offer(dataOffer,mime);
                        }

                        wl_data_device_send_selection(lDevice->resource(),dataOffer);
                    }
                }

                surface->client()->imp()->m_lastKeyboardEnterEventSerial = LWayland::nextSerial();
                wl_keyboard_send_enter(surface->client()->keyboardResource(),surface->client()->lastKeyboardKeyEventSerial(),surface->resource(),&m_keys);
                m_keyboardFocusSurface = surface;
                sendModifiersEvent();
            }
            else
                m_keyboardFocusSurface = nullptr;
        }

    }
    else
    {
        // If a surface has focus
        if(focusSurface() && focusSurface()->client()->keyboardResource())
        {
            focusSurface()->client()->imp()->m_lastKeyboardLeaveEventSerial = LWayland::nextSerial();
            wl_keyboard_send_leave(focusSurface()->client()->keyboardResource(),focusSurface()->client()->lastKeyboardLeaveEventSerial(),focusSurface()->resource());
        }
        m_keyboardFocusSurface = nullptr;
    }
}

void LKeyboard::sendKeyEvent(UInt32 keyCode, UInt32 keyState)
{
    // If no surface has focus
    if(!focusSurface())
        return;

    // If do not have a wl_keyboard
    if(!focusSurface()->client()->keyboardResource())
        return;

    focusSurface()->client()->imp()->m_lastKeyboardKeyEventSerial = LWayland::nextSerial();
    wl_keyboard_send_key(focusSurface()->client()->keyboardResource(),focusSurface()->client()->lastKeyboardKeyEventSerial(),LTime::ms(),keyCode,keyState);
}

void LKeyboard::sendModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group)
{
    // If no surface has focus
    if(!focusSurface())
        return;

    // If do not have a wl_keyboard
    if(!focusSurface()->client()->keyboardResource())
        return;

    focusSurface()->client()->imp()->m_lastKeyboardModifiersEventSerial = LWayland::nextSerial();
    wl_keyboard_send_modifiers(focusSurface()->client()->keyboardResource(),focusSurface()->client()->lastKeyboardModifiersEventSerial(),depressed,latched,locked,group);
}

void LKeyboard::sendModifiersEvent()
{
    sendModifiersEvent(m_modifiersState.depressed, m_modifiersState.latched, m_modifiersState.locked, m_modifiersState.group);
}

xkb_keysym_t LKeyboard::keySymbol(UInt32 keyCode)
{
    return xkb_state_key_get_one_sym(m_xkbKeymapState,keyCode+8);
}

void LKeyboard::updateModifiers()
{
    m_modifiersState.depressed = xkb_state_serialize_mods(m_xkbKeymapState, XKB_STATE_MODS_DEPRESSED);
    m_modifiersState.latched = xkb_state_serialize_mods(m_xkbKeymapState, XKB_STATE_MODS_LATCHED);
    m_modifiersState.locked = xkb_state_serialize_mods(m_xkbKeymapState, XKB_STATE_MODS_LOCKED);
    m_modifiersState.group = xkb_state_serialize_layout(m_xkbKeymapState, XKB_STATE_LAYOUT_EFFECTIVE);
    keyModifiersEvent(m_modifiersState.depressed,m_modifiersState.latched,m_modifiersState.locked,m_modifiersState.group);
}

#if LOUVRE_SEAT_VERSION >= 4

    Int32 LKeyboard::repeatRate() const
    {
        return m_repeatRate;
    }

    Int32 LKeyboard::repeatDelay() const
    {
        return m_repeatDelay;
    }

    void LKeyboard::setRepeatInfo(Int32 rate, Int32 msDelay)
    {
        m_repeatRate = rate;
        m_repeatDelay = msDelay;

        for(LClient *lClient : compositor()->clients())
            if(lClient->keyboardResource() && wl_resource_get_version(lClient->keyboardResource()) >= 4)
                wl_keyboard_send_repeat_info(lClient->keyboardResource(),rate,msDelay);

    }

#endif

/* VIRTUAL */

void LKeyboard::keyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group)
{
    sendModifiersEvent(depressed,latched,locked,group);
}

void LKeyboard::keyEvent(UInt32 keyCode, UInt32 keyState)
{
    xkb_keysym_t sym = keySymbol(keyCode);

    sendKeyEvent(keyCode,keyState);

    if(keyState == LIBINPUT_KEY_STATE_RELEASED)
    {
        // Ends compositor if ESC is pressed
        if(sym == XKB_KEY_F7)
        {
            LWayland::terminateDisplay();
            exit(0);
        }
        else if(sym == XKB_KEY_F1)
        {
            if (fork()==0)
            {
                setsid();
                system("weston-terminal");
                exit(0);
            }
        }
        else if(sym == XKB_KEY_F2)
        {
            if (fork()==0)
            {
                setsid();
                system("google-chrome --enable-features=UseOzonePlatform --ozone-platform=wayland");
                exit(0);
            }
        }
        else if(sym == XKB_KEY_F3)
        {
            if (fork()==0)
            {
                setsid();
                system("GDK_BACKEND=wayland gedit");
                exit(0);
            }
        }
        else if (sym == XKB_KEY_F4)
        {
            if (fork()==0)
            {
                setsid();
                system("GDK_BACKEND=wayland gnome-terminal");
                exit(0);
            }
        }
        else if (sym == XKB_KEY_XF86AudioLowerVolume)
        {
            if (fork()==0)
            {
                system("pactl -- set-sink-volume 0 -10%");
                exit(0);
            }
        }
        else if (sym == XKB_KEY_XF86AudioRaiseVolume)
        {
            if (fork()==0)
            {
                system("pactl -- set-sink-volume 0 +10%");
                exit(0);
            }
        }
        else if(sym == XKB_KEY_Escape)
        {
            if(focusSurface() && focusSurface()->toplevel() && focusSurface()->toplevel()->fullscreen())
                focusSurface()->toplevel()->configure(focusSurface()->toplevel()->state() & ~LToplevelRole::Fullscreen);
        }
        else if(sym == XKB_KEY_F8)
        {
            for(LSurface *surface : compositor()->surfaces())
                surface->setMinimized(false);

            compositor()->repaintAllOutputs();
        }
        else if(sym == XKB_KEY_F9)
        {
            compositor()->cursor()->output()->sc = true;
        }
    }
}
