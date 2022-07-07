#include "Surface.h"

Surface::Surface(wl_resource *surface, LClient *client, GLuint textureUnit) : LSurface(surface, client, textureUnit) {}
