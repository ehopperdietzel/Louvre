#include "Compositor.h"

#include <signal.h>

void intHandler(int) {}

int main(int, char *[])
{
    // Prevent kill with ctrl+c
    signal(SIGINT,intHandler);

    /*
    eventfd_t v = 10;
    Int32 fd = eventfd(0,EFD_SEMAPHORE);

    eventfd_write(fd,1);
    eventfd_write(fd,1);
    eventfd_write(fd,1);

    while(v > 0)
    {
        eventfd_read(fd,&v);
        printf("%" PRId64 "\n",v);
    }
    */

    // Backend
    const char *backendPath = "./libLBackendDRM.so";
    //const char *backendPath = "./libLBackendX11.so";

    Compositor compositor(backendPath);

    // Start the compositor
    compositor.start();

    return 0;
}
