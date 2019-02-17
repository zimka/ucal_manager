set(H_LIST
	common/defaults.h common/timestamp.h common/utils.h common/keys.h
)
set(CPP_LIST
	common/timestamp.cpp
)
add_library(common ${H_LIST} ${CPP_LIST})

target_include_directories(common PUBLIC ${PROJECT_SOURCE_DIR}/third_party)
target_include_directories(common INTERFACE .)