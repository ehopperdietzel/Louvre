#include <LClient.h>
#include <LCompositor.h>
#include <LPositionerPrivate.h>

using namespace Louvre;

LPositioner::LPositioner(LClient *client)
{
    m_imp = new LPositionerPrivate();
    m_imp->client = client;
}

LPositioner::~LPositioner()
{
    delete m_imp;
}

LClient *LPositioner::client() const
{
    return m_imp->client;
}

wl_resource *LPositioner::resource() const
{
    return m_imp->resource;
}

const LSize &LPositioner::size() const
{
    return m_imp->size;
}

const LRect &LPositioner::anchorRect() const
{
    return m_imp->anchorRect;
}

UInt32 LPositioner::anchor() const
{
    return m_imp->anchor;
}

UInt32 LPositioner::gravity() const
{
    return m_imp->gravity;
}

const LPoint &LPositioner::offset() const
{
    return m_imp->offset;
}

#if LOUVRE_XDG_WM_BASE_VERSION >=3

    bool LPositioner::isReactive() const
    {
        return m_imp->isReactive;
    }

    const LSize &LPositioner::parentSize() const
    {
        return m_imp->parentSize;
    }

    UInt32 LPositioner::parentConfigureSerial() const
    {
        return m_imp->parentConfigureSerial;
    }

#endif

LPoint LPositioner::calculatePopupPosition(const LRect &outputRect, const LPoint &parentPosition)
{
    /*
    // TODO use output rect + constrains
    (void)outputRect;

    LPoint parentPos = parentPosition;
    LPoint popupOrigin;
    LPoint anchorPos;
    LSize popupSize = size();

    switch(anchor())
    {
        case Anchor::None:
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
    */
    return LPoint();
}

UInt32 LPositioner::constraintAdjustment() const
{
    return m_imp->constraintAdjustment;
}

LPositioner::LPositionerPrivate *LPositioner::imp() const
{
    return m_imp;
}

