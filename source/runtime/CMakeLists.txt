set(HEADERS
    runtime/block.h
    runtime/state_machine.h
    runtime/i_state.h
    runtime/core.h
)
set(SOURCES
    runtime/block.cpp
    runtime/state_machine.cpp
    runtime/core.cpp
)

add_library(runtime ${HEADERS} ${SOURCES})
if (USE_REAL_DEVICE)
    target_link_libraries(runtime PUBLIC common device_real storage)
else()
    target_link_libraries(runtime PUBLIC common device_mock storage)
endif()
target_include_directories(runtime PUBLIC .)
