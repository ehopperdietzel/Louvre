#include "LDataOffer.h"

using namespace Louvre;

LDataOffer::LDataOffer(wl_resource *resource)
{
    m_resource = resource;
}

wl_resource *LDataOffer::resource() const
{
    return m_resource;
}
