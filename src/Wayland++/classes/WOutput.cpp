#include "WOutput.h"
#include <string.h>

using namespace WaylandPlus;

WOutput::WOutput(const char *drmDeviceName,drmModeConnector *connector,UInt32 crtcId)
{
    int devNameLen = strlen(drmDeviceName);
    _devName = new char(devNameLen+1);
    strcpy(_devName,drmDeviceName);
    _devName[devNameLen] = '\0';
    _connector = connector;
    _crtc_id = crtcId;
}

WOutput::~WOutput()
{
    drmModeFreeConnector(_connector);
    delete []_devName;
}
