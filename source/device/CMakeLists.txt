set(H_LIST
    device/timer.h device/device.h device/acquire.h
)
set(CPP_LIST
    device/timer.cpp device/mock_device.cpp
)
if (WIN32)
    list(APPEND CPP_LIST device/daqboard_device.cpp)
endif()


add_library(device ${H_LIST} ${CPP_LIST})
target_link_libraries(device PUBLIC common storage)
#target_include_directories(device PUBLIC ${CMAKE_BINARY_DIR}/generated/)
target_include_directories(device PUBLIC ${PROJECT_SOURCE_DIR}/third_party)
target_include_directories(device INTERFACE .)
if (WIN32)
    find_library(DAQX_LIBRARY
        DAQX
        HINTS "${CMAKE_SOURCE_DIR}/lib"
    )
    target_link_libraries(device PUBLIC ${DAQX_LIBRARY})
endif()
