#include <iostream>
#include <raylib.h>
#include <limits>
int main(int argc, char* argv[]) {
    printf("Hello, This is Game Pong Online In Terminal!\n");
    std::cout << "Input S or C (Server or Client): ";
    char ch;
    if (!(std::cin >> ch)) return 0;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 

    if (ch == 'S' || ch == 's') {
        std::cout << "Running as Server...\n";
    } else if (ch == 'C' || ch == 'c') {
        std::cout << "Running as Client...\n";
    } else {
        std::cout << "Lựa chọn không hợp lệ.\n";
    }

    return 0;

    return 0;
}