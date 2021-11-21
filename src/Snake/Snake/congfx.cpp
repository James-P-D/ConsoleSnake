#include <windows.h>
#include <iostream>

#define TOP_LEFT_CORNER     219//  218
#define TOP_RIGHT_CORNER    219//  191
#define BOTTOM_LEFT_CORNER  219//  192
#define BOTTOM_RIGHT_CORNER 219//  217
#define HORIZONTAL_LINE     219//  196
#define VERTICAL_LINE       219//  179

/***************************************************************
 * set_cursor_position()
 ***************************************************************/

void set_cursor_position(int x, int y) {
    COORD pos = { (short)x, (short)y };
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    !SetConsoleCursorPosition(output, pos);
}

/***************************************************************
 * get_console_size()
 ***************************************************************/

void get_console_size(int& width, int& height) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

    width = (int)(csbi.srWindow.Right - csbi.srWindow.Left + 1);
    height = (int)(csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
}

/***************************************************************
 * set_console_show_flag()
 ***************************************************************/

void set_console_show_flag(bool show_flag) {
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(output, &cursorInfo);

    cursorInfo.bVisible = show_flag;
    SetConsoleCursorInfo(output, &cursorInfo);
}

/***************************************************************
 * draw_top_border()
 ***************************************************************/

void draw_top_border(int width) {
    set_cursor_position(0, 0);
    for (int x = 0; x < width; x++) {
        if (x == 0) std::cout << char(TOP_LEFT_CORNER);
        else if (x == (width - 1)) std::cout << char(TOP_RIGHT_CORNER);
        else std::cout << char(HORIZONTAL_LINE);
    }
}

/***************************************************************
 * draw_bottom_border()
 ***************************************************************/

void draw_bottom_border(int width, int height) {
    set_cursor_position(0, height-1);
    for (int x = 0; x < width; x++) {
        if (x == 0) std::cout << char(BOTTOM_LEFT_CORNER);
        else if (x == (width - 1)) std::cout << char(BOTTOM_RIGHT_CORNER);
        else std::cout << char(HORIZONTAL_LINE);
    }
}

/***************************************************************
 * draw_bottom_border_with_score()
 ***************************************************************/

void draw_bottom_border_with_score(int width, int height, int score) {
    draw_bottom_border(width, height);
    set_cursor_position(1, height - 1);
    std::cout << " Score: " << score << " ";
}

/***************************************************************
 * draw_bottom_border_with_score_and_message()
 ***************************************************************/

void draw_bottom_border_with_score_and_message(int width, int height, int score, const char* message) {
    draw_bottom_border(width, height);
    set_cursor_position(1, height - 1);
    std::cout << " Score: " << score << " ";
    set_cursor_position(20, height - 1);
    std::cout << " " << message << " ";
}

/***************************************************************
 * draw_side_borders()
 ***************************************************************/

void draw_side_borders(int width, int height) {
    for (int y = 1; y < (height - 1); y++) {
        set_cursor_position(0, y);
        std::cout << char(VERTICAL_LINE);
        for (int x = 0; x < width-2; x++) {
            std::cout << " ";
        }
        std::cout << char(VERTICAL_LINE);
    }
}

/***************************************************************
 * draw_border()
 ***************************************************************/

void draw_border(int width, int height) {
    draw_top_border(width);
    draw_side_borders(width, height);
    draw_bottom_border(width, height);
}