#ifndef BOARD_H
#define BOARD_H

int snake_length;
int dir_x, dir_y;
int game_over;
int score;

void draw_board(int height, int width, char snake_head_char, char fruit_char);

void init_game(int height, int width, int obstacle_level);

void draw_board(int height, int width, char snake_head_char, char fruit_char);

void update_snake(int height, int width, int obstacle_level);

void handle_input();
#endif
