#include <iostream>
#include <vector>
#include <algorithm>
#include <future>
#include <thread>
#include <memory>
#include <unordered_map>

// Platform-specific window class include
#if defined(_WIN32)
#include "deps/WinMS.h"
#elif defined(__linux__)
#include "deps/WinLinux.h"
#elif defined(__APPLE__)
#include "deps/WinMac.h"
#else
#error "Unsupported platform"
#endif

using namespace std;

// This API is accessed from JS, from a webview/Web running inside a window,
// that loads a single HTML file.
namespace warp {
    namespace api {
        namespace windows {

            unordered_map<std::string, std::unique_ptr<Win>> instances;


            // Helper to get Win instance by id or throw
            Win& get(const std::string& id) {
                auto it = instances.find(id);
                if (it == instances.end()) {
                    throw std::runtime_error("No window with ID " + id + " found.");
                }
                return *(it->second);
            }

            // Create a new window instance (string id)
            std::future<void> create(const std::string& id) {
                return std::async(std::launch::async, [=]() {
                    auto win = std::make_unique<Win>(id);
                    win->loop();
                    instances[id] = std::move(win);
                    std::cout << "[Async] Created window with ID " << id << std::endl;
                    });
            }

            // Destroy a window instance by ID
            std::future<void> destroy(const std::string& id) {
                return std::async(std::launch::async, [=]() {
                    try {
                        get(id).destroy();
                        instances.erase(id);
                        std::cout << "[Async] Destroyed window with ID " << id << std::endl;
                    }
                    catch (const std::runtime_error& e) {
                        std::cout << "[Async] " << e.what() << std::endl;
                    }
                    });
            }

            // Width getter
            std::future<int> width(const std::string& id) {
                return std::async(std::launch::async, [=]() -> int {
                    try {
                        return get(id).width();
                    }
                    catch (const std::runtime_error& e) {
                        std::cout << "[Async] " << e.what() << std::endl;
                        return -1;
                    }
                    });
            }

            // Width setter
            std::future<void> width(const std::string& id, int newWidth) {
                return std::async(std::launch::async, [=]() {
                    try {
                        get(id).width(newWidth);
                        std::cout << "[Async] Set width of window " << id << " to " << newWidth << std::endl;
                    }
                    catch (const std::runtime_error& e) {
                        std::cout << "[Async] " << e.what() << std::endl;
                    }
                    });
            }

            // Height getter
            std::future<int> height(const std::string& id) {
                return std::async(std::launch::async, [=]() -> int {
                    try {
                        return get(id).height();
                    }
                    catch (const std::runtime_error& e) {
                        std::cout << "[Async] " << e.what() << std::endl;
                        return -1;
                    }
                    });
            }

            // Height setter
            std::future<void> height(const std::string& id, int newHeight) {
                return std::async(std::launch::async, [=]() {
                    try {
                        get(id).height(newHeight);
                        std::cout << "[Async] Set height of window " << id << " to " << newHeight << std::endl;
                    }
                    catch (const std::runtime_error& e) {
                        std::cout << "[Async] " << e.what() << std::endl;
                    }
                    });
            }

            // X getter
            std::future<int> x(const std::string& id) {
                return std::async(std::launch::async, [=]() -> int {
                    try {
                        return get(id).x();
                    }
                    catch (const std::runtime_error& e) {
                        std::cout << "[Async] " << e.what() << std::endl;
                        return -1;
                    }
                    });
            }

            // X setter
            std::future<void> x(const std::string& id, int newX) {
                return std::async(std::launch::async, [=]() {
                    try {
                        get(id).x(newX);
                        std::cout << "[Async] Set X of window " << id << " to " << newX << std::endl;
                    }
                    catch (const std::runtime_error& e) {
                        std::cout << "[Async] " << e.what() << std::endl;
                    }
                    });
            }

            // Y getter
            std::future<int> y(const std::string& id) {
                return std::async(std::launch::async, [=]() -> int {
                    try {
                        return get(id).y();
                    }
                    catch (const std::runtime_error& e) {
                        std::cout << "[Async] " << e.what() << std::endl;
                        return -1;
                    }
                    });
            }

            // Y setter
            std::future<void> y(const std::string& id, int newY) {
                return std::async(std::launch::async, [=]() {
                    try {
                        get(id).y(newY);
                        std::cout << "[Async] Set Y of window " << id << " to " << newY << std::endl;
                    }
                    catch (const std::runtime_error& e) {
                        std::cout << "[Async] " << e.what() << std::endl;
                    }
                    });
            }

            // State getter
            std::future<int> state(const std::string& id) {
                return std::async(std::launch::async, [=]() -> int {
                    try {
                        return get(id).state();
                    }
                    catch (const std::runtime_error& e) {
                        std::cout << "[Async] " << e.what() << std::endl;
                        return -1;
                    }
                    });
            }

            // State setter
            std::future<void> state(const std::string& id, int newState) {
                return std::async(std::launch::async, [=]() {
                    try {
                        get(id).state(newState);
                        std::cout << "[Async] Set state of window " << id << " to " << newState << std::endl;
                    }
                    catch (const std::runtime_error& e) {
                        std::cout << "[Async] " << e.what() << std::endl;
                    }
                    });
            }

        } // namespace windows
    } // namespace api
} // namespace warp


int main() {
    // Example object usage
    auto win = make_unique<Win>("MyWindowID");
    win->width(480);
    win->height(360);
    win->state(1); // Normal
    //usage through reflection, asyncronous
    auto createFuture = warp::api::windows::create("MyWindow2");
    createFuture.get(); // wait for creation
    warp::api::windows::state("MyWindow2", 1).get();
    warp::api::windows::x("MyWindow2", 100).get();
    warp::api::windows::width("MyWindow2", 1024).get();

    return win->loop();
}
