#ifndef LDATAOFFER_H
#define LDATAOFFER_H

#include <LNamespaces.h>

class Louvre::LDataOffer
{
public:
    LDataOffer(wl_resource *resource);
    wl_resource *resource() const;
private:
    wl_resource *p_resource = nullptr;
};

#endif // LDATAOFFER_H
