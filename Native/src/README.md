# architecture explained

3 files are in the folder

api hoolds the static API as namespaces, methods are all the same, asyncronous.

Win and Web are classes, I named them capital im not sure is this normal in cpp.


## first toDo
In our example api is also main

1.when run, main uses windows.create to create a new window.

this method uses the Win class to create a system win.

the class is different for different OS, api file is same.

2.Win class uses Fenster to obtain a system window.

3.Inside this system window we run a webview, refered to as "host web" or "host win",  full width and height of window and set to scale with window resizing.

4.the webview loads a file from hostWin.html vendor/pages/.

the file holds an example engine and everything needed for a warp window to work, this is the latest copy from a working example.


Once run,the file expects for the host webview to respond to its messages so you will get few red ones because your webview is not responding.


## second toDo

implement messaging, and api method envoking through it

implement first methods needed to finish the window creation (5,6 methods)

once this is made we have an architecture blueprint, we can add methods, same as in the Api.cs file




