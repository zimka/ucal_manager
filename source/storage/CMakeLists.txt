set(H_LIST
	storage/signal.h 
)
set(CPP_LIST
	storage/signal.cpp
)
add_library(storage ${H_LIST} ${CPP_LIST})

target_include_directories(storage INTERFACE .)