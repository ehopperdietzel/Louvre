#include <stdio.h>
#include <wayland-server.h>
#include <MyCompositor.h>

#include <WPoint.h>

using namespace Wpp;

int main(int argc, char *argv[])
{
    (void)argc;(void)argv;

    /* Create and start the compositor */
    MyCompositor *compositor = new MyCompositor();
    compositor->start();
    return 0;
}
