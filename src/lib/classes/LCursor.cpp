#include "LCursor.h"
#include <LOutput.h>
#include <LTexture.h>
#include <LPointF.h>
#include <LRect.h>
#include <LOpenGL.h>

#include <X11/Xcursor/Xcursor.h>
#include <string.h>
#include <LCompositor.h>

using namespace Louvre;

LCursor::LCursor(LOutput *output)
{
    setOutput(output);
    p_x11Texture = new LTexture();
    setSize(LPoint(24,24));
    loadDefaultCursors();
    setCursor(LCursor::Arrow);
}

void LCursor::setCursorTheme(const char *themeName)
{
    if(p_cursorTheme != NULL)
        delete []p_cursorTheme;

    Int32 length = strlen(themeName);
    p_cursorTheme = new char(length+1);
    p_cursorTheme[length] = '\0';
    memcpy(p_cursorTheme,themeName,length);
}

void LCursor::setCursor(const char *cursorName)
{
    if(lastCursor.name && lastCursor.type == LLastCursorType::String && strcmp(cursorName,lastCursor.name) == 0)
        return;

    if(lastCursor.name != nullptr)
        delete []lastCursor.name;

    Int32 len = strlen(cursorName);

    lastCursor.name = new char(len+1);
    memcpy(lastCursor.name,cursorName,len);
    lastCursor.name[len] = '\0';

    XcursorImage *cursor =  XcursorLibraryLoadImage(cursorName,p_cursorTheme,64);
    p_x11Texture->setData(cursor->width,cursor->height,cursor->pixels,GL_RGBA,GL_UNSIGNED_BYTE);
    setTexture(p_x11Texture,LPointF(cursor->xhot,cursor->yhot));
    XcursorImageDestroy(cursor);
    lastCursor.type = LLastCursorType::String;
}

void LCursor::setCursor(CursorNames cursor)
{
    if(lastCursor.type == LLastCursorType::Default && lastCursor.defaultName == int(cursor))
        return;

    setTexture(p_cursors[int(cursor)].texture,p_cursors[int(cursor)].hotspot);
    lastCursor.defaultName = int(cursor);
    lastCursor.type = LLastCursorType::Default;
}

void LCursor::setTexture(LTexture *texture, const LPointF &hotspot)
{
    lastCursor.type = LLastCursorType::Texture;
    p_texture = texture;
    p_hotspot = hotspot;
    compositor()->p_backend->setCursor(p_output,p_texture,p_size*p_output->getOutputScale());
    update();
}

void LCursor::setOutput(LOutput *output)
{
    p_output = output;
}

void LCursor::move(float x, float y)
{
    setPosition(p_pos + LPointF(x,y));
    update();
}

void Louvre::LCursor::setPosition(const LPointF &position)
{
    p_pos = position;

    if(p_pos.x() > p_output->rect().w())
        p_pos.setX(p_output->rect().w());
    if(p_pos.x() < p_output->rect().x())
        p_pos.setX(p_output->rect().x());

    if(p_pos.y() > p_output->rect().h())
        p_pos.setY(p_output->rect().h());
    if(p_pos.y() < p_output->rect().y())
        p_pos.setY(p_output->rect().y());

    update();
}

void LCursor::setHotspot(const LPointF &hotspot)
{
    p_hotspot = hotspot;
    update();
}

void LCursor::paint()
{
    if(p_output == nullptr || p_texture == nullptr)
        return;

    LPointF hotspot = (p_hotspot*p_size)/p_texture->size();

    p_output->painter()->drawTexture(p_texture,LRect(LPoint(),p_texture->size()),LRect(p_pos-hotspot,p_size));
}

void LCursor::setSize(const LSizeF &size)
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

bool LCursor::hasHardwareSupport() const
{
    return compositor()->p_backend->hasHardwareCursorSupport();
}

LCompositor *LCursor::compositor() const
{
    return p_output->compositor();
}

void LCursor::loadDefaultCursors()
{
    XcursorImage *cursor =  XcursorLibraryLoadImage("arrow",p_cursorTheme,64);
    p_cursors[0].texture = new LTexture(1);
    p_cursors[0].texture->setData(cursor->width,cursor->height,cursor->pixels,GL_RGBA,GL_UNSIGNED_BYTE);
    p_cursors[0].hotspot.setX(cursor->xhot);
    p_cursors[0].hotspot.setY(cursor->yhot);
    XcursorImageDestroy(cursor);
}

void LCursor::update()
{
    LPoint hotspot;

    if(p_texture != nullptr)
        hotspot = (p_hotspot*p_size)/p_texture->size();

    LPoint pos = (p_pos- hotspot)*p_output->getOutputScale();

    if(pos != p_prevPos)
    {
        p_prevPos = pos;
        compositor()->p_backend->setCursorPosition(p_output,pos);
    }

    if(p_output && !hasHardwareSupport())
        p_output->repaint();
}
