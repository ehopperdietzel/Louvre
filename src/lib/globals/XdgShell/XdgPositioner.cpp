#include "XdgPositioner.h"
#include <stdio.h>
#include <LPositioner.h>
#include <LSurface.h>


void Louvre::Extensions::XdgShell::Positioner::destroy_resource(wl_resource *resource)
{
    (void)resource;
    printf("Xdg Positioner resource destroyed.\n");
}

void Louvre::Extensions::XdgShell::Positioner::destroy(wl_client *client, wl_resource *resource)
{
    (void)client;
    wl_resource_destroy(resource);
}

void Louvre::Extensions::XdgShell::Positioner::set_size(wl_client *client, wl_resource *resource, Int32 width, Int32 height)
{
    (void)client;

    if (width <= 0 || height <= 0)
    {
        printf("ERROR: xdg_popup : set_size\n");
        wl_resource_post_error(resource, XDG_POSITIONER_ERROR_INVALID_INPUT,"xdg_positioner.set_size requested with non-positive dimensions");
        return;
    }

    LPositioner *positioner = (LPositioner*)wl_resource_get_user_data(resource);
    positioner->p_size.setW(width);
    positioner->p_size.setH(height);
}

void Louvre::Extensions::XdgShell::Positioner::set_anchor_rect(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{
    (void)client;

    if (width <= 0 || height <= 0)
    {
        printf("ERROR: xdg_popup : set_anchor_rect\n");
        wl_resource_post_error(resource, XDG_POSITIONER_ERROR_INVALID_INPUT,"xdg_positioner.set_anchor_rect requested with non-positive dimensions");
        return;
    }

    LPositioner *positioner = (LPositioner*)wl_resource_get_user_data(resource);
    positioner->p_anchorRect.setX(x);
    positioner->p_anchorRect.setY(y);
    positioner->p_anchorRect.setW(width);
    positioner->p_anchorRect.setH(height);
}

void Louvre::Extensions::XdgShell::Positioner::set_anchor(wl_client *client, wl_resource *resource, UInt32 anchor)
{
    (void)client;
    LPositioner *positioner = (LPositioner*)wl_resource_get_user_data(resource);
    positioner->p_anchor = (Anchor)anchor;
}

void Louvre::Extensions::XdgShell::Positioner::set_gravity(wl_client *client, wl_resource *resource, UInt32 gravity)
{
    (void)client;
    LPositioner *positioner = (LPositioner*)wl_resource_get_user_data(resource);
    positioner->p_gravity = (Gravity)gravity;
}

void Louvre::Extensions::XdgShell::Positioner::set_constraint_adjustment(wl_client *client, wl_resource *resource, UInt32 constraintAdjustment)
{
    (void)client;
    LPositioner *positioner = (LPositioner*)wl_resource_get_user_data(resource);
    positioner->p_constraintAdjustment = (ConstraintAdjustment)constraintAdjustment;
}

void Louvre::Extensions::XdgShell::Positioner::set_offset(wl_client *client, wl_resource *resource, Int32 x, Int32 y)
{
    (void)client;
    LPositioner *positioner = (LPositioner*)wl_resource_get_user_data(resource);
    positioner->p_offset = LPoint(x,y);
}

void Louvre::Extensions::XdgShell::Positioner::set_reactive(wl_client *client, wl_resource *resource)
{
    (void)client;
    LPositioner *positioner = (LPositioner*)wl_resource_get_user_data(resource);
    positioner->p_isReactive = true;
    printf("POPUP IS REACTIVE\n");
}

void Louvre::Extensions::XdgShell::Positioner::set_parent_size(wl_client *client, wl_resource *resource, Int32 parentWidth, Int32 parentHeight)
{
    (void)client;
    LPositioner *positioner = (LPositioner*)wl_resource_get_user_data(resource);
    positioner->p_parentSize = LSize(parentWidth,parentHeight);
}

void Louvre::Extensions::XdgShell::Positioner::set_parent_configure(wl_client *client, wl_resource *resource, UInt32 serial)
{
    (void)client;
    //LPositioner *positioner = (LPositioner*)wl_resource_get_user_data(resource);

    // TODO
}
