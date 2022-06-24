#ifndef LSURFACEPRIVATE_H
#define LSURFACEPRIVATE_H

#include <LSurface.h>

class Louvre::LSurface::LSurfacePrivate
{
public:

    LSurfacePrivate() = default;
    ~LSurfacePrivate() = default;

    void m_bufferToTexture();

    struct State
    {
        UInt32 type = Undefined;
        wl_resource *buffer = nullptr;
        LRegion inputRegion;
        LRegion opaqueRegion;
        list<LRect>damages;
        list<LRect>bufferDamages;
        LSize size;
    };

    State current,pending;

    LRegion m_currentDamages;
    LRegion m_translucentRegion;

    // Roles
    LBaseSurfaceRole *m_role = nullptr;

    LTexture *m_texture = nullptr;
    list<LSurface*>m_children;

    // Buffer
    void setBufferScale(Int32 scale);

    LClient     *m_client         = nullptr;
    LSurface    *m_parent         = nullptr;

    wl_resource *m_resource       = nullptr;

    wl_resource *m_xdgSurfaceResource = nullptr;
    wl_resource *m_frameCallback   = nullptr;

    Int32 m_bufferScale = 1;

    bool m_isDamaged = false;

    LPoint m_pos = LPoint(0,200);
    bool m_minimized = false;

    bool m_roleChangeNotified = false;
    bool m_textureChanged = false;

    bool m_opaqueRegionChanged = false;
    bool m_inputRegionChanged = true;
    bool m_damagesChanged = false;

};


#endif // LSURFACEPRIVATE_H
