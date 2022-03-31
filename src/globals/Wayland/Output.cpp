#include "Output.h"
#include <WCompositor.h>
#include <WOutput.h>

using namespace std;

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

    // TODO: This should be updated when the compositor screen changes

    wl_resource *resource = wl_resource_create(client, &wl_output_interface, version, id);
    wl_resource_set_implementation(resource, &output_implementation, NULL, &Output::resource_destroy);
    wl_output_send_geometry(
        resource,
        0,
        0,
        2880,
        1800,
        WL_OUTPUT_SUBPIXEL_HORIZONTAL_RGB,
        "Fake name\0",
        "Fake Model\0",
        wl_output_transform::WL_OUTPUT_TRANSFORM_NORMAL);

    wl_output_send_mode(
        resource,
        wl_output_mode::WL_OUTPUT_MODE_CURRENT,
        2880,
        1800,
        60);

    wl_output_send_scale(resource, compositor->getOutputs().front()->getOutputScale());
    wl_output_send_done(resource);

    /*
    list<WOutput*>outputs = compositor->getOutputs();

    for(list<WOutput*>::iterator it = outputs.begin(); it != outputs.end(); ++it)
    {
        WOutput *output = *it;
        wl_resource *resource = wl_resource_create(client, &wl_output_interface, version, id);
        wl_resource_set_implementation(resource, &output_implementation, output, &Output::resource_destroy);
        wl_output_send_geometry(
            resource,
            0,
            0,
            output->getCurrentConnector().mmWidth,
            output->getCurrentConnector().mmHeight,
            output->getCurrentConnector().subpixel,
            output->getCurrentMode().name,
            "Fake Model",
            wl_output_transform::WL_OUTPUT_TRANSFORM_NORMAL);

        wl_output_send_mode(
            resource,
            wl_output_mode::WL_OUTPUT_MODE_CURRENT,
            output->getCurrentMode().hdisplay,
            output->getCurrentMode().vdisplay,
            output->getCurrentMode().vrefresh);

        wl_output_send_scale(resource, output->getOutputScale());
        wl_output_send_done(resource);
    }
    */

}
