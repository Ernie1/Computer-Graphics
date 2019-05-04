#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <math.h>

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

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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
    
    uint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    float vertices[] = {
        -2.0f, -2.0f, -2.0f, 0.5f,  0.0f,  0.0f,
         2.0f, -2.0f, -2.0f, 0.0f,  0.5f,  0.0f,
         2.0f,  2.0f, -2.0f, 0.0f,  0.0f,  0.5f,
        -2.0f,  2.0f, -2.0f, 0.5f,  0.5f,  0.0f,
        -2.0f, -2.0f,  2.0f, 0.0f,  0.5f,  0.5f,
         2.0f, -2.0f,  2.0f, 0.5f,  0.0f,  0.5f,
         2.0f,  2.0f,  2.0f, 1.0f,  0.0f,  0.0f,
        -2.0f,  2.0f,  2.0f, 0.0f,  1.0f,  0.0f
    };
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    uint indices[] = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        7, 3, 0, 0, 4, 7,
        6, 2, 0, 0, 5, 6,
        0, 1, 5, 5, 4, 0,
        3, 2, 6, 6, 7, 3
    };
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
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
    if (!success) {
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
    bool depth_test = true;
    bool translate = false;
    bool rotate = false;
    bool scale = false;
    bool bonus = false;
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);
        
        // imgui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Transformation");
        
        ImGui::Checkbox("Depth Test", &depth_test);
        ImGui::Checkbox("Translate", &translate);
        if (translate)
        {
            rotate = false;
            scale = false;
            bonus = false;
        }
        ImGui::Checkbox("Rotate", &rotate);
        if (rotate)
        {
            scale = false;
            translate = false;
            bonus = false;
        }
        ImGui::Checkbox("Scale", &scale);
        if (scale)
        {
            translate = false;
            rotate = false;
            bonus = false;
        }
        ImGui::Checkbox("Bonus", &bonus);
        if (bonus)
        {
            translate = false;
            rotate = false;
            scale = false;
        }
        
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        depth_test ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
        // create transformations
        glm::mat4 model(1.0f);
        glm::mat4 view(1.0f);
        glm::mat4 projection(1.0f);
        if (translate)
            model = glm::translate(model, glm::vec3(sin((float)glfwGetTime()) * 5.0f, 0.0f, 0.0f));
        else if (rotate)
            model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(1.0f, 0.0f, 1.0f));
        else if (scale)
            model = glm::scale(model, glm::vec3(sin((float)glfwGetTime()) + 1.2f, 1.0f, 1.0f));
        else if (bonus)
        {
            model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
            model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -15.0f));
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        // retrieve the matrix uniform locations
        uint modelLoc = glGetUniformLocation(shaderProgram, "model");
        uint viewLoc  = glGetUniformLocation(shaderProgram, "view");
        uint projLoc = glGetUniformLocation(shaderProgram, "projection");
        // pass to the shaders
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        if (bonus)
        {
            model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
            float time = (float)glfwGetTime();
            model = glm::translate(model, glm::vec3(-cos(time*2) * 20.0f, 0.0f, sin(time*2) * 20.0f));
            model = glm::rotate(model, (time*5) * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            uint modelLoc = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }
        
        // swap buffers
        glfwSwapBuffers(window);
        // poll IO events
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    
    glfwTerminate();
    return 0;
}
