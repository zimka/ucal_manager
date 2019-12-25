#include <iostream>
#include <common/exceptions.h>
#include <service/server_impl.h>
#include <common/logger.h>


int main_service() {
    auto logger = common::createDefaultLogger();
    logger->log("Init server");
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
