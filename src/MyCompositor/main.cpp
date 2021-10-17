#include <stdio.h>
#include <wayland-server.h>
#include <MyCompositor.h>

int main(int argc, char *argv[])
{
    (void)argc;(void)argv;

    /* Create and start compositor */
    MyCompositor *compositor = new MyCompositor();
    compositor->start();
    return 0;
}
