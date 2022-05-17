#include "WKeyboard.h"
#include <WSurface.h>
#include <WSeat.h>
#include <WClient.h>
#include <WCompositor.h>
#include <WWayland.h>
#include <WTime.h>

using namespace Wpp;

Wpp::WKeyboard::WKeyboard(WSurface *surface)
{
    p_surface = surface;
    wl_array_init(&p_nullKeys);
}

WKeyboard::~WKeyboard()
{
    wl_array_release(&p_nullKeys);
}

void Wpp::WKeyboard::sendKeyEvent(UInt32 keyCode, UInt32 keyState) const
{
    if(!hasFocus() || !p_hasResource()) return;

    wl_keyboard_send_key(p_resource,WWayland::nextSerial(),WTime::ms(),keyCode,keyState);
}

void Wpp::WKeyboard::sendKeyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group) const
{
    if(!hasFocus() || !p_hasResource()) return;

    wl_keyboard_send_modifiers(p_resource,WWayland::nextSerial(),depressed,latched,locked,group);
}

void Wpp::WKeyboard::setFocus()
{
    if(hasFocus()) return;

    if(p_hasResource())
    {
        surface()->compositor()->seat()->p_keyboardFocusSurface = surface();

        wl_keyboard_send_enter(p_resource,WWayland::nextSerial(),surface()->resource(),&p_nullKeys);
    }
    else
        surface()->compositor()->seat()->p_keyboardFocusSurface = nullptr;
}

void Wpp::WKeyboard::unsetFocus() const
{
    if(!hasFocus()) return;

    if(p_hasResource())
       wl_keyboard_send_leave(p_resource,WWayland::nextSerial(),surface()->resource());

    surface()->compositor()->seat()->p_keyboardFocusSurface = nullptr;
}

bool Wpp::WKeyboard::hasFocus() const
{
    return seat()->keyboardFocus() == surface();
}

WSeat *Wpp::WKeyboard::seat() const
{
    return surface()->compositor()->seat();
}

WSurface *WKeyboard::surface() const
{
    return p_surface;
}

bool WKeyboard::p_hasResource() const
{
    return ((p_resource = surface()->client()->keyboardResource()) != nullptr);
}


