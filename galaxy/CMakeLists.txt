# CMakeList.txt : CMake project for blackhole, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (blackhole "blackhole.cpp" "blackhole.h" "shaderUtil.cpp" "glad.c" "camera.cpp")


# TODO: Add tests and install targets if needed.
# Include GLFW headers
target_include_directories(blackhole PRIVATE
    ${CMAKE_SOURCE_DIR}/dependencies/include
)

# Link GLFW + OpenGL
target_link_directories(blackhole PRIVATE
    ${CMAKE_SOURCE_DIR}/dependencies/lib-vc2022
)
if (NOT APPLE)
    target_link_libraries(blackhole PUBLIC glfw3 opengl32)
else ()
    set(OpenGL_GL_PREFERENCE LEGACY)
    find_package(OpenGL REQUIRED)
    target_link_libraries(blackhole
        PRIVATE
        OpenGL::GL
        "-framework Cocoa"
        "-framework OpenGL"
        "-framework IOKit"
        "-framework CoreVideo"
        glfw3
    )
endif()