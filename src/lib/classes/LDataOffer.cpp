#include "LDataOffer.h"

using namespace Louvre;

LDataOffer::LDataOffer(wl_resource *resource)
{
    p_resource = resource;
}

wl_resource *LDataOffer::resource() const
{
    return p_resource;
}
