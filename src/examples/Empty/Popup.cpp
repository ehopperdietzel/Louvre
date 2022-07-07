#include "Popup.h"

Popup::Popup(wl_resource *popup, LSurface *surface, LPositioner *positioner) : LPopupRole(popup, surface, positioner) {}
