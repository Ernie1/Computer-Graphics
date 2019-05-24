#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <math.h>

const uint SCR_WIDTH = 800;
const uint SCR_HEIGHT = 600;

double move_x, move_y;

std::vector<std::pair<double, double>> main_nodes;
std::vector<std::pair<double, double>> bezier_points;

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    move_x = xpos;
    move_y = ypos;
}

double factorial(int n)
{
    int fact = 1;
    for (int i = 1; i <= n; ++i)
        fact = fact * i;
    return (double) fact;
}

void make_bezier()
{
    bezier_points.clear();
    int n = (int) main_nodes.size() - 1;
    for (double t = 0; t <= 1; t += 0.001)
    {
        double x = 0, y = 0;
        for (int i = 0; i <= n; ++i)
        {
            double B = factorial(n) / factorial(i) / factorial(n - i) *
            pow(t, i) * pow((1 - t), (n - i));
            x += main_nodes[i].first * B;
            y += main_nodes[i].second * B;
        }
        bezier_points.push_back(std::pair<double, double>(x, y));
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (button)
        {
            case GLFW_MOUSE_BUTTON_LEFT:
                main_nodes.push_back(std::pair<double, double>(2 * move_x / SCR_WIDTH - 1.0f, 1.0f - 2 * move_y / SCR_HEIGHT));
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
                main_nodes.pop_back();
                break;
        }
        make_bezier();
    }
}

void draw_fixed()
{
    glClearColor (0.85, 0.85, 0.85, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glPointSize(12.0f);
    glBegin(GL_POINTS);
    glColor3f(0.4, 0.4, 0.4);
    for (auto i : main_nodes)
    {
        glVertex2f(i.first, i.second);
    }
    glEnd();
    
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glColor3f(0.4, 0.4, 0.4);
    for (int i = 1; i < main_nodes.size(); ++i)
    {
        glVertex2f(main_nodes[i - 1].first, main_nodes[i - 1].second);
        glVertex2f(main_nodes[i].first, main_nodes[i].second);
    }
    glEnd();
    
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    glColor3f(1.0, 0.0, 0.0);
    for (auto i : bezier_points)
    {
        glVertex2f(i.first, i.second);
    }
    glEnd();
}


int main()
{
    glfwInit();
    
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CG_HW8", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    
    while (!glfwWindowShouldClose(window))
    {
        draw_fixed();
        
        glfwSetCursorPosCallback(window, cursor_position_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}
