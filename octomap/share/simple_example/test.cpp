#include <iostream>
#include <string>
#include <chrono>

void print_time(std::string s) {
    auto currentTime = std::chrono::system_clock::now();

    auto duration = currentTime.time_since_epoch();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration);

    long long microsecCount = microseconds.count();

    std::cout << s << ":" << microsecCount << std::endl;
}

int main() {
    print_time("start");
    return 0;
}