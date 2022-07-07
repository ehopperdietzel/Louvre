#include "Compositor.h"

int main(int, char *[])
{
    // Backend
    const char *backendPath = "./libLBackendX11.so";

    // Start the compositor
    Compositor compositor(backendPath);
    compositor.start();

    return 0;
}
