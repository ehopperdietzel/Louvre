#include "WClient.h"


WClient::WClient(wl_client *cli, wl_resource *res)
{
    client = cli;
    resource = res;
}
