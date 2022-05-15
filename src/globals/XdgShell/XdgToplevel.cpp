#include "XdgToplevel.h"
#include <stdio.h>
#include <WSurface.h>
#include <WCompositor.h>
#include <xdg-shell.h>

using namespace Wpp;

void Extensions::XdgShell::Toplevel::destroy_resource(wl_resource *resource)
{
    (void)resource;
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    if(surface->p_parent != nullptr)
        surface->p_parent->children().remove(surface);

    printf("TOP LEVEL DESTROYED.\n");
}

void Extensions::XdgShell::Toplevel::destroy (wl_client *client, wl_resource *resource)
{
    (void)client;
    wl_resource_destroy(resource);
}
void Extensions::XdgShell::Toplevel::set_parent (wl_client *client, wl_resource *resource, wl_resource *parent)
{
    (void)client;(void)resource;(void)parent;

    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    if(parent == NULL)
    {
        if(surface->p_parent != nullptr)
            surface->p_parent->children().remove(surface);
        surface->p_parent = nullptr;
    }
    else
    {
        surface->p_parent = (WSurface*)wl_resource_get_user_data(parent);
        surface->p_parent->children().push_back(surface);
    }

    surface->parentChangeRequest();
}
void Extensions::XdgShell::Toplevel::set_title (wl_client *client, wl_resource *resource, const char *title)
{
    (void)client;
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->setTitle(title);
    //printf("App Title: %s\n",title);
}
void Extensions::XdgShell::Toplevel::set_app_id (wl_client *client, wl_resource *resource, const char *app_id)
{
    (void)client;
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->setAppId(app_id);
    //printf("App Id: %s\n",app_id);
}
void Extensions::XdgShell::Toplevel::show_window_menu (wl_client *client, wl_resource *resource, wl_resource *seat, UInt32 serial, Int32 x, Int32 y)
{
    (void)client;(void)resource;(void)seat;(void)serial;(void)x;(void)y;
}
void Extensions::XdgShell::Toplevel::move(wl_client *client, wl_resource *resource, wl_resource *seat, UInt32 serial)
{
    (void)client;(void)seat;(void)serial;
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->moveStartRequest();

}
void Extensions::XdgShell::Toplevel::resize (wl_client *client, wl_resource *resource, wl_resource *seat, UInt32 serial, UInt32 edges)
{
    printf("Resize\n");
    (void)client;(void)seat;(void)serial;
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->resizeStartRequest((ResizeEdge)edges);
}
void Extensions::XdgShell::Toplevel::set_max_size (wl_client *client, wl_resource *resource, Int32 width, Int32 height)
{
    (void)client;
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->_maxSize = {width,height};
    surface->maxSizeChangeRequest();
}
void Extensions::XdgShell::Toplevel::set_min_size (wl_client *client, wl_resource *resource, Int32 width, Int32 height)
{
    (void)client;(void)resource;(void)width;(void)height;
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->_minSize = {width,height};
    surface->minSizeChangeRequest();
}
void Extensions::XdgShell::Toplevel::set_maximized (wl_client *client, wl_resource *resource)
{
    (void)client;(void)resource;
    //printf ("surface requested maximize\n");
}
void Extensions::XdgShell::Toplevel::unset_maximized (wl_client *client, wl_resource *resource)
{
    (void)client;(void)resource;
}
void Extensions::XdgShell::Toplevel::set_fullscreen(wl_client *client, wl_resource *resource, wl_resource *output)
{
    (void)client;(void)resource;(void)output;
}
void Extensions::XdgShell::Toplevel::unset_fullscreen(wl_client *client, wl_resource *resource)
{
    (void)client;(void)resource;
}
void Extensions::XdgShell::Toplevel::set_minimized(wl_client *client, wl_resource *resource)
{
    (void)client;(void)resource;
}

