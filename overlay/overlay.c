#include "overlay.h"
#include <stdio.h>
#include <stdlib.h>

int enumerate_windows(Display *display, int (*callback)(Window window, char *window_name, pid_t pid, void *arg), void *arg)
{
	Window root_window;
	Atom property;
	Atom actual_type;
	int actual_format;
	unsigned long nitems;
	unsigned long bytes_after;
	unsigned char *data;
	int ret;

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
				ret = callback(window_list[i], window_name, pid_list[0], arg);

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

Display *init_overlay(void)
{
	Display *display = (Display *)NULL;

	if (!glfwInit())
		return display;
	
	display = XOpenDisplay(NULL);
	if (!display)
		shutdown_overlay(display);

	return display;
}

void shutdown_overlay(Display *display)
{
	glfwTerminate();
	if (display)
		XCloseDisplay(display);
}

int get_window_info(Display *display, Window window, struct window_info *geom)
{
	Window root_window;
	Window child;
	XWindowAttributes window_attr;
	int x;
	int y;

	root_window = RootWindow(display, DefaultScreen(display));
	if (XTranslateCoordinates(display, window, root_window, 0, 0, &x, &y, &child) < Success)
		return -1;
	
	if (XGetWindowAttributes(display, window, &window_attr) < Success)
		return -1;
	
	geom->border = window_attr.border_width;
	geom->depth = window_attr.depth;
	geom->x = x;
	geom->y = y;
	geom->width = window_attr.width;
	geom->height = window_attr.height;

	return 0;
}

int run_overlay(Display *display, Window window, int (*main_loop_callback)(struct window_info *winfo, void *arg), void *arg)
{
	GLFWwindow *glfw_window;
	struct window_info winfo;

	if (get_window_info(display, window, &winfo))
		return -1;

	glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

	glfw_window = glfwCreateWindow(winfo.width, winfo.height, "Overlay", NULL, NULL);
	glfwSetWindowAttrib(glfw_window, GLFW_DECORATED, GLFW_FALSE);

	glfwMakeContextCurrent(glfw_window);
	glfwShowWindow(glfw_window);
	glfwSwapInterval(1);
	
	while (!get_window_info(display, window, &winfo) && !glfwWindowShouldClose(glfw_window)) {
		glfwPollEvents();
		glfwSetWindowPos(glfw_window, winfo.x, winfo.y);
		glfwSetWindowSize(glfw_window, winfo.width, winfo.height);

		if (main_loop_callback(&winfo, arg))
			break;
		
		glfwSwapBuffers(glfw_window);
	}

	glfwDestroyWindow(glfw_window);

	return 0;
}
