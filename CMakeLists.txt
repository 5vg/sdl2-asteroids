cmake_minimum_required(VERSION 3.15)
project(asteroids CXX)

set(CMAKE_CONFIGURATION_TYPES Debug Release RelWithDebInfo)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin )
set(CMAKE_CXX_STANDARD 11)

file(GLOB_RECURSE asteroids_SRC CONFIGURE_DEPENDS
    "src/*.h"
    "src/*.cpp"
)

find_package(SDL2 REQUIRED)

add_executable(${PROJECT_NAME} ${asteroids_SRC})
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../thirdparty/stb)

set_property (DIRECTORY PROPERTY VS_STARTUP_PROJECT "asteroids")

set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)

SET(CMAKE_SUPPRESS_REGENERATION TRUE)
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:SDL2::SDL2>
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
    VERBATIM)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/data
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
    VERBATIM)