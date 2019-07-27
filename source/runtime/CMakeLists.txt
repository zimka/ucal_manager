set(HEADERS
    runtime/block.h
)
set(SOURCES
    runtime/block.cpp
)

add_library(runtime ${HEADERS} ${SOURCES})
target_link_libraries(runtime PUBLIC common)
target_include_directories(device PUBLIC .)
