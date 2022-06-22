#ifndef LCOMPOSITORPRIVATE_H
#define LCOMPOSITORPRIVATE_H

#include <LCompositor.h>

class Louvre::LCompositor::LCompositorPrivate
{
public:
    LCompositorPrivate() = default;
    ~LCompositorPrivate() = default;

    LCursor *m_cursor = nullptr;
    LSeat *m_seat = nullptr;

    std::thread::id m_threadId;
    mutex m_renderMutex;

    // Clients
    list<LClient*>m_clients;

    // Outputs
    list<LOutput*>m_outputs;

    // Surfaces
    list<LSurface*>m_surfaces;

    // Destroyed clients
    list<LClient*>m_destroyedClients;

    int libinputFd, waylandFd;
    eventfd_t libinputVal, waylandVal = 1;

    LDataSource *m_dataSource = nullptr;
    LOpenGL *m_painter;

    bool _started = false;

    LGraphicBackend *m_backend = nullptr;
};

#endif // LCOMPOSITORPRIVATE_H
