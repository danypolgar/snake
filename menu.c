#include <ncurses.h>
#include "menu.h"

int delay = 100000;


int menu() {
    int speed_index = 1, head_index = 0, fruit_index = 0, color_index = 0, obstacle_index = 0;
    int option = 0;

    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    start_color();

    while (1) {
        const int max_options = 5;
        const int fruit_colors[] = {COLOR_MAGENTA, COLOR_YELLOW, COLOR_BLUE};
        const int snake_colors[] = {COLOR_GREEN, COLOR_RED, COLOR_CYAN};
        const char *fruit_symbols[] = {"*", "+", "X"};
        const char *head_symbols[] = {"@", "%", "O"};

        const int speed_values[] = {150000, 100000, 60000};
        clear();
        mvprintw(3, 13, "--- Snake Game ---");

        for (int i = 0; i < max_options; i++) {
            const char *speed_labels[] = {"Slow", "Normal", "Fast"};
            const char *obstacle_labels[] = {"None", "Few", "Many"};
            const char *color_names[] = {"Green/Magenta", "Red/Yellow", "Cyan/Blue"};
            const int options_width = 10;
            if (i == option) {
                attron(A_REVERSE);
            }
            switch (i) {
                case 0:
                    mvprintw(6 + i * 2, options_width, "<-- Speed: %s -->", speed_labels[speed_index]);
                    break;
                case 1:
                    mvprintw(6 + i * 2, options_width, "<-- Head:  %s -->", head_symbols[head_index]);
                    break;
                case 2:
                    mvprintw(6 + i * 2, options_width, "<-- Fruit: %s -->", fruit_symbols[fruit_index]);
                    break;
                case 3:
                    mvprintw(6 + i * 2, options_width, "<-- Color Theme: %s -->", color_names[color_index]);
                    break;
                case 4:
                    mvprintw(6 + i * 2, options_width, "<-- Obstacles: %s -->", obstacle_labels[obstacle_index]);
                    break;
            }
            if (i == option) {
                attroff(A_REVERSE);
            }
        }

        const int manual_width = 12;
        mvprintw(17, manual_width, "Move with the arrow keys");
        mvprintw(18, manual_width, "Press Enter to Start");
        mvprintw(19, manual_width, "Press ESC to Quit");
        refresh();

        int ch = getch();
        switch (ch) {
            case KEY_UP:
                option = (option - 1 + max_options) % max_options;
                break;
            case KEY_DOWN:
                option = (option + 1) % max_options;
                break;
            case KEY_LEFT:
                if (option == 0)
                    speed_index = (speed_index - 1 + 3) % 3;
                else if (option == 1)
                    head_index = (head_index - 1 + 3) % 3;
                else if (option == 2)
                    fruit_index = (fruit_index - 1 + 3) % 3;
                else if (option == 3)
                    color_index = (color_index - 1 + 3) % 3;
                else if (option == 4)
                    obstacle_index = (obstacle_index - 1 + 3) % 3;
                break;
            case KEY_RIGHT:
                if (option == 0)
                    speed_index = (speed_index + 1) % 3;
                else if (option == 1)
                    head_index = (head_index + 1) % 3;
                else if (option == 2)
                    fruit_index = (fruit_index + 1) % 3;
                else if (option == 3)
                    color_index = (color_index + 1) % 3;
                else if (option == 4)
                    obstacle_index = (obstacle_index + 1) % 3;
                break;
            case '\n':
                delay = speed_values[speed_index];
                snake_head_char = head_symbols[head_index][0];
                fruit_char = fruit_symbols[fruit_index][0];
                obstacle_level = obstacle_index;
                init_pair(1, snake_colors[color_index], COLOR_BLACK);
                init_pair(2, fruit_colors[color_index], COLOR_BLACK);
                return 1;
            case 27:
                return 0;
        }
    }
}
