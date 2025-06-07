#pragma once


#include <objc/objc.h>
#include <objc/message.h>
#include <objc/runtime.h>
#include <CoreGraphics/CoreGraphics.h>
#include <CoreFoundation/CoreFoundation.h>
#include <string>
#include <cstring>
#include <stdexcept>

class WinMac {
public:
    WinMac(const char* title, int width = 640, int height = 480)
        : width_(width), height_(height), x_(0), y_(0), mouse_(0), mod_(0)
    {
        std::memset(keys_, 0, sizeof(keys_));
        title_ = std::string(title);

        // Setup Objective-C runtime (like fenster_open)
        msg(id, cls("NSApplication"), "sharedApplication");
        msg1(void, NSApp, "setActivationPolicy:", NSInteger, 0);

        wnd_ = msg4(id, msg(id, cls("NSWindow"), "alloc"),
                    "initWithContentRect:styleMask:backing:defer:", CGRect,
                    CGRectMake(0, 0, width_, height_), NSUInteger, 3,
                    NSUInteger, 2, BOOL, NO);

        // Delegate class for window close
        Class windelegate =
            objc_allocateClassPair((Class)cls("NSObject"), "FensterDelegate", 0);
        class_addMethod(windelegate, sel_getUid("windowShouldClose:"),
                        (IMP)fenster_should_close, "c@:@");
        objc_registerClassPair(windelegate);

        id delegate = msg(id, (id)windelegate, "alloc");
        delegate = msg(id, delegate, "init");
        msg1(void, wnd_, "setDelegate:", id, delegate);

        // View class for drawing
        Class viewclass = objc_allocateClassPair((Class)cls("NSView"), "FensterView", 0);
        class_addMethod(viewclass, sel_getUid("drawRect:"), (IMP)fenster_draw_rect, "v@:{CGRect={CGPoint=dd}{CGSize=dd}}");
        objc_registerClassPair(viewclass);

        id view = msg(id, (id)viewclass, "alloc");
        view = msg(id, view, "init");

        // Associate this C++ WinMac instance pointer with the view
        objc_setAssociatedObject(view, (const void*)"fenster", (id)this, OBJC_ASSOCIATION_ASSIGN);

        msg1(void, wnd_, "setContentView:", id, view);

        id nsTitle = msg1(id, cls("NSString"), "stringWithUTF8String:", const char*, title_.c_str());
        msg1(void, wnd_, "setTitle:", id, nsTitle);
        msg1(void, wnd_, "makeKeyAndOrderFront:", id, nullptr);
        msg(wnd_, "center");
        msg1(void, NSApp, "activateIgnoringOtherApps:", BOOL, YES);
    }

    ~WinMac() {
        msg(wnd_, "close");
    }

    // Window properties
    int width() const { return width_; }
    void width(int w) { width_ = w; /* resizing can be implemented if needed */ }

    int height() const { return height_; }
    void height(int h) { height_ = h; }

    int x() const { return x_; }
    void x(int nx) { x_ = nx; }

    int y() const { return y_; }
    void y(int ny) { y_ = ny; }

    int mouse() const { return mouse_; }
    int mod() const { return mod_; }
    bool key(int k) const { return keys_[k]; }

    // Run one iteration of event loop; returns false if window closed
    bool loop() {
        msg1(void, msg(id, wnd_, "contentView"), "setNeedsDisplay:", BOOL, YES);

        id ev = msg4(id, NSApp,
                     "nextEventMatchingMask:untilDate:inMode:dequeue:",
                     NSUInteger, NSUIntegerMax,
                     id, nullptr,
                     id, NSDefaultRunLoopMode,
                     BOOL, YES);
        if (!ev) return false;

        NSUInteger evtype = msg(NSUInteger, ev, "type");

        switch (evtype) {
        case 1: // NSEventTypeMouseDown
            mouse_ |= 1;
            break;
        case 2: // NSEventTypeMouseUp
            mouse_ &= ~1;
            break;
        case 5: // NSEventTypeMouseMoved
        case 6: {
            CGPoint xy = msg(CGPoint, ev, "locationInWindow");
            x_ = (int)xy.x;
            y_ = (int)(height_ - xy.y);
            break;
        }
        case 10: // NSEventTypeKeyDown
        case 11: { // NSEventTypeKeyUp
            NSUInteger k = msg(NSUInteger, ev, "keyCode");
            keys_[k < 127 ? FENSTER_KEYCODES[k] : 0] = (evtype == 10);
            NSUInteger modflags = msg(NSUInteger, ev, "modifierFlags") >> 17;
            mod_ = (modflags & 0xc) | ((modflags & 1) << 1) | ((modflags >> 1) & 1);
            break;
        }
        }

        msg1(void, NSApp, "sendEvent:", id, ev);
        return true;
    }

private:
    std::string title_;
    int width_, height_, x_, y_;
    int mouse_;
    int mod_;
    bool keys_[256]{};

    id wnd_ = nullptr;

    // Obj-C runtime helpers
    static id msg(id o, SEL s) {
        return ((id (*)(id, SEL))objc_msgSend)(o, s);
    }
    template<typename R, typename... Args>
    static R msg(id o, SEL s, Args... args) {
        return ((R (*)(id, SEL, Args...))objc_msgSend)(o, s, args...);
    }

    // Convenience macros to match fenster style
    #define msg(r,o,s) ((r(*)(id,SEL))objc_msgSend)(o, sel_getUid(s))
    #define msg1(r,o,s,A,a) ((r(*)(id,SEL,A))objc_msgSend)(o, sel_getUid(s), a)
    #define msg2(r,o,s,A,a,B,b) ((r(*)(id,SEL,A,B))objc_msgSend)(o, sel_getUid(s), a, b)
    #define msg3(r,o,s,A,a,B,b,C,c) ((r(*)(id,SEL,A,B,C))objc_msgSend)(o, sel_getUid(s), a, b, c)
    #define msg4(r,o,s,A,a,B,b,C,c,D,d) ((r(*)(id,SEL,A,B,C,D))objc_msgSend)(o, sel_getUid(s), a, b, c, d)
    #define cls(x) ((id)objc_getClass(x))

    // fenster drawRect callback
    static void fenster_draw_rect(id self, SEL _cmd, CGRect rect) {
        (void)rect;
        struct WinMac* that = (WinMac*)objc_getAssociatedObject(self, "fenster");
        if (!that) return;

        CGContextRef ctx =
            msg(CGContextRef, msg(id, cls("NSGraphicsContext"), sel_getUid("currentContext")), sel_getUid("graphicsPort"));
        CGColorSpaceRef space = CGColorSpaceCreateDeviceRGB();

        CGDataProviderRef provider = CGDataProviderCreateWithData(
            NULL, that->buf(), that->width_ * that->height_ * 4, NULL);

        CGImageRef img = CGImageCreate(
            that->width_, that->height_,
            8, 32, that->width_ * 4, space,
            kCGImageAlphaNoneSkipFirst | kCGBitmapByteOrder32Little,
            provider, NULL, false, kCGRenderingIntentDefault);

        CGColorSpaceRelease(space);
        CGDataProviderRelease(provider);

        CGContextDrawImage(ctx, CGRectMake(0, 0, that->width_, that->height_), img);
        CGImageRelease(img);
    }

    // fenster windowShouldClose delegate callback
    static BOOL fenster_should_close(id self, SEL _cmd, id window) {
        (void)self; (void)_cmd; (void)window;
        msg1(void, NSApp, "terminate:", id, NSApp);
        return YES;
    }

    // Buffer pointer for pixel data (you'll need to implement your own buffer or pass in)
    unsigned char* buf() {
        // TODO: Replace with your framebuffer pointer if you have one
        static unsigned char dummy[640*480*4] = {};
        return dummy;
    }

    // Keycode mapping table
    static constexpr uint8_t FENSTER_KEYCODES[128] = {
        65,83,68,70,72,71,90,88,67,86,0,66,81,87,69,82,89,84,
        49,50,51,52,54,53,61,57,55,45,56,48,93,79,85,91,73,80,
        10,76,74,39,75,59,92,44,47,78,77,46,9,32,96,8,0,27,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,26,2,3,
        127,0,5,0,4,0,20,19,18,17,0
    };
};

#endif // __APPLE__
