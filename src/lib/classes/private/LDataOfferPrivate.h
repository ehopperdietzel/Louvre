#ifndef LDATAOFFERPRIVATE_H
#define LDATAOFFERPRIVATE_H

#include <LDataOffer.h>

class Louvre::LDataOffer::LDataOfferPrivate
{
public:
    LDataOfferPrivate() = default;
    ~LDataOfferPrivate() = default;

    wl_resource *m_resource = nullptr;
    LDataSource *m_dataSource = nullptr;
    LDataDevice *m_dataDevice = nullptr;
};

#endif // LDATAOFFERPRIVATE_H
