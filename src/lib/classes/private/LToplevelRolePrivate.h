#ifndef LTOPLEVELROLEPRIVATE_H
#define LTOPLEVELROLEPRIVATE_H

#include <LToplevelRole.h>

class Louvre::LToplevelRole::LToplevelRolePrivate
{
public:
    LToplevelRolePrivate() = default;
    ~LToplevelRolePrivate() = default;

    struct TopLevelConfiguration
    {
        bool set = false;
        LSize size = LSize();
        UChar8 flags = Deactivated;
        UInt32 serial = 0;
    };

    LToplevelRole *toplevel;

    UChar8 stateFlags = Deactivated;
    TopLevelConfiguration currentConf;
    TopLevelConfiguration sentConf;
    TopLevelConfiguration pendingConf;

    void dispachLastConfiguration();

    bool hasPendingMinSize = false;
    bool hasPendingMaxSize = false;
    LSize minSize,pendingMinSize;
    LSize maxSize,pendingMaxSize;
    LRect windowGeometry;

    void setAppId(const char *appId);
    void setTitle(const char *title);
    char *appId = nullptr;
    char *title = nullptr;

};

#endif // LTOPLEVELROLEPRIVATE_H
