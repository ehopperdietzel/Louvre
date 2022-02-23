#include "Output.h"
#include <WCompositor.h>


static struct wl_output_interface output_implementation = {&WaylandPlus::Globals::Output::release};

void WaylandPlus::Globals::Output::release(wl_client *client,wl_resource *resource)
{
    (void)client;(void)resource;
}

void WaylandPlus::Globals::Output::resource_destroy(wl_resource *resource)
{
    (void)resource;
}

void WaylandPlus::Globals::Output::bind(wl_client *client, void *data, UInt32 version, UInt32 id)
{
    WCompositor *compositor = (WCompositor*)data;
    wl_resource *resource = wl_resource_create(client, &wl_output_interface, version, id);
    wl_resource_set_implementation(resource, &output_implementation, nullptr, &Output::resource_destroy);

    // TODO: This should be updated when the compositor screen changes
    wl_output_send_geometry(resource,0,0,compositor->screenWidth(),compositor->screenHeight(),wl_output_subpixel::WL_OUTPUT_SUBPIXEL_HORIZONTAL_RGB,"Cuarzo Software","Fake Model",wl_output_transform::WL_OUTPUT_TRANSFORM_NORMAL);
    wl_output_send_mode(resource,wl_output_mode::WL_OUTPUT_MODE_CURRENT,compositor->screenWidth(),compositor->screenHeight(),60000);
    wl_output_send_scale(resource,1);
    wl_output_send_done(resource);

}
