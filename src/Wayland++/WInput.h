#ifndef WINPUT_H
#define WINPUT_H

class WCompositor;

int initInput(WCompositor *compositor);
void processInput();
int getKeymapFD();
int getKeymapSize();


#endif // WINPUT_H
