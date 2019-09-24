set(HEADERS
    runtime/block.h
    runtime/state_machine.h
    runtime/i_state.h
)
set(SOURCES
    runtime/block.cpp
    runtime/state_machine.cpp
)

add_library(runtime ${HEADERS} ${SOURCES})
target_link_libraries(runtime PUBLIC common device)
target_include_directories(runtime PUBLIC .)
