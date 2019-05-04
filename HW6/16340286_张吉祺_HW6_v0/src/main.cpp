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

const char *cubeVertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;"
"layout (location = 1) in vec3 aNormal;"
"out vec3 FragPos;"
"out vec3 Normal;"
"uniform mat4 model;"
"uniform mat4 view;"
"uniform mat4 projection;"
"void main()"
"{"
"   FragPos = vec3(model * vec4(aPos, 1.0));"
"   gl_Position = projection * view * vec4(FragPos, 1.0);"
"   Normal = mat3(transpose(inverse(model))) * aNormal;"
"}\0";

const char *cubeFragmentShaderSource =
"#version 330 core\n"
"in vec3 FragPos;"
"in vec3 Normal;"
"out vec4 FragColor;"
"uniform vec3 lightPos;"
"uniform vec3 lightColor;"
"uniform vec3 objectColor;"
"uniform vec3 viewPos;"
"uniform float ambientStrength;"
"uniform float diffuseStrength;"
"uniform float specularStrength;"
"uniform float shininess;"
"void main()"
"{"
"   vec3 ambient = ambientStrength * lightColor;"
"   vec3 norm = normalize(Normal);"
"   vec3 lightDir = normalize(lightPos - FragPos);"
"   float diff = max(dot(norm, lightDir), 0.0);"
"   vec3 diffuse = diffuseStrength * diff * lightColor;"
"   vec3 viewDir = normalize(viewPos - FragPos);"
"   vec3 reflectDir = reflect(-lightDir, norm);"
"   float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);"
"   vec3 specular = specularStrength * spec * lightColor;"
"   FragColor = vec4((ambient + diffuse + specular) * objectColor, 1.0);"
"}\0";

const char *cubeGouraudVertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;"
"layout (location = 1) in vec3 aNormal;"
"uniform mat4 model;"
"uniform mat4 view;"
"uniform mat4 projection;"
"uniform vec3 lightPos;"
"uniform vec3 lightColor;"
"uniform vec3 objectColor;"
"uniform vec3 viewPos;"
"uniform float ambientStrength;"
"uniform float diffuseStrength;"
"uniform float specularStrength;"
"out vec4 vertexColor;"
"void main()"
"{"
"   vec3 FragPos = vec3(model * vec4(aPos, 1.0));"
"   gl_Position = projection * view * vec4(FragPos, 1.0);"
"   vec3 ambient = ambientStrength * lightColor;"
"   vec3 norm = normalize(mat3(transpose(inverse(model))) * aNormal);"
"   vec3 lightDir = normalize(lightPos - FragPos);"
"   float diff = max(dot(norm, lightDir), 0.0);"
"   vec3 diffuse = diffuseStrength * diff * lightColor;"
"   vec3 viewDir = normalize(viewPos - FragPos);"
"   vec3 reflectDir = reflect(-lightDir, norm);"
"   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);"
"   vec3 specular = specularStrength * spec * lightColor;"
"   vertexColor = vec4((ambient + diffuse + specular) * objectColor, 1.0);"
"}\0";

const char *cubeGouraudFragmentShaderSource =
"#version 330 core\n"
"in vec4 vertexColor;"
"out vec4 FragColor;"
"void main()"
"{"
"   FragColor = vertexColor;"
"}\0";

const char *lightVertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;"
"uniform mat4 model;"
"uniform mat4 view;"
"uniform mat4 projection;"
"void main()"
"{"
"   gl_Position = projection * view * model * vec4(aPos, 1.0);"
"}\0";

const char *lightFragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;"
"void main()"
"{"
"   FragColor = vec4(1.0f);"
"}\0";

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

bool enableInput = true;
void processInput(GLFWwindow *window, float cameraSpeed, float sensitivity, Camera & camera)
{
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        enableInput = !enableInput;
    if (!enableInput)
        return;
    
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
    
    float vertices[] =
    {
        -2.0f, -2.0f, -2.0f,  0.0f,  0.0f, -1.0f,
         2.0f, -2.0f, -2.0f,  0.0f,  0.0f, -1.0f,
         2.0f,  2.0f, -2.0f,  0.0f,  0.0f, -1.0f,
         2.0f,  2.0f, -2.0f,  0.0f,  0.0f, -1.0f,
        -2.0f,  2.0f, -2.0f,  0.0f,  0.0f, -1.0f,
        -2.0f, -2.0f, -2.0f,  0.0f,  0.0f, -1.0f,
        
        -2.0f, -2.0f,  2.0f,  0.0f,  0.0f,  1.0f,
         2.0f, -2.0f,  2.0f,  0.0f,  0.0f,  1.0f,
         2.0f,  2.0f,  2.0f,  0.0f,  0.0f,  1.0f,
         2.0f,  2.0f,  2.0f,  0.0f,  0.0f,  1.0f,
        -2.0f,  2.0f,  2.0f,  0.0f,  0.0f,  1.0f,
        -2.0f, -2.0f,  2.0f,  0.0f,  0.0f,  1.0f,
        
        -2.0f,  2.0f,  2.0f, -1.0f,  0.0f,  0.0f,
        -2.0f,  2.0f, -2.0f, -1.0f,  0.0f,  0.0f,
        -2.0f, -2.0f, -2.0f, -1.0f,  0.0f,  0.0f,
        -2.0f, -2.0f, -2.0f, -1.0f,  0.0f,  0.0f,
        -2.0f, -2.0f,  2.0f, -1.0f,  0.0f,  0.0f,
        -2.0f,  2.0f,  2.0f, -1.0f,  0.0f,  0.0f,
        
         2.0f,  2.0f,  2.0f,  1.0f,  0.0f,  0.0f,
         2.0f,  2.0f, -2.0f,  1.0f,  0.0f,  0.0f,
         2.0f, -2.0f, -2.0f,  1.0f,  0.0f,  0.0f,
         2.0f, -2.0f, -2.0f,  1.0f,  0.0f,  0.0f,
         2.0f, -2.0f,  2.0f,  1.0f,  0.0f,  0.0f,
         2.0f,  2.0f,  2.0f,  1.0f,  0.0f,  0.0f,
        
        -2.0f, -2.0f, -2.0f,  0.0f, -1.0f,  0.0f,
         2.0f, -2.0f, -2.0f,  0.0f, -1.0f,  0.0f,
         2.0f, -2.0f,  2.0f,  0.0f, -1.0f,  0.0f,
         2.0f, -2.0f,  2.0f,  0.0f, -1.0f,  0.0f,
        -2.0f, -2.0f,  2.0f,  0.0f, -1.0f,  0.0f,
        -2.0f, -2.0f, -2.0f,  0.0f, -1.0f,  0.0f,
        
        -2.0f,  2.0f, -2.0f,  0.0f,  1.0f,  0.0f,
         2.0f,  2.0f, -2.0f,  0.0f,  1.0f,  0.0f,
         2.0f,  2.0f,  2.0f,  0.0f,  1.0f,  0.0f,
         2.0f,  2.0f,  2.0f,  0.0f,  1.0f,  0.0f,
        -2.0f,  2.0f,  2.0f,  0.0f,  1.0f,  0.0f,
        -2.0f,  2.0f, -2.0f,  0.0f,  1.0f,  0.0f
    };
    
    uint cubeVAO, lightVAO, VBO;
    
    glGenVertexArrays(1, &cubeVAO);
    
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // pos
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1); // color
    
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // pos
    
    // shaders
    int cubeVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(cubeVertexShader, 1, &cubeVertexShaderSource, NULL);
    glCompileShader(cubeVertexShader);
    
    int cubeFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(cubeFragmentShader, 1, &cubeFragmentShaderSource, NULL);
    glCompileShader(cubeFragmentShader);
    
    int cubeGouraudVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(cubeGouraudVertexShader, 1, &cubeGouraudVertexShaderSource, NULL);
    glCompileShader(cubeGouraudVertexShader);
    
    int cubeGouraudFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(cubeGouraudFragmentShader, 1, &cubeGouraudFragmentShaderSource, NULL);
    glCompileShader(cubeGouraudFragmentShader);
    
    int lightVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(lightVertexShader, 1, &lightVertexShaderSource, NULL);
    glCompileShader(lightVertexShader);
    
    int lightFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(lightFragmentShader, 1, &lightFragmentShaderSource, NULL);
    glCompileShader(lightFragmentShader);
    
    // shader program object
    int cubeShaderProgram = glCreateProgram();
    glAttachShader(cubeShaderProgram, cubeVertexShader);
    glAttachShader(cubeShaderProgram, cubeFragmentShader);
    glLinkProgram(cubeShaderProgram);
    
    int cubeGouraudShaderProgram = glCreateProgram();
    glAttachShader(cubeGouraudShaderProgram, cubeGouraudVertexShader);
    glAttachShader(cubeGouraudShaderProgram, cubeGouraudFragmentShader);
    glLinkProgram(cubeGouraudShaderProgram);
    
    int lightShaderProgram = glCreateProgram();
    glAttachShader(lightShaderProgram, lightVertexShader);
    glAttachShader(lightShaderProgram, lightFragmentShader);
    glLinkProgram(lightShaderProgram);
    
    glDeleteShader(cubeVertexShader);
    glDeleteShader(cubeFragmentShader);
    glDeleteShader(lightVertexShader);
    glDeleteShader(lightFragmentShader);
    
    // imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();
    
    // render loop
    float ambientStrength = 0.1, diffuseStrength = 1.0, specularStrength = 0.5, shininess = 32.0;
    Camera camera = Camera(glm::vec3(8.0f, -8.0f, 10.0f));
    bool encircle = false, phong = true, gouraud = false;
    int cubeProgram = cubeShaderProgram;
    glm::vec3 lightPos(-5.0f, -5.0f, 5.0f);
    while (!glfwWindowShouldClose(window))
    {
        processInput(window, 0.05f, 0.2f, camera);
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        
        // imgui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Shading");
        
        ImGui::Checkbox("Phong", &phong);
        if (phong || !gouraud)
        {
            phong = true;
            gouraud = false;
            cubeProgram = cubeShaderProgram;
        }
        ImGui::Checkbox("Gouraud", &gouraud);
        if (gouraud || !phong)
        {
            gouraud = true;
            phong = false;
            cubeProgram = cubeGouraudShaderProgram;
        }
        ImGui::Checkbox("Encircle", &encircle);
        ImGui::SliderFloat("Ambient", &ambientStrength, 0.0f, 2.0f);
        ImGui::SliderFloat("Diffuse", &diffuseStrength, 0.0f, 2.0f);
        ImGui::SliderFloat("Specular", &specularStrength, 0.0f, 2.0f);
        ImGui::SliderFloat("Shininess", &shininess, 2.0f, 256.0f);
        
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        if (encircle)
        {
            float radius =5.0f;
            float lightX = sin(glfwGetTime()) * radius;
            float lightZ = cos(glfwGetTime()) * radius;
            lightPos = glm::vec3(lightX, -5.0f, lightZ);
        }
        
        glUseProgram(cubeProgram);
        glUniform3f(glGetUniformLocation(cubeProgram, "objectColor"), 1.0f, 0.5f, 0.31f);
        glUniform3f(glGetUniformLocation(cubeProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
        glUniform3fv(glGetUniformLocation(cubeProgram, "lightPos"), 1, glm::value_ptr(lightPos));
        
        glm::mat4 model(1.0f);
        glm::mat4 view = camera.getView();
        glm::mat4 projection = glm::perspective(45.0f, (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);
        
        // pass to the shaders
        glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(glGetUniformLocation(cubeProgram, "viewPos"), 1, glm::value_ptr(camera.getCameraPos()));
        glUniform1f(glGetUniformLocation(cubeProgram, "ambientStrength"), ambientStrength);
        glUniform1f(glGetUniformLocation(cubeProgram, "diffuseStrength"), diffuseStrength);
        glUniform1f(glGetUniformLocation(cubeProgram, "specularStrength"), specularStrength);
        glUniform1f(glGetUniformLocation(cubeProgram, "shininess"), shininess);
        
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        glUseProgram(lightShaderProgram);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        // swap buffers
        glfwSwapBuffers(window);
        // poll IO events
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
    
    glfwTerminate();
    return 0;
}
