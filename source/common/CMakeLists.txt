set(H_LIST
    common/measures.h common/timestamp.h common/utils.h common/keys.h common/exceptions.h
    common/config.h
    common/logger.h
)
set(CPP_LIST
    common/timestamp.cpp
    common/config.cpp
    common/logger.cpp
)
add_library(common ${H_LIST} ${CPP_LIST})

target_include_directories(common PUBLIC ${PROJECT_SOURCE_DIR}/third_party)
target_include_directories(common INTERFACE .)
