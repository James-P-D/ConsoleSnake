#include <iostream>
#include <windows.h>
#include <chrono>
#include "congfx.hpp"

using std::cout;
using std::endl;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

/***************************************************************
 * Game state constants
 ***************************************************************/

const int INITIALISE = 0;
const int PLAYING = 1;
const int WON = 2;
const int LOST = 3;
const int QUIT = 5;

/***************************************************************
 * Snake direction constants
 ***************************************************************/

const int LEFT = 1;
const int RIGHT = 2;
const int UP = 3;
const int DOWN = 4;

/***************************************************************
 * Snake direction constants
 ***************************************************************/

const int EMPTY = 0;
const int SNAKE = 1;
const int FOOD = 2;

/***************************************************************
 * Misc. constants
 ***************************************************************/

const int INTERVAL_MS = 100;

const int ANSI_BLACK = 30;
const int ANSI_RED = 31;
const int ANSI_GREEN = 32;

/***************************************************************
 * draw_cell()
 ***************************************************************/

void draw_cell(int* cells, int total_rows, int x, int y) {
    int col = x + 1;
    int row = (y / 2) + 1;
    set_cursor_position(col, row);

    int top_part = (y % 2 == 0) ? cells[(x * total_rows) + y] : cells[(x * total_rows) + (y - 1)];
    int bot_part = (y % 2 == 0) ? cells[(x * total_rows) + (y + 1)] : cells[(x * total_rows) + y];

    int top_color = (top_part == SNAKE) ? ANSI_RED : ((top_part == FOOD) ? ANSI_GREEN : ANSI_BLACK);
    int bot_color = (bot_part == SNAKE) ? ANSI_RED : ((bot_part == FOOD) ? ANSI_GREEN : ANSI_BLACK);

    printf("\x1B[%d;%dm\xDF\033[0m", top_color, bot_color + 10);
}

 /***************************************************************
 * main()
 ***************************************************************/

int main() {
    // Get the size of the terminal and hide the cursor
    int width, height;
    set_console_show_flag(false);
    get_console_size(width, height);
    
    // Initialise the random number generator
    srand((unsigned)time(NULL));

    // Initialise variables for snake coordinate array
    SHORT cols = width - 2;
    SHORT rows = (height - 2) * 2;
    COORD* snake_coords = new COORD[cols * rows];
    // Initialise index of snake coordinates
    int snake_coord_index = 0;
    int snake_length = 0;
    int snake_direction = LEFT;
    int* cells = new int[cols * rows];

    // Inititialise food
    COORD food = { 0,0 };
    // Set the game state
    int game_state = INITIALISE;
    // Get the current time so we can animate every INTERVAL-ms
    auto last_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    
    while (game_state != QUIT) {
        // If the game is over, through win or loss, ask if they want to play again, and
        // either quit or reinitialise the board
        if ((game_state == LOST) || (game_state == WON)) {
            if (GetAsyncKeyState('n') || GetAsyncKeyState('N')) {
                game_state = QUIT;
            }
            else if (GetAsyncKeyState('y') || GetAsyncKeyState('Y')) {
                game_state = INITIALISE;
            }
        }
        else if (game_state == INITIALISE) {
            // If we are initialising for a new game, set the snake vars
            snake_coord_index = 0;
            snake_coords[snake_coord_index] = { (SHORT)(cols / 2), (SHORT)(rows / 2) };
            snake_length = 1;
            snake_direction = LEFT;

            // Initialise the cells board to empty
            for (int x = 0; x < cols; x++) {
                for (int y = 0; y < rows; y++) {
                    cells[(x * rows) + y] = EMPTY;
                }
            }

            // Draw the border and then border with zero score
            draw_border(width, height);
            draw_bottom_border_with_score(width, height, snake_length - 1);

            // Find a spare place on the board to put the food
            do {
                food = { (SHORT)(rand() % cols), (SHORT)(rand() % rows) };
            } while (cells[(food.X * rows) + food.Y] == SNAKE);
            cells[(food.X * rows) + food.Y] = FOOD;

            // Draw the food cell, and start playing
            draw_cell(cells, rows, food.X, food.Y);
            game_state = PLAYING;
        }
        else if (game_state == PLAYING) {
            if (GetAsyncKeyState(VK_ESCAPE)) game_state = QUIT;
            else if (GetAsyncKeyState(VK_UP)) snake_direction = UP;
            else if (GetAsyncKeyState(VK_DOWN)) snake_direction = DOWN;
            else if (GetAsyncKeyState(VK_LEFT)) snake_direction = LEFT;
            else if (GetAsyncKeyState(VK_RIGHT)) snake_direction = RIGHT;

            auto current_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

            if (current_time > (last_time + INTERVAL_MS)) {
                // Record the current time so we can wait INTERVAL_MS again for the next move
                last_time = current_time;

                // Get the current head
                COORD current_head = snake_coords[snake_coord_index];
                // Get the next COORD in the array for the new head of the snake
                int next_snake_coord_index = snake_coord_index + 1;
                // (remember to wrap!)
                if (next_snake_coord_index > ((cols * rows) - 1)) next_snake_coord_index = 0;

                // Copy the COORD for the new head
                snake_coords[next_snake_coord_index].X = snake_coords[snake_coord_index].X;
                snake_coords[next_snake_coord_index].Y = snake_coords[snake_coord_index].Y;

                // ..and set the new head coordinate dependant on the direction we are moving
                if (snake_direction == LEFT) snake_coords[next_snake_coord_index].X--;
                else if (snake_direction == RIGHT) snake_coords[next_snake_coord_index].X++;
                else if (snake_direction == UP) snake_coords[next_snake_coord_index].Y--;
                else if (snake_direction == DOWN) snake_coords[next_snake_coord_index].Y++;

                // Check to see if we've hit a wall
                if ((snake_coords[next_snake_coord_index].X < 0) ||
                    (snake_coords[next_snake_coord_index].X >= cols) ||
                    (snake_coords[next_snake_coord_index].Y < 0) ||
                    (snake_coords[next_snake_coord_index].Y >= rows)) {

                    game_state = LOST;
                    draw_bottom_border_with_score_and_message(width, height, (snake_length - 1), "You lost by hitting a wall! Play again? [Y/N]");
                }
                // Check to see if we've hit our own tail
                else if (cells[(snake_coords[next_snake_coord_index].X * rows) + snake_coords[next_snake_coord_index].Y] == SNAKE) {

                    game_state = LOST;
                    draw_bottom_border_with_score_and_message(width, height, (snake_length - 1), "You lost by hitting your tail! Play again? [Y/N]");
                }
                // Check to see if we've found food
                else if ((snake_coords[next_snake_coord_index].X == food.X) &&
                    (snake_coords[next_snake_coord_index].Y == food.Y)) {

                    // Increase the length of the snake
                    snake_length++;

                    // Update score
                    draw_bottom_border_with_score(width, height, snake_length - 1);

                    // Check to see if we've won
                    if (snake_length == (rows * cols)) {
                        game_state = WON;
                        draw_bottom_border_with_score_and_message(width, height, (snake_length - 1), "You won! Play again? [Y/N]");
                    }
                    else {
                        // Only incremement head, *DON'T* remove the last tail segment. We want the tail to grow
                        // by 1 item each time we eat
                        cells[(snake_coords[next_snake_coord_index].X * rows) + snake_coords[next_snake_coord_index].Y] = SNAKE;

                        // Find another spare cell to put the food
                        do {
                            food = { (short)(rand() % cols), (short)(rand() % rows) };
                        } while (cells[(food.X * rows) + food.Y] == SNAKE);
                        cells[(food.X * rows) + food.Y] = FOOD;

                        // Draw the food
                        draw_cell(cells, rows, food.X, food.Y);
                        // Draw the new head
                        draw_cell(cells, rows, snake_coords[next_snake_coord_index].X, snake_coords[next_snake_coord_index].Y);
                        // Update the index
                        snake_coord_index = next_snake_coord_index;
                    }
                }
                else {
                    // Find the tail (very last item)
                    int snake_end_coord_index = next_snake_coord_index - snake_length;
                    // (and remember to wrap!)
                    if (snake_end_coord_index < 0) snake_end_coord_index += (cols * rows);

                    // Set the new head of the snake
                    cells[(snake_coords[next_snake_coord_index].X * rows) + snake_coords[next_snake_coord_index].Y] = SNAKE;
                    // Remove the last item of the tail
                    cells[(snake_coords[snake_end_coord_index].X * rows) + snake_coords[snake_end_coord_index].Y] = EMPTY;

                    // Draw the head
                    draw_cell(cells, rows, snake_coords[next_snake_coord_index].X, snake_coords[next_snake_coord_index].Y);
                    // Draw the tail (remove)
                    draw_cell(cells, rows, snake_coords[snake_end_coord_index].X, snake_coords[snake_end_coord_index].Y);

                    // Update the index
                    snake_coord_index = next_snake_coord_index;
                }
            }
        }
    }

    // Make the cursor visible again
    set_console_show_flag(true);

    // Clean-up!
    delete[] snake_coords;

    // Return to OS
    return 0;
}