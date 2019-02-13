add_library(common common/defaults.h common/timestamp.h common/timestamp.cpp)
target_include_directories(common PUBLIC ${PROJECT_SOURCE_DIR}/third_party)
target_include_directories(common INTERFACE .)