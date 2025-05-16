#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include "board.h"

#define MAX_SNAKE_LENGTH 100
#define MAX_OBSTACLES 50

typedef struct {
    int x, y;
} Point;

int obstacle_count = 0;
int dir_x = 1, dir_y = 0;
int snake_length = 5;
int game_over = 0;
int score = 0;

Point snake[MAX_SNAKE_LENGTH];
Point obstacles[MAX_OBSTACLES];
Point food;

int calculate_obstacle_placements(int obstacle_level) {
    if (obstacle_level == 1) {
        return 10;
    }
    if (obstacle_level == 2) {
        return 20;
    }
    return 0;
}

void generate_obstacles(int height, int width, int obstacle_level) {
    obstacle_count = 0;
    int obstacle_placements = calculate_obstacle_placements(obstacle_level);
    for (int i = 0; i < obstacle_placements; i++) {
        Point obs;
        do {
            obs.x = rand() % (width - 2) + 1;
            obs.y = rand() % (height - 2) + 1;
        } while (obs.x == food.x && obs.y == food.y);
        obstacles[obstacle_count++] = obs;
    }
}

void init_game(int height, int width, int obstacle_level) {
    clear();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    srand(time(NULL));

    snake_length = 5;
    dir_x = 1;
    dir_y = 0;
    score = 0;
    game_over = 0;

    for (int i = 0; i < snake_length; i++) {
        snake[i].x = width / 2 - i;
        snake[i].y = height / 2;
    }

    food.x = rand() % (width - 2) + 1;
    food.y = rand() % (height - 2) + 1;

    generate_obstacles(height, width, obstacle_level);
}

void draw_board(int height, int width, char snake_head_char, char fruit_char) {
    clear();
    for (int i = 0; i < width; i++) {
        mvprintw(0, i, "#");
        mvprintw(height - 1, i, "#");
    }
    for (int i = 0; i < height; i++) {
        mvprintw(i, 0, "#");
        mvprintw(i, width - 1, "#");
    }

    for (int i = 0; i < obstacle_count; i++) {
        mvaddch(obstacles[i].y, obstacles[i].x, 'X');
    }

    for (int i = 0; i < snake_length; i++) {
        attron(COLOR_PAIR(1));
        mvaddch(snake[i].y, snake[i].x, i == 0 ? snake_head_char : 'o');
        attroff(COLOR_PAIR(1));
    }

    attron(COLOR_PAIR(2));
    mvaddch(food.y, food.x, fruit_char);
    attroff(COLOR_PAIR(2));

    mvprintw(height, 0, "Score: %d", score);
    refresh();
}

int calculate_score_gain(int obstacle_level) {
    if (obstacle_level == 0) {
        return 10;
    }
    if (obstacle_level == 1) {
        return 20;
    }
    return 30;
}

void update_snake(int height, int width, int obstacle_level) {
    Point new_head = {snake[0].x + dir_x, snake[0].y + dir_y};

    if (new_head.x <= 0 || new_head.x >= width - 1 || new_head.y <= 0 || new_head.y >= height - 1) {
        game_over = 1;
        return;
    }

    for (int i = 0; i < snake_length; i++) {
        if (snake[i].x == new_head.x && snake[i].y == new_head.y) {
            game_over = 1;
            return;
        }
    }

    for (int i = 0; i < obstacle_count; i++) {
        if (obstacles[i].x == new_head.x && obstacles[i].y == new_head.y) {
            game_over = 1;
            return;
        }
    }

    for (int i = snake_length - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }

    snake[0] = new_head;

    if (new_head.x == food.x && new_head.y == food.y) {
        if (snake_length < MAX_SNAKE_LENGTH) {
            snake_length++;
        }
        food.x = rand() % (width - 2) + 1;
        food.y = rand() % (height - 2) + 1;

        const int score_gain = calculate_score_gain(obstacle_level);
        score += score_gain;
    }
}

void handle_input() {
    int ch = getch();
    switch (ch) {
        case KEY_UP:
            if (dir_y != 1) {
                dir_x = 0;
                dir_y = -1;
            }
            break;
        case KEY_DOWN:
            if (dir_y != -1) {
                dir_x = 0;
                dir_y = 1;
            }
            break;
        case KEY_LEFT:
            if (dir_x != 1) {
                dir_x = -1;
                dir_y = 0;
            }
            break;
        case KEY_RIGHT:
            if (dir_x != -1) {
                dir_x = 1;
                dir_y = 0;
            }
            break;
    }
}
