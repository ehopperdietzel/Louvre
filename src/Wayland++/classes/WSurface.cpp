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

using namespace WaylandPlus;

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

WSurface::WSurface(UInt32 id, wl_resource *res, WClient *client, GLuint textureUnit)
{
    get_egl_func();
    _texture = new WTexture(textureUnit);
    srand(time(NULL));
    _resource = res;
    _client = client;
    _id = id;
}

WSurface::~WSurface()
{
    delete []_appId;
    delete []_title;
}

void WSurface::sendPointerButtonEvent(UInt32 buttonCode, UInt32 buttonState, UInt32 milliseconds)
{
    if(_client->getPointer())
    {
        wl_pointer_send_button(_client->getPointer(),pointerSerial,milliseconds,buttonCode,buttonState);
        pointerSerial++;
        if(_client->_wl_pointer_version >= 5)
            wl_pointer_send_frame(_client->getPointer());
    }
}

void WSurface::sendPointerMotionEvent(double x, double y, UInt32 milliseconds)
{
    if(_client->getPointer())
    {
        wl_pointer_send_motion(
                    _client->getPointer(),
                    milliseconds,
                    wl_fixed_from_double(x),
                    wl_fixed_from_double(y));

        if(_client->_wl_pointer_version >= 5)
            wl_pointer_send_frame(_client->getPointer());
    }
}

void WSurface::sendPointerEnterEvent(double x, double y)
{
    if(getCompositor()->_pointerFocusSurface == this)
        return;

    // Send previus surface a leave event
    if(getCompositor()->getPointerFocusSurface())
        getCompositor()->getPointerFocusSurface()->sendPointerLeaveEvent();

    if(_client->getPointer())
    {
        wl_pointer_send_enter(
                    _client->getPointer(),
                    pointerSerial,
                    _resource,
                    wl_fixed_from_double(x),
                    wl_fixed_from_double(y));
        pointerSerial++;
        if(_client->_wl_pointer_version >= 5)
            wl_pointer_send_frame(_client->getPointer());
        getCompositor()->_pointerFocusSurface = this;
    }
}

void WSurface::sendPointerLeaveEvent()
{
    if(getCompositor()->_pointerFocusSurface != this)
        return;

    if(_client->getPointer())
    {
        wl_pointer_send_leave(_client->getPointer(),pointerSerial,_resource);
        pointerSerial++;
        if(_client->_wl_pointer_version >= 5)
            wl_pointer_send_frame(_client->getPointer());
    }

    getCompositor()->_pointerFocusSurface = nullptr;
}

void WSurface::sendKeyEvent(UInt32 keyCode, UInt32 keyState, UInt32 milliseconds)
{
    if(_client->getKeyboard())
        wl_keyboard_send_key(_client->getKeyboard(),0,milliseconds,keyCode,keyState);
}

void WSurface::sendKeyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group)
{
    if(_client->getKeyboard())
        wl_keyboard_send_modifiers(_client->getKeyboard(),0,depressed,latched,locked,group);
}

void WSurface::sendKeyboardEnterEvent()
{
    if(getCompositor()->_keyboardFocusSurface == this)
        return;

    if(_client->getKeyboard())
        wl_keyboard_send_enter(_client->getKeyboard(),0,_resource,&nullKeys);

    getCompositor()->_keyboardFocusSurface = this;

}

void WSurface::sendKeyboardLeaveEvent()
{
    if(getCompositor()->_keyboardFocusSurface != this)
        return;

    if(_client->getKeyboard())
        wl_keyboard_send_leave(_client->getKeyboard(),0,_resource);

    getCompositor()->_keyboardFocusSurface = nullptr;
}

void WSurface::sendConfigureEvent(Int32 width, Int32 height, SurfaceStateFlags states)
{
    if(xdg_toplevel != nullptr) //xdgShellVersion >= 2
    {
        wl_array dummy;
        wl_array_init(&dummy);
        UInt32 index = 0;

        if((states & (SurfaceStateFlags)SurfaceState::Activated))
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_ACTIVATED;
            index++;
        }
        if(states & (SurfaceStateFlags)SurfaceState::Fullscreen)
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_FULLSCREEN;
            index++;
        }
        if(states & (SurfaceStateFlags)SurfaceState::Maximized)
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_MAXIMIZED;
            index++;
        }
        if(states & (SurfaceStateFlags)SurfaceState::Resizing)
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_RESIZING;
            index++;
        }

        if(states & (SurfaceStateFlags)SurfaceState::TiledBottom)
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_TILED_BOTTOM;
            index++;
        }
        if(states & (SurfaceStateFlags)SurfaceState::TiledLeft)
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_TILED_LEFT;
            index++;
        }
        if(states & (SurfaceStateFlags)SurfaceState::TiledRight)
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_TILED_RIGHT;
            index++;
        }
        if(states & (SurfaceStateFlags)SurfaceState::TiledTop)
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_TILED_TOP;
            index++;
        }

        xdg_toplevel_send_configure(xdg_toplevel,width,height,&dummy);
        wl_array_release(&dummy);
        xdg_surface_send_configure(xdg_shell,configureSerial);
        configureSerial++;
    }
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

const char *WSurface::getAppId()
{
    return _appId;
}

const char *WSurface::getTitle()
{
    return _title;
}

Int32 WSurface::getWidth()
{
    return _texture->width()/getBufferScale();
}

Int32 WSurface::getHeight()
{
    return _texture->height()/getBufferScale();
}


WPositioner *WSurface::getPositioner()
{
    return _positioner;
}

const Rect WSurface::getDecorationGeometry()
{
    return _decorationGeometry;
}

Int32 WSurface::getMinWidth()
{
    return _minSize.width;
}

Int32 WSurface::getMinHeight()
{
    return _minSize.height;
}

Int32 WSurface::getMaxWidth()
{
    return _maxSize.width;
}

Int32 WSurface::getMaxHeight()
{
    return _maxSize.height;
}

Int32 WSurface::getBufferScale()
{
    return _bufferScale;
}

WTexture *WSurface::getTexture()
{
    return _texture;
}

bool WSurface::isDamaged()
{
    return _isDamaged;
}

void WSurface::applyDamages()
{

    if(eglQueryWaylandBufferWL(WBackend::getEGLDisplay(), buffer, EGL_TEXTURE_FORMAT, &texture_format))
    {
        //printf("EGL buffer\n");
        EGLint width, height;
        eglQueryWaylandBufferWL(WBackend::getEGLDisplay(), buffer, EGL_WIDTH, &width);
        eglQueryWaylandBufferWL(WBackend::getEGLDisplay(), buffer, EGL_HEIGHT, &height);
        EGLAttrib attribs = EGL_NONE;
        EGLImage image = eglCreateImage(WBackend::getEGLDisplay(), EGL_NO_CONTEXT, EGL_WAYLAND_BUFFER_WL, buffer, &attribs);
        _texture->setData(width, height, &image, WTexture::Type::EGL);
        eglDestroyImage (WBackend::getEGLDisplay(), image);
    }
    else
    {
        wl_shm_buffer *shm_buffer = wl_shm_buffer_get(buffer);
        wl_shm_buffer_begin_access(shm_buffer);
        UInt32 width = wl_shm_buffer_get_width(shm_buffer);
        UInt32 height = wl_shm_buffer_get_height(shm_buffer);
        void *data = wl_shm_buffer_get_data(shm_buffer);
        //surface->getCompositor()->addTextureToRingBuffer(surface,width,height,data,WTexture::Type::SHM);

        _texture->setData(width, height, data);
        wl_shm_buffer_end_access(shm_buffer);
    }

    //eventfd_write(surface->getCompositor()->compositorFd,1);
    //surface->getCompositor()->cv.notify_one();



    wl_buffer_send_release(buffer);

    if (frame_callback != nullptr)
    {

        wl_callback_send_done(frame_callback,getCompositor()->getMilliseconds());
        wl_resource_destroy(frame_callback);
        frame_callback = nullptr;
    }

    _isDamaged = false;

}

void WSurface::setBufferScale(Int32 scale)
{
    _bufferScale = scale;
}

wl_resource *WSurface::getResource()
{
    return _resource;
}

WClient *WSurface::getClient()
{
    return _client;
}

WCompositor *WSurface::getCompositor()
{
    return _client->getCompositor();
}

UInt32 WSurface::getId()
{
    return _id;
}

SurfaceType WSurface::getType()
{
    return _type;
}

WaylandPlus::WSurface *WSurface::getParent()
{
    return _parent;
}

const list<WaylandPlus::WSurface *> WSurface::getChildren()
{
    return _children;
}


