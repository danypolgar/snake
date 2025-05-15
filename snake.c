#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#define WIDTH  40
#define HEIGHT 20
#define MAX_SNAKE_LENGTH 100
#define MAX_NAME_LENGTH 32
#define MAX_LEADERBOARD 10
#define MAX_OBSTACLES 50

typedef struct {
    int x, y;
} Point;

typedef struct {
    char name[MAX_NAME_LENGTH];
    int score;
} ScoreEntry;

Point snake[MAX_SNAKE_LENGTH];
int snake_length = 5;
int dir_x = 1, dir_y = 0;
Point food;
Point obstacles[MAX_OBSTACLES];
int obstacle_count = 0;
int game_over = 0;
int score = 0;
int delay = 100000;

char snake_head_char = '@';
char fruit_char = '*';
int snake_color_pair = 1;
int fruit_color_pair = 2;
int difficulty = 0; // 0 = Easy, 1 = Normal, 2 = Hard

ScoreEntry leaderboard[MAX_LEADERBOARD];int leaderboard_size = 0;

const char *leaderboard_file = "leaderboard.txt";

const char *color_names[] = { "Green", "Red", "Cyan" };
int snake_colors[] = { COLOR_GREEN, COLOR_RED, COLOR_CYAN };
int fruit_colors[] = { COLOR_MAGENTA, COLOR_YELLOW, COLOR_BLUE };

void load_leaderboard() {
    FILE *file = fopen(leaderboard_file, "r");
    if (file) {
        leaderboard_size = 0;
        while (fscanf(file, "%s %d", leaderboard[leaderboard_size].name, &leaderboard[leaderboard_size].score) == 2) {
            leaderboard_size++;
            if (leaderboard_size >= MAX_LEADERBOARD) break;
        }
        fclose(file);
    }
}

void save_leaderboard() {
    FILE *file = fopen(leaderboard_file, "w");
    if (file) {
        for (int i = 0; i < leaderboard_size; i++) {
            fprintf(file, "%s %d\n", leaderboard[i].name, leaderboard[i].score);
        }
        fclose(file);
    }
}

void update_leaderboard(const char *player_name, int player_score) {
    ScoreEntry new_entry;
    strncpy(new_entry.name, player_name, MAX_NAME_LENGTH);
    new_entry.name[MAX_NAME_LENGTH - 1] = '\0';
    new_entry.score = player_score;

    int pos = leaderboard_size;
    if (leaderboard_size < MAX_LEADERBOARD) leaderboard_size++;
    for (int i = 0; i < leaderboard_size - 1; i++) {
        if (player_score > leaderboard[i].score) {
            pos = i;
            break;
        }
    }

    for (int i = leaderboard_size - 1; i > pos; i--) {
        leaderboard[i] = leaderboard[i - 1];
    }
    leaderboard[pos] = new_entry;
    save_leaderboard();
}

int menu() {
    const char *speed_labels[] = { "Slow", "Normal", "Fast" };
    int speed_values[] = { 150000, 100000, 60000 };
    char *head_symbols[] = { "@", "#", "O" };
    char *fruit_symbols[] = { "*", "&", "X" };
    const char *difficulty_labels[] = { "Easy", "Normal", "Hard" };

    int speed_index = 1, head_index = 0, fruit_index = 0, color_index = 0, difficulty_index = 0;
    int option = 0;
    const int max_options = 5;

    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    start_color();

    while (1) {
        clear();
        mvprintw(3, (WIDTH - 14) / 2, "--- Snake Game ---");

        for (int i = 0; i < max_options; i++) {
            if (i == option) attron(A_REVERSE);
            switch (i) {
                case 0:
                    mvprintw(6 + i * 2, (WIDTH - 20) / 2, "Speed: %s", speed_labels[speed_index]);
                    break;
                case 1:
                    mvprintw(6 + i * 2, (WIDTH - 20) / 2, "Head:  %s", head_symbols[head_index]);
                    break;
                case 2:
                    mvprintw(6 + i * 2, (WIDTH - 20) / 2, "Fruit: %s", fruit_symbols[fruit_index]);
                    break;
                case 3:
                    mvprintw(6 + i * 2, (WIDTH - 24) / 2, "Color Theme: %s", color_names[color_index]);
                    break;
                case 4:
                    mvprintw(6 + i * 2, (WIDTH - 24) / 2, "Difficulty: %s", difficulty_labels[difficulty_index]);
                    break;
            }
            if (i == option) attroff(A_REVERSE);
        }

        mvprintw(18, (WIDTH - 32) / 2, "\u2191/\u2193 Select, \u2190/\u2192 Change, Enter = Start");
        mvprintw(19, (WIDTH - 16) / 2, "ESC or q = Quit");
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
                    difficulty_index = (difficulty_index - 1 + 3) % 3;
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
                    difficulty_index = (difficulty_index + 1) % 3;
                break;
            case '\n':
                delay = speed_values[speed_index];
                snake_head_char = head_symbols[head_index][0];
                fruit_char = fruit_symbols[fruit_index][0];
                difficulty = difficulty_index;
                init_pair(1, snake_colors[color_index], COLOR_BLACK);
                init_pair(2, fruit_colors[color_index], COLOR_BLACK);
                return 1;
            case 27:
            case 'q':
                return 0;
        }
    }
}

void generate_obstacles() {
    obstacle_count = 0;
    int num = difficulty == 1 ? 10 : (difficulty == 2 ? 25 : 0);
    for (int i = 0; i < num; i++) {
        Point obs;
        do {
            obs.x = rand() % (WIDTH - 2) + 1;
            obs.y = rand() % (HEIGHT - 2) + 1;
        } while ((obs.x == food.x && obs.y == food.y));
        obstacles[obstacle_count++] = obs;
    }
}

void init_game() {
    clear();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    srand(time(NULL));

    snake_length = 5;
    dir_x = 1; dir_y = 0;
    score = 0;
    game_over = 0;

    for (int i = 0; i < snake_length; i++) {
        snake[i].x = WIDTH / 2 - i;
        snake[i].y = HEIGHT / 2;
    }

    food.x = rand() % (WIDTH - 2) + 1;
    food.y = rand() % (HEIGHT - 2) + 1;

    generate_obstacles();
    load_leaderboard();
}

void draw_board() {
    clear();
    for (int i = 0; i < WIDTH; i++) {
        mvprintw(0, i, "#");
        mvprintw(HEIGHT - 1, i, "#");
    }
    for (int i = 0; i < HEIGHT; i++) {
        mvprintw(i, 0, "#");
        mvprintw(i, WIDTH - 1, "#");
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

    mvprintw(HEIGHT, 0, "Score: %d", score);
    refresh();
}

void update_snake() {
    Point new_head = { snake[0].x + dir_x, snake[0].y + dir_y };

    if (new_head.x <= 0 || new_head.x >= WIDTH - 1 || new_head.y <= 0 || new_head.y >= HEIGHT - 1) {
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

    for (int i = snake_length - 1; i > 0; i--)
        snake[i] = snake[i - 1];

    snake[0] = new_head;

    if (new_head.x == food.x && new_head.y == food.y) {
        if (snake_length < MAX_SNAKE_LENGTH) snake_length++;
        food.x = rand() % (WIDTH - 2) + 1;
        food.y = rand() % (HEIGHT - 2) + 1;

        int score_gain = difficulty == 0 ? 10 : (difficulty == 1 ? 20 : 30);
        score += score_gain;
    }
}

void handle_input() {
    int ch = getch();
    switch (ch) {
        case KEY_UP:
            if (dir_y != 1) { dir_x = 0; dir_y = -1; }
            break;
        case KEY_DOWN:
            if (dir_y != -1) { dir_x = 0; dir_y = 1; }
            break;
        case KEY_LEFT:
            if (dir_x != 1) { dir_x = -1; dir_y = 0; }
            break;
        case KEY_RIGHT:
            if (dir_x != -1) { dir_x = 1; dir_y = 0; }
            break;
    }
}

void show_leaderboard() {
    clear();
    mvprintw(1, (WIDTH - 14) / 2, "Leaderboard:");
    for (int i = 0; i < leaderboard_size; i++) {
        mvprintw(3 + i, (WIDTH - 24) / 2, "%2d. %-10s %5d", i + 1, leaderboard[i].name, leaderboard[i].score);
    }
    mvprintw(15, (WIDTH - 28) / 2, "Press any key to exit...");
    refresh();
    getch();
}

int main() {
    if (!menu()) {
        endwin();
        return 0;
    }

    init_game();

    while (!game_over) {
        draw_board();
        handle_input();
        update_snake();
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
