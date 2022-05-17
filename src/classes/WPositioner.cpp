#include "WPositioner.h"
#include <WClient.h>
#include <WCompositor.h>

using namespace Wpp;

WPositioner::WPositioner(WClient *client)
{
    p_client = client;
    p_client->positioners.push_back(this);
}

WPoint WPositioner::calculatePopupPosition(const WRect &outputRect, const WPoint &parentPosition)
{
    // TODO use output rect + constrains
    (void)outputRect;

    WPoint parentPos = parentPosition;
    WPoint popupOrigin;
    WPoint anchorPos;
    WSize popupSize = size();

    switch(anchor())
    {
        case XDG_POSITIONER_ANCHOR_NONE:
        {
            anchorPos = anchorRect().bottomRight()/2;
        }break;
        case XDG_POSITIONER_ANCHOR_TOP:
        {
            anchorPos.setX(anchorRect().w()/2);
        }break;
        case XDG_POSITIONER_ANCHOR_BOTTOM:
        {
            anchorPos.setX(anchorRect().w()/2);
            anchorPos.setY(anchorRect().h());
        }break;
        case XDG_POSITIONER_ANCHOR_LEFT:
        {
            anchorPos.setY(anchorRect().h()/2);
        }break;
        case XDG_POSITIONER_ANCHOR_RIGHT:
        {
            anchorPos.setX(anchorRect().w());
            anchorPos.setY(anchorRect().h()/2);
        }break;
        case XDG_POSITIONER_ANCHOR_TOP_LEFT:
        {
            // (0,0)
        }break;
        case XDG_POSITIONER_ANCHOR_BOTTOM_LEFT:
        {
            anchorPos.setY(anchorRect().h());
        }break;
        case XDG_POSITIONER_ANCHOR_TOP_RIGHT:
        {
            anchorPos.setX(anchorRect().w());
        }break;
        case XDG_POSITIONER_ANCHOR_BOTTOM_RIGHT:
        {
            anchorPos = anchorRect().bottomRight();
        }break;
    }

    switch(gravity())
    {
        case XDG_POSITIONER_GRAVITY_NONE:
        {
            popupOrigin = popupSize/2;
        }break;
        case XDG_POSITIONER_GRAVITY_TOP:
        {
            popupOrigin.setX(popupSize.w()/2);
            popupOrigin.setY(popupSize.h());
        }break;
        case XDG_POSITIONER_GRAVITY_BOTTOM:
        {
            popupOrigin.setX(popupSize.w()/2);
        }break;
        case XDG_POSITIONER_GRAVITY_LEFT:
        {
            popupOrigin.setX(popupSize.w());
            popupOrigin.setY(popupSize.h()/2);
        }break;
        case XDG_POSITIONER_GRAVITY_RIGHT:
        {
            popupOrigin.setY(popupSize.h()/2);
        }break;
        case XDG_POSITIONER_GRAVITY_TOP_LEFT:
        {
            popupOrigin = popupSize;
        }break;
        case XDG_POSITIONER_GRAVITY_BOTTOM_LEFT:
        {
            popupOrigin.setX(popupSize.w());
        }break;
        case XDG_POSITIONER_GRAVITY_TOP_RIGHT:
        {
            popupOrigin.setY(popupSize.h());
        }break;
        case XDG_POSITIONER_GRAVITY_BOTTOM_RIGHT:
        {
            // (0,0)
        }break;
    }

    return parentPos + anchorRect().topLeft() + anchorPos - popupOrigin + offset();
}

