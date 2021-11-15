#include <WInput.h>
#include <WCompositor.h>

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <libudev.h>
#include <libinput.h>


WCompositor *comp;
libinput *li;
libinput_event *ev;
udev *udev;

static int open_restricted(const char *path, int flags, void *user_data)
{
    (void)user_data;
    int fd = open(path, flags);
    return fd;
}

static void close_restricted(int fd, void *user_data)
{
    (void)user_data;
    close(fd);
}

const static struct libinput_interface interface = {
        .open_restricted = open_restricted,
        .close_restricted = close_restricted,
};

void processInput()
{
    while(1)
    {
      ev = libinput_get_event(li);

      if(ev == NULL)
      {
          libinput_dispatch(li);
          return;
      }

      libinput_event_type eventType = libinput_event_get_type(ev);


      if(eventType == LIBINPUT_EVENT_POINTER_MOTION)
      {
          libinput_event_pointer *pointerEvent = libinput_event_get_pointer_event(ev);
          comp->setPointerPos(comp->getPointerX() + libinput_event_pointer_get_dx(pointerEvent), comp->getPointerY() + libinput_event_pointer_get_dy(pointerEvent));
      }
      else if(eventType == LIBINPUT_EVENT_POINTER_BUTTON)
      {
          libinput_event_pointer *pointerEvent = libinput_event_get_pointer_event(ev);

          uint32_t button = libinput_event_pointer_get_button(pointerEvent);
          libinput_button_state state = libinput_event_pointer_get_button_state(pointerEvent);

          comp->pointerClickEvent(comp->getPointerX(),comp->getPointerY(),button,state);
      }


      // Sends event to the compositor
      comp->libinputEvent(ev);
      libinput_event_destroy(ev);
      libinput_dispatch(li);
    }

    //libinput_unref(li);
}



int initInput(WCompositor *compositor)
{
    comp = compositor;
    udev = udev_new();
    li = libinput_udev_create_context(&interface, NULL, udev);

    libinput_udev_assign_seat(li, "seat0");
    libinput_dispatch(li);

    printf("Libinput initialized.\n");
    return libinput_get_fd(li);
}
