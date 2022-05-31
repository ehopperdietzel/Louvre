#include "Compositor.h"

#include <signal.h>

void intHandler(int) {}

int main(int, char *[])
{
    // Prevent kill with ctrl+c
    signal(SIGINT,intHandler);

    // Create the compositor
    Compositor compositor;

    // Start the compositor
    compositor.start();

    return 0;
}
