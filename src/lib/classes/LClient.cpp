#include "LClient.h"
#include <LClientPrivate.h>
#include <LCompositor.h>

using namespace Louvre;

LClient::LClient(wl_client *client, LCompositor *compositor)
{
    m_imp = new LClientPrivate();
    m_imp->m_client = client;
    m_imp->m_compositor = compositor;
}

LClient::~LClient()
{
    delete m_imp;
    printf("CLIENT DESTROYED.\n");
}

LCompositor *LClient::compositor() const
{
    return m_imp->m_compositor;
}

LSeat *LClient::seat() const
{
    return m_imp->m_compositor->seat();
}

wl_client *LClient::client() const
{
    return m_imp->m_client;
}

UInt32 LClient::id() const
{
    return m_imp->m_id;
}

LDataDevice *LClient::dataDevice() const
{
    return m_imp->m_dataDevice;
}

const list<LSurface *> &LClient::surfaces() const
{
    return m_imp->m_surfaces;
}

wl_resource *LClient::seatResource() const
{
    return m_imp->m_seatResource;
}

wl_resource *LClient::pointerResource() const
{
    return m_imp->m_pointerResource;
}

wl_resource *LClient::keyboardResource() const
{
    return m_imp->m_keyboardResource;
}

wl_resource *LClient::touchResource() const
{
    return m_imp->m_touchResource;
}

UInt32 LClient::lastPointerEnterEventSerial() const
{
    return m_imp->m_lastPointerEnterEventSerial;
}

UInt32 LClient::lastPointerLeaveEventSerial() const
{
    return m_imp->m_lastPointerLeaveEventSerial;
}

UInt32 LClient::lastPointerButtonEventSerial() const
{
    return m_imp->m_lastPointerButtonEventSerial;
}

UInt32 LClient::lastKeyboardEnterEventSerial() const
{
    return m_imp->m_lastKeyboardEnterEventSerial;
}

UInt32 LClient::lastKeyboardLeaveEventSerial() const
{
    return m_imp->m_lastKeyboardLeaveEventSerial;
}

UInt32 LClient::lastKeyboardKeyEventSerial() const
{
    return m_imp->m_lastKeyboardKeyEventSerial;
}

UInt32 LClient::lastKeyboardModifiersEventSerial() const
{
    return m_imp->m_lastKeyboardModifiersEventSerial;
}

LClient::LClientPrivate *LClient::imp() const
{
    return m_imp;
}

wl_resource *LClient::xdgWmBaseResource() const
{
    return m_imp->m_xdgWmBaseResource;
}

