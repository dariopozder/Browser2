 # Linux
#gcc win.c -lX11 -lasound -o warp && ./warp
# macOS
#cc win.c -framework Cocoa -framework AudioToolbox -o warp && ./warp
# windows

#note the backslash differnce in path for execution
gcc src/win.c -lgdi32 -lwinmm -o dist/warp.exe && dist\warp.exe