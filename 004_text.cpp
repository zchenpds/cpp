#include <list>
#include <string>
#include <iostream>
#include <cstdio>
#include <termios.h>

namespace TE {
    using Data = std::list<char>;
    using DataPtr = Data::iterator;

    class RawTerminal {
        termios termios0{}, termios1{};
        bool success = false;
    public:
        RawTerminal() {
            if (tcgetattr(0, &termios0) < 0)
                    perror("tcsetattr()");
            termios1 = termios0;
            termios1.c_lflag &= ~(ECHO|ICANON|ISIG);
            termios1.c_cc[VMIN] = 1;
            termios1.c_cc[VTIME] = 0;
            if (tcsetattr(0, TCSANOW, &termios1) < 0)
                    perror("tcsetattr ICANON");
            else success = true;
        }
        ~RawTerminal() {
            if (success && tcsetattr(0, TCSADRAIN, &termios0) < 0)
                    perror ("tcsetattr ~ICANON");
        }
    };

    class Text {
        Data data;
        DataPtr cursor;
        size_t ln{}, col{};
        bool need_update_col = false;
        void updateCol() {
            auto temp = cursor;
            col = 0;
            while (temp != data.begin()) {
                if (*--temp != '\n') ++col;
                else break;
            }
            need_update_col = false;
        }
    public:

    // Constructor
        Text(): cursor(data.end()) { std::cout << "\e[?25l"; }
        ~Text() { std::cout << "\e[2J\e[H\e[?25h"; }
    
    // Main
        void run() {
            std::cout << "\e[2J\e[H" << *this;
            for (char ch; ch = std::getchar();) {
                // printf("%d\n", (int)ch); continue;
                switch (ch) {
                    case 0x1B:
                        if (std::getchar() == '[') {
                            ch = std::getchar();
                            switch (ch) {
                                case 'A': moveUp(); break;
                                case 'B': moveDown(); break;
                                case 'C': moveRight(); break;
                                case 'D': moveLeft(); break;
                            }
                        }
                        break;
                    case 0x7F: backspace(); break;
                    case 3: case 28: return; // ctrl + c
                    
                    default:
                        insert(ch);
                }
                std::cout << "\e[2J\e[H" << *this;
            }
        }

    // Modifiers
        void insert(char ch) {
            data.insert(cursor, ch);
            if (ch == '\n') {++ln; col = 0;}
            else if (need_update_col) updateCol();
            else ++col;
        }
        void del() {
            if (cursor != data.end()) {
                cursor = data.erase(cursor);
            }
        }
        void backspace() {
            if (cursor != data.begin()) {
                cursor = data.erase(--cursor);
                if (*cursor == '\n') {--ln; updateCol();}
                else if (need_update_col) updateCol();
                else --col;
            }
        }
    
    // Cursor moving
        void moveUp()    {
            size_t n = 2;
            while (cursor != data.begin()) {
                if (*--cursor == '\n') --n;
                if (n == 0) {
                    ++cursor;
                    break;
                }
            }
            if (n != 2) --ln;
            for (size_t c = 0; c < col; ++c) {
                if (cursor == data.end() || *++cursor == '\n') {
                    need_update_col = true;
                    break;
                }
            }
        }
        void moveDown()  {
            auto temp = cursor;
            while (temp != data.end()) {
                if (*temp++ == '\n') break;
            }
            if (temp == data.end()) return;
            ++ln;
            cursor = temp;
            for (size_t c = 0; c < col; ++c) {
                if (cursor == data.end() || *++cursor == '\n') {
                    need_update_col = true;
                    break;
                }
            }
        }
        void moveRight() {
            if (cursor != data.end()) {
                if (*cursor == '\n') {++ln; col = 0;}
                else if (need_update_col) updateCol();
                else ++col;
                ++cursor;
            }
        }
        void moveLeft() {
            if (cursor != data.begin()) {
                --cursor;

                if (*cursor == '\n') {--ln; updateCol();}
                else if (need_update_col) updateCol();
                else --col;
            }
        }
    
    // Print
        friend std::ostream& operator<<(std::ostream& lhs, const Text& rhs) {
            for (auto it = begin(rhs.data); it != end(rhs.data); ++it) {
                if (it == rhs.cursor) {
                    std::cout << "\e[7m" << (*it == '\n' ? std::string(" \n") : std::string(1, *it))<< "\e[0m";
                }
                else std::cout << *it;
            }
            if (end(rhs.data) == rhs.cursor) std::cout << "\e[7m \e[0m";

            std::cout << "\n\nLn " << rhs.ln + 1 << ", Col " << rhs.col + 1;
            return lhs;
        }
    };

    class TextTerminal: public RawTerminal, public Text {};
}

int main() {
    using namespace TE;
    TextTerminal text;
    text.run();
    return 0;
}