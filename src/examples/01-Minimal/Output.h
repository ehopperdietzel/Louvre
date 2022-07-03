#ifndef OUTPUT_H
#define OUTPUT_H

#include <LOutput.h>
#include <LRegion.h>

using namespace Louvre;

class Output : public LOutput
{
public:
    Output();
    void initializeGL() override;
    void paintGL(Int32 currentBuffer) override;

    // Background texture
    LTexture *backgroundTexture;

    // Regions exposed when surfaces pos or size change
    LRegion exposedRegion[2];
    bool fullRefresh = true;
};

#endif // OUTPUT_H
