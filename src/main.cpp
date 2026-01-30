#include "game.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

int width = 640;
int height = 480;

double x_1 = rand() % width;
double x_2 = rand() % width;
double x_3 = rand() % width;
double y_1 = rand() % height;
double y_2 = rand() % height;
double y_3 = rand() % height;

//velocities
double vx_1 = 0.005 * (rand() % width) - 0.0025;
double vx_2 = 0.005 * (rand() % width) - 0.0025;
double vx_3 = 0.005 * (rand() % width) - 0.0025;
double vy_1 = 0.005 * (rand() % height) - 0.0025;
double vy_2 = 0.005 * (rand() % height) - 0.0025;
double vy_3 = 0.005 * (rand() % height) - 0.0025;

double t = 0;

GLFWwindow* window;

//fn to update screen
void draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    //draw
    glColor3f(1, 1, 1);
    glBegin(GL_TRIANGLES);

    glColor3f(sin(t * 0.9), cos(t * 0.4), 1); //vertex 1
    glVertex2f(x_1, y_1);

    glColor3f(sin(t * 0.8), cos(t * 0.5), 1); //vertex 2
    glVertex2f(x_2, y_2);

    glColor3f(sin(t * 0.7), cos(t * 0.6), 1); //vertex 3
    glVertex2f(x_3, y_3);

    //velocity applied
    x_1 += vx_1;
    x_2 += vx_2;
    x_3 += vx_3;
    y_1 += vy_1;
    y_2 += vy_2;
    y_3 += vy_3;

    //bounces off edges
    if (x_1 < 0 || x_1 >= width) vx_1 *= -1;
    if (x_2 < 0 || x_2 >= width) vx_2 *= -1;
    if (x_3 < 0 || x_3 >= width) vx_3 *= -1;
    if (y_1 < 0 || y_1 >= height) vy_1 *= -1;
    if (y_2 < 0 || y_2 >= height) vy_2 *= -1;
    if (y_3 < 0 || y_3 >= height) vy_3 *= -1;

    t += 0.016;

    glEnd();

    glfwSwapBuffers(window);
}

void mouseclick(double x, double y) {
    std::cout << "clicked on (" << x << ", " << y << ")" << std::endl;
}

void keydown(int key) {
    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, true);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        mouseclick(x, y);
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        keydown(key);
    }
}

int main() {
    //initialize OpenGL
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    //window
    window = glfwCreateWindow(width, height, "Chess", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    //callbacks
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);

    //set color
    glClearColor(0, 0, 0, 1);

    //set up a 2d proj
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();   


    //main loop
    while (!glfwWindowShouldClose(window)) {
        draw();
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
