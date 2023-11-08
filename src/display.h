#ifndef SHOW_H
#define SHOW_H

void init_screen(int deal);
int toggle_help(int show_help);
void display_card(int card, int selected, int x, int y);
void display_cursor(int x, int y, char c);
void update_screen();
void exit_game();

#endif
