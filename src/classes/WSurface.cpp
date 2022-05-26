#include "WSurface.h"
#include <WClient.h>
#include <WCompositor.h>
#include <WBackend.h>
#include <WWayland.h>
#include <WSeat.h>

#include <time.h>
#include <stdlib.h>

#include <xdg-shell.h>

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <WOutput.h>

#include <unistd.h>
#include <WToplevelRole.h>


#include <WTime.h>

using namespace Wpp;

PFNEGLQUERYWAYLANDBUFFERWL eglQueryWaylandBufferWL = NULL;

EGLint texture_format;

void get_egl_func()
{
     eglQueryWaylandBufferWL = (PFNEGLQUERYWAYLANDBUFFERWL) eglGetProcAddress ("eglQueryWaylandBufferWL");
}


void WSurface::bufferSizeChangeRequest()
{
    if(type() == Toplevel && toplevel() == seat()->resizingToplevel())
        seat()->updateResizingToplevelPos();
}

WToplevelRole *WSurface::toplevel() const
{
    return p_toplevelRole;
}

WSurface::WSurface(wl_resource *surface, WClient *client, GLuint textureUnit)
{
    get_egl_func();
    p_texture = new WTexture(textureUnit);
    srand(time(NULL));
    p_resource = surface;
    p_client = client;
}

const WPoint &WSurface::pos(bool getRolePos) const
{
    if(!getRolePos)
        return p_pos;

    if(type() == Toplevel)
    {
        p_xdgPos = p_pos - toplevel()->windowGeometry().topLeft();
        return p_xdgPos;
    }

    //if(type() == Popup)
        //return p_pos - toplevel()->windowGeometry().topLeft();
    
}

void WSurface::setPos(const WPoint &newPos)
{
    p_pos = newPos;
}

void WSurface::setPos(Int32 x, Int32 y)
{
    setX(x);
    setY(y);
}

void WSurface::setX(Int32 x)
{
    p_pos.setX(x);
}

void WSurface::setY(Int32 y, bool useRolePos)
{
    if(!useRolePos)
        p_pos.setY(y);
    else
    {
        if(type() == Toplevel)
            p_pos.setY(p_pos.y() + toplevel()->windowGeometry().y());
    }
}

void WSurface::setMinimized(bool state)
{
    p_minimized = state;
}

bool WSurface::minimized() const
{
    return p_minimized;
}

WSurface::~WSurface()
{

}


void WSurface::sendConfigurePopupEvent(Int32 x, Int32 y, Int32 width, Int32 height)
{
    if(p_xdg_popup != nullptr)
        xdg_popup_send_configure(p_xdg_popup,x,y,width,height);
}

WSeat *WSurface::seat() const
{
    return compositor()->seat();
}


bool WSurface::inputRegionContainsPoint(const WPoint &surfacePos, const WPoint &point)
{
    return current.inputRegion.containsPoint(point-surfacePos);
}

WPoint WSurface::mapToLocal(const WPoint &point)
{
    return point - pos();
}


void WSurface::applyDamages()
{

    if(!p_isDamaged || resource() == nullptr)
        return;

    WOutput *output = compositor()->outputs().front();
    Int32 width, height;
    WSize prevSize = size();

    if(eglQueryWaylandBufferWL(output->getDisplay(), current.buffer, EGL_TEXTURE_FORMAT, &texture_format))
    {
        //printf("EGL buffer\n");

        eglQueryWaylandBufferWL(output->getDisplay(), current.buffer, EGL_WIDTH, &width);
        eglQueryWaylandBufferWL(output->getDisplay(), current.buffer, EGL_HEIGHT, &height);
        EGLAttrib attribs = EGL_NONE;
        EGLImage image = eglCreateImage(output->getDisplay(), EGL_NO_CONTEXT, EGL_WAYLAND_BUFFER_WL, current.buffer, &attribs);
        p_texture->setData(width, height, &image, texture_format, GL_UNSIGNED_BYTE, WTexture::BufferType::EGL);
        eglDestroyImage (output->getDisplay(), image);
    }
    else
    {
        //printf("SHM buffer\n");
        wl_shm_buffer *shm_buffer = wl_shm_buffer_get(current.buffer);
        wl_shm_buffer_begin_access(shm_buffer);
        width = wl_shm_buffer_get_width(shm_buffer);
        height = wl_shm_buffer_get_height(shm_buffer);
        void *data = wl_shm_buffer_get_data(shm_buffer);
        UInt32 format =  wl_shm_buffer_get_format(shm_buffer);

        GLenum bufferFormat;

        //printf("STRIDE %i %i\n",wl_shm_buffer_get_stride(shm_buffer),width);

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
            exit(1);
        }

        p_texture->setData(width, height, data, bufferFormat, GL_UNSIGNED_BYTE);
        wl_shm_buffer_end_access(shm_buffer);
    }


    wl_buffer_send_release(current.buffer);
    p_isDamaged = false;


    if(prevSize != size())
        bufferSizeChangeRequest();


    //wl_client_flush(_client->_client);
}

void WSurface::requestNextFrame()
{
    if(p_frameCallback)
    {
        wl_callback_send_done(p_frameCallback,WTime::ms());
        wl_resource_destroy(p_frameCallback);
        p_frameCallback = nullptr;
    }
}

void WSurface::setBufferScale(Int32 scale)
{
    p_bufferScale = scale;
}

WCompositor *WSurface::compositor() const
{
    if(p_client != nullptr)
        return p_client->compositor();
    else
        return nullptr;
}

WSurface *findTopmostParent(WSurface *surface)
{
    if(surface->parent() == nullptr)
        return surface;

    return findTopmostParent(surface->parent());
}
Wpp::WSurface *WSurface::topParent() const
{
    if(parent() == nullptr)
        return nullptr;

    return findTopmostParent(parent());
}


