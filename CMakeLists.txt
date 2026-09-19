cmake_minimum_required(VERSION 3.16)
project(ConsoleGeo VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(ConsoleGeo ${SOURCES})

target_include_directories(ConsoleGeo PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

if(MSVC)
    target_compile_options(ConsoleGeo PRIVATE /O2)
else()
    target_compile_options(ConsoleGeo PRIVATE -O3)
endif()

if(WIN32 AND NOT MSVC)
    target_link_options(ConsoleGeo PRIVATE -static-libgcc -static-libstdc++)
endif()

set(FTXUI_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(FTXUI_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(FTXUI_USE_WCHAR OFF CACHE BOOL "" FORCE)

add_subdirectory(
    "${CMAKE_CURRENT_SOURCE_DIR}/thrid_party/ftxui/FTXUI-main"
    "${CMAKE_BINARY_DIR}/_deps/ftxui-build"
)

target_link_libraries(ConsoleGeo PRIVATE
    ftxui::dom
    ftxui::screen
    ftxui::component
)