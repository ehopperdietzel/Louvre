#include "XdgToplevel.h"
#include <stdio.h>
#include <WSurface.h>
#include <WCompositor.h>

using namespace WaylandPlus;

void Extensions::XdgShell::Toplevel::destroy (wl_client *client, wl_resource *resource)
{
    (void)client;(void)resource;
}
void Extensions::XdgShell::Toplevel::set_parent (wl_client *client, wl_resource *resource, wl_resource *parent)
{
    (void)client;(void)resource;(void)parent;
}
void Extensions::XdgShell::Toplevel::set_title (wl_client *client, wl_resource *resource, const char *title)
{
    (void)client;
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->setTitle(title);
    printf("App Title: %s\n",title);
}
void Extensions::XdgShell::Toplevel::set_app_id (wl_client *client, wl_resource *resource, const char *app_id)
{
    (void)client;
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->setAppId(app_id);
    printf("App Id: %s\n",app_id);
}
void Extensions::XdgShell::Toplevel::show_window_menu (wl_client *client, wl_resource *resource, wl_resource *seat, UInt32 serial, Int32 x, Int32 y)
{
    (void)client;(void)resource;(void)seat;(void)serial;(void)x;(void)y;
}
void Extensions::XdgShell::Toplevel::move(wl_client *client, wl_resource *resource, wl_resource *seat, UInt32 serial)
{
    (void)client;(void)seat;(void)serial;
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->getCompositor()->surfaceMoveEvent(surface);

}
void Extensions::XdgShell::Toplevel::resize (wl_client *client, wl_resource *resource, wl_resource *seat, UInt32 serial, UInt32 edges)
{
    (void)client;(void)resource;(void)seat;(void)serial;(void)edges;
}
void Extensions::XdgShell::Toplevel::set_max_size (wl_client *client, wl_resource *resource, Int32 width, Int32 height)
{
    (void)client;
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->setMaxWidth(width);
    surface->setMaxHeight(height);
    surface->getCompositor()->surfaceMaxSizeChanged(surface,width,height);
}
void Extensions::XdgShell::Toplevel::set_min_size (wl_client *client, wl_resource *resource, Int32 width, Int32 height)
{
    (void)client;(void)resource;(void)width;(void)height;
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->setMinWidth(width);
    surface->setMinHeight(height);
    surface->getCompositor()->surfaceMinSizeChanged(surface,width,height);
}
void Extensions::XdgShell::Toplevel::set_maximized (wl_client *client, wl_resource *resource)
{
    (void)client;(void)resource;
    printf ("surface requested maximize\n");
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

