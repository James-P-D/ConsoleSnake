// Cursor and window size
void set_cursor_position(int x, int y);
void get_console_size(int& width, int& height);
void set_console_show_flag(bool show_flag);

// Border drawing
void draw_top_border(int width);
void draw_bottom_border(int width, int height);
void draw_side_borders(int width, int height);
void draw_border(int width, int height);

