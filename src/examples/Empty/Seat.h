#ifndef SEAT_H
#define SEAT_H

#include <LSeat.h>

using namespace Louvre;

class Seat : public LSeat
{
public:
    Seat(LCompositor *compositor);

    /* void libinputEvent(libinput_event *ev) override; */
};

#endif // SEAT_H
