#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "board.h"

using std::uint8_t;
using std::uint64_t;

//Game state
ChessBoard game;

int width = 640;
int height = 480;

//global board consts
double board_x = 0.5 * (width - height);
double board_y = 0;
double board_size = height;

double square_size = board_size * 0.9 * 0.125;
double square_x = board_x + board_size * 0.05;
double square_y = board_y + board_size * 0.05;

bool viewFromWhite = true;

//tracks the board coords of square selected
int select_x = -1;
int select_y = -1;

GLFWwindow* window;

inline uint64_t visual_square(int x, int y) {
    if (viewFromWhite) {
        return x + y * 8;
    } else {
        return (7 - x) + (7 - y) * 8;
    }
}

inline int vx_to_bx(int vx) { return viewFromWhite ? vx : (7 - vx); }
inline int vy_to_by(int vy) { return viewFromWhite ? vy : (7 - vy); }

inline int bx_to_vx(int bx) { return viewFromWhite ? bx : (7 - bx); }
inline int by_to_vy(int by) { return viewFromWhite ? by : (7 - by); }

void drawPiece(int x, int y, int size, int piece)
{
	glBegin(GL_POLYGON);
	glColor3f(1, 1, 1); //light pieces



	size = abs(size);

	switch (piece)
	{
	case BLACK_PAWN:
		glColor3f(0, 0, 0);
	case WHITE_PAWN:
		glVertex2i(x - size / 16, y);
		glVertex2i(x - size / 6, y + size / 3);
		glVertex2i(x + size / 6, y + size / 3);
		glVertex2i(x + size / 16, y);
		glVertex2i(x + size / 8, y - size / 16);
		glVertex2i(x + size / 8, y - size / 6);
		glVertex2i(x + size / 16, y - size / 4);
		glVertex2i(x - size / 16, y - size / 4);
		glVertex2i(x - size / 8, y - size / 6);
		glVertex2i(x - size / 8, y - size / 16);
		break;
	case BLACK_KNIGHT:
		glColor3f(0, 0, 0);
	case WHITE_KNIGHT:
		glVertex2i(x + size / 8, y);
		glVertex2i(x + size / 8, y + size / 3);
		glVertex2i(x - size / 4, y + size / 3);
		glVertex2i(x - size / 8, y - size / 3);
		glVertex2i(x - size / 16, y - size / 4);
		glVertex2i(x + size / 12, y - size / 4);
		glVertex2i(x + size / 3, y - size / 5);
		glVertex2i(x + size / 3, y);
		break;
	case BLACK_BISHOP:
		glColor3f(0, 0, 0);
	case WHITE_BISHOP:
		glVertex2i(x - size / 6, y + size / 3);
		glVertex2i(x - size / 8, y + size / 3.5);
		glVertex2i(x - size / 5, y + size / 6);
		glVertex2i(x - size / 5, y);
		glVertex2i(x - size / 32, y - size / 3);
		glVertex2i(x - size / 24, y - size / 3);
		glVertex2i(x - size / 24, y - size / 2.7);
		glVertex2i(x + size / 24, y - size / 2.7);
		glVertex2i(x + size / 24, y - size / 3);
		glVertex2i(x + size / 32, y - size / 3);
		glVertex2i(x + size / 5, y - size / 32);
		glVertex2i(x - size / 8, y + size / 4);
		glVertex2i(x + size / 5, y + size / 16);
		glVertex2i(x + size / 5, y + size / 6);
		glVertex2i(x + size / 8, y + size / 3.5);
		glVertex2i(x + size / 6, y + size / 3);
		break;
	case BLACK_ROOK:
		glColor3f(0, 0, 0);
	case WHITE_ROOK:
		glVertex2i(x, y + size / 3);
		glVertex2i(x - size / 4, y + size / 3);
		glVertex2i(x - size / 4, y + size / 4);
		glVertex2i(x - size / 6, y + size / 4);
		glVertex2i(x - size / 6, y);
		glVertex2i(x - size / 5, y);
		glVertex2i(x - size / 5, y - size / 5);
		glVertex2i(x - size / 8, y - size / 5);
		glVertex2i(x - size / 8, y - size / 10);
		glVertex2i(x - size / 24, y - size / 10);
		glVertex2i(x - size / 24, y - size / 5);
		glVertex2i(x + size / 24, y - size / 5);
		glVertex2i(x + size / 24, y - size / 10);
		glVertex2i(x + size / 8, y - size / 10);
		glVertex2i(x + size / 8, y - size / 5);
		glVertex2i(x + size / 5, y - size / 5);
		glVertex2i(x + size / 5, y);
		glVertex2i(x + size / 6, y);
		glVertex2i(x + size / 6, y + size / 4);
		glVertex2i(x + size / 4, y + size / 4);
		glVertex2i(x + size / 4, y + size / 3);
		break;
	case BLACK_KING:
		glColor3f(0, 0, 0);
	case WHITE_KING:
		glEnd();
		glBegin(GL_QUADS);
		glVertex2i(x - size / 16, y - size / 2.5);
		glVertex2i(x + size / 16, y - size / 2.5);
		glVertex2i(x + size / 16, y);
		glVertex2i(x - size / 16, y);
		glVertex2i(x - size / 8, y - size / 3.5);
		glVertex2i(x + size / 8, y - size / 3.5);
		glVertex2i(x + size / 8, y - size / 3);
		glVertex2i(x - size / 8, y - size / 3);
		glEnd();
		glBegin(GL_POLYGON);
		goto continue_draw;
	case BLACK_QUEEN:
		glColor3f(0, 0, 0);
	case WHITE_QUEEN:
	continue_draw:
		glVertex2i(x - size / 4, y + size / 3);
		glVertex2i(x - size / 3, y - size / 6);
		glVertex2i(x - size / 5, y - size / 7);
		glVertex2i(x - size / 16, y - size / 5);
		glVertex2i(x - size / 16, y - size / 3.5);
		glVertex2i(x + size / 16, y - size / 3.5);
		glVertex2i(x + size / 15, y - size / 5);
		glVertex2i(x + size / 5, y - size / 7);
		glVertex2i(x + size / 3, y - size / 6);
		glVertex2i(x + size / 4, y + size / 3);
		break;
	default:
		break;
	}

	glEnd();
}


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
                glColor3f(0.7, 0.65, 0.55); //light squares
            } else {
                glColor3f(0.51, 0.47, 0.71); //dark squares
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

        int highlight_x = vx_to_bx(select_x);
        int highlight_y = vy_to_by(select_y);

        glVertex2f(square_x + square_size * highlight_x, square_y + square_size * highlight_y);
        glVertex2f(square_x + square_size * (highlight_x + 1), square_y + square_size * highlight_y);
        glVertex2f(square_x + square_size * (highlight_x + 1), square_y + square_size * (highlight_y + 1));
        glVertex2f(square_x + square_size * highlight_x, square_y + square_size * (highlight_y + 1));
        glEnd();
    }

    //draw chess pieces
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            uint8_t square = visual_square(x, y);
            drawPiece(square_x + square_size * (0.5 + x), square_y + square_size * (0.5 + y), square_size, game.getPiece(square));
        }
    }

    glfwSwapBuffers(window);
}


void mouseclick(double x, double y) {
    //std::cout << "clicked on (" << x << ", " << y << ")" << std::endl;

    int click_x = vx_to_bx(floor((x - square_x) / square_size));
    int click_y = vy_to_by(floor((y - square_y) / square_size));

    if (click_x < 0 || click_x >= 8 || click_y < 0 || click_y >= 8) return; //click is not on the board

    if (click_x == select_x && click_y == select_y) {
        select_x = -1;
        select_y = -1;
        return;
    }



    if (select_x < 8 && select_x >= 0 && select_y < 8 && select_y >= 0 && game.getPiece(select_x + 8 * select_y) != EMPTY) {
        
        game.move(select_x + 8 * select_y, click_x + 8 * click_y); //move prev selected piece to currently selected square
        select_x = -1;
        select_y = -1;
    } else {
        select_x = click_x;
        select_y = click_y;
    }
    



}

void keydown(int key) {
    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, true);
    } else if (key == GLFW_KEY_X) {
        viewFromWhite = !viewFromWhite;
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
