#ifndef WKEYBOARD_H
#define WKEYBOARD_H

#include <WNamespaces.h>

class Wpp::WKeyboard
{
public:
    WKeyboard(WSurface *surface);
    ~WKeyboard();

    // Events
    void sendKeyEvent(UInt32 keyCode, UInt32 keyState) const;
    void sendKeyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group) const;
    void setFocus();
    void unsetFocus() const;

    bool hasFocus() const;
    WSeat *seat() const;
    WSurface *surface() const;

private:
    bool p_hasResource() const;
    WSurface *p_surface = nullptr;
    mutable wl_resource *p_resource;
    wl_array p_nullKeys;
};

#endif // WKEYBOARD_H
