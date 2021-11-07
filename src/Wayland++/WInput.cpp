#include <WInput.h>
#include <WCompositor.h>

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <libudev.h>
#include <libinput.h>
#include <poll.h>
#include <pthread.h>


struct pollfd pfd = {};

static int open_restricted(const char *path, int flags, void *user_data)
{
  int fd = open(path, flags);
  pfd.fd = fd;
  pfd.events = -1;
  pfd.revents = -1;
  return fd < 0 ? -errno : fd;
}

static void close_restricted(int fd, void *user_data)
{
  close(fd);
}

const static struct libinput_interface interface = {
        .open_restricted = open_restricted,
        .close_restricted = close_restricted,
};

void *inputLoop(void*data)
{
    WCompositor *compositor = (WCompositor*)data;
    struct libinput *li;
    struct libinput_event *ev;
    struct udev *udev = udev_new();

    li = libinput_udev_create_context(&interface, NULL, udev);

    libinput_udev_assign_seat(li, "seat0");
    libinput_dispatch(li);

    while(1)
    {
      ev = libinput_get_event(li);

      if(ev == NULL)
      {
          libinput_dispatch(li);
          continue;
      }

      // Sends event to the compositor
      compositor->libinputEvent(ev);
      libinput_event_destroy(ev);
      libinput_dispatch(li);
      poll(&pfd,1,-1);
    }

    libinput_unref(li);
}

void initInput(WCompositor *compositor)
{
    pthread_t inputThread;
    pthread_create(&inputThread, NULL, inputLoop, compositor);
    return;
}
