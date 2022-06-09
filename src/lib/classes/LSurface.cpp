#include "LSurface.h"
#include <LClient.h>
#include <LCompositor.h>
#include <LBackend.h>
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
        return (LCursorRole*)p_role;
    else
        return nullptr;
}

LToplevelRole *LSurface::toplevel() const
{
    if(roleType() == Toplevel)
        return (LToplevelRole*)p_role;
    else
        return nullptr;
}

LPopupRole *LSurface::popup() const
{
    if(roleType() == Popup)
        return (LPopupRole*)p_role;
    else
        return nullptr;
}

LSubsurfaceRole *LSurface::subsurface() const
{
    if(roleType() == Subsurface)
        return (LSubsurfaceRole*)p_role;
    else
        return nullptr;
}

LBaseSurfaceRole *LSurface::role() const
{
    return p_role;
}

LSurface::LSurface(wl_resource *surface, LClient *client, GLuint textureUnit)
{
    eglQueryWaylandBufferWL = (PFNEGLQUERYWAYLANDBUFFERWL) eglGetProcAddress ("eglQueryWaylandBufferWL");
    p_texture = new LTexture(textureUnit);
    srand(time(NULL));
    p_resource = surface;
    p_client = client;
}

LSurface::~LSurface()
{
    delete p_texture;
}

void LSurface::bufferSizeChangeRequest()
{
    if(roleType() == Toplevel && toplevel() == seat()->pointer()->resizingToplevel())
        seat()->pointer()->updateResizingToplevelPos();
}

const LPoint &LSurface::pos(bool useRolePos) const
{
    if(useRolePos && roleType() != Undefined)
        return role()->rolePos();

    return p_pos;
}

void LSurface::setPos(const LPoint &newPos)
{
    p_pos = newPos;
}

void LSurface::setPos(Int32 x, Int32 y)
{
    setX(x);
    setY(y);
}

void LSurface::setX(Int32 x)
{
    p_pos.setX(x);
}

void LSurface::setY(Int32 y, bool useRolePos)
{
    if(!useRolePos)
        p_pos.setY(y);
    else
    {
        if(roleType() == Toplevel)
            p_pos.setY(p_pos.y() + toplevel()->windowGeometry().y());
    }
}

const LSize &LSurface::size(bool useBufferSize) const
{
    if(useBufferSize)
        return texture()->size();
    return current.size;
}

void LSurface::setMinimized(bool state)
{
    p_minimized = state;
}

bool LSurface::minimized() const
{
    return p_minimized;
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
    return current.inputRegion.containsPoint(point-surfacePos);
}

LPoint LSurface::mapToLocal(const LPoint &point)
{
    return point - pos();
}


void LSurface::applyDamages()
{

    if(!p_isDamaged || resource() == nullptr)
        return;

    LOutput *output = compositor()->outputs().front();
    Int32 width, height;
    EGLint texture_format;

    // EGL
    if(eglQueryWaylandBufferWL(output->getDisplay(), current.buffer, EGL_TEXTURE_FORMAT, &texture_format))
    {
        eglQueryWaylandBufferWL(output->getDisplay(), current.buffer, EGL_WIDTH, &width);
        eglQueryWaylandBufferWL(output->getDisplay(), current.buffer, EGL_HEIGHT, &height);
        EGLAttrib attribs = EGL_NONE;
        EGLImage image = eglCreateImage(output->getDisplay(), EGL_NO_CONTEXT, EGL_WAYLAND_BUFFER_WL, current.buffer, &attribs);
        p_texture->setData(width, height, &image, texture_format, GL_UNSIGNED_BYTE, LTexture::BufferType::EGL);
        eglDestroyImage (output->getDisplay(), image);
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
            wl_client_destroy(client()->client());
            return;
        }

        p_texture->setData(width, height, data, bufferFormat, GL_UNSIGNED_BYTE);

        wl_shm_buffer_end_access(shm_buffer);

    }

    wl_buffer_send_release(current.buffer);
    current.buffer = nullptr;
    p_isDamaged = false;
}

void LSurface::requestNextFrame()
{
    if(p_frameCallback)
    {
        wl_callback_send_done(p_frameCallback,LTime::ms());
        wl_resource_destroy(p_frameCallback);
        p_frameCallback = nullptr;
    }
}

wl_resource *LSurface::xdgSurfaceResource() const
{
    return p_xdgSurfaceResource;
}

void LSurface::setBufferScale(Int32 scale)
{
    p_bufferScale = scale;
}

LCompositor *LSurface::compositor() const
{
    if(p_client != nullptr)
        return p_client->compositor();
    else
        return nullptr;
}

Louvre::LSurface *LSurface::parent() const
{
    return p_parent;
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
    return p_children;
}


