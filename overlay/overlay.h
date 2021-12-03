#ifndef OVERLAY_H
#define OVERLAY_H

#include <X11/Xlib.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>

int enumerate_windows(int (*callback)(Window window, char *window_name, pid_t pid));
int create_overlay(Window window);

#endif
