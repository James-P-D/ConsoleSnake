#include <iostream>
#include <windows.h>
#include "congfx.hpp"

#define PLAYING 1
#define WON 2
#define LOST 3
#define CONTINUE 4

int main()
{
    int width, height;
    set_console_show_flag(false);
    get_console_size(width, height);

    
    int cols = width - 2;
    int rows = (height - 2) * 2;
    
    COORD* snake = new COORD[cols, rows];



    COORD snake_head = { width / 2, height / 2 };
    COORD snake_tail = { width / 2, height / 2 };
    int snake_length = 1;


    draw_border(width, height);


    set_console_show_flag(true);

    delete snake;
    return 0;
}