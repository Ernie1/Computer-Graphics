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

bool drawing = false;

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    move_x = xpos;
    move_y = ypos;
}

void draw_process(std::vector<std::pair<double, double>> nodes, double t)
{
    if (nodes.size() < 2)
        return;
    std::vector<std::pair<double, double>> next_nodes;
    for (int i = 0; i < nodes.size() - 1; ++i)
    {
        double x = (1 - t) * nodes[i].first + t * nodes[i + 1].first;
        double y = (1 - t) * nodes[i].second + t * nodes[i + 1].second;
        next_nodes.push_back(std::pair<double, double>(x, y));
    }
    if (next_nodes.size() == 1)
    {
        glPointSize(12.0f);
        glBegin(GL_POINTS);
        glColor3f(1.0, 0.0, 0.0);
        glVertex2f(next_nodes[0].first, next_nodes[0].second);
        glEnd();

        bezier_points.push_back(next_nodes[0]);
    }
    else
    {
        for (int i = 0; i < next_nodes.size() - 1; ++i)
        {
            glLineWidth(2.0f);
            glBegin(GL_LINES);
            glColor3f(0.6, 0.6, 0.6);
            for (int i = 1; i < next_nodes.size(); ++i)
            {
                glVertex2f(next_nodes[i - 1].first, next_nodes[i - 1].second);
                glVertex2f(next_nodes[i].first, next_nodes[i].second);
            }
            glEnd();
        }
        draw_process(next_nodes, t);
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
    
    double current_t = 0.0;
    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
            drawing = !drawing;
        
        draw_fixed();
        
        if (drawing)
        {
            if (current_t <= 1.0)
            {
                current_t += 0.001;
                draw_process(main_nodes, current_t);
            }
            glfwSetCursorPosCallback(window, NULL);
            glfwSetMouseButtonCallback(window, NULL);
        }
        else
        {
            bezier_points.clear();
            current_t = 0.0;
            glfwSetCursorPosCallback(window, cursor_position_callback);
            glfwSetMouseButtonCallback(window, mouse_button_callback);
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}
