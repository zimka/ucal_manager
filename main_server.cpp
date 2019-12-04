#include <iostream>
#include <common/exceptions.h>
#include <service/server_impl.h>


int main_service() {
    service::RunService("0.0.0.0:10003");
    return 0;
}

int main() {
    try {
        return main_service();
    }
    catch (common::UcalManagerException& e) {
        std::cout << "Failed with exception:" << e.what()<< std::endl;
    }
    return 0;
}
