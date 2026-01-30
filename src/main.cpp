#include "game.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

int width = 640;
int height = 480;

//global board consts
double board_x = 0.5 * (width - height);
double board_y = 0;
double board_size = height;

double square_size = board_size * 0.9 * 0.125;
double square_x = board_x + board_size * 0.05;
double square_y = board_y + board_size * 0.05;

int white = 0; //0 if white, 1 if black

//tracks the board coords of square selected
int select_x = -1;
int select_y = -1;

GLFWwindow* window;

//fn to update screen
void draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    //draw background
    glColor3f(0.3, 0.15, 0.1);
    glBegin(GL_QUADS);
    glVertex2f(board_x, board_y);
    glVertex2f(board_x + board_size, board_y);
    glVertex2f(board_x + board_size, board_y + board_size);
    glVertex2f(board_x, board_y + board_size);

    //draw board squares
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            if ((x + y) % 2 == 0) {
                glColor3f(0.94, 0.95, 0.94);
            } else {
                glColor3f(0.51, 0.47, 0.71);
            }

            glVertex2f(square_x + square_size * x, square_y + square_size * y);
            glVertex2f(square_x + square_size * (x + 1), square_y + square_size * y);
            glVertex2f(square_x + square_size * (x + 1), square_y + square_size * (y + 1));
            glVertex2f(square_x + square_size * x, square_y + square_size * (y + 1));
        }
    }

    glEnd();

    //highlight selected square
    if (select_x >= 0 && select_y >= 0 && select_x < 8 && select_y < 8) {
        glColor3f(0, 1, 0);
        glBegin(GL_QUADS);

        glVertex2f(square_x + square_size * select_x, square_y + square_size * select_y);
        glVertex2f(square_x + square_size * (select_x + 1), square_y + square_size * select_y);
        glVertex2f(square_x + square_size * (select_x + 1), square_y + square_size * (select_y + 1));
        glVertex2f(square_x + square_size * select_x, square_y + square_size * (select_y + 1));
        glEnd();
    }

    glfwSwapBuffers(window);
}

void mouseclick(double x, double y) {
    //std::cout << "clicked on (" << x << ", " << y << ")" << std::endl;
    select_x = floor((x - square_x) / square_size);
    select_y = floor((y - square_y) / square_size);
}

void keydown(int key) {
    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, true);
    }
}

void resize() {
    if (width > height) {
        board_size = height;
        board_x = 0.5 * (width - height);
        board_y = 0;
    } else {
        board_size = width;
        board_x = 0;
        board_y = 0.5 * (height - width);
    }

    square_size = board_size * 0.9 * 0.125;
    square_x = board_x + board_size * 0.05;
    square_y = board_y + board_size * 0.05;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        int fbW, fbH;
        glfwGetFramebufferSize(window, &fbW, &fbH);

        int winW, winH;
        glfwGetWindowSize(window, &winW, &winH);

        // scale to framebuffer coordinates
        double sx = x * (double)fbW / (double)winW;
        double sy = y * (double)fbH / (double)winH;

        mouseclick(sx, sy);
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        keydown(key);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    ::width = width;
    ::height = height;
    glViewport(0, 0, width, height);
    resize();
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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    resize();



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
