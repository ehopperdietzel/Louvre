#include "MyCompositor.h"

int main(int argc, char *argv[])
{
    (void)argc;(void)argv;

    // Create the compositor
    MyCompositor compositor;

    // Start the compositor
    compositor.start();

    return 0;
}
