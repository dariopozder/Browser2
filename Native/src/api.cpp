#include <iostream>
#include <vector>
#include <algorithm>
#include <future>
#include <thread>
#include <chrono>

using namespace std;

namespace warp {
    namespace api {
        namespace windows {

            class Window {
            public:
                int x, y, width, height, id;

                Window(int id, int x, int y, int width, int height)
                    : id(id), x(x), y(y), width(width), height(height) {}

                void display() const {
                    cout << "Window ID: " << id
                        << ", Position: (" << x << ", " << y << ")"
                        << ", Size: " << width << "x" << height << endl;
                }
            };

            vector<Window> instances;

            // Create a new window instance
            future<void> create(int id, int x, int y, int width, int height) {
                return async(launch::async, [=]() {
                    this_thread::sleep_for(chrono::milliseconds(200));
                    Window newWindow(id, x, y, width, height);
                    instances.push_back(newWindow);
                    cout << "[Async] Created window with ID " << id << endl;
                    });
            }

            // Destroy a window instance by ID
            future<void> destroy(int id) {
                return async(launch::async, [=]() {
                    this_thread::sleep_for(chrono::milliseconds(150));
                    auto it = remove_if(instances.begin(), instances.end(),
                        [id](const Window& w) { return w.id == id; });

                    if (it != instances.end()) {
                        instances.erase(it, instances.end());
                        cout << "[Async] Destroyed window with ID " << id << endl;
                    }
                    else {
                        cout << "[Async] No window with ID " << id << " found." << endl;
                    }
                    });
            }

            void displayAll() {
                cout << "=== Displaying Windows ===" << endl;
                for (const auto& win : instances) {
                    win.display();
                }
                cout << "===========================" << endl;
            }

            void test() {
                auto f1 = create(1, 100, 100, 800, 600);
                auto f2 = create(2, 200, 200, 640, 480);

                f1.wait();
                f2.wait();

                displayAll();

                auto f3 = destroy(1);
                f3.wait();

                displayAll();
            }
        }
        namespace network {

            static string base64_encode(const string& in) {
                static const string base64_chars =
                    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

                string out;
                int val = 0, valb = -6;
                for (unsigned char c : in) {
                    val = (val << 8) + c;
                    valb += 8;
                    while (valb >= 0) {
                        out.push_back(base64_chars[(val >> valb) & 0x3F]);
                        valb -= 6;
                    }
                }
                if (valb > -6)
                    out.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
                while (out.size() % 4)
                    out.push_back('=');
                return out;
            }

            // Curl write callback scoped here
            static size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* output) {
                output->append((char*)contents, size * nmemb);
                return size * nmemb;
            }

            // Actual async GET implementation
            future<string> get(const string& url) {
                return async(launch::async, [=]() -> string {
                    CURL* curl = curl_easy_init();
                    string response;

                    if (curl) {
                        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
                        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
                        CURLcode res = curl_easy_perform(curl);
                        curl_easy_cleanup(curl);

                        if (res != CURLE_OK) {
                            cerr << "[Error] curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
                            return "";
                        }
                    }

                    string encoded = base64_encode(response);
                    string mimeType = "text/plain"; // Can be enhanced
                    return "data:" + mimeType + ";base64," + encoded;
                    });
            }

        }
    }
}





int main() {
   // warp::api::windows::test();
    return 0;
}