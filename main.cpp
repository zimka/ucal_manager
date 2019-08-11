#include <iostream>
#include "common/measures.h"
#include "device/acquire.h"

int main() {
    auto device = device::acquireDevice();
    std::cout << "Device type: " << typeid(*device).name() << std::endl;
    std::cout << "Press 'Enter'" << std::endl;
    std::cin.get();
}
