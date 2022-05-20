#include <atomic>
#include <thread>
#include <chrono>
#include <string>
#include <iostream>
#include <sstream>
#include <mutex>
using namespace std::chrono_literals;


class Teleoperate {
    std::atomic<bool> ok;
    std::mutex mtx;
    std::chrono::time_point<std::chrono::system_clock> end_time{std::chrono::time_point<std::chrono::system_clock>::max()};
    void setSpeed(int lspeed, int rspeed) {printf(" L:%2d  R:%2d\n", lspeed, rspeed);}
    void stop() {
        setSpeed(0, 0);
        end_time = std::chrono::time_point<std::chrono::system_clock>::max();
    }

    void checkTimeOut() {
        while (ok) {
            {
                std::lock_guard lck(mtx);
                if (std::chrono::system_clock::now() > end_time) {
                    stop();
                }
            }
            std::this_thread::sleep_for(1ms);
        }
    }
public:

    void run() {
        ok = true;
        std::thread thread(&Teleoperate::checkTimeOut, this);
        for (std::string line; std::getline(std::cin, line);) {
            char command = 's';
            int value = 0;
            std::istringstream iss(line);
            iss >> command >> value;

            if (value == 0) {
                if (command = 's') {
                    stop();
                }
                continue;
            }

            if (command >= 'A' && command <= 'Z') command += 32;

            std::lock_guard lck(mtx);
            switch (command)
            {
            case 'q': setSpeed( 0,  1); break;
            case 'a': setSpeed(-1,  1); break;
            case 'z': setSpeed(-1,  0); break;
            case 'w': setSpeed( 1,  1); break;
            case 's': setSpeed( 0,  0); break;
            case 'x': setSpeed(-1, -1); break;
            case 'e': setSpeed( 1,  0); break;
            case 'd': setSpeed( 1, -1); break;
            case 'c': setSpeed( 0, -1); break;
            
            default:
                break;
            }
            end_time = std::chrono::system_clock::now() + std::chrono::milliseconds(value);
        }

        ok = false;
        thread.join();
    }
};


int main() {
    Teleoperate t;
    t.run();
    return 0;
}