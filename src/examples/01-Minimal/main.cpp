#include "Compositor.h"

#include <signal.h>

void intHandler(int) {}

int main(int, char *[])
{
    // Prevent kill with ctrl+c
    //signal(SIGINT,intHandler);

    // Backend
    const char *backendPath = "./libLBackendDRM.so";
    //const char *backendPath = "./libLBackendX11.so";

    Compositor compositor(backendPath);

    // Start the compositor
    compositor.start();

    return 0;
}
