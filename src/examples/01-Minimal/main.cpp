#include "Compositor.h"

#include <signal.h>

void intHandler(int) {}

int main(int, char *[])
{
    // Prevent kill with ctrl+c
    signal(SIGINT,intHandler);

    // Create the compositor

    //const char *backendPath = "../../../src/backends/DRM/build/libLBackendDRM.so";
    //const char *backendPath = "../../../src/backends/X11/build/libLBackendX11.so";

    //const char *backendPath = "./libLBackendDRM.so";
    const char *backendPath = "./libLBackendX11.so";
    Compositor compositor(backendPath);

    // Start the compositor
    compositor.start();

    return 0;
}
