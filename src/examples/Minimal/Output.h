#ifndef OUTPUT_H
#define OUTPUT_H

#include <LOutput.h>

using namespace Louvre;

class Output : public LOutput
{
public:
    Output();
    void paintGL(Int32 currentBuffer) override;
};

#endif // OUTPUT_H
