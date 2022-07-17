#include <LCursorPrivate.h>
#include <LOutputPrivate.h>
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
    m_imp = new LCursorPrivate();
    m_imp->cursor = this;
    setOutput(output);
    m_imp->x11Texture = new LTexture();
    setSize(LPoint(22,22));
    m_imp->loadDefaultCursors();
}

LCursor::~LCursor()
{
    delete m_imp;
}

/*
void LCursor::setCursorTheme(const char *themeName)
{
    if(m_cursorTheme != NULL)
        delete []m_cursorTheme;

    Int32 length = strlen(themeName);
    m_cursorTheme = new char(length+1);
    m_cursorTheme[length] = '\0';
    memcpy(m_cursorTheme,themeName,length);
}
*/

/*
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
*/
void LCursor::setCursor(CursorNames cursor)
{
    if(m_imp->lastCursor.type == LCursorPrivate::LLastCursorType::Default && m_imp->lastCursor.defaultName == int(cursor))
        return;

    setTexture(m_imp->cursors[int(cursor)].texture,m_imp->cursors[int(cursor)].hotspot);
    m_imp->lastCursor.defaultName = int(cursor);
    m_imp->lastCursor.type = LCursorPrivate::LLastCursorType::Default;
}

void LCursor::setTexture(LTexture *texture, const LPointF &hotspot)
{
    m_imp->lastCursor.type = LCursorPrivate::LLastCursorType::Texture;
    m_imp->texture = texture;
    m_imp->hotspot = hotspot;
    m_imp->update();
    compositor()->imp()->m_backend->setCursor(m_imp->output,m_imp->texture,m_imp->size*m_imp->output->getOutputScale());
}

void LCursor::setOutput(LOutput *output)
{
    if(output == m_imp->output)
        return;

    if(m_imp->output)
        setVisible(false);

    m_imp->output = output;
    setVisible(true);
}

void LCursor::move(float x, float y)
{
    setPosition(m_imp->pos + LPointF(x,y));
    m_imp->update();
}

void Louvre::LCursor::setPosition(const LPointF &position)
{
    if(!m_imp->output)
        return;

    for(LOutput *output : compositor()->outputs())
    {
        if(output->rect().containsPoint(position) && output)
            setOutput(output);
    }

    m_imp->pos = position;

    LRect area = m_imp->output->rect();
    if(m_imp->pos.x() > area.x() + area.w())
        m_imp->pos.setX(area.x() + area.w());
    if(m_imp->pos.x() < area.x())
        m_imp->pos.setX(area.x());

    if(m_imp->pos.y() > area.y() + area.h())
        m_imp->pos.setY(area.y() + area.h());
    if(m_imp->pos.y() < area.y())
        m_imp->pos.setY(area.y());

    m_imp->update();
}

void LCursor::setHotspot(const LPointF &hotspot)
{
    m_imp->hotspot = hotspot;
    m_imp->update();
}

void LCursor::paint()
{
    if(!visible() || !m_imp->output || !m_imp->texture)
        return;

    glEnable(GL_BLEND);
    m_imp->output->painter()->drawTexture(m_imp->texture,LRect(LPoint(),m_imp->texture->size()),m_imp->rect);
}

void LCursor::setSize(const LSizeF &size)
{
    m_imp->size = size;

    if(hasHardwareSupport())
    {
        if(size.w() > 64.f)
            m_imp->size.setW(64.f);

        if(size.h() > 64.f)
            m_imp->size.setH(64.f);
    }

    m_imp->update();
}

void LCursor::setVisible(bool state)
{
    if(state == visible())
        return;

    m_imp->isVisible = state;

    if(!m_imp->isVisible)
        compositor()->imp()->m_backend->setCursor(m_imp->output,nullptr,m_imp->size);
    else if(m_imp->texture)
    {
        m_imp->update();
        compositor()->imp()->m_backend->setCursor(m_imp->output,m_imp->texture,m_imp->size*m_imp->output->getOutputScale());
    }

}

bool LCursor::visible() const
{
    return m_imp->isVisible;
}

bool LCursor::hasHardwareSupport() const
{
    return compositor()->imp()->m_backend->hasHardwareCursorSupport();
}

const LPointF &LCursor::position() const
{
    return m_imp->pos;
}

const LPointF &LCursor::hotspot() const
{
    return m_imp->hotspot;
}

LTexture *LCursor::texture() const
{
    return m_imp->texture;
}

LCompositor *LCursor::compositor() const
{
    if(m_imp->output)
        return m_imp->output->compositor();
    return nullptr;
}

LOutput *LCursor::output() const
{
    return m_imp->output;
}

const LRect &LCursor::rect() const
{
    return m_imp->rect;
}

LCursor::LCursorPrivate *LCursor::imp() const
{
    return m_imp;
}

void LCursor::LCursorPrivate::loadDefaultCursors()
{
    XcursorImage *cursor =  XcursorLibraryLoadImage("arrow",cursorTheme,64);
    cursors[0].texture = new LTexture(1);
    cursors[0].texture->setData(cursor->width,cursor->height,cursor->pixels,GL_RGBA,GL_UNSIGNED_BYTE);
    cursors[0].hotspot.setX(cursor->xhot);
    cursors[0].hotspot.setY(cursor->yhot);
    XcursorImageDestroy(cursor);
}

void LCursor::LCursorPrivate::update()
{

    if(!isVisible || !output || !texture)
        return;

    LPointF newHotspot;
    newHotspot = (hotspot*size)/texture->size();

    LPointF newPos = pos - newHotspot - output->rect().topLeft();

    rect.setX(newPos.x());
    rect.setY(newPos.y());
    rect.setW(size.w());
    rect.setH(size.h());

    newPos*=output->getOutputScale();


    if(cursor->hasHardwareSupport())
        output->compositor()->imp()->m_backend->setCursorPosition(output,newPos);
    else
        output->repaint();
}

