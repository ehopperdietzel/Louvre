#ifndef LCLIENTPRIVATE_H
#define LCLIENTPRIVATE_H

#include <LClient.h>

class Louvre::LClient::LClientPrivate
{
public:
    LClientPrivate() = default;
    ~LClientPrivate() = default;

    std::list<LSurface*> m_surfaces;

    LCompositor         *m_compositor               = nullptr;
    LDataDevice         *m_dataDevice               = nullptr;
    wl_client           *m_client                   = nullptr;

    wl_resource         *m_seatResource             = nullptr;
    wl_resource         *m_pointerResource          = nullptr;
    wl_resource         *m_keyboardResource         = nullptr;
    wl_resource         *m_touchResource            = nullptr;
    wl_resource         *m_xdgWmBaseResource        = nullptr;

    LSurface            *m_pointerFocusedSurface    = nullptr;
    LSurface            *m_keyboardFocusedSurface   = nullptr;
    LSurface            *m_touchFocusedSurface      = nullptr;

    UInt32               m_lastPointerEnterEventSerial      = 0;
    UInt32               m_lastPointerLeaveEventSerial      = 0;
    UInt32               m_lastPointerButtonEventSerial     = 0;

    UInt32               m_lastKeyboardEnterEventSerial     = 0;
    UInt32               m_lastKeyboardLeaveEventSerial     = 0;
    UInt32               m_lastKeyboardKeyEventSerial       = 0;
    UInt32               m_lastKeyboardModifiersEventSerial = 0;

    UInt32 m_id;

};

#endif // LCLIENTPRIVATE_H
