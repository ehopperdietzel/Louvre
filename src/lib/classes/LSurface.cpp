#include <LSurfacePrivate.h>
#include <LClient.h>
#include <LCompositor.h>
#include <LWayland.h>
#include <LSeat.h>

#include <time.h>
#include <stdlib.h>

#include <xdg-shell.h>

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <LOutput.h>

#include <unistd.h>
#include <LToplevelRole.h>
#include <LPopupRole.h>
#include <LSubsurfaceRole.h>
#include <LPointer.h>


#include <LTime.h>

using namespace Louvre;

PFNEGLQUERYWAYLANDBUFFERWL eglQueryWaylandBufferWL = NULL;

LCursorRole *LSurface::cursor() const
{
    if(roleType() == Cursor)
        return (LCursorRole*)m_imp->m_role;
    else
        return nullptr;
}

LToplevelRole *LSurface::toplevel() const
{
    if(roleType() == Toplevel)
        return (LToplevelRole*)m_imp->m_role;
    else
        return nullptr;
}

LPopupRole *LSurface::popup() const
{
    if(roleType() == Popup)
        return (LPopupRole*)m_imp->m_role;
    else
        return nullptr;
}

LSubsurfaceRole *LSurface::subsurface() const
{
    if(roleType() == Subsurface)
        return (LSubsurfaceRole*)m_imp->m_role;
    else
        return nullptr;
}

LBaseSurfaceRole *LSurface::role() const
{
    return m_imp->m_role;
}

LSurface::LSurface(wl_resource *surface, LClient *client, GLuint textureUnit)
{
    m_imp = new LSurfacePrivate();
    eglQueryWaylandBufferWL = (PFNEGLQUERYWAYLANDBUFFERWL) eglGetProcAddress ("eglQueryWaylandBufferWL");
    m_imp->m_texture = new LTexture(textureUnit);
    srand(time(NULL));
    m_imp->m_resource = surface;
    m_imp->m_client = client;
}

LSurface::~LSurface()
{
    delete m_imp->m_texture;
    delete m_imp;
}

void LSurface::roleChanged()
{
    if(toplevel() && parent())
    {
        LSurface *parent = topParent();
        setPos(parent->pos() + parent->toplevel()->windowGeometry().bottomRight()/2 - toplevel()->windowGeometry().bottomRight()/2);
    }
}

void LSurface::parentChanged()
{

}

void LSurface::bufferScaleChanged()
{

}

void LSurface::bufferSizeChanged()
{
    if(roleType() == Toplevel && toplevel() == seat()->pointer()->resizingToplevel())
        seat()->pointer()->updateResizingToplevelPos();
}

void LSurface::opaqueRegionChanged()
{

}

void LSurface::inputRegionChanged()
{

}

const LPoint &LSurface::pos(bool useRolePos) const
{
    if(useRolePos && roleType() != Undefined)
        return role()->rolePos();

    return m_imp->m_pos;
}

void LSurface::setPos(const LPoint &newPos)
{
    m_imp->m_pos = newPos;
}

void LSurface::setPos(Int32 x, Int32 y)
{
    setX(x);
    setY(y);
}

void LSurface::setX(Int32 x)
{
    m_imp->m_pos.setX(x);
}

void LSurface::setY(Int32 y, bool useRolePos)
{
    if(!useRolePos)
        m_imp->m_pos.setY(y);
    else
    {
        if(roleType() == Toplevel)
            m_imp->m_pos.setY(m_imp->m_pos.y() + toplevel()->windowGeometry().y());
    }
}

const LSize &LSurface::size(bool useBufferSize) const
{
    if(useBufferSize)
        return texture()->size();
    return m_imp->current.size;
}

const LRegion &LSurface::inputRegion() const
{
    return m_imp->current.inputRegion;
}

const LRegion &LSurface::opaqueRegion() const
{
    return m_imp->current.opaqueRegion;
}

const LRegion &LSurface::translucentRegion() const
{
    return m_imp->m_translucentRegion;
}

const LRegion &LSurface::damages() const
{
    return m_imp->m_currentDamages;
}

void LSurface::setMinimized(bool state)
{
    m_imp->m_minimized = state;
}

Int32 LSurface::bufferScale() const
{
    return m_imp->m_bufferScale;
}

LTexture *LSurface::texture() const
{
    return m_imp->m_texture;
}

bool LSurface::isDamaged() const
{
    return m_imp->m_isDamaged;
}

bool LSurface::minimized() const
{
    return m_imp->m_minimized;
}

LSeat *LSurface::seat() const
{
    return compositor()->seat();
}

UInt32 LSurface::roleType() const
{
    if(role())
        return role()->roleId();
    else
        return Undefined;
}


bool LSurface::inputRegionContainsPoint(const LPoint &surfacePos, const LPoint &point)
{
    return m_imp->current.inputRegion.containsPoint(point-surfacePos);
}

LPoint LSurface::mapToLocal(const LPoint &point)
{
    return point - pos();
}

bool LSurface::textureChanged() const
{
    return m_imp->m_textureChanged;
}

void LSurface::requestNextFrame()
{
    m_imp->m_textureChanged = false;
    if(m_imp->m_frameCallback)
    {
        wl_callback_send_done(m_imp->m_frameCallback,LTime::ms());
        wl_resource_destroy(m_imp->m_frameCallback);
        m_imp->m_frameCallback = nullptr;
        wl_client_flush(client()->client());
    }
}

wl_resource *LSurface::resource() const
{
    return m_imp->m_resource;
}

wl_resource *LSurface::xdgSurfaceResource() const
{
    return m_imp->m_xdgSurfaceResource;
}

LClient *LSurface::client() const
{
    return m_imp->m_client;
}

LCompositor *LSurface::compositor() const
{
    if(m_imp->m_client != nullptr)
        return m_imp->m_client->compositor();
    else
        return nullptr;
}

Louvre::LSurface *LSurface::parent() const
{
    return m_imp->m_parent;
}

LSurface *findTopmostParent(LSurface *surface)
{
    if(surface->parent() == nullptr)
        return surface;

    return findTopmostParent(surface->parent());
}
Louvre::LSurface *LSurface::topParent() const
{
    if(parent() == nullptr)
        return nullptr;

    return findTopmostParent(parent());
}

const list<Louvre::LSurface *> &LSurface::children() const
{
    return m_imp->m_children;
}

LSurface::LSurfacePrivate *LSurface::imp() const
{
    return m_imp;
}

// Private

void LSurface::LSurfacePrivate::m_bufferToTexture()
{

    if(!m_isDamaged || !m_resource)
        return;

    //LOutput *output = compositor()->outputs().front();
    Int32 width, height;
    EGLint texture_format;


    // EGL
    if(eglQueryWaylandBufferWL(LWayland::eglDisplay(), current.buffer, EGL_TEXTURE_FORMAT, &texture_format))
    {
        eglQueryWaylandBufferWL(LWayland::eglDisplay(), current.buffer, EGL_WIDTH, &width);
        eglQueryWaylandBufferWL(LWayland::eglDisplay(), current.buffer, EGL_HEIGHT, &height);
        EGLAttrib attribs = EGL_NONE;
        EGLImage image = eglCreateImage(LWayland::eglDisplay(), EGL_NO_CONTEXT, EGL_WAYLAND_BUFFER_WL, current.buffer, &attribs);
        m_texture->setData(width, height, &image, texture_format, GL_UNSIGNED_BYTE, LTexture::BufferType::EGL);
        eglDestroyImage(LWayland::eglDisplay(), image);
    }
    // SHM
    else
    {
        wl_shm_buffer *shm_buffer = wl_shm_buffer_get(current.buffer);
        wl_shm_buffer_begin_access(shm_buffer);
        width = wl_shm_buffer_get_width(shm_buffer);
        height = wl_shm_buffer_get_height(shm_buffer);
        void *data = wl_shm_buffer_get_data(shm_buffer);
        UInt32 format =  wl_shm_buffer_get_format(shm_buffer);

        GLenum bufferFormat;

        if( format == WL_SHM_FORMAT_XRGB8888)
        {
            bufferFormat = GL_BGR;
        }
        else if(format == WL_SHM_FORMAT_ARGB8888)
        {
            bufferFormat = GL_BGRA;
        }
        else
        {
            printf("Texture format not supported.\n");
            wl_shm_buffer_end_access(shm_buffer);
            wl_client_destroy(m_client->client());
            return;
        }

        m_texture->setData(width, height, data, bufferFormat, GL_UNSIGNED_BYTE);

        wl_shm_buffer_end_access(shm_buffer);

    }

    wl_buffer_send_release(current.buffer);
    current.buffer = nullptr;
    m_isDamaged = false;
}
