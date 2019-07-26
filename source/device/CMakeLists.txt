set(H_LIST
    device/timer.h device/device.h
)
set(CPP_LIST
    device/timer.cpp device/mock_device.cpp device/daqboard_device.cpp
)
add_library(device ${H_LIST} ${CPP_LIST})
target_link_libraries(device PUBLIC common storage)
target_include_directories(device PUBLIC ${PROJECT_SOURCE_DIR}/third_party)
target_include_directories(device INTERFACE .)
