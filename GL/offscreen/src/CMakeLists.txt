execute_process(COMMAND gengetopt --input=${PROJECT_SOURCE_DIR}/doc/cmdline.ggo --include-getopt --output-dir=${PROJECT_SOURCE_DIR}/src)
execute_process(COMMAND mv ${PROJECT_SOURCE_DIR}/src/cmdline.c ${PROJECT_SOURCE_DIR}/src/cmdline.cpp)

# include   -I
include_directories(
    ${CMAKE_GLFW_DIR}/deps
    ${CMAKE_GLFW_DIR}/include
    )
# library   -L
link_directories(${CMAKE_GLFW_DIR}/build/src)

set (SRC_LIST

${CMAKE_GLFW_DIR}/deps/glad.c
cmdline.cpp
cmdline.h
pngdecoder.c
pngdecoder.h
offscreen.cpp
offscreen.h
main.cpp
)

add_executable(offscreen ${SRC_LIST})

target_link_libraries(offscreen glfw3 m X11 dl pthread png)
