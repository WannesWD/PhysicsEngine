cmake_minimum_required (VERSION 3.8)

set( HEADER_FILES
	"inc/d3dx12.h"
    "inc/Helpers.h"
)

set ( SRC_FILES
	"src/main.cpp"
)

add_executable( Test0 WIN32
    ${HEADER_FILES} 
    ${SRC_FILES}
)

target_include_directories( Test0
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries( Test0
    d3d12.lib
    dxgi.lib
    dxguid.lib
)