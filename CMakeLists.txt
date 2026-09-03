cmake_minimum_required(VERSION 3.20)
project(ConsoleGeo VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(ConsoleGeo ${SOURCES})

target_include_directories(ConsoleGeo PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_compile_options(ConsoleGeo PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/O2>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-O3>
)

if(WIN32)
    if(MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    else()
        target_link_options(ConsoleGeo PRIVATE -static)
    endif()
endif()

set(FTXUI_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(FTXUI_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(FTXUI_USE_WCHAR OFF CACHE BOOL "" FORCE)

add_subdirectory(
    "${CMAKE_CURRENT_SOURCE_DIR}/thrid_party/ftxui/FTXUI-main"
    "${CMAKE_BINARY_DIR}/_deps/ftxui-build"
    EXCLUDE_FROM_ALL
)

target_link_libraries(ConsoleGeo PRIVATE
    ftxui::dom
    ftxui::screen
    ftxui::component
)