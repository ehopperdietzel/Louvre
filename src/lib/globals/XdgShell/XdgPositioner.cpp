#include "XdgPositioner.h"
#include <stdio.h>
#include <LPositionerPrivate.h>
#include <LSurface.h>
#include <xdg-shell.h>

void Louvre::Extensions::XdgShell::Positioner::destroy_resource(wl_resource *)
{
    printf("XDG_POSITIONER DESTROYED.\n");
    /* We don't destroy it here.
     * It is destroyed when the popup is destroyed or repositioned */
}

void Louvre::Extensions::XdgShell::Positioner::destroy(wl_client *, wl_resource *resource)
{
    wl_resource_destroy(resource);
}

void Louvre::Extensions::XdgShell::Positioner::set_size(wl_client *, wl_resource *resource, Int32 width, Int32 height)
{
    if (width <= 0 || height <= 0)
    {
        wl_resource_post_error(resource, XDG_POSITIONER_ERROR_INVALID_INPUT,"xdg_positioner.set_size requested with non-positive dimensions");
        return;
    }

    LPositioner *positioner = (LPositioner*)wl_resource_get_user_data(resource);
    positioner->imp()->size.setW(width);
    positioner->imp()->size.setH(height);
}

void Louvre::Extensions::XdgShell::Positioner::set_anchor_rect(wl_client *, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{
    if (width <= 0 || height <= 0)
    {
        wl_resource_post_error(resource, XDG_POSITIONER_ERROR_INVALID_INPUT,"xdg_positioner.set_anchor_rect requested with non-positive dimensions");
        return;
    }

    LPositioner *positioner = (LPositioner*)wl_resource_get_user_data(resource);
    positioner->imp()->anchorRect.setX(x);
    positioner->imp()->anchorRect.setY(y);
    positioner->imp()->anchorRect.setW(width);
    positioner->imp()->anchorRect.setH(height);
}

void Louvre::Extensions::XdgShell::Positioner::set_anchor(wl_client *, wl_resource *resource, UInt32 anchor)
{
    LPositioner *positioner = (LPositioner*)wl_resource_get_user_data(resource);
    positioner->imp()->anchor = anchor;
}

void Louvre::Extensions::XdgShell::Positioner::set_gravity(wl_client *, wl_resource *resource, UInt32 gravity)
{
    LPositioner *positioner = (LPositioner*)wl_resource_get_user_data(resource);
    positioner->imp()->gravity = gravity;
}

void Louvre::Extensions::XdgShell::Positioner::set_constraint_adjustment(wl_client *, wl_resource *resource, UInt32 constraintAdjustment)
{
    LPositioner *positioner = (LPositioner*)wl_resource_get_user_data(resource);
    positioner->imp()->constraintAdjustment = constraintAdjustment;
}

void Louvre::Extensions::XdgShell::Positioner::set_offset(wl_client *, wl_resource *resource, Int32 x, Int32 y)
{
    LPositioner *positioner = (LPositioner*)wl_resource_get_user_data(resource);
    positioner->imp()->offset = LPoint(x,y);
}

#if LOUVRE_XDG_WM_BASE_VERSION >=3

    void Louvre::Extensions::XdgShell::Positioner::set_reactive(wl_client *, wl_resource *resource)
    {
        LPositioner *positioner = (LPositioner*)wl_resource_get_user_data(resource);
        positioner->imp()->isReactive = true;
    }

    void Louvre::Extensions::XdgShell::Positioner::set_parent_size(wl_client *, wl_resource *resource, Int32 parentWidth, Int32 parentHeight)
    {
        LPositioner *positioner = (LPositioner*)wl_resource_get_user_data(resource);
        positioner->imp()->parentSize = LSize(parentWidth,parentHeight);
    }

    void Louvre::Extensions::XdgShell::Positioner::set_parent_configure(wl_client *, wl_resource *resource, UInt32 serial)
    {
        // TODO
    }

#endif
