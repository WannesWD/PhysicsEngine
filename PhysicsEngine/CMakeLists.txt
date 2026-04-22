# setup inpsired by https://www.3dgep.com/learning-directx-12-1/#CMake
cmake_minimum_required (VERSION 3.8)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # Output directory for static lib (.LIB)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # Output directory for shared lib (.DLL)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # Output directory for executables (.EXE)

project ("PhysicsEngine")

add_subdirectory(Test0)
add_subdirectory(Test1)
add_subdirectory( DX12Lib )

set_directory_properties(PROPERTIES VS_STARTUP_PROJECT Test0)