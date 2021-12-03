#include "overlay.h"
#include <string.h>

int window_callback(Window window, char *window_name, pid_t pid)
{
	printf("[*] Window ID: %d\n", window);
	printf("[*] Window Name: %s\n", window_name);
	printf("[*] Window PID: %d\n", pid);
	printf("====================\n");

	if (!strcmp(window_name, "Counter-Strike: Source - OpenGL")) {
		create_overlay(window);
		return -1;
	}

	return 0;
}

int main()
{
	enumerate_windows(window_callback);
	return 0;
}