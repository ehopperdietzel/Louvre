#include "WCursor.h"
#include <WOutput.h>
#include <WTexture.h>
#include <WPointF.h>
#include <WRect.h>
#include <WOpenGL.h>

#include <X11/Xcursor/Xcursor.h>
#include <string.h>

using namespace Wpp;

WCursor::WCursor(WOutput *output)
{
    setOutput(output);
    p_x11Texture = new WTexture();
    setSize(WPoint(24,24));
    setCursor("arrow");
}

void WCursor::setCursorTheme(const char *themeName)
{
    if(p_cursorTheme != NULL)
        delete []p_cursorTheme;

    Int32 length = strlen(themeName);
    p_cursorTheme = new char(length+1);
    p_cursorTheme[length] = '\0';
    memcpy(p_cursorTheme,themeName,length);
}

void WCursor::setCursor(const char *cursorName)
{
    XcursorImage *cursor =  XcursorLibraryLoadImage(cursorName,p_cursorTheme,64);
    p_x11Texture->setData(cursor->width,cursor->height,cursor->pixels,GL_RGBA,GL_UNSIGNED_BYTE);
    setTexture(p_x11Texture,WPointF(cursor->xhot,cursor->yhot));
    XcursorImageDestroy(cursor);
}

void WCursor::setTexture(WTexture *texture, const WPointF &hotspot)
{
    WBackend::setCursor(p_output,texture,p_size*p_output->getOutputScale());
    p_texture = texture;
    p_hotspot = hotspot;
    update();
}

void WCursor::setOutput(WOutput *output)
{
    p_output = output;
}

void WCursor::move(float x, float y)
{
    p_pos += WPointF(x,y);
    update();
}

void Wpp::WCursor::setPosition(const WPointF &position)
{
    p_pos = position;
    update();
}

void WCursor::setHotspot(const WPointF &hotspot)
{
    p_hotspot = hotspot;
    update();
}

void WCursor::paint()
{
    if(p_output == nullptr || p_texture == nullptr)
        return;

    WPointF hotspot = (p_hotspot*p_size)/p_texture->size();

    p_output->painter()->drawTexture(p_texture,WRect(WPoint(),p_texture->size()),WRect(p_pos-hotspot,p_size));
}

void WCursor::setSize(const WSizeF &size)
{
    p_size = size;

    if(hasHardwareSupport())
    {
        if(size.w() > 64.f)
            p_size.setW(64.f);

        if(size.h() > 64.f)
            p_size.setH(64.f);
    }

    update();
}

void WCursor::update()
{
    WPoint hotspot;

    if(p_texture != nullptr)
        hotspot = (p_hotspot*p_size)/p_texture->size();

    WPoint pos = (p_pos- hotspot)*p_output->getOutputScale();

    if(pos != p_prevPos)
    {
        p_prevPos = pos;
        WBackend::setCursorPosition(p_output,pos);
    }

    if(p_output && !hasHardwareSupport())
        p_output->repaint();
}
