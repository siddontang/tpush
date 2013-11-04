set(TPUSH_SRCS
    tpush.cpp
    ini.c
    config.cpp
    pushconnection.cpp
    channel.cpp
    pushloop.cpp
    pushserver.cpp
    httppushserver.cpp
    wspushserver.cpp
    wsdelimproto.cpp
    )

include_directories(${TPUSH_SOURCE_DIR}/tnet/core)
include_directories(${TPUSH_SOURCE_DIR}/tnet/http)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

add_executable(tpush ${TPUSH_SRCS})
target_link_libraries(tpush tnet_core tnet_http)
