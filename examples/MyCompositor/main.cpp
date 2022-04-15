#include <stdio.h>
#include <wayland-server.h>
#include <MyCompositor.h>

#include <WPoint.h>

using namespace Wpp;

int main(int argc, char *argv[])
{
    (void)argc;(void)argv;

    WPoint A(2,4);
    WPoint B(2,2);

    WPoint C = (A + B)*B + 1.1f;

    printf("X:%i,Y:%i\n",C.x(),C.y());

    /* Create and start compositor */
    MyCompositor *compositor = new MyCompositor();
    compositor->start();
    return 0;
}
