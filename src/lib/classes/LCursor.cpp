#include "LCursor.h"
#include <LOutput.h>
#include <LTexture.h>
#include <LPointF.h>
#include <LRect.h>
#include <LOpenGL.h>

#include <X11/Xcursor/Xcursor.h>
#include <string.h>

#include <LCompositorPrivate.h>

using namespace Louvre;

LCursor::LCursor(LOutput *output)
{
    setOutput(output);
    m_x11Texture = new LTexture();
    setSize(LPoint(24,24));
    loadDefaultCursors();
    setCursor(LCursor::Arrow);
}

void LCursor::setCursorTheme(const char *themeName)
{
    if(m_cursorTheme != NULL)
        delete []m_cursorTheme;

    Int32 length = strlen(themeName);
    m_cursorTheme = new char(length+1);
    m_cursorTheme[length] = '\0';
    memcpy(m_cursorTheme,themeName,length);
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

    XcursorImage *cursor =  XcursorLibraryLoadImage(cursorName,m_cursorTheme,64);
    m_x11Texture->setData(cursor->width,cursor->height,cursor->pixels,GL_RGBA,GL_UNSIGNED_BYTE);
    setTexture(m_x11Texture,LPointF(cursor->xhot,cursor->yhot));
    XcursorImageDestroy(cursor);
    lastCursor.type = LLastCursorType::String;
}

void LCursor::setCursor(CursorNames cursor)
{
    if(lastCursor.type == LLastCursorType::Default && lastCursor.defaultName == int(cursor))
        return;

    setTexture(m_cursors[int(cursor)].texture,m_cursors[int(cursor)].hotspot);
    lastCursor.defaultName = int(cursor);
    lastCursor.type = LLastCursorType::Default;
}

void LCursor::setTexture(LTexture *texture, const LPointF &hotspot)
{
    lastCursor.type = LLastCursorType::Texture;
    m_texture = texture;
    m_hotspot = hotspot;
    compositor()->imp()->m_backend->setCursor(m_output,m_texture,m_size*m_output->getOutputScale());
    update();
}

void LCursor::setOutput(LOutput *output)
{
    if(output == m_output)
        return;

    if(m_output)
        setVisible(false);
    m_output = output;
    setVisible(true);
}

void LCursor::move(float x, float y)
{
    setPosition(m_pos + LPointF(x,y));
    update();
}

void Louvre::LCursor::setPosition(const LPointF &position)
{
    if(!m_output)
        return;

    for(LOutput *output : compositor()->outputs())
    {
        if(output->rect().containsPoint(position) && output)
            setOutput(output);
    }

    m_pos = position;

    LRect area = m_output->rect();
    if(m_pos.x() > area.x() + area.w())
        m_pos.setX(area.x() + area.w());
    if(m_pos.x() < area.x())
        m_pos.setX(area.x());

    if(m_pos.y() > area.y() + area.h())
        m_pos.setY(area.y() + area.h());
    if(m_pos.y() < area.y())
        m_pos.setY(area.y());

    update();
}

void LCursor::setHotspot(const LPointF &hotspot)
{
    m_hotspot = hotspot;
    update();
}

void LCursor::paint()
{
    if(!visible() || !m_output || !m_texture)
        return;

    LPointF hotspot = (m_hotspot*m_size)/m_texture->size();
    glEnable(GL_BLEND);
    m_output->painter()->drawTexture(m_texture,LRect(LPoint(),m_texture->size()),LRect(m_pos-hotspot,m_size));
}

void LCursor::setSize(const LSizeF &size)
{
    m_size = size;

    if(hasHardwareSupport())
    {
        if(size.w() > 64.f)
            m_size.setW(64.f);

        if(size.h() > 64.f)
            m_size.setH(64.f);
    }

    update();
}

void LCursor::setVisible(bool state)
{
    if(state == visible())
        return;

    m_isVisible = state;

    if(!m_isVisible)
        compositor()->imp()->m_backend->setCursor(m_output,nullptr,LPoint());
    else if(m_texture)
    {
        compositor()->imp()->m_backend->setCursor(m_output,m_texture,m_size*m_output->getOutputScale());
        update();
    }

}

bool LCursor::visible() const
{
    return m_isVisible;
}

bool LCursor::hasHardwareSupport() const
{
    return compositor()->imp()->m_backend->hasHardwareCursorSupport();
}

LCompositor *LCursor::compositor() const
{
    return m_output->compositor();
}

LOutput *LCursor::output() const
{
    return m_output;
}

void LCursor::loadDefaultCursors()
{
    XcursorImage *cursor =  XcursorLibraryLoadImage("arrow",m_cursorTheme,64);
    m_cursors[0].texture = new LTexture(1);
    m_cursors[0].texture->setData(cursor->width,cursor->height,cursor->pixels,GL_RGBA,GL_UNSIGNED_BYTE);
    m_cursors[0].hotspot.setX(cursor->xhot);
    m_cursors[0].hotspot.setY(cursor->yhot);
    XcursorImageDestroy(cursor);
}

void LCursor::update()
{

    if(!visible() || !m_output || !m_texture)
        return;

    LPoint hotspot;
    hotspot = (m_hotspot*m_size)/m_texture->size();

    LPoint pos = (m_pos- hotspot - m_output->rect().topLeft())*m_output->getOutputScale();

    if(pos != m_prevPos)
    {
        m_prevPos = pos;
        compositor()->imp()->m_backend->setCursorPosition(m_output,pos);
    }

    if(!hasHardwareSupport())
        m_output->repaint();
}
