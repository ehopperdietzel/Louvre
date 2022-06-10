#include "Compositor.h"

#include <signal.h>

void intHandler(int) {}

int main(int, char *[])
{
    // Prevent kill with ctrl+c
    //signal(SIGINT,intHandler);

    // Create the compositor
    Compositor compositor("../../../src/backends/DRM/build/libLBackendDRM.so");

    // Start the compositor
    compositor.start();

    return 0;
}
