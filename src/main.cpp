#define GL_SILENCE_DEPRECATION
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "board.h"
#include "bitboards.h"
#include "attacks.h"
#include "square.h"
#include "movegen.h"
#include <vector>

using std::uint8_t;
using std::uint64_t;

//Game state
ChessBoard game = ChessBoard(tricky_position);

int width = 640;
int height = 480;

//global board consts
double board_x = 0.5 * (width - height);
double board_y = 0;
double board_size = height;

double square_size = board_size * 0.9 * 0.125;
double square_x = board_x + board_size * 0.05;
double square_y = board_y + board_size * 0.05;

//tracks the board coords of square selected
int select_x = -1;
int select_y = -1;

//global vars that keep track of promotions
int promote_x = -1;
int promote_y = -1;

//stack of previous moves
Move moveStack[1000];
int moveIndex = 0;

GLFWwindow* window;

static void drawPiece(int x, int y, int size, int piece) {
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

static void flipBoard() {
    square_x += 8 * square_size;
    square_y += 8 * square_size;
    square_size *= -1;
}



//fn to update screen
static void draw() {
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

        int highlight_x = select_x;
        int highlight_y = select_y;

        glVertex2f(square_x + square_size * highlight_x, square_y + square_size * highlight_y);
        glVertex2f(square_x + square_size * (highlight_x + 1), square_y + square_size * highlight_y);
        glVertex2f(square_x + square_size * (highlight_x + 1), square_y + square_size * (highlight_y + 1));
        glVertex2f(square_x + square_size * highlight_x, square_y + square_size * (highlight_y + 1));
        
        glEnd();
    }

    //show legal moves
    if (select_x < 8 && select_x >= 0 && select_y < 8 && select_y >= 0) {
        //use attack tables to highlight all legal moves
    }

    //show prev move
    if (moveIndex > 0) {
    int from = moveStack[moveIndex - 1].from();
    int to   = moveStack[moveIndex - 1].to();

    glLineWidth(std::abs(square_size) * 0.15);
    glColor3f(0, 1, 0);
    glBegin(GL_LINES);
    glVertex2f(square_x + square_size * (from % 8 + 0.5), square_y + square_size * (from / 8 + 0.5));
    glVertex2f(square_x + square_size * (to % 8 + 0.5), square_y + square_size * (to / 8 + 0.5));
    glEnd();
}
    

    //draw chess pieces
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            uint8_t square = x + y * 8;;
            drawPiece(square_x + square_size * (0.5 + x), square_y + square_size * (0.5 + y), square_size, game.getPiece(square));
        }
    }



    //draw promotion UI elements
    if (promote_y == 0) {
        glBegin(GL_QUADS);

        glColor3f(1, 1, 1);
        glVertex2f(square_x + square_size * (promote_x - 0.1), square_y - square_size * 0.1);
        glVertex2f(square_x + square_size * (promote_x + 1.1), square_y - square_size * 0.1);
        glVertex2f(square_x + square_size * (promote_x + 1.1), square_y + square_size * 4.1);
        glVertex2f(square_x + square_size * (promote_x - 0.1), square_y + square_size * 4.1);

        glColor3f(0, 0, 0);
        glVertex2f(square_x + square_size * promote_x, square_y);
        glVertex2f(square_x + square_size * (promote_x + 1), square_y);
        glVertex2f(square_x + square_size * (promote_x + 1), square_y + square_size * 4);
        glVertex2f(square_x + square_size * promote_x, square_y + square_size * 4);

        glEnd();

        drawPiece(square_x + square_size * (0.5 + promote_x), square_y + square_size * 0.5, square_size, WHITE_QUEEN);
		drawPiece(square_x + square_size * (0.5 + promote_x), square_y + square_size * 1.5, square_size, WHITE_ROOK);
		drawPiece(square_x + square_size * (0.5 + promote_x), square_y + square_size * 2.5, square_size, WHITE_BISHOP);
		drawPiece(square_x + square_size * (0.5 + promote_x), square_y + square_size * 3.5, square_size, WHITE_KNIGHT);
    } else if (promote_y == 7) {
        glBegin(GL_QUADS);

		glColor3f(0, 0, 0);
		glVertex2f(square_x + square_size * (promote_x - 0.1), square_y + square_size * 8.1);
		glVertex2f(square_x + square_size * (promote_x + 1.1), square_y + square_size * 8.1);
		glVertex2f(square_x + square_size * (promote_x + 1.1), square_y + square_size * 3.9);
		glVertex2f(square_x + square_size * (promote_x - 0.1), square_y + square_size * 3.9);


		glColor3f(1, 1, 1);
		glVertex2f(square_x + square_size * promote_x, square_y + square_size * 8);
		glVertex2f(square_x + square_size * (promote_x + 1), square_y + square_size * 8);
		glVertex2f(square_x + square_size * (promote_x + 1), square_y + square_size * 4);
		glVertex2f(square_x + square_size * promote_x, square_y + square_size * 4);

		glEnd();

		drawPiece(square_x + square_size * (0.5 + promote_x), square_y + square_size * 7.5, square_size, BLACK_QUEEN);
		drawPiece(square_x + square_size * (0.5 + promote_x), square_y + square_size * 6.5, square_size, BLACK_ROOK);
		drawPiece(square_x + square_size * (0.5 + promote_x), square_y + square_size * 5.5, square_size, BLACK_BISHOP);
		drawPiece(square_x + square_size * (0.5 + promote_x), square_y + square_size * 4.5, square_size, BLACK_KNIGHT);
    }

    glfwSwapBuffers(window);
}


static void mouseclick(double x, double y) {
    std::vector<Move> moves;
    generate_moves(game.curr_state(), moves);

    
    //std::cout << "all moves for " << int(game.curr_state().turn) << ": " << std::endl;
    for (const auto& m : moves) {
        //std::cout << m.to_string() << "\n";
    }
    //std::cout << "clicked on (" << x << ", " << y << ")" << std::endl;
    

    int click_x = floor((x - square_x) / square_size);
    int click_y = floor((y - square_y) / square_size);

    if (promote_y == 0) {
        //std::cout << "promoted" << std::endl;
        if (click_x == promote_x) {
            uint8_t promotion = EMPTY;
            if (click_y == 0) {
                promotion = WHITE_QUEEN;
            } else if (click_y == 1) {
                promotion = WHITE_ROOK;
            } else if (click_y == 2) {
                promotion = WHITE_BISHOP;
            } else if (click_y == 3) {
                promotion = WHITE_KNIGHT;
            }
            if (promotion != EMPTY) {
                moveStack[moveIndex - 1].promote(promotion);
                game.undo();
                game.move(moveStack[moveIndex - 1]);
                
                select_x = -1;
                select_y = -1;

                promote_x = -1;
                promote_y = -1;

                return;
            }
        }
    } else if (promote_y == 7) {
        //std::cout << "promoted" << std::endl;
        if (click_x == promote_x) {
            uint8_t promotion = EMPTY;
            if (click_y == 7) {
                promotion = BLACK_QUEEN;
            } else if (click_y == 6) {
                promotion = BLACK_ROOK;
            } else if (click_y == 5) {
                promotion = BLACK_BISHOP;
            } else if (click_y == 4) {
                promotion = BLACK_KNIGHT;
            }
            if (promotion != EMPTY) {
                moveStack[moveIndex - 1].promote(promotion);
                game.undo();
                game.move(moveStack[moveIndex - 1]);

                select_x = -1;
                select_y = -1;

                promote_x = -1;
                promote_y = -1;

                return;
            }
        }
    }

    if (click_x == select_x && click_y == select_y) { //clicked on the same square
        select_x = -1;
        select_y = -1;
        return;
    }

    //otherwise, if piece is selected, move it (if possible)
    else if (select_x < 8 && select_x >= 0 && select_y < 8 && select_y >= 0 && game.getPiece(select_x + 8 * select_y) != EMPTY) {
        if (click_x < 0 || click_x >= 8 || click_y < 0 || click_y >= 8) {
            return; //click is not on the board
        }

        uint8_t from = select_x + 8 * select_y;
        uint8_t to = click_x + 8 * click_y;
        uint8_t piece = game.getPiece(from);
        uint8_t promoted = EMPTY;
        MoveFlag flags = MF_NONE;
        if (game.getPiece(to) != EMPTY) {
            flags = MoveFlag(flags | MF_CAPTURE);
        }

        Move move(from, to, piece, promoted, flags);

        moveStack[moveIndex] = move;
        game.move(moveStack[moveIndex++]);

        //check to see if pawn promoted
        if (click_y == 7 && game.getPiece(to) == BLACK_PAWN || click_y == 0 && game.getPiece(to) == WHITE_PAWN) {
            promote_x = click_x;
            promote_y = click_y;
        } else {
            promote_x = -1;
            promote_y = -1;
        }

        select_x = -1;
        select_y = -1;
    } else {
        select_x = click_x;
        select_y = click_y;
        //std::cout << "is attacked by white: " << is_square_attacked(select_x + select_y * 8, WHITE, game.curr_state()) << std::endl;
    }
}

static void keydown(int key) {
    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, true);
    } else if (key == GLFW_KEY_X) {
        flipBoard();
    } else if (key == GLFW_KEY_Z && moveIndex > 0) {
        game.undo();
        --moveIndex;
    }
}

static void resize() {
    bool isFlipped = square_size < 0;

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

    if (isFlipped) {
        flipBoard();
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
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

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        keydown(key);
    }
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    ::width = width;
    ::height = height;
    glViewport(0, 0, width, height);
    resize();
}

void init_all() {
    //initialize leaper atk tables
    init_leapers_attacks();
    
    //init_magic_numbers();

    init_sliders_attacks(true);
    init_sliders_attacks(false);
}

int main() {
    init_all();

    /*//testing using prints
    for (int i = 7; i < 20; ++i) {
        game.print_bitboard(kingAttacks[i]);
    }

    testing random numbers
    std::cout << get_random_number() << std::endl;
    std::cout << get_random_number() << std::endl;
    std::cout << get_random_number() << std::endl;
    std::cout << get_random_number() << std::endl;
    std::cout << get_random_number() << std::endl;
        

    game.print_bitboard((uint64_t)get_random_u32_number());
    game.print_bitboard((uint64_t)(get_random_u32_number() & 0xFFFF)); //slice upper (from MS1B side) 16 bits
    game.print_bitboard(get_random_u64_number());
    game.print_bitboard(generate_magic_number());

    

    //init_magic_numbers(); unnecessary, hard coded them

    for (int square = 0; square < 64; ++square) {
        std::cout << "0x" << std::hex << rook_magic_numbers[square] << std::dec << "ULL,\n";
    }
    std::cout << "\n\n";
    for (int square = 0; square < 64; ++square) {
        std::cout << "0x" << std::hex << bishop_magic_numbers[square] << std::dec << "ULL,\n";
    }
    
   
    uint64_t occupancy = 1ULL << 26;
    occupancy |= 1ULL << 30;

    occupancy |= 1ULL << 19;
    occupancy |= 1ULL << 35;
    occupancy |= 1ULL << 26;


    occupancy  |= 1ULL << 10;   // a7 (left)

    game.print_bitboard(occupancy);
    game.print_bitboard(get_bishop_attacks(uint8_t(12), occupancy));
    game.print_bitboard(get_rook_attacks(uint8_t(12), occupancy));
    */

    //game.print_occupancy();


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
