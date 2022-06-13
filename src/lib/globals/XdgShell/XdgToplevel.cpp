#include "XdgToplevel.h"
#include <stdio.h>
#include <LSurface.h>
#include <LCompositor.h>
#include <xdg-shell.h>
#include <LToplevelRole.h>
#include <LSeat.h>
#include <LPointer.h>

using namespace Louvre;

void Extensions::XdgShell::Toplevel::destroy_resource(wl_resource *resource)
{
    printf("XDG_TOPLEVEL DESTROYED.\n");

    LToplevelRole *lToplevel = (LToplevelRole*)wl_resource_get_user_data(resource);

    // Remove focus
    if(lToplevel->seat()->pointer()->resizingToplevel() == lToplevel)
        lToplevel->seat()->pointer()->stopResizingToplevel();

    if(lToplevel->seat()->pointer()->movingTopLevel() == lToplevel)
        lToplevel->seat()->pointer()->stopMovingTopLevel();

    if(lToplevel->seat()->activeTopLevel() == lToplevel)
        lToplevel->seat()->p_activeTopLevel = nullptr;

    // Notify
    lToplevel->compositor()->destroyToplevelRequest(lToplevel);

    // Unset role
    if(lToplevel->surface())
    {
        // Parent
        if(lToplevel->surface()->parent())
        {
            lToplevel->surface()->parent()->p_children.remove(lToplevel->surface());
            lToplevel->surface()->p_parent = nullptr;
            lToplevel->surface()->parentChangeRequest();
        }

        lToplevel->surface()->current.type = LSurface::Undefined;
        lToplevel->surface()->p_role = nullptr;
        lToplevel->surface()->typeChangeRequest();
    }

    delete lToplevel;
}

void Extensions::XdgShell::Toplevel::destroy (wl_client *, wl_resource *resource)
{
    wl_resource_destroy(resource);
}

void Extensions::XdgShell::Toplevel::set_parent (wl_client *, wl_resource *resource, wl_resource *parent)
{
    LToplevelRole *lToplevel = (LToplevelRole*)wl_resource_get_user_data(resource);

    if(parent == NULL)
    {
        if(lToplevel->surface()->parent())
            lToplevel->surface()->parent()->p_children.remove(lToplevel->surface());
        lToplevel->surface()->p_parent = nullptr;
    }
    else
    {
        lToplevel->surface()->p_parent = ((LToplevelRole*)wl_resource_get_user_data(parent))->surface();
        lToplevel->surface()->parent()->p_children.push_front(lToplevel->surface());
    }

    lToplevel->surface()->parentChangeRequest();
}

void Extensions::XdgShell::Toplevel::set_title (wl_client *, wl_resource *resource, const char *title)
{
    LToplevelRole *lToplevel = (LToplevelRole*)wl_resource_get_user_data(resource);
    lToplevel->setTitle(title);
}

void Extensions::XdgShell::Toplevel::set_app_id (wl_client *, wl_resource *resource, const char *app_id)
{
    LToplevelRole *lToplevel = (LToplevelRole*)wl_resource_get_user_data(resource);
    lToplevel->setAppId(app_id);
}

void Extensions::XdgShell::Toplevel::show_window_menu (wl_client *client, wl_resource *resource, wl_resource *seat, UInt32 serial, Int32 x, Int32 y)
{
    /* TODO */
}

void Extensions::XdgShell::Toplevel::move(wl_client *, wl_resource *resource, wl_resource */*seat*/, UInt32 /*serial*/)
{
    LToplevelRole *lToplevel = (LToplevelRole*)wl_resource_get_user_data(resource);
    lToplevel->startMoveRequest();
}

void Extensions::XdgShell::Toplevel::resize(wl_client *, wl_resource *resource, wl_resource */*seat*/, UInt32 /*serial*/, UInt32 edges)
{
    LToplevelRole *lToplevel = (LToplevelRole*)wl_resource_get_user_data(resource);
    lToplevel->startResizeRequest((LToplevelRole::Edge)edges);
}

void Extensions::XdgShell::Toplevel::set_max_size (wl_client *, wl_resource *resource, Int32 width, Int32 height)
{
    LToplevelRole *lToplevel = (LToplevelRole*)wl_resource_get_user_data(resource);
    lToplevel->p_maxSize.setW(width);
    lToplevel->p_maxSize.setH(height);
    lToplevel->maxSizeChanged();
}

void Extensions::XdgShell::Toplevel::set_min_size (wl_client *, wl_resource *resource, Int32 width, Int32 height)
{
    LToplevelRole *lToplevel = (LToplevelRole*)wl_resource_get_user_data(resource);
    lToplevel->p_minSize.setW(width);
    lToplevel->p_minSize.setH(height);
    lToplevel->minSizeChanged();
}

void Extensions::XdgShell::Toplevel::set_maximized (wl_client *, wl_resource *resource)
{
    LToplevelRole *lToplevel = (LToplevelRole*)wl_resource_get_user_data(resource);
    lToplevel->setMaximizedRequest();
}

void Extensions::XdgShell::Toplevel::unset_maximized (wl_client *, wl_resource *resource)
{
    LToplevelRole *lToplevel = (LToplevelRole*)wl_resource_get_user_data(resource);
    lToplevel->unsetMaximizedRequest();
}

void Extensions::XdgShell::Toplevel::set_fullscreen(wl_client *, wl_resource *resource, wl_resource *output)
{
    LOutput *lOutput = nullptr;

    if(output)
        lOutput = (LOutput*)wl_resource_get_user_data(output);

    LToplevelRole *lTopLevel = (LToplevelRole*)wl_resource_get_user_data(resource);
    lTopLevel->setFullscreenRequest(lOutput);
}

void Extensions::XdgShell::Toplevel::unset_fullscreen(wl_client *, wl_resource *resource)
{
    LToplevelRole *lToplevel = (LToplevelRole*)wl_resource_get_user_data(resource);
    lToplevel->unsetFullscreenRequest();
}

void Extensions::XdgShell::Toplevel::set_minimized(wl_client *, wl_resource *resource)
{
    LToplevelRole *lToplevel= (LToplevelRole*)wl_resource_get_user_data(resource);
    lToplevel->setMinimizedRequest();
}

#if LOUVRE_XDG_WM_BASE_VERSION >= 4
    void Extensions::XdgShell::Toplevel::configure_bounds(wl_client *client, wl_resource *resource, Int32 width, Int32 height)
    {
        // TODO
    }
#endif
