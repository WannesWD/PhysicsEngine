cmake_minimum_required (VERSION 3.8)

set( HEADER_FILES
	"inc/d3dx12.h"
    "inc/Helpers.h"
)

set ( SRC_FILES
	"src/main.cpp"
)

add_executable( Test1 WIN32
    ${HEADER_FILES} 
    ${SRC_FILES}
)

target_include_directories( Test1
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries( Test1
    d3d12.lib
    dxgi.lib
    dxguid.lib
)