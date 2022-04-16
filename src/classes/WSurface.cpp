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
    _resource = surface;
    _client = client;
}

WSurface::~WSurface()
{
    delete []_appId;
    delete []_title;
}

void WSurface::sendPointerButtonEvent(UInt32 buttonCode, UInt32 buttonState, UInt32 milliseconds)
{
    if(_client->getPointer() != nullptr)
    {
        wl_pointer_send_button(_client->getPointer(),pointerSerial,milliseconds,buttonCode,buttonState);
        pointerSerial++;
        if(_client->_wl_pointer_version >= 5)
            wl_pointer_send_frame(_client->getPointer());

    }
}

void WSurface::sendPointerMotionEvent(double x, double y, UInt32 milliseconds)
{
    if(_client->getPointer() != nullptr)
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
    if(_client->getPointer() != nullptr)
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
    }
}

void WSurface::sendPointerLeaveEvent()
{
    if(_client->getPointer() != nullptr)
    {
        wl_pointer_send_leave(_client->getPointer(),pointerSerial,_resource);
        pointerSerial++;
        if(_client->_wl_pointer_version >= 5)
            wl_pointer_send_frame(_client->getPointer());

    }
}

void WSurface::sendKeyEvent(UInt32 keyCode, UInt32 keyState)
{
    if(_client->getKeyboard() != nullptr)
    {
        wl_keyboard_send_key(_client->getKeyboard(),keyboardSerial,compositor()->getMilliseconds(),keyCode,keyState);
        keyboardSerial++;        
    }
}

void WSurface::sendKeyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group)
{
    if(_client->getKeyboard() != nullptr)
    {
        wl_keyboard_send_modifiers(_client->getKeyboard(),keyboardSerial,depressed,latched,locked,group);
        keyboardSerial++;
    }
}

void WSurface::sendKeyboardEnterEvent()
{
    if(_client->getKeyboard() != nullptr)
    {
        wl_keyboard_send_enter(_client->getKeyboard(),keyboardSerial,_resource,&nullKeys);
        keyboardSerial++;
    }
}

void WSurface::sendKeyboardLeaveEvent()
{
    if(_client->getKeyboard() != nullptr)
    {
        wl_keyboard_send_leave(_client->getKeyboard(),keyboardSerial,_resource);
        keyboardSerial++;
    }
}

void WSurface::sendConfigureEvent(Int32 width, Int32 height, SurfaceStateFlags states)
{
    if(xdg_toplevel != nullptr && xdg_shell != nullptr) //xdgShellVersion >= 2
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

void WSurface::applyDamages()
{
    if(!_isDamaged || resource() == nullptr )
        return;

    WOutput *output = compositor()->getOutputs().front();
    Int32 width, height;
    WSize prevSize = size();

    if(eglQueryWaylandBufferWL(output->getDisplay(), committedBuffer, EGL_TEXTURE_FORMAT, &texture_format))
    {
        //printf("EGL buffer\n");

        eglQueryWaylandBufferWL(output->getDisplay(), committedBuffer, EGL_WIDTH, &width);
        eglQueryWaylandBufferWL(output->getDisplay(), committedBuffer, EGL_HEIGHT, &height);
        _texture->_format = texture_format;
        EGLAttrib attribs = EGL_NONE;
        EGLImage image = eglCreateImage(output->getDisplay(), EGL_NO_CONTEXT, EGL_WAYLAND_BUFFER_WL, committedBuffer, &attribs);
        _texture->setData(width, height, &image, WTexture::Type::EGL);
        eglDestroyImage (output->getDisplay(), image);
    }
    else
    {
        //printf("SHM buffer\n");
        wl_shm_buffer *shm_buffer = wl_shm_buffer_get(committedBuffer);
        wl_shm_buffer_begin_access(shm_buffer);
        width = wl_shm_buffer_get_width(shm_buffer);
        height = wl_shm_buffer_get_height(shm_buffer);
        void *data = wl_shm_buffer_get_data(shm_buffer);
        UInt32 format =  wl_shm_buffer_get_format(shm_buffer);
        if( format == WL_SHM_FORMAT_XRGB8888 )
            _texture->_format = GL_RGB;
        else
            _texture->_format = GL_RGBA;

        _texture->setData(width, height, data);
        wl_shm_buffer_end_access(shm_buffer);
    }

    //WWayland::apply_damage_emit(this);


    wl_buffer_send_release(committedBuffer);
    _isDamaged = false;

    if (frame_callback != nullptr)
    {

        wl_callback_send_done(frame_callback,compositor()->getMilliseconds());
        wl_resource_destroy(frame_callback);
        frame_callback = nullptr;
    }

    if(prevSize != size())
        bufferSizeChangeRequest();
}

void WSurface::setBufferScale(Int32 scale)
{
    _bufferScale = scale;
}

WCompositor *WSurface::compositor()
{
    if(_client != nullptr)
        return _client->getCompositor();
    else
        return nullptr;
}


