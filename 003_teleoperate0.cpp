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
    int lspeed{}, rspeed{};
    void setLeftSpeed(int speed) {lspeed = speed; printf(" L:%2d ", speed);}
    void setRightSpeed(int speed) {rspeed = speed; printf(" R:%2d\n", speed);}
    void stop() {setLeftSpeed(0); setRightSpeed(0);}

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
                    end_time = std::chrono::time_point<std::chrono::system_clock>::max();
                }
                continue;
            }

            if (command >= 'A' && command <= 'Z') command += 32;

            std::lock_guard lck(mtx);
            switch (command)
            {
            case 'q': setLeftSpeed( 0); setRightSpeed( 1); break;
            case 'a': setLeftSpeed(-1); setRightSpeed( 1); break;
            case 'z': setLeftSpeed(-1); setRightSpeed( 0); break;
            case 'w': setLeftSpeed( 1); setRightSpeed( 1); break;
            case 's': setLeftSpeed( 0); setRightSpeed( 0); break;
            case 'x': setLeftSpeed(-1); setRightSpeed(-1); break;
            case 'e': setLeftSpeed( 1); setRightSpeed( 0); break;
            case 'd': setLeftSpeed( 1); setRightSpeed(-1); break;
            case 'c': setLeftSpeed( 0); setRightSpeed(-1); break;
            
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