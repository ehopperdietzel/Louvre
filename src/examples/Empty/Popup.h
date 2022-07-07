#ifndef POPUP_H
#define POPUP_H

#include <LPopupRole.h>

using namespace Louvre;

class Popup : public LPopupRole
{
public:
    Popup(wl_resource *popup, LSurface *surface, LPositioner *positioner);
};

#endif // POPUP_H
