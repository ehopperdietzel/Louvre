#ifndef LCURSOR_H
#define LCURSOR_H

#include <LNamespaces.h>
#include <LPointF.h>
#include <LBackend.h>
#include <LPoint.h>

class Louvre::LCursor
{
public:
    LCursor(LOutput *output);

    void setCursorTheme(const char *themeName);
    void setCursor(const char *cursorName);
    void setTexture(LTexture *texture, const LPointF &hotspot);
    void setOutput(LOutput *output);
    void move(float x, float y);
    void setPosition(const LPointF &position);
    void setHotspot(const LPointF &hotspot);
    void paint();
    void setSize(const LSizeF &size);

    bool hasHardwareSupport() const {return LBackend::hasHardwareCursorSupport();}
    const LPointF &position() const {return p_pos;}
    const LPointF &hotspot()  const {return p_hotspot;}
    LTexture *texture()       const {return p_texture;}

private:
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
