#ifndef LCURSOR_H
#define LCURSOR_H

#include <LNamespaces.h>
#include <LPointF.h>
#include <LPoint.h>
#include <LTexture.h>
#include <LRect.h>

class Louvre::LCursor
{
public:
    LCursor(LOutput *output);

    enum CursorNames : UChar8
    {
        Arrow = 0
    };

    void setCursorTheme(const char *themeName);
    void setCursor(const char *cursorName);
    void setCursor(CursorNames cursor);
    void setTexture(LTexture *texture, const LPointF &hotspot);
    void setOutput(LOutput *output);
    void move(float x, float y);
    void setPosition(const LPointF &position);
    void setHotspot(const LPointF &hotspot);
    void paint();
    void setSize(const LSizeF &size);
    void setVisible(bool state);

    bool visible() const;
    bool hasHardwareSupport() const;
    const LPointF &position() const {return m_pos;}
    const LPointF &hotspot()  const {return m_hotspot;}
    LTexture *texture()       const {return m_texture;}
    LCompositor *compositor() const;
    LOutput *output() const;

    const LRect &rect() const {return m_rect;}

private:

    struct LCursorData
    {
        LTexture *texture = nullptr;
        LPointF hotspot = LPointF();
    };

    void loadDefaultCursors();

    LCursorData m_cursors[1];
    LRect m_rect;

    enum LLastCursorType
    {
        Texture,
        String,
        Default
    };

    struct LLastCursorData
    {
        LSizeF size;
        LPointF offset;
        char *name = nullptr;
        UInt32 textureId;
        LLastCursorType type = LLastCursorType::String;
        int defaultName = -1;
    }lastCursor;

    void update();
    char *m_cursorTheme = NULL;
    LTexture *m_x11Texture;
    LTexture *m_texture = nullptr;
    LOutput *m_output = nullptr;
    LPointF m_pos;
    LPointF m_hotspot;
    LSizeF m_size = LSizeF(64.f,64.f);

    bool m_isVisible = true;
};

#endif // LCURSOR_H
