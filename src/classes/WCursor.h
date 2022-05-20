#ifndef WCURSOR_H
#define WCURSOR_H

#include <WNamespaces.h>
#include <WPointF.h>
#include <WBackend.h>
#include <WPoint.h>

class Wpp::WCursor
{
public:
    WCursor(WOutput *output);

    void setCursorTheme(const char *themeName);
    void setCursor(const char *cursorName);
    void setTexture(WTexture *texture, const WPointF &hotspot);
    void setOutput(WOutput *output);
    void move(float x, float y);
    void setPosition(const WPointF &position);
    void setHotspot(const WPointF &hotspot);
    void paint();
    void setSize(const WSizeF &size);

    bool hasHardwareSupport() const {return WBackend::hasHardwareCursorSupport();}
    const WPointF &position() const {return p_pos;}
    const WPointF &hotspot()  const {return p_hotspot;}
    WTexture *texture()       const {return p_texture;}

private:
    void update();

    char *p_cursorTheme = NULL;
    WTexture *p_x11Texture;
    WTexture *p_texture = nullptr;
    WOutput *p_output = nullptr;
    WPointF p_pos;
    WPointF p_hotspot;
    WSizeF p_size = WSizeF(64.f,64.f);
    WPoint p_prevPos;
};

#endif // WCURSOR_H
