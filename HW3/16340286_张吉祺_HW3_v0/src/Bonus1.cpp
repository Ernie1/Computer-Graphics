#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <queue>
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

void bresenhamLine(std::priority_queue<std::pair<int, int>>& coordinates_xy,
                   int x_start, int y_start, int x_end, int y_end)
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
        coordinates_xy.push(std::pair<int, int>(
            rotate ? x_start + i : y_last,
            rotate ? sign * y_last : sign * (x_start + i)));
    }
}

std::vector<std::pair<int, int>> rasterization(std::priority_queue<std::pair<int, int>> coordinates_xy)
{
    std::vector<std::pair<int, int>> rasterized_coordinates_xy;
    while(!coordinates_xy.empty())
    {
        std::pair<int, int> end = coordinates_xy.top();
        std::pair<int, int> start = end;
        coordinates_xy.pop();
        if(!coordinates_xy.empty())
        {
            start = coordinates_xy.top();
            if(start.first != end.first)
                start = end;
            else
                coordinates_xy.pop();
        }
        rasterized_coordinates_xy.push_back(std::pair<int, int>(start.second, start.first));
        rasterized_coordinates_xy.push_back(std::pair<int, int>(end.second, start.first));
        for(int x = start.second; x <= end.second; ++x)
            rasterized_coordinates_xy.push_back(std::pair<int, int>(x, start.first));
    }
    return rasterized_coordinates_xy;
}

float* normalize(std::vector<std::pair<int, int>> rasterized_coordinates_xy)
{
    int cnt = (int) rasterized_coordinates_xy.size();
    float* coordinates = new float[3 * cnt];
    for(int i = 0; i < cnt; ++i)
    {
        coordinates[3 * i] = (float) rasterized_coordinates_xy[i].first / SCR_WIDTH;
        coordinates[3 * i + 1] = (float) rasterized_coordinates_xy[i].second / SCR_HEIGHT;
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
    int v2_y = -500; //-200
    int v3_x = 300;
    int v3_y = -500;
    
    std::priority_queue<std::pair<int, int>> coordinates_xy;
    bresenhamLine(coordinates_xy, v1_x, v1_y, v2_x, v2_y);
    bresenhamLine(coordinates_xy, v2_x, v2_y, v3_x, v3_y);
    bresenhamLine(coordinates_xy, v3_x, v3_y, v1_x, v1_y);
    
    std::vector<std::pair<int, int>> rasterized_coordinates_xy = rasterization(coordinates_xy);
    int cnt = (int) rasterized_coordinates_xy.size();
    float* coordinates = normalize(rasterized_coordinates_xy);
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
