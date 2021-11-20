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

#define PLAYING  1
#define WON      2
#define LOST     3
#define CONTINUE 4
#define QUIT     5

/***************************************************************
 * Snake direction constants
 ***************************************************************/

#define LEFT     1
#define RIGHT    2
#define UP       3
#define DOWN     4

/***************************************************************
 * Snake direction constants
 ***************************************************************/

#define EMPTY    0
#define SNAKE    1
#define FOOD     2

/***************************************************************
 * Misc. constants
 ***************************************************************/

#define INTERVAL_MS 100

#define ANSI_BLACK 30
#define ANSI_RED   31
#define ANSI_GREEN 32

/***************************************************************
 * draw_cell()
 ***************************************************************/

void draw_cell(int* cells, int total_rows, int x, int y) {
    int col = x + 1;
    int row = y / 2;
    set_cursor_position(col, row);

    int top_part = (y % 2 == 0) ? cells[(x * total_rows) + y] : cells[(x * total_rows) + (y - 1)];
    int bot_part = (y % 2 == 0) ? cells[(x * total_rows) + (y - 1)] : cells[(x * total_rows) + y];

    int top_color = (top_part == SNAKE) ? ANSI_RED : ((top_part == FOOD) ? ANSI_GREEN : ANSI_BLACK);
    int bot_color = (bot_part == SNAKE) ? ANSI_RED : ((bot_part == FOOD) ? ANSI_GREEN : ANSI_BLACK);

    printf("\x1B[%d;%dm\xDF\033[0m", top_color, bot_color + 10);
    //printf("\x1B[%d;%dm\xDC\033[0m", top_color, bot_color+10);
}


 /***************************************************************
 * main()
 ***************************************************************/

int main()
{
    
    

    int width, height;
    set_console_show_flag(false);
    get_console_size(width, height);
    srand((unsigned)time(NULL));
    HANDLE h_in = GetStdHandle(STD_INPUT_HANDLE);
   
    SHORT cols = width - 2;
    SHORT rows = (height - 2) * 2;

    COORD* snake_coords = new COORD[cols * rows];
    int snake_coord_index = 0;
    snake_coords[snake_coord_index] = { (SHORT)(cols / 2), (SHORT)(rows / 2) };
    int snake_length = 1;
    int snake_direction = LEFT;

    printf("cols = %d, rows = %d\n", cols, rows);

    int* cells = new int[cols * rows];
    for (int x = 0; x < cols; x++)
        for (int y = 0; y < rows; y++)
            cells[(x * rows) + y] = EMPTY;

    draw_border(width, height);

    COORD food;
    do {
        food = { (short)(rand() % cols), (short)(rand() % rows) };
    } while (cells[(food.X * rows) + food.Y] == SNAKE);
    cells[(food.X * rows)+ food.Y] = FOOD;

    draw_cell(cells, rows, food.X, food.Y);

    auto last_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();


    int game_state = PLAYING;
    while (game_state != QUIT) {
        if (game_state == LOST) continue;

        if (GetAsyncKeyState(VK_ESCAPE)) game_state = QUIT;
        else if (GetAsyncKeyState(VK_UP)) snake_direction = UP;
        else if (GetAsyncKeyState(VK_DOWN)) snake_direction = DOWN;
        else if (GetAsyncKeyState(VK_LEFT)) snake_direction = LEFT;
        else if (GetAsyncKeyState(VK_RIGHT)) snake_direction = RIGHT;
                
        auto current_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

        if (current_time > (last_time + INTERVAL_MS)) {
            last_time = current_time;

            COORD current_head = snake_coords[snake_coord_index];
            int next_snake_coord_index = snake_coord_index + 1;
            if (next_snake_coord_index > ((cols * rows) - 1)) next_snake_coord_index = 0;

            snake_coords[next_snake_coord_index].X = snake_coords[snake_coord_index].X;
            snake_coords[next_snake_coord_index].Y = snake_coords[snake_coord_index].Y;

            if (snake_direction == LEFT) snake_coords[next_snake_coord_index].X--;
            else if (snake_direction == RIGHT) snake_coords[next_snake_coord_index].X++;
            else if (snake_direction == UP) snake_coords[next_snake_coord_index].Y--;
            else if (snake_direction == DOWN) snake_coords[next_snake_coord_index].Y++;
            
            if ((snake_coords[next_snake_coord_index].X < 0) ||
                (snake_coords[next_snake_coord_index].X >= cols) ||
                (snake_coords[next_snake_coord_index].Y < 0) ||
                (snake_coords[next_snake_coord_index].Y >= rows)) {

                game_state = LOST;
                cout << "LOST!";

            }else if (cells[(snake_coords[next_snake_coord_index].X * rows) + snake_coords[next_snake_coord_index].Y] == SNAKE) {

                game_state = LOST;
                cout << "LOST!";
            }
            else if ((snake_coords[next_snake_coord_index].X == food.X) && 
                     (snake_coords[next_snake_coord_index].Y == food.Y)) {

                snake_length++;
                if (snake_length == (rows * cols)) {
                    game_state = WON;
                }
                else {
                    cells[(snake_coords[next_snake_coord_index].X * rows) + snake_coords[next_snake_coord_index].Y] = SNAKE;

                    do {
                        food = { (short)(rand() % cols), (short)(rand() % rows) };
                    } while (cells[(food.X * rows) + food.Y] != SNAKE);
                    cells[(food.X * rows) + food.Y] = FOOD;
                    
                    draw_cell(cells, rows, food.X, food.Y);
                    draw_cell(cells, rows, snake_coords[next_snake_coord_index].X, snake_coords[next_snake_coord_index].Y);

                    snake_coord_index = next_snake_coord_index;
                }
            }
            else {
                cells[(snake_coords[next_snake_coord_index].X * rows) + snake_coords[next_snake_coord_index].Y] = SNAKE;
                
                // IS THIS NEXT LINE CORRECT?!?
                int snake_end_coord_index = next_snake_coord_index - snake_length;
                // ALSO DON't WE NEED TO WRAP?


                cells[(snake_coords[snake_end_coord_index].X * rows) + snake_coords[snake_end_coord_index].Y] = EMPTY;

                //printf("%d\n", cells[snake_coords[snake_end_coord_index].X, snake_coords[snake_end_coord_index].Y]);
                //printf("%d\n", cells[snake_coords[next_snake_coord_index].X, snake_coords[next_snake_coord_index].Y]);

                draw_cell(cells, rows, snake_coords[snake_end_coord_index].X, snake_coords[snake_end_coord_index].Y);
                draw_cell(cells, rows, snake_coords[next_snake_coord_index].X, snake_coords[next_snake_coord_index].Y);

                snake_coord_index = next_snake_coord_index;
            }
        }
    }

    set_console_show_flag(true);

    delete[] snake_coords;
    return 0;
}