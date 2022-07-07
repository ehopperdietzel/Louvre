#include <LCompositor.h>

#include <signal.h>

using namespace Louvre;

void intHandler(int) {}

int main(int, char *[])
{
    // Prevent kill with ctrl+c
    //signal(SIGINT,intHandler);

    // Backend
    //const char *backendPath = "./libLBackendDRM.so";
    const char *backendPath = "./libLBackendX11.so";

    // Start the compositor
    LCompositor compositor(backendPath);
    compositor.start();

    return 0;
}
