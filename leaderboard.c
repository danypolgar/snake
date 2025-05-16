#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include "leaderboard.h"

#define MAX_LEADERBOARD 10

typedef struct {
    char name[MAX_NAME_LENGTH];
    int score;
} ScoreEntry;

char *leaderboard_file = "leaderboard.txt";
ScoreEntry leaderboard[MAX_LEADERBOARD];
int leaderboard_size = 0;

void load_leaderboard() {
    FILE *file = fopen(leaderboard_file, "r");
    if (file) {
        leaderboard_size = 0;
        while (fscanf(file, "%s %d", leaderboard[leaderboard_size].name, &leaderboard[leaderboard_size].score) == 2) {
            leaderboard_size++;
            if (leaderboard_size >= MAX_LEADERBOARD) {
                break;
            }
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

void update_leaderboard(char *player_name, int player_score) {
    ScoreEntry new_entry;
    if (strcmp(player_name, "") == 0) {
        strncpy(new_entry.name, "unknown", MAX_NAME_LENGTH);
    } else {
        strncpy(new_entry.name, player_name, MAX_NAME_LENGTH);
    }
    new_entry.name[MAX_NAME_LENGTH - 1] = '\0';
    new_entry.score = player_score;

    int pos = leaderboard_size;
    if (leaderboard_size < MAX_LEADERBOARD) {
        leaderboard_size++;
    }
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

void show_leaderboard() {
    clear();
    mvprintw(1, 13, "Leaderboard:");
    for (int i = 0; i < leaderboard_size; i++) {
        mvprintw(3 + i, 8, "%2d. %-10s %5d", i + 1, leaderboard[i].name, leaderboard[i].score);
    }
    mvprintw(15, 6, "Press any key to exit...");
    refresh();
    getch();
}
