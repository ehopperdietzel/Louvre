#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <LKeyboard.h>

using namespace Louvre;

class Keyboard : public LKeyboard
{
public:
    Keyboard(LSeat *seat);
};

#endif // KEYBOARD_H
