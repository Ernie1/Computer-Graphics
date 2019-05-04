#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Camera.h"

const uint SCR_WIDTH = 800;
const uint SCR_HEIGHT = 600;

const char *vertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 Color;\n"
"out vec3 vetexColor;\n"
"uniform mat4 model;"
"uniform mat4 view;"
"uniform mat4 projection;"
"void main()\n"
"{\n"
"   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"   vetexColor = Color;\n"
"}\n\0";

const char *fragmentShaderSource =
"#version 330 core\n"
"in vec3 vetexColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(vetexColor, 1.0f);\n"
"}\n\0";

void frame_resize(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

bool mouseFirst = true;
float mouseX = 0.0f, mouseY = 0.0f, mouseMoveX = 0.0f, mouseMoveY = 0.0f;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (mouseFirst)
    {
        mouseX = xpos;
        mouseY = ypos;
        mouseFirst = false;
    }
    mouseMoveX = xpos - mouseX;
    mouseMoveY = ypos - mouseY;
    mouseX = xpos;
    mouseY = ypos;
}

void processInput(GLFWwindow *window, float cameraSpeed, float sensitivity, Camera & camera)
{
    // keyboard
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.moveForward(cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.moveBack(cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.moveLeft(cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.moveRight(cameraSpeed);
    // mouse
    glfwSetCursorPosCallback(window, mouse_callback);
    camera.rotate(mouseMoveX * sensitivity, mouseMoveY * sensitivity);
    mouseMoveX = 0;
    mouseMoveY = 0;
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    const char* glsl_version = "#version 130";
#endif
    
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CG_HW4", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, frame_resize);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    uint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    
    float vertices[] =
    {
        -2.0f, -2.0f, -2.0f,  0.000f, 0.988f, 0.314f,
         2.0f, -2.0f, -2.0f,  0.000f, 0.988f, 0.314f,
         2.0f,  2.0f, -2.0f,  0.000f, 0.988f, 0.314f,
         2.0f,  2.0f, -2.0f,  0.000f, 0.988f, 0.314f,
        -2.0f,  2.0f, -2.0f,  0.000f, 0.988f, 0.314f,
        -2.0f, -2.0f, -2.0f,  0.000f, 0.988f, 0.314f,
        
        -2.0f, -2.0f,  2.0f,  0.000f, 0.988f, 0.592f,
         2.0f, -2.0f,  2.0f,  0.000f, 0.988f, 0.592f,
         2.0f,  2.0f,  2.0f,  0.000f, 0.988f, 0.592f,
         2.0f,  2.0f,  2.0f,  0.000f, 0.988f, 0.592f,
        -2.0f,  2.0f,  2.0f,  0.000f, 0.988f, 0.592f,
        -2.0f, -2.0f,  2.0f,  0.000f, 0.988f, 0.592f,
        
        -2.0f,  2.0f,  2.0f,  0.000f, 0.988f, 0.859f,
        -2.0f,  2.0f, -2.0f,  0.000f, 0.988f, 0.859f,
        -2.0f, -2.0f, -2.0f,  0.000f, 0.988f, 0.859f,
        -2.0f, -2.0f, -2.0f,  0.000f, 0.988f, 0.859f,
        -2.0f, -2.0f,  2.0f,  0.000f, 0.988f, 0.859f,
        -2.0f,  2.0f,  2.0f,  0.000f, 0.988f, 0.859f,
        
         2.0f,  2.0f,  2.0f,  0.000f, 0.859f, 0.988f,
         2.0f,  2.0f, -2.0f,  0.000f, 0.859f, 0.988f,
         2.0f, -2.0f, -2.0f,  0.000f, 0.859f, 0.988f,
         2.0f, -2.0f, -2.0f,  0.000f, 0.859f, 0.988f,
         2.0f, -2.0f,  2.0f,  0.000f, 0.859f, 0.988f,
         2.0f,  2.0f,  2.0f,  0.000f, 0.859f, 0.988f,
        
        -2.0f, -2.0f, -2.0f,  0.000f, 0.576f, 0.988f,
         2.0f, -2.0f, -2.0f,  0.000f, 0.576f, 0.988f,
         2.0f, -2.0f,  2.0f,  0.000f, 0.576f, 0.988f,
         2.0f, -2.0f,  2.0f,  0.000f, 0.576f, 0.988f,
        -2.0f, -2.0f,  2.0f,  0.000f, 0.576f, 0.988f,
        -2.0f, -2.0f, -2.0f,  0.000f, 0.576f, 0.988f,
        
        -2.0f,  2.0f, -2.0f,  0.000f, 0.329f, 0.988f,
         2.0f,  2.0f, -2.0f,  0.000f, 0.329f, 0.988f,
         2.0f,  2.0f,  2.0f,  0.000f, 0.329f, 0.988f,
         2.0f,  2.0f,  2.0f,  0.000f, 0.329f, 0.988f,
        -2.0f,  2.0f,  2.0f,  0.000f, 0.329f, 0.988f,
        -2.0f,  2.0f, -2.0f,  0.000f, 0.329f, 0.988f
    };
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // pos
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1); // color
    
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind VBO
    
    // shaders
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // shader program object
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    // imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();
    
    // render loop
    bool orthographic = true, perspective = false, encircle = false, bonus = false;
    float left = -4.0f, right = 4.0f, bottom = -4.0f, top = 4.0f, zNear = 0.1f, zFar = 100.0f;
    float fovy = 45.0f, aspect = (float)SCR_WIDTH/(float)SCR_HEIGHT;
    Camera camera = Camera();
    while (!glfwWindowShouldClose(window))
    {
        processInput(window, 0.05f, 0.2f, camera);
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);
        
        // imgui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Transformation");
        
        ImGui::Checkbox("Orthographic", &orthographic);
        if (orthographic)
        {
            perspective = false;
            encircle = false;
            bonus = false;
        }
        ImGui::Checkbox("Perspective", &perspective);
        if (perspective)
        {
            orthographic = false;
            encircle = false;
            bonus = false;
        }
        ImGui::Checkbox("Encircle", &encircle);
        if (encircle)
        {
            perspective = false;
            orthographic = false;
            bonus = false;
        }
        ImGui::Checkbox("Bonus", &bonus);
        if (bonus)
        {
            perspective = false;
            encircle = false;
            orthographic = false;
        }
        
        glEnable(GL_DEPTH_TEST);
        // create transformations
        glm::mat4 model(1.0f);
        glm::mat4 view(1.0f);
        glm::mat4 projection(1.0f);
        
        if (orthographic)
        {
            ImGui::SliderFloat("left", &left, -20.0f, 20.0f);
            ImGui::SliderFloat("right", &right, -20.0f, 20.0f);
            ImGui::SliderFloat("bottom", &bottom, -20.0f, 20.0f);
            ImGui::SliderFloat("top", &top, -20.0f, 20.0f);
            ImGui::SliderFloat("near", &zNear, -1.0f, 1.0f);
            ImGui::SliderFloat("far", &zFar, -100.0f, 100.0f);
            model = glm::translate(model, glm::vec3(-1.5f, 0.5f, -1.5f));
            view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));
            projection = glm::ortho(left, right, bottom, top, zNear, zFar);
        }
        else if (perspective)
        {
            ImGui::SliderFloat("fovy", &fovy, 30.0f, 60.0f);
            ImGui::SliderFloat("aspect", &aspect, 0.0f, 10.0f);
            ImGui::SliderFloat("near", &zNear, -1.0f, 1.0f);
            ImGui::SliderFloat("far", &zFar, -100.0f, 100.0f);
            model = glm::translate(model, glm::vec3(-1.5f, 0.5f, -1.5f));
            view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));
            projection = glm::perspective(fovy, aspect, zNear, zFar);
        }
        else if (encircle)
        {
            float radius = 10.0f;
            float camX = sin(glfwGetTime()) * radius;
            float camZ = cos(glfwGetTime()) * radius;
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
            view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            projection = glm::perspective(45.0f, (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);
        }
        else if (bonus)
        {
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
            view = camera.getView();
            projection = glm::perspective(45.0f, (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);
        }
        // retrieve the matrix uniform locations
        uint modelLoc = glGetUniformLocation(shaderProgram, "model");
        uint viewLoc  = glGetUniformLocation(shaderProgram, "view");
        uint projLoc = glGetUniformLocation(shaderProgram, "projection");
        // pass to the shaders
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // swap buffers
        glfwSwapBuffers(window);
        // poll IO events
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    
    glfwTerminate();
    return 0;
}
