#include "XdgToplevel.h"
#include <stdio.h>
#include <WSurface.h>
#include <WCompositor.h>
#include <xdg-shell.h>
#include <WToplevelRole.h>
#include <WSeat.h>

using namespace Wpp;

void Extensions::XdgShell::Toplevel::destroy_resource(wl_resource *resource)
{
    (void)resource;
    WToplevelRole *topLevel = (WToplevelRole*)wl_resource_get_user_data(resource);

    if(topLevel->seat()->resizingToplevel() == topLevel)
        topLevel->seat()->stopResizingToplevel();

    if(topLevel->seat()->movingTopLevel() == topLevel)
        topLevel->seat()->stopMovingTopLevel();

    topLevel->compositor()->destroyTopLevelRequest(topLevel);
    delete topLevel;

}

void Extensions::XdgShell::Toplevel::destroy (wl_client *client, wl_resource *resource)
{
    (void)client;
    wl_resource_destroy(resource);
}
void Extensions::XdgShell::Toplevel::set_parent (wl_client *client, wl_resource *resource, wl_resource *parent)
{
    (void)client;(void)resource;(void)parent;

    WToplevelRole *toplevel = (WToplevelRole*)wl_resource_get_user_data(resource);
    if(parent == NULL)
    {
        if(toplevel->surface()->parent())
            toplevel->surface()->parent()->p_children.remove(toplevel->surface());
        toplevel->surface()->p_parent = nullptr;
    }
    else
    {
        toplevel->surface()->p_parent = (WSurface*)wl_resource_get_user_data(parent);
        toplevel->surface()->p_parent->p_children.push_back(toplevel->surface());
    }

    toplevel->surface()->parentChangeRequest();
}
void Extensions::XdgShell::Toplevel::set_title (wl_client *client, wl_resource *resource, const char *title)
{
    (void)client;
    WToplevelRole *toplevel = (WToplevelRole*)wl_resource_get_user_data(resource);
    toplevel->setTitle(title);
}
void Extensions::XdgShell::Toplevel::set_app_id (wl_client *client, wl_resource *resource, const char *app_id)
{
    (void)client;
    WToplevelRole *toplevel = (WToplevelRole*)wl_resource_get_user_data(resource);
    toplevel->setAppId(app_id);
}
void Extensions::XdgShell::Toplevel::show_window_menu (wl_client *client, wl_resource *resource, wl_resource *seat, UInt32 serial, Int32 x, Int32 y)
{
    (void)client;(void)resource;(void)seat;(void)serial;(void)x;(void)y;
}
void Extensions::XdgShell::Toplevel::move(wl_client *client, wl_resource *resource, wl_resource *seat, UInt32 serial)
{
    (void)client;(void)seat;(void)serial;
    WToplevelRole *toplevel = (WToplevelRole*)wl_resource_get_user_data(resource);
    toplevel->startMoveRequest();

}
void Extensions::XdgShell::Toplevel::resize(wl_client *client, wl_resource *resource, wl_resource *seat, UInt32 serial, UInt32 edges)
{
    (void)client;(void)seat;(void)serial;
    WToplevelRole *toplevel = (WToplevelRole*)wl_resource_get_user_data(resource);
    toplevel->startResizeRequest((WToplevelRole::Edge)edges);
}
void Extensions::XdgShell::Toplevel::set_max_size (wl_client *client, wl_resource *resource, Int32 width, Int32 height)
{
    (void)client;
    WToplevelRole *toplevel = (WToplevelRole*)wl_resource_get_user_data(resource);
    toplevel->p_maxSize.setW(width);
    toplevel->p_maxSize.setH(height);
    toplevel->maxSizeChanged();
}
void Extensions::XdgShell::Toplevel::set_min_size (wl_client *client, wl_resource *resource, Int32 width, Int32 height)
{
    (void)client;
    WToplevelRole *toplevel = (WToplevelRole*)wl_resource_get_user_data(resource);
    toplevel->p_minSize.setW(width);
    toplevel->p_minSize.setH(height);
    toplevel->minSizeChanged();
}
void Extensions::XdgShell::Toplevel::set_maximized (wl_client *client, wl_resource *resource)
{
    (void)client;
    WToplevelRole *topLevel = (WToplevelRole*)wl_resource_get_user_data(resource);
    topLevel->maximizeRequest();
}
void Extensions::XdgShell::Toplevel::unset_maximized (wl_client *client, wl_resource *resource)
{
    (void)client;
    WToplevelRole *topLevel = (WToplevelRole*)wl_resource_get_user_data(resource);
    topLevel->unmaximizeRequest();
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
    (void)client;
    WToplevelRole *topLevel = (WToplevelRole*)wl_resource_get_user_data(resource);
    topLevel->minimizeRequest();
}

