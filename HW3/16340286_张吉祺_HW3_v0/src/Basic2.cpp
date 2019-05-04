#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <math.h>

const uint SCR_WIDTH = 800;
const uint SCR_HEIGHT = 600;

const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
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

std::vector<int> bresenhamCircle(int c_x, int c_y, int r)
{
    int cnt = r / sqrt(2);
    std::vector<int> coordinates_xy(16 * cnt);
    int x = 0;
    int y = r;
    int d_i = 3 - 2 * r;
    coordinates_xy[0] = x;
    coordinates_xy[1] = y;
    for(int i = 1; i < cnt; ++i)
    {
        if(d_i <= 0)
            d_i += 4 * x + 6;
        else
        {
            d_i += 4 * (x - y) + 10;
            --y;
        }
        coordinates_xy[2 * i] = ++x;
        coordinates_xy[2 * i + 1] = y;
    }
    for(int i = 0; i < cnt; ++i)
    {
        coordinates_xy[2 * cnt + 2 * i]      =  coordinates_xy[2 * i + 1] + c_x;
        coordinates_xy[2 * cnt + 2 * i + 1]  =  coordinates_xy[2 * i] + c_y;
        coordinates_xy[4 * cnt + 2 * i]      =  coordinates_xy[2 * i + 1] + c_x;
        coordinates_xy[4 * cnt + 2 * i + 1]  = -coordinates_xy[2 * i] + c_y;
        coordinates_xy[6 * cnt + 2 * i]      =  coordinates_xy[2 * i] + c_x;
        coordinates_xy[6 * cnt + 2 * i + 1]  = -coordinates_xy[2 * i + 1] + c_y;
        coordinates_xy[8 * cnt + 2 * i]      = -coordinates_xy[2 * i] + c_x;
        coordinates_xy[8 * cnt + 2 * i + 1]  = -coordinates_xy[2 * i + 1] + c_y;
        coordinates_xy[10 * cnt + 2 * i]     = -coordinates_xy[2 * i + 1] + c_x;
        coordinates_xy[10 * cnt + 2 * i + 1] = -coordinates_xy[2 * i] + c_y;
        coordinates_xy[12 * cnt + 2 * i]     = -coordinates_xy[2 * i + 1] + c_x;
        coordinates_xy[12 * cnt + 2 * i + 1] =  coordinates_xy[2 * i] + c_y;
        coordinates_xy[14 * cnt + 2 * i]     = -coordinates_xy[2 * i] + c_x;
        coordinates_xy[14 * cnt + 2 * i + 1] =  coordinates_xy[2 * i + 1] + c_y;
        coordinates_xy[2 * i]               +=  c_x;
        coordinates_xy[2 * i + 1]           +=  c_y;
    }
    return coordinates_xy;
}

float* normalize(std::vector<int> coordinates_xy, int cnt)
{
    float* coordinates = new float[3 * cnt];
    for(int i = 0; i < cnt; ++i)
    {
        coordinates[3 * i] = (float)coordinates_xy[2 * i] / SCR_WIDTH;
        coordinates[3 * i + 1] = (float)coordinates_xy[2 * i + 1] / SCR_HEIGHT;
        coordinates[3 * i + 2] = 0.0f;
    }
    return coordinates;
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CG_HW3", NULL, NULL);
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
    
    std::vector<int> coordinates_xy = bresenhamCircle(100, -100, 400);
    
    int cnt = (int) coordinates_xy.size() / 2;
    float* coordinates = normalize(coordinates_xy, cnt);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 3 * cnt * sizeof(float), coordinates, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind VBO
    glBindVertexArray(0); // unbind VAO
    
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
    
    // render loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        
        glPointSize(5);
        glDrawArrays(GL_POINTS, 0, cnt);
        
        // swap buffers
        glfwSwapBuffers(window);
        
        // poll IO events
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    delete[] coordinates;
    
    glfwTerminate();
    return 0;
}
