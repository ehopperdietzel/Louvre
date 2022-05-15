#include "WCursor.h"
#include <WOutput.h>
#include <WTexture.h>
#include <WPointF.h>
#include <WRect.h>
#include <WOpenGL.h>

using namespace Wpp;

WCursor::WCursor(WOutput *output)
{
    setOutput(output);
}

void WCursor::setTexture(WTexture *texture, WPointF hotspot)
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

void Wpp::WCursor::setPosition(WPointF position)
{
    p_pos = position;
    update();
}

void WCursor::setHotspot(WPointF hotspot)
{
    p_hotspot = hotspot;
    update();
}

void WCursor::paint()
{
    if(p_output == nullptr || p_texture == nullptr)
        return;

    WPointF hotspot = WPointF((p_hotspot.w()*p_size.w())/float(p_texture->size().w()),(p_hotspot.h()*p_size.h())/float(p_texture->size().h()));

    p_output->painter()->drawTexture(p_texture,WRect(WPoint(0,0),p_texture->size()),WRect(p_pos-hotspot,p_size));
}

void WCursor::setSize(WSizeF size)
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
    WPointF hotspot;

    if(p_texture != nullptr)
        hotspot = WPointF((p_hotspot.w()*p_size.w())/float(p_texture->size().w()),(p_hotspot.h()*p_size.h())/float(p_texture->size().h()));

    WBackend::setCursorPosition(p_output,(p_pos- hotspot)*p_output->getOutputScale());
}
