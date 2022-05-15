#ifndef WCURSOR_H
#define WCURSOR_H

#include <WNamespaces.h>
#include <WPointF.h>
#include <WBackend.h>

class Wpp::WCursor
{
public:
    WCursor(WOutput *output);

    void setTexture(WTexture *texture, WPointF hotspot);
    void setOutput(WOutput *output);
    bool hasHardwareSupport(){return WBackend::hasHardwareCursorSupport();}
    void move(float x, float y);
    void setPosition(WPointF position);
    void setHotspot(WPointF hotspot);
    void paint();
    void setSize(WSizeF size);

    WPointF position(){return p_pos;}
    WPointF hotspot(){return p_hotspot;}
    WTexture *texture(){return p_texture;}

private:
    void update();
    WTexture *p_texture = nullptr;
    WOutput *p_output = nullptr;
    WPointF p_pos;
    WPointF p_hotspot;
    WSizeF p_size = WSizeF(64.f,64.f);
};

#endif // WCURSOR_H
