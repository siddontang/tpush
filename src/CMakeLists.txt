set(TPUSH_SRCS
    tpush.cpp
    )

include_directories($(TPUSH_SOURCE_DIR)/tnet/core)
include_directories($(TPUSH_SOURCE_DIR)/tnet/http)

add_executable(tpush ${TPUSH_SRCS})
target_link_libraries(tpush tnet_core tnet_http)
