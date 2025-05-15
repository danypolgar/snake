#ifndef LEADERBOARD_H
#define LEADERBOARD_H

int leaderboard_size;
char *leaderboard_file;

#define MAX_NAME_LENGTH 32

void save_leaderboard();
void update_leaderboard(const char *player_name, int player_score);
void load_leaderboard();
void show_leaderboard(int width);

#endif
