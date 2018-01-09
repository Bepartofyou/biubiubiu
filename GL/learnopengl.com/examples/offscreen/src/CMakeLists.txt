if(UNIX)
    execute_process(COMMAND gengetopt --input=${PROJECT_SOURCE_DIR}/doc/cmdline.ggo --include-getopt --output-dir=${PROJECT_SOURCE_DIR}/src)
    execute_process(COMMAND mv ${PROJECT_SOURCE_DIR}/src/cmdline.c ${PROJECT_SOURCE_DIR}/src/cmdline.cpp)
endif(UNIX)

# include   -I
include_directories(
    ${CMAKE_GL_LIBS_DIR}/include 
    ${CMAKE_GL_LIBS_DIR}/include/glm
    )
    
# library   -L
link_directories(${CMAKE_GL_LIBS_DIR}/lib)

set(SRC_LIST

cmdline.cpp
cmdline.h
stb_image.h
shader.hpp
glad.c
main.cpp 
glfwWin.cpp 
)

add_executable(offscreen ${SRC_LIST})

if(UNIX)
    target_link_libraries(offscreen glfw3 GL X11 pthread dl) 
else(UNIX)
    target_link_libraries(offscreen glfw3 opengl32)
endif(UNIX)
