cmake_minimum_required(VERSION 3.16)
project(camera LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_VERSION VERSION_LESS "3.28")
    include(${CMAKE_SOURCE_DIR}/cmake/ImportSFML.cmake)
else ()
    include(FetchContent)
    FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.1.0
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM
    )
    set(SFML_USE_SYSTEM_DEPS OFF)
    FetchContent_MakeAvailable(SFML)
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        target_link_libraries(freetype PRIVATE harfbuzz)
    endif ()
endif ()

configure_file(${CMAKE_SOURCE_DIR}/assets/grunge-tileset.png ${CMAKE_BINARY_DIR}/assets/grunge-tileset.png COPYONLY)

add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE
    SFML::Graphics
    SFML::Window
)
