#include <ncurses.h>
#include <unistd.h>
#include "leaderboard.h"
#include "menu.h"
#include "board.h"

#define WIDTH  40
#define HEIGHT 20

char snake_head_char = '@';
char fruit_char = '*';
int obstacle_level = 0;

int main() {
    if (!menu()) {
        endwin();
        return 0;
    }

    init_game(HEIGHT, WIDTH, obstacle_level);
    load_leaderboard();

    while (!game_over) {
        draw_board(HEIGHT, WIDTH, snake_head_char, fruit_char);
        handle_input();
        update_snake(HEIGHT, WIDTH, obstacle_level);
        usleep(delay);
    }

    echo();
    nodelay(stdscr, FALSE);
    char name[MAX_NAME_LENGTH];
    clear();
    mvprintw(HEIGHT / 2 - 1, (WIDTH - 10) / 2, "Game Over!");
    mvprintw(HEIGHT / 2, (WIDTH - 18) / 2, "Final Score: %d", score);
    mvprintw(HEIGHT / 2 + 1, (WIDTH - 25) / 2, "Enter your name: ");
    getnstr(name, MAX_NAME_LENGTH - 1);
    noecho();

    update_leaderboard(name, score);
    show_leaderboard();

    endwin();
    return 0;
}
