#include "XdgPositioner.h"
#include <stdio.h>
#include <WPositioner.h>
#include <WSurface.h>


void WaylandPlus::Extensions::XdgShell::Positioner::destroy_resource(wl_resource *resource)
{
    printf("Xdg Positioner resource destroyed.\n");
    WPositioner *positioner = (WPositioner*)wl_resource_get_user_data(resource);
    delete positioner;
}

void WaylandPlus::Extensions::XdgShell::Positioner::destroy(wl_client *client, wl_resource *resource)
{
    (void)client;(void)resource;
    printf("Xdg Positioner resource destroyed.\n");
}

void WaylandPlus::Extensions::XdgShell::Positioner::set_size(wl_client *client, wl_resource *resource, Int32 width, Int32 height)
{
    (void)client;
    WPositioner *positioner = (WPositioner*)wl_resource_get_user_data(resource);
    positioner->_size = {width,height};
    if(positioner->_linkedSurface != nullptr)
        positioner->_linkedSurface->positionerChangeRequest();
}

void WaylandPlus::Extensions::XdgShell::Positioner::set_anchor_rect(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{
    (void)client;
    WPositioner *positioner = (WPositioner*)wl_resource_get_user_data(resource);
    positioner->_anchorRect = {x,y,width,height};
    if(positioner->_linkedSurface != nullptr)
        positioner->_linkedSurface->positionerChangeRequest();
}

void WaylandPlus::Extensions::XdgShell::Positioner::set_anchor(wl_client *client, wl_resource *resource, UInt32 anchor)
{
    (void)client;
    WPositioner *positioner = (WPositioner*)wl_resource_get_user_data(resource);
    positioner->_anchor = (Anchor)anchor;
    if(positioner->_linkedSurface != nullptr)
        positioner->_linkedSurface->positionerChangeRequest();
}

void WaylandPlus::Extensions::XdgShell::Positioner::set_gravity(wl_client *client, wl_resource *resource, UInt32 gravity)
{
    (void)client;
    WPositioner *positioner = (WPositioner*)wl_resource_get_user_data(resource);
    positioner->_gravity = (Gravity)gravity;
    if(positioner->_linkedSurface != nullptr)
        positioner->_linkedSurface->positionerChangeRequest();
}

void WaylandPlus::Extensions::XdgShell::Positioner::set_constraint_adjustment(wl_client *client, wl_resource *resource, UInt32 constraintAdjustment)
{
    (void)client;
    WPositioner *positioner = (WPositioner*)wl_resource_get_user_data(resource);
    positioner->_constraintAdjustment = (ConstraintAdjustment)constraintAdjustment;
    if(positioner->_linkedSurface != nullptr)
        positioner->_linkedSurface->positionerChangeRequest();
}

void WaylandPlus::Extensions::XdgShell::Positioner::set_offset(wl_client *client, wl_resource *resource, Int32 x, Int32 y)
{
    (void)client;
    WPositioner *positioner = (WPositioner*)wl_resource_get_user_data(resource);
    positioner->_offset = {x,y};
    if(positioner->_linkedSurface != nullptr)
        positioner->_linkedSurface->positionerChangeRequest();
}

void WaylandPlus::Extensions::XdgShell::Positioner::set_reactive(wl_client *client, wl_resource *resource)
{
    (void)client;
    WPositioner *positioner = (WPositioner*)wl_resource_get_user_data(resource);
    if(positioner->_linkedSurface != nullptr)
        positioner->_linkedSurface->positionerChangeRequest();
}

void WaylandPlus::Extensions::XdgShell::Positioner::set_parent_size(wl_client *client, wl_resource *resource, Int32 parent_width, Int32 parent_height)
{
    (void)client;
    WPositioner *positioner = (WPositioner*)wl_resource_get_user_data(resource);
    positioner->_parentSize = {parent_width,parent_height};
    if(positioner->_linkedSurface != nullptr)
        positioner->_linkedSurface->positionerChangeRequest();
}

void WaylandPlus::Extensions::XdgShell::Positioner::set_parent_configure(wl_client *client, wl_resource *resource, UInt32 serial)
{
    (void)client;
    WPositioner *positioner = (WPositioner*)wl_resource_get_user_data(resource);
    if(positioner->_linkedSurface != nullptr)
        positioner->_linkedSurface->positionerChangeRequest();
}
