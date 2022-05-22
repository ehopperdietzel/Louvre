#include "MyCompositor.h"

#include <signal.h>

void intHandler(int) {}

int main(int argc, char *argv[])
{
    (void)argc;(void)argv;

    // Prevent kill with ctrl+c
    signal(SIGINT,intHandler);

    // Create the compositor
    MyCompositor compositor;

    // Start the compositor
    compositor.start();

    return 0;
}
