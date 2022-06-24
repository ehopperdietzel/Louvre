#ifndef LDATAOFFER_H
#define LDATAOFFER_H

#include <LNamespaces.h>

class Louvre::LDataOffer
{
public:
    LDataOffer(wl_resource *resource,LDataDevice *dataDevice);
    ~LDataOffer();
    wl_resource *resource() const;
    LDataSource *dataSource() const;
    LDataDevice *dataDevice() const;

    class LDataOfferPrivate;
    LDataOfferPrivate *imp() const;
private:
    LDataOfferPrivate *m_imp = nullptr;
};

#endif // LDATAOFFER_H
