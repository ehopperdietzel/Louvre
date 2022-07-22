#include "XdgPopup.h"

#include <LCompositor.h>
#include <LSurfacePrivate.h>
#include <LClient.h>
#include <LPositioner.h>
#include <LPopupRolePrivate.h>
#include <xdg-shell.h>
#include <LSeat.h>
#include <LPointer.h>
#include <LKeyboard.h>

using namespace Louvre;

void Extensions::XdgShell::Popup::destroy_resource(wl_resource *resource)
{
    printf("XDG_POPUP DESTROYED.\n");

    LPopupRole *lPopup = (LPopupRole*)wl_resource_get_user_data(resource);

    // Notify
    lPopup->compositor()->destroyPopupRequest(lPopup);

    // Unset role
    if(lPopup->surface())
    {
        lPopup->surface()->imp()->current.type = LSurface::Undefined;
        lPopup->surface()->imp()->m_role = nullptr;
        lPopup->surface()->roleChanged();
    }


    if(lPopup->positioner())
        delete lPopup->imp()->positioner;

    delete lPopup;
}

void Extensions::XdgShell::Popup::destroy(wl_client *, wl_resource *resource)
{
    //LPopupRole *lPopup = (LPopupRole*)wl_resource_get_user_data(resource);

    /*
    if(!lPopup->surface()->children().empty())
    {

        wl_resource_post_error(
                    lPopup->surface()->client()->xdgWmBaseResource(),
                    XDG_WM_BASE_ERROR_NOT_THE_TOPMOST_POPUP,
                    "The client tried to map or destroy a non-topmost popup.");

    }
    */

    wl_resource_destroy(resource);
}
void Extensions::XdgShell::Popup::grab(wl_client *, wl_resource *resource, wl_resource *seat, UInt32 serial)
{
    LPopupRole *lPopup = (LPopupRole*)wl_resource_get_user_data(resource);

    if(lPopup->surface()->client()->lastPointerButtonEventSerial() == serial || lPopup->surface()->client()->lastKeyboardKeyEventSerial() == serial)
    {

        if(!lPopup->surface()->parent() || (lPopup->compositor()->seat()->pointer()->focusSurface() != lPopup->surface()->parent() && lPopup->compositor()->seat()->keyboard()->focusSurface() != lPopup->surface()->parent()))
        {
            wl_resource_post_error(
                        resource,
                        XDG_POPUP_ERROR_INVALID_GRAB,
                        "Invalid grab. Popup parent did not have an implicit grab.");
            return;
        }


        lPopup->grabSeatRequest();
    }
}

#if LOUVRE_XDG_WM_BASE_VERSION >= 3
void Extensions::XdgShell::Popup::reposition(wl_client *client, wl_resource *resource, wl_resource *positioner, UInt32 serial)
{
    (void)client;

    LPositioner *lPositioner = (LPositioner*)wl_resource_get_user_data(positioner);
    LPopupRole *lPopup = (LPopupRole*)wl_resource_get_user_data(resource);
    lPopup->imp()->repositionSerial = serial;

    if(lPopup->positioner())
        delete lPopup->imp()->positioner;

    lPopup->imp()->positioner = lPositioner;
    lPopup->configureRequest();
}
#endif
