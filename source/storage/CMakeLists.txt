set(H_LIST
    storage/signal.h
    storage/frame.h
    storage/storage.h
)
set(CPP_LIST
    storage/signal.cpp
    storage/frame.cpp
    storage/storage.cpp
)
add_library(storage ${H_LIST} ${CPP_LIST})
target_link_libraries(storage PUBLIC common)
target_include_directories(storage PUBLIC ${PROJECT_SOURCE_DIR}/third_party)
target_include_directories(storage INTERFACE .)
