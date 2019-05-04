#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

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

void swap(int& a, int& b)
{
    int c(std::move(a));
    a = std::move(b);
    b = std::move(c);
}

std::vector<int> bresenhamLine(int x_start, int y_start, int x_end, int y_end)
{
    int sign = 1;
    int delta_x = x_end - x_start;
    if(delta_x < 0)
    {
        delta_x *= -1;
        swap(x_start, x_end);
        swap(y_start, y_end);
    }
    int delta_y = y_end - y_start;
    // 斜率为负，作y轴轴对称
    if(delta_y < 0)
    {
        delta_y *= -1;
        sign = -1;
        swap(x_start, x_end);
        swap(y_start, y_end);
        x_start *= -1;
        x_end *= -1;
    }
    bool rotate = false;
    // 斜率大于1，x y交换
    if(delta_y > delta_x)
    {
        swap(x_start, y_start);
        swap(x_end, y_end);
        swap(delta_x, delta_y);
        rotate = true;
    }
    std::vector<int> coordinates_xy(2 * delta_x);
    int p_i = 2 * delta_y - delta_x;
    int y_last = y_start;
    for(int i = 0; i < delta_x; ++i)
    {
        if(p_i <= 0)
        p_i += 2 * delta_y;
        else
        {
            ++y_last;
            p_i += 2 * delta_y - 2 * delta_x;
        }
        coordinates_xy[2 * i] = rotate ? sign * y_last : sign * (x_start + i);
        coordinates_xy[2 * i + 1] = rotate ? x_start + i : y_last;
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
    
    int v1_x = -300;
    int v1_y = 500;
    int v2_x = -300;
    int v2_y = -500;
    int v3_x = 300;
    int v3_y = -500;
    
    std::vector<int> coordinates_xy = bresenhamLine(v1_x, v1_y, v2_x, v2_y);
    std::vector<int> line_23 = bresenhamLine(v2_x, v2_y, v3_x, v3_y);
    coordinates_xy.insert(coordinates_xy.end(), line_23.begin(), line_23.end());
    std::vector<int> line_31 = bresenhamLine(v3_x, v3_y, v1_x, v1_y);
    coordinates_xy.insert(coordinates_xy.end(), line_31.begin(), line_31.end());
    
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
