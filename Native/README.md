# Browser2 Native
c++ Warp Browser testing repo

Fenster is a minimal cross platform frame buffer implementation for lin, win and mac.
It includes a fb/canvas, mouse events, basic sound and native/OS window handling.

the same guy produces webview, cross platform web control


these are my notes, dont take them seriously

## installation for for webview
Development: apt install libgtk-3-dev libwebkit2gtk-4.0-dev

Production: apt install libgtk-3-0 libwebkit2gtk-4.0-37



## Compiling 

Linux
gcc win.c -lX11 -lasound -o warp && ./warp

macOS
cc win.c -framework Cocoa -framework AudioToolbox -o warp && ./warp


Windows
note the backslash differnce in path for execution
gcc src/win.c -lgdi32 -lwinmm -o dist/warp.exe && dist\warp.exe
