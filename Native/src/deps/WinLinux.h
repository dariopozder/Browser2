#pragma once

#include <string>
#include <stdexcept>
#include <iostream>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <unistd.h>

class Win {
public:
    Win(const std::string& id, int w = 640, int h = 480)
        : id(id), width_(w), height_(h), x_(0), y_(0), state_(1) // state 1 = visible
    {
        display = XOpenDisplay(nullptr);
        if (!display) {
            throw std::runtime_error("Cannot open X display");
        }

        screen = DefaultScreen(display);

        window = XCreateSimpleWindow(display, RootWindow(display, screen),
            x_, y_, width_, height_, 1,
            BlackPixel(display, screen),
            WhitePixel(display, screen));

        XSelectInput(display, window, ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask);
        XMapWindow(display, window);
        XStoreName(display, window, id.c_str());

        wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(display, window, &wmDeleteMessage, 1);
    }

    ~Win() {
        destroy();
    }

    // Disable copy
    Win(const Win&) = delete;
    Win& operator=(const Win&) = delete;

    int width() const {
        return width_;
    }

    void width(int newWidth) {
        width_ = newWidth;
        XResizeWindow(display, window, width_, height_);
    }

    int height() const {
        return height_;
    }

    void height(int newHeight) {
        height_ = newHeight;
        XResizeWindow(display, window, width_, height_);
    }

    int x() const {
        return x_;
    }

    void x(int newX) {
        x_ = newX;
        XMoveWindow(display, window, x_, y_);
    }

    int y() const {
        return y_;
    }

    void y(int newY) {
        y_ = newY;
        XMoveWindow(display, window, x_, y_);
    }

    int state() const {
        return state_;
    }

    void state(int newState) {
        state_ = newState;
        if (state_ == 0) {
            XUnmapWindow(display, window);
        }
        else {
            XMapWindow(display, window);
        }
    }

    void destroy() {
        if (display) {
            XDestroyWindow(display, window);
            XCloseDisplay(display);
            display = nullptr;
        }
    }

    int loop() {
        if (!display) return 0;

        XEvent event;
        while (XPending(display)) {
            XNextEvent(display, &event);

            if (event.type == ClientMessage) {
                if (static_cast<unsigned long>(event.xclient.data.l[0]) == wmDeleteMessage) {
                    // Window close requested
                    return 0;
                }
            }
            else if (event.type == ConfigureNotify) {
                XConfigureEvent xce = event.xconfigure;
                x_ = xce.x;
                y_ = xce.y;
                width_ = xce.width;
                height_ = xce.height;
            }
            else if (event.type == Expose) {
                // Redraw can be handled here if needed
            }
            // Additional event handling can be added here
        }

        return 1; // continue running
    }

private:
    std::string id;
    Display* display = nullptr;
    Window window = 0;
    int screen = 0;
    int width_, height_;
    int x_, y_;
    int state_;
    Atom wmDeleteMessage;
};
