#include "overlay.h"

Window *g_overlayed_window;

int enumerate_windows(int (*callback)(Window window, char *window_name, pid_t pid))
{
	Display *display;
	Window root_window;
	Atom property;
	Atom actual_type;
	int actual_format;
	unsigned long nitems;
	unsigned long bytes_after;
	unsigned char *data;
	int ret;

	display = XOpenDisplay(NULL);
	root_window = RootWindow(display, DefaultScreen(display));
	property = XInternAtom(display, "_NET_CLIENT_LIST", True); /* Window List Property */

	ret = XGetWindowProperty(
		display, root_window, property, 0, BUFSIZ, False,
		AnyPropertyType, &actual_type, &actual_format, &nitems,
		&bytes_after, &data
	);

	if (ret >= Success) {
		Window *window_list = (Window *)data;
		unsigned long i;
		char *window_name;
		unsigned long tmp;
		pid_t *pid_list;

		property = XInternAtom(display, "_NET_WM_PID", True); /* Window PID Property */

		for (i = 0; i < nitems; ++i) {
			XGetWindowProperty(
				display, window_list[i], property, 0, BUFSIZ,
				False, AnyPropertyType, &actual_type,
				&actual_format, &tmp, &tmp, &data
			);

			pid_list = (pid_t *)data;
			window_name = (char *)NULL;
			ret = XFetchName(display, window_list[i], &window_name);

			if (ret >= Success && window_name)
				ret = callback(window_list[i], window_name, pid_list[0]);

			XFree(window_name);
			XFree(pid_list);

			if (ret)
				break;
		}

		XFree(window_list);

		return 0;
	}

	return -1;
}


