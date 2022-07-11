#include <Compositor.h>

using namespace Louvre;

int main(int, char *[])
{
    // Backend
    const char *backendPath = "./libLBackendDRM.so";

    // Start the compositor
    Compositor compositor(backendPath);
    compositor.start();

    return 0;
}
