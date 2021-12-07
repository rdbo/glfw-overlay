#include "overlay.h"
#include <stdio.h>
#include <string.h>

int render_callback(struct window_info *winfo, void *arg)
{
	glBegin(GL_TRIANGLES);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0, 0, 0);
	glVertex3f(1, 0, 0);
	glVertex3f(0, 1, 0);
	glEnd();
	return 0;
}

int window_callback(Window window, char *window_name, pid_t pid, void *arg)
{
	printf("[*] Window ID: %lu\n", (unsigned long)window);
	printf("[*] Window Name: %s\n", window_name);
	printf("[*] Window PID: %d\n", pid);
	printf("====================\n");

	if (!strcmp(window_name, "Counter-Strike Source - OpenGL")) {
		printf("[*] Game Window Found\n");
		printf("[*] Process ID: %d\n", pid);
		printf("====================\n");
		run_overlay((Display *)arg, window, render_callback, NULL);
		return -1;
	} else if (!strcmp(window_name, "steam")) {
		enumerate_windows((Display *)arg, window_callback, arg);
	}

	return 0;
}

int main()
{
	Display *display;

	display = init_overlay();
	if (!display)
		return -1;
	enumerate_windows(display, window_callback, (void *)display);
	shutdown_overlay(display);
	return 0;
}