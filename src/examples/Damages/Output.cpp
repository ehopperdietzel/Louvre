#include "Output.h"
#include <Compositor.h>
#include <LOpenGL.h>
#include <Surface.h>
#include <LToplevelRole.h>
#include <unistd.h>
#include <LTime.h>
#include <LCursor.h>
#include <SOIL/SOIL.h>

Output::Output():LOutput(){}

void Output::initializeGL()
{
    backgroundTexture = LOpenGL::loadTexture("wallpaper.png");
    first[0] = true;
    first[1] = true;
}

void paintRandom(const LRect &r,LOpenGL *p)
{
    float R = float(rand()%1000)/1000.f;
    float G = float(rand()%1000)/1000.f;
    float B = float(rand()%1000)/1000.f;
    p->drawColor(r,1,0,0,0.5);
}
void Output::paintGL(Int32 currentBuffer)
{
    //Int32 t = LTime::ms();

    if(fullRefresh)
    {
        fullRefresh = false;
        exposedRegion[0].addRect(rect(true));
        exposedRegion[1].addRect(rect(true));
    }

    LOpenGL *p = painter();

    // Prev buffer index
    Int32 prevBuffer = 1 - currentBuffer;

    // Parse the compositor surfaces list
    list<Surface*> &surfaces = (list<Surface*>&)compositor()->surfaces();

    // Check if fullscreen
    Surface *fullScreen = nullptr;
    for(Surface *s: surfaces)
    {
        if(s->toplevel() && s->toplevel()->fullscreen())
        {
            fullScreen = s;
            break;
        }
    }

    // Calc surface damages
    for(Surface *s: surfaces)
    {
        // Skip some surfaces
        if(s->roleType() == LSurface::Cursor || s->roleType() == LSurface::Undefined || s->minimized() || (fullScreen && s != fullScreen))
            continue;

        // Calc rect
        LRect r = LRect(s->pos(true),s->size());

        // Check if pos or size changed (since 2 frames before)
        bool rectChanged = r != s->rct[currentBuffer];

        // Clear total surface damage
        s->dmgT.clear();

        // Store if texture changed for next frame
        s->chg[currentBuffer] = s->textureChanged();

        if(rectChanged || s->changedOrder[currentBuffer])
        {
            // Damages the entire surface
            s->dmgT.addRect(r);

            // Add exposed rect (new rect - old rect)
            LRegion exposed;
            exposed.addRect(s->rct[currentBuffer]);

            if(!s->changedOrder[currentBuffer])
                exposed.subtractRect(r);

            for(const LRect &d : exposed.rects())
                exposedRegion[currentBuffer].addRect(d);

            // Store current damages for next frame
            s->dmg[currentBuffer] = s->damages();

            s->changedOrder[currentBuffer] = false;

            s->rct[currentBuffer] = r;

        }
        else
        {

            // If only changed from prev to current
            if(s->chg[currentBuffer] && !s->chg[prevBuffer])
            {
                //printf("A\n");
                // Store current damages for next frame
                s->dmg[currentBuffer] = s->damages();

                // Use the current damages as total damages
                s->dmgT = s->damages();
                s->dmgT.offset(s->pos(true));
            }
            // If changed from prev to current but also from (prev prev) to prev
            else if(s->chg[currentBuffer] && s->chg[prevBuffer])
            {
                //printf("B\n");

                // Store current damages for next frame
                s->dmg[currentBuffer] = s->damages();

                // Use the current damages plus the prev damages as total damage
                s->dmgT = s->damages();

                for(const LRect &d : s->dmg[prevBuffer].rects())
                    s->dmgT.addRect(d);

                s->dmgT.offset(s->pos(true));

            }
            // If nothing has changed
            else if(!s->chg[currentBuffer] && !s->chg[prevBuffer])
            {
                //printf("C\n");

                // Empty damages for next frame
                s->dmg[currentBuffer].clear();

            }
            // If nothing has changed since last frame but from (prev prev) to prev
            else if(!s->chg[currentBuffer] && s->chg[prevBuffer])
            {
                //printf("D\n");
                // Empty damages for next frame
                s->dmg[currentBuffer].clear();

                // Use prev damages as total damage
                s->dmgT = s->dmg[prevBuffer];
                s->dmgT.offset(s->pos(true));
            }

        }

         s->requestNextFrame();
    }

    // Add cursor to exposed
    if(!compositor()->cursor()->hasHardwareSupport())
    {
        exposedRegion[currentBuffer].addRect(cursorRect[currentBuffer]);
        exposedRegion[currentBuffer].addRect(compositor()->cursor()->rect());
        cursorRect[currentBuffer] = compositor()->cursor()->rect();
    }

    LRegion opaqueDamages;

    glDisable(GL_BLEND);

    // Render opaque damages from front to back
    for(list<Surface*>::reverse_iterator it = surfaces.rbegin(); it != surfaces.rend(); it++)
    {
        Surface *s = *it;

        // Again skip some surfaces
        if(s->roleType() == LSurface::Cursor || s->roleType() == LSurface::Undefined || s->minimized() || (fullScreen && s != fullScreen))
            continue;

        // Calc the opaque region
        s->opaqueR = s->opaqueRegion();
        s->opaqueR.offset(s->pos(true));

        // Calc the translucent region
        s->transR = s->translucentRegion();
        s->transR.offset(s->pos(true));

        s->opaqueT = s->dmgT;

        // Current surface rect
        LRect &r = s->rct[currentBuffer];

        LRegion hidden;
        hidden.addRect(LRect(s->pos(true),s->size()));

        // Subtract opaqueDamages (sub opaque regions from surfaces above)
        for(const LRect &d : opaqueDamages.rects())
        {
            s->opaqueT.subtractRect(d);
            hidden.subtractRect(d);
        }

        if(!hidden.rects().empty())
            s->requestNextFrame();


        // Add exposed regions
        for(const LRect &d : exposedRegion[currentBuffer].rects())
            s->opaqueT.addRect(d);

        s->opaqueT.clip(r);

        // Calc inv trans damage
        s->transT = s->opaqueT;

        // Subtract opque region
        for(const LRect &d : s->opaqueR.rects())
        {
            s->transT.subtractRect(d);
            opaqueDamages.addRect(d);
            exposedRegion[currentBuffer].subtractRect(d);
        }

        // Subtract transparent region
        for(const LRect &d : s->transR.rects())
            s->opaqueT.subtractRect(d);

        //s->opaqueT.clip(rect());

        // Add damaged transparency to exposed
        for(const LRect &d : s->transT.rects())
            exposedRegion[currentBuffer].addRect(d);


        // Draw opaque rects
        for(const LRect &d : s->opaqueT.rects())
        {

            p->drawTexture(s->texture(),LRect(d.topLeft() - s->pos(true),d.bottomRight())*s->bufferScale(),d);

            //printf("(%i,%i,%i,%i)\n",d.x(),d.y(),d.w(),d.h());
            //paintRandom(d,painter());
        }

        //printf("TOTAL RECTS %lu\n",s->opaqueT.rects().size());


    }


    exposedRegion[currentBuffer].clip(rect());

    // Render background
    if(!fullScreen)
    {
        for(const LRect &d : exposedRegion[currentBuffer].rects())
        {
            p->drawTexture(backgroundTexture,d*getOutputScale(),d);
            //p->drawColor(d,0.3,0.3,0.7,1);
            //paintRandom(d,painter());
        }
    }


    // Render translucent damages from back to front
    glEnable(GL_BLEND);

    LRegion totalRendered;


    for(Surface *s : surfaces)
    {
        // Again skip some surfaces
        if(s->roleType() == LSurface::Cursor || s->roleType() == LSurface::Undefined || s->minimized() || (fullScreen && s != fullScreen))
            continue;

        LRect &r = s->rct[currentBuffer];

        s->transT = exposedRegion[currentBuffer];

        s->transT.clip(r);

        for(const LRect &d : totalRendered.rects())
            s->transT.addRect(d);

        for(const LRect &d : s->opaqueR.rects())
            s->transT.subtractRect(d);

        s->transT.clip(r);

        // Draw opaque rects
        for(const LRect &d : s->transT.rects())
            p->drawTexture(s->texture(),LRect(d.topLeft() - s->pos(true),d.bottomRight())*s->bufferScale(),d);

        for(const LRect &d : s->opaqueT.rects())
            totalRendered.addRect(d);

        //if(!s->transT.rects().empty() || !s->opaqueT.rects().empty())


    }  

    if(first[currentBuffer])
    {
        p->drawColor(LRect(0,0,rect().w(),LOUVRE_TB_H/getOutputScale()),1,1,1,0.75);
        first[currentBuffer] = false;
    }
    else if(!fullScreen)
    {
        exposedRegion[currentBuffer].subtractRect(LRect(0,LOUVRE_TB_H/getOutputScale(),rect().w(),rect().h()));

        for(const LRect &d : exposedRegion[currentBuffer].rects())
            p->drawColor(d,1,1,1,0.75);
    }

    exposedRegion[currentBuffer].clear();

    //Int32 f = LTime::ms() - t;

    /*
    UChar8 pix[600*1000*4];
    glReadPixels(0,600,1000,600,GL_RGBA,GL_UNSIGNED_BYTE,pix);
    LTexture *tex = new LTexture();
    tex->setData(1000,600,pix,GL_RGBA,GL_UNSIGNED_BYTE);
    p->drawTexture(tex,LRect(0,0,1000,600), LRect(400,400,200,200));
    */

    if(sc)
    {
        sc = false;
        UChar8 *pix = new UChar8[2880*1800*4];
        glReadPixels(0,0,2880,1800,GL_RGBA,GL_UNSIGNED_BYTE,pix);

        SOIL_save_image("/home/eduardo/Desktop/sh.bmp",SOIL_SAVE_TYPE_BMP,2880,1800,4,pix);

        delete []pix;
    }
}
