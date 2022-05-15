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

using namespace Wpp;

wl_array nullKeys;

PFNEGLQUERYWAYLANDBUFFERWL eglQueryWaylandBufferWL = NULL;

EGLint texture_format;

void get_egl_func()
{
     eglQueryWaylandBufferWL = (PFNEGLQUERYWAYLANDBUFFERWL) eglGetProcAddress ("eglQueryWaylandBufferWL");
}


void createNullKeys()
{
    wl_array_init(&nullKeys);
}

WSurface::WSurface(wl_resource *surface, WClient *client, GLuint textureUnit)
{
    get_egl_func();
    _texture = new WTexture(textureUnit);
    srand(time(NULL));
    p_resource = surface;
    p_client = client;
}

WSurface::~WSurface()
{
    delete []_appId;
    delete []_title;
}

void WSurface::sendPointerButtonEvent(UInt32 buttonCode, UInt32 buttonState, UInt32 milliseconds)
{
    if(p_client->p_pointerResource != nullptr)
    {
        wl_pointer_send_button(p_client->p_pointerResource,pointerSerial,milliseconds,buttonCode,buttonState);
        pointerSerial++;
        if(wl_resource_get_version(p_client->p_pointerResource) >= 5)
            wl_pointer_send_frame(p_client->p_pointerResource);
    }
}

void WSurface::sendPointerMotionEvent(double x, double y, UInt32 milliseconds)
{
    if(p_client->p_pointerResource != nullptr)
    {
        wl_pointer_send_motion(
                    p_client->p_pointerResource,
                    milliseconds,
                    wl_fixed_from_double(x),
                    wl_fixed_from_double(y));

        if(wl_resource_get_version(p_client->p_pointerResource) >= 5)
            wl_pointer_send_frame(p_client->p_pointerResource);

    }
}

void WSurface::sendPointerEnterEvent(double x, double y)
{
    if(p_client->p_pointerResource != nullptr)
    {
        wl_pointer_send_enter(
                    p_client->p_pointerResource,
                    pointerSerial,
                    p_resource,
                    wl_fixed_from_double(x),
                    wl_fixed_from_double(y));
        pointerSerial++;
        if(wl_resource_get_version(p_client->p_pointerResource) >= 5)
            wl_pointer_send_frame(p_client->p_pointerResource);
    }
}

void WSurface::sendPointerLeaveEvent()
{
    if(p_client->p_pointerResource != nullptr)
    {
        wl_pointer_send_leave(p_client->p_pointerResource,pointerSerial,p_resource);
        pointerSerial++;
        if(wl_resource_get_version(p_client->p_pointerResource) >= 5)
            wl_pointer_send_frame(p_client->p_pointerResource);

    }
}

void WSurface::sendKeyEvent(UInt32 keyCode, UInt32 keyState)
{
    if(p_client->p_keyboardResource != nullptr)
    {
        wl_keyboard_send_key(p_client->p_keyboardResource,keyboardSerial,compositor()->getMilliseconds(),keyCode,keyState);
        keyboardSerial++;        
    }
}

void WSurface::sendKeyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group)
{
    if(p_client->p_keyboardResource != nullptr)
    {
        wl_keyboard_send_modifiers(p_client->p_keyboardResource,keyboardSerial,depressed,latched,locked,group);
        keyboardSerial++;
    }
}

void WSurface::sendKeyboardEnterEvent()
{
    if(p_client->p_keyboardResource != nullptr)
    {
        wl_keyboard_send_enter(p_client->p_keyboardResource,keyboardSerial,p_resource,&nullKeys);
        keyboardSerial++;
    }
}

void WSurface::sendKeyboardLeaveEvent()
{
    if(p_client->p_keyboardResource != nullptr)
    {
        wl_keyboard_send_leave(p_client->p_keyboardResource,keyboardSerial,p_resource);
        keyboardSerial++;
    }
}

void WSurface::sendConfigureToplevelEvent(Int32 width, Int32 height, SurfaceStateFlags states)
{
    pendingConfigure = true;
    pendingConfigureSize.setW(width);
    pendingConfigureSize.setH(height);
    pendingConfigureStates = states;
    //dispachLastConfiguration();
    //eventfd_write(compositor()->libinputFd,1);
    //compositor()->repaintAllOutputs();
}

void WSurface::sendConfigurePopupEvent(Int32 x, Int32 y, Int32 width, Int32 height)
{
    if(xdg_popup != nullptr)
        xdg_popup_send_configure(xdg_popup,x,y,width,height);
}

WSize WSurface::calculateResizeRect(WPoint cursorPosDelta, WSize initialSize, ResizeEdge edge)
{
    switch(edge)
    {
        case ResizeEdge::Bottom:
        {
            initialSize.setH(initialSize.h() - cursorPosDelta.y());
        }break;
        case ResizeEdge::Right:
        {
            initialSize.setW(initialSize.w() - cursorPosDelta.x());
        }break;
        case ResizeEdge::BottomRight:
        {
            initialSize.setH(initialSize.h() - cursorPosDelta.y());
            initialSize.setW(initialSize.w() - cursorPosDelta.x());
        }break;
        case ResizeEdge::Top:
        {
            initialSize.setH(initialSize.h() + cursorPosDelta.y());
        }break;
        case ResizeEdge::Left:
        {
            initialSize.setW(initialSize.w() + cursorPosDelta.x());
        }break;
        case ResizeEdge::TopLeft:
        {
            initialSize.setH(initialSize.h() + cursorPosDelta.y());
            initialSize.setW(initialSize.w() + cursorPosDelta.x());
        }break;
        case ResizeEdge::BottomLeft:
        {
            initialSize.setH(initialSize.h() - cursorPosDelta.y());
            initialSize.setW(initialSize.w() + cursorPosDelta.x());
        }break;
        case ResizeEdge::TopRight:
        {
            initialSize.setH(initialSize.h() + cursorPosDelta.y());
            initialSize.setW(initialSize.w() - cursorPosDelta.x());
        }break;
    }

    return initialSize;
}

void WSurface::setAppId(const char *appId)
{
    delete []_appId;
    _appId = new char[strlen(appId)+1];
    strcpy(_appId,appId);
}

void WSurface::setTitle(const char *title)
{
    delete []_title;
    _title = new char[strlen(title)+1];
    strcpy(_title,title);
}

void WSurface::dispachLastConfiguration()
{
    ack_configure = false;

    if(type() == SurfaceType::Toplevel)
    {
        if(xdg_toplevel != nullptr)
        {
            wl_array dummy;
            wl_array_init(&dummy);
            UInt32 index = 0;

            if((pendingConfigureStates & (SurfaceStateFlags)SurfaceState::Activated))
            {
                wl_array_add(&dummy, sizeof(xdg_toplevel_state));
                xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
                s[index] = XDG_TOPLEVEL_STATE_ACTIVATED;
                index++;
            }
            if(pendingConfigureStates & (SurfaceStateFlags)SurfaceState::Fullscreen)
            {
                wl_array_add(&dummy, sizeof(xdg_toplevel_state));
                xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
                s[index] = XDG_TOPLEVEL_STATE_FULLSCREEN;
                index++;
            }
            if(pendingConfigureStates & (SurfaceStateFlags)SurfaceState::Maximized)
            {
                wl_array_add(&dummy, sizeof(xdg_toplevel_state));
                xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
                s[index] = XDG_TOPLEVEL_STATE_MAXIMIZED;
                index++;
            }
            if(pendingConfigureStates & (SurfaceStateFlags)SurfaceState::Resizing)
            {
                wl_array_add(&dummy, sizeof(xdg_toplevel_state));
                xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
                s[index] = XDG_TOPLEVEL_STATE_RESIZING;
                index++;
            }

            if(wl_resource_get_version(xdg_toplevel) >= 2)
            {
                if(pendingConfigureStates & (SurfaceStateFlags)SurfaceState::TiledBottom)
                {
                    wl_array_add(&dummy, sizeof(xdg_toplevel_state));
                    xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
                    s[index] = XDG_TOPLEVEL_STATE_TILED_BOTTOM;
                    index++;
                }
                if(pendingConfigureStates & (SurfaceStateFlags)SurfaceState::TiledLeft)
                {
                    wl_array_add(&dummy, sizeof(xdg_toplevel_state));
                    xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
                    s[index] = XDG_TOPLEVEL_STATE_TILED_LEFT;
                    index++;
                }
                if(pendingConfigureStates & (SurfaceStateFlags)SurfaceState::TiledRight)
                {
                    wl_array_add(&dummy, sizeof(xdg_toplevel_state));
                    xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
                    s[index] = XDG_TOPLEVEL_STATE_TILED_RIGHT;
                    index++;
                }
                if(pendingConfigureStates & (SurfaceStateFlags)SurfaceState::TiledTop)
                {
                    wl_array_add(&dummy, sizeof(xdg_toplevel_state));
                    xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
                    s[index] = XDG_TOPLEVEL_STATE_TILED_TOP;
                    index++;
                }
            }

            xdg_toplevel_send_configure(xdg_toplevel,pendingConfigureSize.w(),pendingConfigureSize.h(),&dummy);
            wl_array_release(&dummy);
        }
    }

    if(xdg_shell != nullptr)
    {
        xdg_surface_send_configure(xdg_shell,configureSerial);
        //printf("CONFIGURE serial %i\n",configureSerial);
        configureSerial++;
    }

    pendingConfigure = false;

}
void WSurface::applyDamages()
{

    if(!_isDamaged || resource() == nullptr)
        return;

    WOutput *output = compositor()->outputs().front();
    Int32 width, height;
    WSize prevSize = size();

    if(eglQueryWaylandBufferWL(output->getDisplay(), current.buffer, EGL_TEXTURE_FORMAT, &texture_format))
    {
        //printf("EGL buffer\n");

        eglQueryWaylandBufferWL(output->getDisplay(), current.buffer, EGL_WIDTH, &width);
        eglQueryWaylandBufferWL(output->getDisplay(), current.buffer, EGL_HEIGHT, &height);
        _texture->_format = texture_format;
        EGLAttrib attribs = EGL_NONE;
        EGLImage image = eglCreateImage(output->getDisplay(), EGL_NO_CONTEXT, EGL_WAYLAND_BUFFER_WL, current.buffer, &attribs);
        _texture->setData(width, height, &image, WTexture::Type::EGL);
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
            _texture->_format = GL_BGR;
        }
        else if(format == WL_SHM_FORMAT_ARGB8888)
        {
            _texture->_format = GL_BGRA;
        }
        else
        {
            printf("Texture format not supported.\n");
            exit(1);
        }

        _texture->setData(width, height, data);
        wl_shm_buffer_end_access(shm_buffer);
    }


    wl_buffer_send_release(current.buffer);
    _isDamaged = false;

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
    if(frameCallback)
    {
        wl_callback_send_done(frameCallback,compositor()->getMilliseconds());
        wl_resource_destroy(frameCallback);
        frameCallback = nullptr;
    }
}

void WSurface::setBufferScale(Int32 scale)
{
    _bufferScale = scale;
}

WCompositor *WSurface::compositor()
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
Wpp::WSurface *WSurface::topParent()
{
    if(parent() == nullptr)
        return nullptr;

    return findTopmostParent(parent());
}


