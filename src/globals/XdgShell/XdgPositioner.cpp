#include "XdgPositioner.h"
#include <stdio.h>
#include <WPositioner.h>
#include <WSurface.h>


void Wpp::Extensions::XdgShell::Positioner::destroy_resource(wl_resource *resource)
{
    (void)resource;
    printf("Xdg Positioner resource destroyed.\n");
}

void Wpp::Extensions::XdgShell::Positioner::destroy(wl_client *client, wl_resource *resource)
{
    (void)client;
    wl_resource_destroy(resource);
}

void Wpp::Extensions::XdgShell::Positioner::set_size(wl_client *client, wl_resource *resource, Int32 width, Int32 height)
{
    (void)client;

    if (width <= 0 || height <= 0)
    {
        wl_resource_post_error(resource, XDG_POSITIONER_ERROR_INVALID_INPUT,"xdg_positioner.set_size requested with non-positive dimensions");
        return;
    }

    WPositioner *positioner = (WPositioner*)wl_resource_get_user_data(resource);
    positioner->p_size.setW(width);
    positioner->p_size.setH(height);
}

void Wpp::Extensions::XdgShell::Positioner::set_anchor_rect(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{
    (void)client;

    if (width <= 0 || height <= 0)
    {
        wl_resource_post_error(resource, XDG_POSITIONER_ERROR_INVALID_INPUT,"xdg_positioner.set_anchor_rect requested with non-positive dimensions");
        return;
    }

    WPositioner *positioner = (WPositioner*)wl_resource_get_user_data(resource);
    positioner->p_anchorRect.setX(x);
    positioner->p_anchorRect.setY(y);
    positioner->p_anchorRect.setW(width);
    positioner->p_anchorRect.setH(height);
}

void Wpp::Extensions::XdgShell::Positioner::set_anchor(wl_client *client, wl_resource *resource, UInt32 anchor)
{
    (void)client;
    WPositioner *positioner = (WPositioner*)wl_resource_get_user_data(resource);
    positioner->p_anchor = (Anchor)anchor;
}

void Wpp::Extensions::XdgShell::Positioner::set_gravity(wl_client *client, wl_resource *resource, UInt32 gravity)
{
    (void)client;
    WPositioner *positioner = (WPositioner*)wl_resource_get_user_data(resource);
    positioner->p_gravity = (Gravity)gravity;
}

void Wpp::Extensions::XdgShell::Positioner::set_constraint_adjustment(wl_client *client, wl_resource *resource, UInt32 constraintAdjustment)
{
    (void)client;
    WPositioner *positioner = (WPositioner*)wl_resource_get_user_data(resource);
    positioner->p_constraintAdjustment = (ConstraintAdjustment)constraintAdjustment;
}

void Wpp::Extensions::XdgShell::Positioner::set_offset(wl_client *client, wl_resource *resource, Int32 x, Int32 y)
{
    (void)client;
    WPositioner *positioner = (WPositioner*)wl_resource_get_user_data(resource);
    positioner->p_offset = WPoint(x,y);
}

void Wpp::Extensions::XdgShell::Positioner::set_reactive(wl_client *client, wl_resource *resource)
{
    (void)client;
    WPositioner *positioner = (WPositioner*)wl_resource_get_user_data(resource);
    positioner->p_isReactive = true;
}

void Wpp::Extensions::XdgShell::Positioner::set_parent_size(wl_client *client, wl_resource *resource, Int32 parentWidth, Int32 parentHeight)
{
    (void)client;
    WPositioner *positioner = (WPositioner*)wl_resource_get_user_data(resource);
    positioner->p_parentSize = WSize(parentWidth,parentHeight);
}

void Wpp::Extensions::XdgShell::Positioner::set_parent_configure(wl_client *client, wl_resource *resource, UInt32 serial)
{
    (void)client;
    //WPositioner *positioner = (WPositioner*)wl_resource_get_user_data(resource);

    // TODO
}
