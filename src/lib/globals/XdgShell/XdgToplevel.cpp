#include "XdgToplevel.h"
#include <stdio.h>
#include <LSurfacePrivate.h>
#include <LCompositor.h>
#include <xdg-shell.h>
#include <LToplevelRolePrivate.h>
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
        lToplevel->seat()->m_activeTopLevel = nullptr;

    // Notify
    lToplevel->compositor()->destroyToplevelRequest(lToplevel);

    // Unset role
    if(lToplevel->surface())
    {
        // Parent
        if(lToplevel->surface()->parent())
        {
            lToplevel->surface()->parent()->imp()->m_children.remove(lToplevel->surface());
            lToplevel->surface()->imp()->m_parent = nullptr;
            lToplevel->surface()->parentChanged();
        }

        lToplevel->surface()->imp()->current.type = LSurface::Undefined;
        lToplevel->surface()->imp()->m_role = nullptr;
        lToplevel->surface()->imp()->mapped = false;
        lToplevel->surface()->mappingChanged();
        lToplevel->surface()->roleChanged();
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

    printf("TOPLEVEL SET PARENT\n");

    if(parent == NULL)
    {
        removeParent:
        if(lToplevel->surface()->parent())
        {
            lToplevel->surface()->parent()->imp()->m_children.remove(lToplevel->surface());
            lToplevel->surface()->imp()->m_parent = nullptr;
            lToplevel->surface()->parentChanged();
        }
    }
    else
    {
        LSurface *lParent = ((LToplevelRole*)wl_resource_get_user_data(parent))->surface();

        if(lParent->mapped())
        {
            lToplevel->surface()->imp()->m_parent = lParent;
            lToplevel->surface()->parent()->imp()->m_children.push_back(lToplevel->surface());
            lToplevel->surface()->parentChanged();
        }
        // Setting a parent that is not mapped is equal to set a null parent
        else
        {
            goto removeParent;
        }
    }


}

void Extensions::XdgShell::Toplevel::set_title (wl_client *, wl_resource *resource, const char *title)
{
    LToplevelRole *lToplevel = (LToplevelRole*)wl_resource_get_user_data(resource);
    lToplevel->imp()->setTitle(title);
}

void Extensions::XdgShell::Toplevel::set_app_id (wl_client *, wl_resource *resource, const char *app_id)
{
    LToplevelRole *lToplevel = (LToplevelRole*)wl_resource_get_user_data(resource);
    lToplevel->imp()->setAppId(app_id);
}

void Extensions::XdgShell::Toplevel::show_window_menu (wl_client *, wl_resource *resource, wl_resource */*seat*/, UInt32 serial, Int32 x, Int32 y)
{
    LToplevelRole *lToplevel = (LToplevelRole*)wl_resource_get_user_data(resource);

    if(serial == lToplevel->surface()->client()->lastPointerButtonEventSerial() || serial == lToplevel->surface()->client()->lastKeyboardKeyEventSerial())
        lToplevel->showWindowMenuRequest(x,y);

}

void Extensions::XdgShell::Toplevel::move(wl_client *, wl_resource *resource, wl_resource */*seat*/, UInt32 serial)
{
    LToplevelRole *lToplevel = (LToplevelRole*)wl_resource_get_user_data(resource);
    if(serial == lToplevel->surface()->client()->lastPointerButtonEventSerial() || serial == lToplevel->surface()->client()->lastKeyboardKeyEventSerial())
        lToplevel->startMoveRequest();
}

void Extensions::XdgShell::Toplevel::resize(wl_client *, wl_resource *resource, wl_resource */*seat*/, UInt32 serial, UInt32 edges)
{
    if(edges < 0 || edges > 10)
    {
        wl_resource_post_error(resource, XDG_TOPLEVEL_ERROR_INVALID_RESIZE_EDGE, "provided value is not a valid variant of the resize_edge enum.");
        return;
    }

    LToplevelRole *lToplevel = (LToplevelRole*)wl_resource_get_user_data(resource);
    if(serial == lToplevel->surface()->client()->lastPointerButtonEventSerial() || serial == lToplevel->surface()->client()->lastKeyboardKeyEventSerial())
        lToplevel->startResizeRequest((LToplevelRole::Edge)edges);
}

void Extensions::XdgShell::Toplevel::set_max_size (wl_client *, wl_resource *resource, Int32 width, Int32 height)
{
    if(width < 0 || height < 0)
    {
        // Error enum not defined in protocol
        wl_resource_post_error(resource, XDG_TOPLEVEL_ERROR_INVALID_RESIZE_EDGE, "invalid toplevel max size");
        return;
    }

    LToplevelRole *lToplevel = (LToplevelRole*)wl_resource_get_user_data(resource);
    lToplevel->imp()->pendingMaxSize.setW(width);
    lToplevel->imp()->pendingMaxSize.setH(height);
    lToplevel->imp()->hasPendingMaxSize = true;
}

void Extensions::XdgShell::Toplevel::set_min_size (wl_client *, wl_resource *resource, Int32 width, Int32 height)
{
    if(width < 0 || height < 0)
    {
        // Error enum not defined in protocol
        wl_resource_post_error(resource, XDG_TOPLEVEL_ERROR_INVALID_RESIZE_EDGE, "invalid toplevel min size");
        return;
    }
    LToplevelRole *lToplevel = (LToplevelRole*)wl_resource_get_user_data(resource);
    lToplevel->imp()->pendingMinSize.setW(width);
    lToplevel->imp()->pendingMinSize.setH(height);
    lToplevel->imp()->hasPendingMinSize = true;
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
