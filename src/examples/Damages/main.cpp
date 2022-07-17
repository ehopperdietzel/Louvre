#include "Compositor.h"

#include <signal.h>

void intHandler(int) {}

int main(int, char *[])
{
    // Prevent kill with ctrl+c
    signal(SIGINT,intHandler);

    // Backend
    const char *backendPath = "./libLBackendDRM.so";

    // Start the compositor
    Compositor compositor(backendPath);
    compositor.start();

    return 0;
}
