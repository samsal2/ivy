cmake_minimum_required(VERSION 3.23.1)

project(Ivy)

# find external libraries
find_package(Vulkan REQUIRED)

# declare the library 
add_library(${PROJECT_NAME})

# features
set_target_properties(${PROJECT_NAME}
  PROPERTIES 
    C_STANDARD          90
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS        OFF)

# compile options
target_compile_options(${PROJECT_NAME} PUBLIC
  "$<$<COMPILE_LANG_AND_ID:C,Clang,AppleClang>:"
    -Wall
    -Wshadow
    -Wextra
    -Werror
    -Wpedantic
    -pedantic-errors
    -Walloca 
    -Wvla 
    -fdiagnostics-color=always

    -Wno-long-long 
    -Wno-comment
    -Wno-comment
    -Wno-variadic-macros
    -Wno-typedef-redefinition
    -Wno-c99-extensions
    -Wno-c11-extensions
  ">"
  "$<$<AND:$<COMPILE_LANG_AND_ID:C,Clang,AppleClang>,$<CONFIG:Debug>>:"
    -O0
    -g
    -fsanitize=address
    -fsanitize=undefined
    -fsanitize=bounds
  ">"
  "$<$<AND:$<COMPILE_LANG_AND_ID:C,Clang,AppleClang>,$<CONFIG:Release>>:"
    -Ofast
  ">")

# link options
target_link_options(${PROJECT_NAME} PUBLIC
  "$<$<AND:$<COMPILE_LANG_AND_ID:C,Clang,AppleClang>,$<CONFIG:Debug>>:"
    -fsanitize=address
    -fsanitize=undefined
    -fsanitize=bounds
  ">")

# link libraries
target_link_libraries(${PROJECT_NAME} 
  PUBLIC ${Vulkan_LIBRARIES})

# includes
target_include_directories(${PROJECT_NAME} 
  PUBLIC ${Vulkan_INCLUDE_DIR} Source)

target_compile_definitions(${PROJECT_NAME} 
  PUBLIC 
    IVY_API=
    IVY_USE_COCOA=
  "$<$<CONFIG:DEBUG>:" 
    IVY_ENABLE_VULKAN_VALIDATION_LAYERS
  ">"
)

# add sources
add_subdirectory(Dependencies)
add_subdirectory(Source)
add_subdirectory(GLSL)
add_subdirectory(Tests)

add_executable("${PROJECT_NAME}Main" Source/main.c)
target_link_libraries("${PROJECT_NAME}Main" PRIVATE ${PROJECT_NAME})


