#ifndef OUTPUTMANAGER_H
#define OUTPUTMANAGER_H

#include <LOutputManager.h>

using namespace Louvre;

class OutputManager : public LOutputManager
{
public:
    OutputManager(LCompositor *compositor);

    /* void pluggedOutputRequest(LOutput *output) override; */

    /* void unpluggedOutputRequest(LOutput *output) override; */

};

#endif // OUTPUTMANAGER_H
