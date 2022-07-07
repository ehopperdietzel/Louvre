#ifndef LSEAT_H
#define LSEAT_H

#include <LNamespaces.h>
#include <LToplevelRole.h>
#include <LSurface.h>

class Louvre::LSeat
{

public:

    enum CAPABILITIES : UInt32
    {
        POINTER = 1,
        KEYBOARD = 2,
        TOUCH = 4
    };

    /* Configuration */
    LSeat(LCompositor *compositor);
    virtual ~LSeat();

    Int32 libinputFd()  const;
    LCompositor *compositor() const;
    LCursor *cursor() const;
    UInt32 capabilities() const;
    void setCapabilities(UInt32 capabilitiesFlags);

    LSurface *touchFocusSurface() const;
    LToplevelRole *activeTopLevel() const;

    LPointer *pointer() const;
    LKeyboard *keyboard() const;

protected:

    friend class LSurface;
    friend class LToplevelRole;
    friend class Globals::Surface;
    friend class Globals::Pointer;

    /* Libinput events */
    virtual void libinputEvent(libinput_event *ev);

private:

    friend class LWayland;
    friend class LCompositor;
    friend class LPointer;
    friend class LKeyboard;
    friend class Extensions::XdgShell::Toplevel;

    // Wayland
    LCompositor         *m_compositor               = nullptr;
    LPointer            *m_pointer                  = nullptr;
    LKeyboard           *m_keyboard                 = nullptr;

    LSurface            *m_touchFocusSurface        = nullptr;
    LToplevelRole       *m_activeTopLevel           = nullptr;
    UInt32               m_capabilities             = POINTER | KEYBOARD;

    // Libinput
    libinput            *m_li                       = nullptr;
    udev                *m_udev                     = nullptr;
    libinput_interface   m_libinputInterface;

    void processInput();
    static int openRestricted(const char *path, int flags, void *userData);
    static void closeRestricted(int fd, void *userData);


};

#endif // LSEAT_H
