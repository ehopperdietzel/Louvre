#include "WSurface.h"
#include <WClient.h>
#include <WCompositor.h>
#include <WBackend.h>
#include <WWayland.h>

#include <time.h>
#include <stdlib.h>
#include <string.h>

#include <xdg-shell.h>

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <WOutput.h>

#include <unistd.h>



#include <WTime.h>

using namespace Wpp;

PFNEGLQUERYWAYLANDBUFFERWL eglQueryWaylandBufferWL = NULL;

EGLint texture_format;

void get_egl_func()
{
     eglQueryWaylandBufferWL = (PFNEGLQUERYWAYLANDBUFFERWL) eglGetProcAddress ("eglQueryWaylandBufferWL");
}

WSurface::WSurface(wl_resource *surface, WClient *client, GLuint textureUnit)
{
    get_egl_func();
    p_texture = new WTexture(textureUnit);
    srand(time(NULL));
    p_resource = surface;
    p_client = client;
}

WPointer &WSurface::pointer()
{
    return p_pointer;
}

WKeyboard &WSurface::keyboard()
{
    return p_keyboard;
}

WSurface::~WSurface()
{
    delete []p_appId;
    delete []p_title;
}


void WSurface::sendConfigureToplevelEvent(Int32 width, Int32 height, SurfaceStateFlags states)
{
    p_pendingConfigure = true;
    p_pendingConfigureSize.setW(width);
    p_pendingConfigureSize.setH(height);
    p_pendingConfigureStates = states;
    //dispachLastConfiguration();
    //eventfd_write(compositor()->libinputFd,1);
    //compositor()->repaintAllOutputs();
}

void WSurface::sendConfigurePopupEvent(Int32 x, Int32 y, Int32 width, Int32 height)
{
    if(p_xdg_popup != nullptr)
        xdg_popup_send_configure(p_xdg_popup,x,y,width,height);
}

WSize WSurface::calculateResizeRect(const WPoint &cursorPosDelta, const WSize &initialSize, ResizeEdge edge)
{
    WSize newSize = initialSize;
    switch(edge)
    {
        case ResizeEdge::Bottom:
        {
            newSize.setH(initialSize.h() - cursorPosDelta.y());
        }break;
        case ResizeEdge::Right:
        {
            newSize.setW(initialSize.w() - cursorPosDelta.x());
        }break;
        case ResizeEdge::BottomRight:
        {
            newSize.setH(initialSize.h() - cursorPosDelta.y());
            newSize.setW(initialSize.w() - cursorPosDelta.x());
        }break;
        case ResizeEdge::Top:
        {
            newSize.setH(initialSize.h() + cursorPosDelta.y());
        }break;
        case ResizeEdge::Left:
        {
            newSize.setW(initialSize.w() + cursorPosDelta.x());
        }break;
        case ResizeEdge::TopLeft:
        {
            newSize.setH(initialSize.h() + cursorPosDelta.y());
            newSize.setW(initialSize.w() + cursorPosDelta.x());
        }break;
        case ResizeEdge::BottomLeft:
        {
            newSize.setH(initialSize.h() - cursorPosDelta.y());
            newSize.setW(initialSize.w() + cursorPosDelta.x());
        }break;
        case ResizeEdge::TopRight:
        {
            newSize.setH(initialSize.h() + cursorPosDelta.y());
            newSize.setW(initialSize.w() - cursorPosDelta.x());
        }break;
    }

    return newSize;
}

bool WSurface::inputRegionContainsPoint(const WPoint &surfacePos, const WPoint &point)
{
    return current.inputRegion.containsPoint(point-surfacePos);
}

void WSurface::setAppId(const char *appId)
{
    delete []p_appId;
    p_appId = new char[strlen(appId)+1];
    strcpy(p_appId,appId);
}

void WSurface::setTitle(const char *title)
{
    delete []p_title;
    p_title = new char[strlen(title)+1];
    strcpy(p_title,title);
}

void WSurface::dispachLastConfiguration()
{
    ack_configure = false;

    if(type() == SurfaceType::Toplevel)
    {
        if(p_xdg_toplevel != nullptr)
        {
            wl_array dummy;
            wl_array_init(&dummy);
            UInt32 index = 0;

            if((p_pendingConfigureStates & (SurfaceStateFlags)SurfaceState::Activated))
            {
                wl_array_add(&dummy, sizeof(xdg_toplevel_state));
                xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
                s[index] = XDG_TOPLEVEL_STATE_ACTIVATED;
                index++;
            }
            if(p_pendingConfigureStates & (SurfaceStateFlags)SurfaceState::Fullscreen)
            {
                wl_array_add(&dummy, sizeof(xdg_toplevel_state));
                xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
                s[index] = XDG_TOPLEVEL_STATE_FULLSCREEN;
                index++;
            }
            if(p_pendingConfigureStates & (SurfaceStateFlags)SurfaceState::Maximized)
            {
                wl_array_add(&dummy, sizeof(xdg_toplevel_state));
                xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
                s[index] = XDG_TOPLEVEL_STATE_MAXIMIZED;
                index++;
            }
            if(p_pendingConfigureStates & (SurfaceStateFlags)SurfaceState::Resizing)
            {
                wl_array_add(&dummy, sizeof(xdg_toplevel_state));
                xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
                s[index] = XDG_TOPLEVEL_STATE_RESIZING;
                index++;
            }

            if(wl_resource_get_version(p_xdg_toplevel) >= 2)
            {
                if(p_pendingConfigureStates & (SurfaceStateFlags)SurfaceState::TiledBottom)
                {
                    wl_array_add(&dummy, sizeof(xdg_toplevel_state));
                    xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
                    s[index] = XDG_TOPLEVEL_STATE_TILED_BOTTOM;
                    index++;
                }
                if(p_pendingConfigureStates & (SurfaceStateFlags)SurfaceState::TiledLeft)
                {
                    wl_array_add(&dummy, sizeof(xdg_toplevel_state));
                    xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
                    s[index] = XDG_TOPLEVEL_STATE_TILED_LEFT;
                    index++;
                }
                if(p_pendingConfigureStates & (SurfaceStateFlags)SurfaceState::TiledRight)
                {
                    wl_array_add(&dummy, sizeof(xdg_toplevel_state));
                    xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
                    s[index] = XDG_TOPLEVEL_STATE_TILED_RIGHT;
                    index++;
                }
                if(p_pendingConfigureStates & (SurfaceStateFlags)SurfaceState::TiledTop)
                {
                    wl_array_add(&dummy, sizeof(xdg_toplevel_state));
                    xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
                    s[index] = XDG_TOPLEVEL_STATE_TILED_TOP;
                    index++;
                }
            }

            xdg_toplevel_send_configure(p_xdg_toplevel,p_pendingConfigureSize.w(),p_pendingConfigureSize.h(),&dummy);
            wl_array_release(&dummy);
        }
    }

    if(p_xdg_shell != nullptr)
    {
        xdg_surface_send_configure(p_xdg_shell,configureSerial);
        //printf("CONFIGURE serial %i\n",configureSerial);
        configureSerial++;
    }

    p_pendingConfigure = false;

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
        p_texture->_format = texture_format;
        EGLAttrib attribs = EGL_NONE;
        EGLImage image = eglCreateImage(output->getDisplay(), EGL_NO_CONTEXT, EGL_WAYLAND_BUFFER_WL, current.buffer, &attribs);
        p_texture->setData(width, height, &image, WTexture::Type::EGL);
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

        //printf("STRIDE %i %i\n",wl_shm_buffer_get_stride(shm_buffer),width);

        if( format == WL_SHM_FORMAT_XRGB8888)
        {
            p_texture->_format = GL_BGR;
        }
        else if(format == WL_SHM_FORMAT_ARGB8888)
        {
            p_texture->_format = GL_BGRA;
        }
        else
        {
            printf("Texture format not supported.\n");
            exit(1);
        }

        p_texture->setData(width, height, data);
        wl_shm_buffer_end_access(shm_buffer);
    }


    wl_buffer_send_release(current.buffer);
    p_isDamaged = false;

    if(current.windowGeometry != pending.windowGeometry)
    {
        current.windowGeometry = pending.windowGeometry;
        geometryChangeRequest();
    }

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


