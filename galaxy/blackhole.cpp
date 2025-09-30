// blackhole.cpp : Defines the entry point for the application.
//
#include <fstream>
#include <sstream>
#include <string>
#include "blackhole.h"
#include "ray.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "camera.h"
#include "shaderUtil.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

using namespace std;





//made with the "annulus" shape
struct accretionDisk {
    glm::vec3 position;
    float outer_radius;
    float inner_radius;
};

struct BlackHole {
    glm::vec3 position;
    float radius;
    accretionDisk accretionDisk;
};

BlackHole createBlackHole(glm::vec3 position, float black_hole_radius, float disk_outer_radius) {
    //creates a black hole at position with radius
    //also creates an accretion disk
    accretionDisk disk = accretionDisk{ position, disk_outer_radius, black_hole_radius * 2.0f};
    return BlackHole{ position, black_hole_radius, disk};
};

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Mouse tracking
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

// Global camera instance
Camera camera(glm::vec3(0.0f, 6.0f, 55.0f), glm::vec3(0.0f, 5.0f, 5.0f), -90.0f, 0.0f);

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard('W', deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard('S', deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard('A', deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard('D', deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard('U', deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard('B', deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed Y

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required for macOS
    #endif

    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Black Hole Simulation", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }


    

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSwapInterval(1);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // Fullscreen quad with normalized device coordinates
    float quadVertices[] = {
        -1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f
    };

    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0); // unbind VAO


    unsigned int shaderProgram;

    if (GLM_PLATFORM_APPLE)
    {
        shaderProgram = createShaderProgram("../../shaders/vertex.glsl", "../../shaders/fragment.glsl");
    }
    else
    {
        shaderProgram = createShaderProgram("../../../../shaders/vertex.glsl", "../../../../shaders/fragment.glsl");
    }
    int resolutionL = glGetUniformLocation(shaderProgram, "resolution");

    //create a quad that takes up the entire screen, then we can pass all 
    //the work to the fragment shader
    float screenQuad[] = {
        -SCREEN_WIDTH, -SCREEN_HEIGHT, 0.0,
        -SCREEN_WIDTH, SCREEN_HEIGHT, 0.0,
        SCREEN_WIDTH, SCREEN_HEIGHT, 0.0,
        SCREEN_WIDTH, -SCREEN_HEIGHT, 0.0
    };

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //Make black hole
    BlackHole bh = createBlackHole(glm::vec3(0.0, 0.0, 0.0), 6.0f, 35.0f);
    
    int bh_pL = glGetUniformLocation(shaderProgram, "bh_p"); //this creates the location for the black hole position in the shader
    int bh_rL = glGetUniformLocation(shaderProgram, "bh_r"); //this does the same for radius
    int camPosGlobalL = glGetUniformLocation(shaderProgram, "camPosGlobal");
    int camDirGlobalL = glGetUniformLocation(shaderProgram, "camDirGlobal");
    int disk_outer_radiusL = glGetUniformLocation(shaderProgram, "disk_outer_radius");
    int disk_inner_radiusL = glGetUniformLocation(shaderProgram, "disk_inner_radius");
    

    //loads the hdr background (.exr sourced from https://svs.gsfc.nasa.gov/4851/ and converted into hdr)
    int hdrWidth, hdrHeight, hdrChannels;

    float* hdrData;
    if (GLM_PLATFORM_APPLE)
    {
        hdrData = stbi_loadf("../../blackhole/starmap_2020_4k_gal.hdr", &hdrWidth, &hdrHeight, &hdrChannels, 0);
    }
    else
    {
        hdrData = stbi_loadf("../../../../blackhole/starmap_2020_4k_gal.hdr", &hdrWidth, &hdrHeight, &hdrChannels, 0);
    }

    if (!hdrData) {
        std::cerr << "Failed to load HDR image" << std::endl;;
        return -1;
    }

    GLuint hdrTexture;
    glGenTextures(1, &hdrTexture);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, hdrWidth, hdrHeight, 0, GL_RGB, GL_FLOAT, hdrData);


    // lerps to handle minification and magnification of the window
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int hdrMapL = glGetUniformLocation(shaderProgram, "hdrMap");

    stbi_image_free(hdrData); //stb's memory clean up




    bool frozen = false;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */

        glUseProgram(shaderProgram);

        glClear(GL_COLOR_BUFFER_BIT);
        
        //UNCOMMENT THESE IF YOU WANT TO MAKE IT RESIZABLE AND DELETE THE HINT ABOVE BEFORE WINDOW CREATION
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glUniform2f(resolutionL, width, height);
        glUniform3f(bh_pL, bh.position.x, bh.position.y, bh.position.z);
        glUniform1f(bh_rL, bh.radius);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        //defining the camera position and direction. When we make a class it should change this
        glUniform3fv(camPosGlobalL, 1, &camera.Position[0]);
        glUniform3fv(camDirGlobalL, 1, &camera.Forward[0]);
        glUniform3fv(glGetUniformLocation(shaderProgram, "camPosGlobal"), 1, &camera.Position[0]);
        glUniform3fv(glGetUniformLocation(shaderProgram, "camForward"),   1, &camera.Forward[0]);
        glUniform3fv(glGetUniformLocation(shaderProgram, "camRight"),     1, &camera.Right[0]);
        glUniform3fv(glGetUniformLocation(shaderProgram, "camUp"),        1, &camera.Up[0]);
        glUniform1f(glGetUniformLocation(shaderProgram, "fov"), 90.0f);

        //defining accretion disk inner/outer radiuss
        glUniform1f(disk_outer_radiusL, bh.accretionDisk.outer_radius);
        glUniform1f(disk_inner_radiusL, bh.accretionDisk.inner_radius);


        // this puts the background texture into the first open texture and sends info into fragment shader
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glUniform1i(hdrMapL, 0);


        // glEnableClientState(GL_VERTEX_ARRAY);
        // glVertexPointer(3, GL_FLOAT, 0, screenQuad);
        // glDrawArrays(GL_QUADS, 0, 4);
        // glDisableClientState(GL_VERTEX_ARRAY);
         

        if (GLM_PLATFORM_APPLE)
        {
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        } else
        {
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, 0, screenQuad);
            glDrawArrays(GL_QUADS, 0, 4);
            glDisableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, 0, screenQuad);
        }

        /* Swap front and back buffers */



        glfwSwapBuffers(window);


        //adding a frozen thing so that it only renders once until theres a change
        // while (frozen) {
        //     if (!glfwWindowShouldClose(window)) {
        //
        //         glfwWaitEventsTimeout(0.1);
        //
        //         int widthNew, heightNew;
        //         glfwGetFramebufferSize(window, &widthNew, &heightNew);
        //         if (widthNew != width || heightNew != height) {
        //             frozen = false;
        //         }
        //
        //     }
        //     else {
        //         glfwTerminate();
        //         return 0;
        //     }
        //
        // }
        // frozen = true;
        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
