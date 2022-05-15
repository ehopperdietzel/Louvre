#include "WView.h"

WView::WView()
{

}

/*
data->ret = drmModePageFlip(data->deviceFd, data->crtc_id, data->fb->fb_id,DRM_MODE_PAGE_FLIP_EVENT, &waiting_for_flip);
if (data->ret)
{
    printf("Failed to queue page flip: %s\n", strerror(errno));
    return;
}

while (waiting_for_flip)
{

    data->ret = select(data->deviceFd + 1, &data->fds, NULL, NULL, NULL);
    if (data->ret < 0)
    {
        printf("Select err: %s\n", strerror(errno));
        return;
    }
    else if (data->ret == 0)
    {
        printf("Select timeout!\n");
        return;
    }
    else if (FD_ISSET(0, &data->fds))
    {
        printf("User interrupted!\n");
        break;
    }

    drmHandleEvent(data->deviceFd, &data->evctx);
}
*/
