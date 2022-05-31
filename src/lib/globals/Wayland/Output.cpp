#include "Output.h"
#include <LCompositor.h>
#include <LOutput.h>

using namespace std;

static struct wl_output_interface output_implementation = {&Louvre::Globals::Output::release};

void Louvre::Globals::Output::release(wl_client *client,wl_resource *resource)
{
    (void)client;(void)resource;
    Output::resource_destroy(resource);
}

void Louvre::Globals::Output::resource_destroy(wl_resource *resource)
{
    (void)resource;
    printf("OUTPUT DESTROYED.\n");
}

void Louvre::Globals::Output::bind(wl_client *client, void *data, UInt32 version, UInt32 id)
{
    LCompositor *compositor = (LCompositor*)data;

    // TODO: This should be updated when the compositor screen changes

    LOutput *output = compositor->outputs().front();
    wl_resource *resource = wl_resource_create(client, &wl_output_interface, version, id);
    wl_resource_set_implementation(resource, &output_implementation, output, &Output::resource_destroy);
    wl_output_send_geometry(
        resource,
        output->rect(false).x(),
        output->rect(false).y(),
        output->rect(false).w(),
        output->rect(false).h(),
        WL_OUTPUT_SUBPIXEL_HORIZONTAL_RGB,
        "Fake name\0",
        "Fake Model\0",
        WL_OUTPUT_TRANSFORM_NORMAL);

    wl_output_send_mode(
        resource,
        wl_output_mode::WL_OUTPUT_MODE_CURRENT,
        output->rect(false).w(),
        output->rect(false).h(),
        60);

    wl_output_send_scale(resource, compositor->outputs().front()->getOutputScale());
    wl_output_send_done(resource);

    /*
    list<LOutput*>outputs = compositor->getOutputs();

    for(list<LOutput*>::iterator it = outputs.begin(); it != outputs.end(); ++it)
    {
        LOutput *output = *it;
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
