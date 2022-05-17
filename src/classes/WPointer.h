#ifndef WPOINTER_H
#define WPOINTER_H

#include <WNamespaces.h>

class Wpp::WPointer
{
public:
    WPointer(WSurface *surface);

    void sendButtonEvent(UInt32 buttonCode, UInt32 buttonState) const;
    void sendMotionEvent(Float64 x, Float64 y) const;
    void setFocus(Float64 x, Float64 y) const;
    void unsetFocus() const;
    bool hasFocus() const;
    WSeat *seat() const;

private:
    bool p_hasResource() const;
    WSurface *p_surface = nullptr;
    mutable wl_resource *p_resource;

};

#endif // WPOINTER_H
