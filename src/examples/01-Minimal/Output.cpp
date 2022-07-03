#include "Output.h"
#include <Compositor.h>
#include <LOpenGL.h>
#include <Surface.h>
#include <LToplevelRole.h>

Output::Output():LOutput(){}

void Output::initializeGL()
{
    backgroundTexture = LOpenGL::loadTexture("wallpaper.png");
}

void Output::paintGL(Int32 currentBuffer)
{
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
        LRect r = LRect(s->pos(true),s->size()) ;

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

            // Add exposed rect
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
            if(s->textureChanged() && !s->chg[prevBuffer])
            {
                // Store current damages for next frame
                s->dmg[currentBuffer] = s->damages();

                // Use the current damages as total damages
                s->dmgT = s->damages();
                s->dmgT.offset(s->pos(true));
            }
            // If changed from prev to current but also from (prev prev) to prev
            else if(s->textureChanged() && s->chg[prevBuffer])
            {
                // Store current damages for next frame
                s->dmg[currentBuffer] = s->damages();

                // Use the current damages plus the prev damages as total damage
                s->dmgT = s->damages();
                for(const LRect &d : s->dmg[prevBuffer].rects())
                    s->dmgT.addRect(d);
                s->dmgT.offset(s->pos(true));
            }
            // If nothing has changed
            else if(s->textureChanged() && s->chg[prevBuffer])
            {
                // Empty damages for next frame
                s->dmg[currentBuffer].clear();

                // Empty current total damages
                s->dmgT.clear();
            }
            // If nothing has changed since last frame but from (prev prev) to prev
            else if(!s->textureChanged() && s->chg[prevBuffer])
            {
                // Empty damages for next frame
                s->dmg[currentBuffer].clear();

                // Use prev damages as total damage
                s->dmgT = s->dmg[prevBuffer];
                s->dmgT.offset(s->pos(true));
            }


        }

        // Let the client know it can start rendering the next frame (After calling this, textureChanged() returns false until new damages)
        s->requestNextFrame();
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

        s->opaqueAt = opaqueDamages;

        LRegion opaqueT = s->dmgT;
        s->transT = s->dmgT;

        // Calc the translucent region
        LRegion transR = s->translucentRegion();
        transR.offset(s->pos(true));

        // Subtract opaqueDamages
        for(const LRect &d : opaqueDamages.rects())
            opaqueT.subtractRect(d);

        // Check for exposed
        LRegion exposed = exposedRegion[currentBuffer];
        exposed.clip(LRect(s->pos(true),s->size()));

        // Add exposed regions
        for(const LRect &d : exposed.rects())
            opaqueT.addRect(d);

        // Subtract transparent region
        for(const LRect &d : transR.rects())
            opaqueT.subtractRect(d);

        // Subtract to exposed
        for(const LRect &d : opaqueT.rects())
            exposedRegion[currentBuffer].subtractRect(d);

        opaqueT.clip(rect());

        // Draw opaque rects
        for(const LRect &d : opaqueT.rects())
        {
            p->drawTexture(s->texture(),LRect(d.topLeft()-s->pos(true)-rect().topLeft(),d.bottomRight())*s->bufferScale(),LRect(d.topLeft()-rect().topLeft(),d.bottomRight()));

            // Add the opaque rects
            opaqueDamages.addRect(d);
        }

        LRegion opaqueR = s->opaqueRegion();
        opaqueR.offset(s->pos(true));

        // Mark opaque rects
        for(const LRect &d : opaqueR.rects())
        {
            // Add the opaque rects
            opaqueDamages.addRect(d);

            s->transT.subtractRect(d);
        }

        // Add to exposed
        for(const LRect &d : s->transT.rects())
            exposedRegion[currentBuffer].addRect(d);

    }

    exposedRegion[currentBuffer].clip(rect());

    // Render background
    if(!fullScreen)
    {
        for(const LRect &d : exposedRegion[currentBuffer].rects())
        {
            p->drawTexture(backgroundTexture,d*getOutputScale(),d);
        }
    }

    // Render translucent damages from back to front
    glEnable(GL_BLEND);

    for(Surface *s : surfaces)
    {
        // Again skip some surfaces
        if(s->roleType() == LSurface::Cursor || s->roleType() == LSurface::Undefined || s->minimized() || (fullScreen && s != fullScreen))
            continue;

        LRegion transT = exposedRegion[currentBuffer];
        transT.clip(s->rct[currentBuffer]);

        for(const LRect &d : transT.rects())
            s->transT.addRect(d);

        for(const LRect &d : s->opaqueAt.rects())
            s->transT.subtractRect(d);

        s->transT.clip(rect());

        // Draw opaque rects
        for(const LRect &d : s->transT.rects())
        {
            p->drawTexture(s->texture(),LRect(d.topLeft()-s->pos(true)-rect().topLeft(),d.bottomRight())*s->bufferScale(),LRect(d.topLeft()-rect().topLeft(),d.bottomRight()));
        }
    }






    exposedRegion[currentBuffer].clear();
}
