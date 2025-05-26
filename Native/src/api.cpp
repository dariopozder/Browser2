#include <iostream>
#include <vector>
#include <algorithm>
#include <future>
#include <thread>

using namespace std;


//this API is accesed from JS, from a webview/Web that runs inside a fenster/Win,  that runs a single HTML file.



namespace warp {
    namespace api {
        namespace windows {

            vector<Win> instances;

            // Create a new window instance
            future<void> create(int id) {
                return async(launch::async, [=]() {
                   
                    Win newWindow(id);
                    instances.push_back(newWindow);
                    cout << "[Async] Created window with ID " << id << endl;
                    });
            }

            // Destroy a window instance by ID
            future<void> destroy(int id) {
                return async(launch::async, [=]() {
                    auto it = remove_if(instances.begin(), instances.end(),
                        [id](const Win& w) { return w.id == id; });

                    if (it != instances.end()) {
                        instances.erase(it, instances.end());
                        cout << "[Async] Destroyed window with ID " << id << endl;
                    }
                    else {
                        cout << "[Async] No window with ID " << id << " found." << endl;
                    }
                    });
            }
        
        }
       
    }
}


int main() {
    return 0;
}