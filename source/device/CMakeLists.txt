set(H_LIST
    device/timer.h device/device.h
)
set(CPP_LIST
    device/timer.cpp
)

#if (WIN32)
#    list(APPEND CPP_LIST device/daqboard_device.cpp)
#endif()


add_library(device_mock ${H_LIST} ${CPP_LIST} device/mock_device.cpp)
target_link_libraries(device_mock PUBLIC common storage)
target_include_directories(device_mock PUBLIC ${PROJECT_SOURCE_DIR}/third_party)
target_include_directories(device_mock INTERFACE .)
if (WIN32)
    find_library(DAQX_LIBRARY
        DAQX
        HINTS "${CMAKE_SOURCE_DIR}/lib"
    )
    add_library(device_real ${H_LIST} ${CPP_LIST} device/daqboard_device.cpp)
    target_link_libraries(device_real PUBLIC common storage)
    target_include_directories(device_real PUBLIC ${PROJECT_SOURCE_DIR}/third_party)
    target_include_directories(device_real INTERFACE .)
    target_link_libraries(device_real PUBLIC ${DAQX_LIBRARY})
endif()
