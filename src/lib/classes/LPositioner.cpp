#include "LPositioner.h"
#include <LClient.h>
#include <LCompositor.h>

using namespace Louvre;

LPositioner::LPositioner(LClient *client)
{
    m_client = client;
}

LClient *LPositioner::client() const
{
    return m_client;
}

wl_resource *LPositioner::resource() const
{
    return m_resource;
}

const LSize &LPositioner::size() const
{
    return m_size;
}

const LRect &LPositioner::anchorRect() const
{
    return m_anchorRect;
}

LPositioner::LAnchor LPositioner::anchor() const
{
    return m_anchor;
}

LPositioner::LGravity LPositioner::gravity() const
{
    return m_gravity;
}

const LPoint &LPositioner::offset() const
{
    return m_offset;
}

#if LOUVRE_XDG_WM_BASE_VERSION >=3

    bool LPositioner::isReactive() const
    {
        return m_isReactive;
    }

    const LSize &LPositioner::parentSize() const
    {
        return m_parentSize;
    }

    UInt32 LPositioner::parentConfigureSerial() const
    {
        return m_parentConfigureSerial;
    }

#endif

LPoint LPositioner::calculatePopupPosition(const LRect &outputRect, const LPoint &parentPosition)
{
    // TODO use output rect + constrains
    (void)outputRect;

    LPoint parentPos = parentPosition;
    LPoint popupOrigin;
    LPoint anchorPos;
    LSize popupSize = size();

    switch(anchor())
    {
        case LAnchor::LNone:
        {
            anchorPos = anchorRect().bottomRight()/2;
        }break;
        case LAnchor::LTop:
        {
            anchorPos.setX(anchorRect().w()/2);
        }break;
        case LAnchor::LBottom:
        {
            anchorPos.setX(anchorRect().w()/2);
            anchorPos.setY(anchorRect().h());
        }break;
        case LAnchor::LLeft:
        {
            anchorPos.setY(anchorRect().h()/2);
        }break;
        case LAnchor::LRight:
        {
            anchorPos.setX(anchorRect().w());
            anchorPos.setY(anchorRect().h()/2);
        }break;
        case LAnchor::LTopLeft:
        {
            // (0,0)
        }break;
        case LAnchor::LBottomLeft:
        {
            anchorPos.setY(anchorRect().h());
        }break;
        case LAnchor::LTopRight:
        {
            anchorPos.setX(anchorRect().w());
        }break;
        case LAnchor::LBottomRight:
        {
            anchorPos = anchorRect().bottomRight();
        }break;
    }

    switch(gravity())
    {
        case LGravity::LNone:
        {
            popupOrigin = popupSize/2;
        }break;
        case LGravity::LTop:
        {
            popupOrigin.setX(popupSize.w()/2);
            popupOrigin.setY(popupSize.h());
        }break;
        case LGravity::LBottom:
        {
            popupOrigin.setX(popupSize.w()/2);
        }break;
        case LGravity::LLeft:
        {
            popupOrigin.setX(popupSize.w());
            popupOrigin.setY(popupSize.h()/2);
        }break;
        case LGravity::LRight:
        {
            popupOrigin.setY(popupSize.h()/2);
        }break;
        case LGravity::LTopLeft:
        {
            popupOrigin = popupSize;
        }break;
        case LGravity::LBottomLeft:
        {
            popupOrigin.setX(popupSize.w());
        }break;
        case LGravity::LTopRight:
        {
            popupOrigin.setY(popupSize.h());
        }break;
        case LGravity::LBottomRight:
        {
            // (0,0)
        }break;
    }

    return parentPos + anchorRect().topLeft() + anchorPos - popupOrigin + offset();
}

LPositioner::LConstraintAdjustment LPositioner::constraintAdjustment() const
{
    return m_constraintAdjustment;
}

