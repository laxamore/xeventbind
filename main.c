/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Olaf Tomalka
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <X11/Xlib.h>

struct arguments {
    char *script_path;
};

void print_usage(char *path) {
    fprintf(stderr, "Usage: %s script_path\n", path);
    exit(EXIT_FAILURE);
}

void parse_args(int argc, char** argv, struct arguments *args) {
    if (argc != 2)
        print_usage(argv[0]);

    args->script_path = argv[1];
}

void handle_callback(void *data) {
    assert(data);
    struct arguments *args = data;

    pid_t pid = fork();
    int err;
    switch (pid) {
        case -1:
            perror("Failed to fork\n");
            exit(EXIT_FAILURE);
            break;
        case 0: // Child
            printf("here %s", args->script_path);
            err = execlp(args->script_path, args->script_path, NULL);
            if (err == -1) {
                perror("Failed to open callback script\n");
                exit(EXIT_FAILURE);
            }
            break;
    }
}

int xeb_loop(void *data) {
    Display *display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Failed to open display\n");
        return -1;
    }

    int screen_count = ScreenCount(display);
    Window roots[screen_count];

    for (int i = 0; i < screen_count; i++) {
        roots[i] = RootWindow(display, i);

        XWindowAttributes attrs;
        Status status = XGetWindowAttributes(display, roots[i], &attrs);
        if (!status) {
            fprintf(stderr, "Failed to get root window attributes for screen: %d\n", i);
            return -1;
        }

        XSelectInput(display, roots[i], StructureNotifyMask);
    }

    for (;;) {
        XEvent e;
        XNextEvent(display, &e);
        if (e.type == ConfigureNotify) {
            handle_callback(data);
        }
    }
}

int main(int argc, char **argv) {
    struct arguments args;
    parse_args(argc, argv, &args);
    return xeb_loop(&args) ? EXIT_FAILURE : EXIT_SUCCESS;    
}


