#ifndef LCURSOR_H
#define LCURSOR_H

#include <LNamespaces.h>
#include <LPointF.h>
#include <LPoint.h>
#include <LTexture.h>

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

    bool hasHardwareSupport() const;
    const LPointF &position() const {return p_pos;}
    const LPointF &hotspot()  const {return p_hotspot;}
    LTexture *texture()       const {return p_texture;}
    LCompositor *compositor() const;

private:

    struct LCursorData
    {
        LTexture *texture = nullptr;
        LPointF hotspot = LPointF();
    };

    void loadDefaultCursors();

    LCursorData p_cursors[1];

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
    char *p_cursorTheme = NULL;
    LTexture *p_x11Texture;
    LTexture *p_texture = nullptr;
    LOutput *p_output = nullptr;
    LPointF p_pos;
    LPointF p_hotspot;
    LSizeF p_size = LSizeF(64.f,64.f);
    LPoint p_prevPos;
};

#endif // LCURSOR_H
