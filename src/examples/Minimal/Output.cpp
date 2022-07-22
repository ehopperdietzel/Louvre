#include "Output.h"
#include <LOpenGL.h>
#include <LCompositor.h>

Output::Output() : LOutput()
{
    setOutputScale(1);
}

void Output::paintGL(Int32 currentBuffer)
{
    LOpenGL *p = painter();

    p->clearScreen();

    for(LSurface *s : compositor()->surfaces())
    {
        if(!s->mapped() || s->minimized() || s->roleType() == LSurface::Cursor || s->roleType() == LSurface::Undefined)
            continue;

        p->drawTexture(s->texture(),LRect(LPoint(0,0),s->size(true)),LRect(s->pos(true),s->size()));
        s->requestNextFrame();
    }

}
