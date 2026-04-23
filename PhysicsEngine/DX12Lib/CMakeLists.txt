cmake_minimum_required (VERSION 3.8)

set( HEADER_FILES
	"inc/d3dx12.h"
    "inc/Helpers.h"
    "inc/App.h"
    "inc/CommandQueue.h"
    "inc/Game.h"
    "inc/Window.h"
    "inc/PCH.h"
    "inc/Events.h"
    "inc/HighResolutionClock.h"
)

set ( SOURCE_FILES
	"src/App.cpp"
    "src/CommandQueue.cpp"
    "src/Game.cpp"
    "src/Window.cpp"
)


add_library( DX12Lib STATIC
    ${HEADER_FILES}
    ${SOURCE_FILES}
    ${RESOURCE_FILES}
)

target_include_directories( DX12Lib
    PUBLIC inc
)

target_link_libraries( DX12Lib 
    PUBLIC d3d12.lib
    PUBLIC dxgi.lib
    PUBLIC dxguid.lib
    PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/DX12Lib.dir/${CMAKE_CFG_INTDIR}/DX12Lib.res # This is the only way I could figure out how to link the compiled resource file.
)