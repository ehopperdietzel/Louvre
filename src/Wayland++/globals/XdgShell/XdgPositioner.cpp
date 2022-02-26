#include "XdgPositioner.h"
#include <stdio.h>


void WaylandPlus::Extensions::XdgShell::Positioner::destroy_resource(wl_resource *resource)
{
    printf("Xdg Positioner resource destroyed.\n");
    WPositioner *positioner = (WPositioner*)wl_resource_get_user_data(resource);
}

void WaylandPlus::Extensions::XdgShell::Positioner::destroy(wl_client *client, wl_resource *resource)
{
    (void)client;(void)resource;
    printf("Xdg Positioner resource destroyed.\n");
}

void WaylandPlus::Extensions::XdgShell::Positioner::set_size(wl_client *client, wl_resource *resource, Int32 width, Int32 height)
{

}

void WaylandPlus::Extensions::XdgShell::Positioner::set_anchor_rect(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{

}

void WaylandPlus::Extensions::XdgShell::Positioner::set_anchor(wl_client *client, wl_resource *resource, UInt32 anchor)
{

}

void WaylandPlus::Extensions::XdgShell::Positioner::set_gravity(wl_client *client, wl_resource *resource, UInt32 gravity)
{

}

void WaylandPlus::Extensions::XdgShell::Positioner::set_constraint_adjustment(wl_client *client, wl_resource *resource, UInt32 constraintAdjustment)
{

}

void WaylandPlus::Extensions::XdgShell::Positioner::set_offset(wl_client *client, wl_resource *resource, Int32 x, Int32 y)
{

}

void WaylandPlus::Extensions::XdgShell::Positioner::set_reactive(wl_client *client, wl_resource *resource)
{

}

void WaylandPlus::Extensions::XdgShell::Positioner::set_parent_size(wl_client *client, wl_resource *resource, Int32 parent_width, Int32 parent_height)
{

}

void WaylandPlus::Extensions::XdgShell::Positioner::set_parent_configure(wl_client *client, wl_resource *resource, UInt32 serial)
{

}
