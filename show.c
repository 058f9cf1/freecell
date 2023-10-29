#include <ncurses.h>

#define COLOR_BLACK 0
#define BG_GREEN 10

#define BACKGROUND 1
#define INFO 2

WINDOW *table;
WINDOW *info_bar;


int int_len(int n)
{
	int len = 1;

	while(n > 9)
	{
		n = n / 10;
		len++;
	}

	return len;
}


int init_screen(int deal)
{
	int rows, cols;

	initscr();
	curs_set(0);
	getmaxyx(stdscr, rows, cols);

	start_color();
	use_default_colors();
	if(can_change_color() && COLORS >= 16)
	{
		init_color(COLOR_BLACK, 0, 0, 0);
		init_color(BG_GREEN, 0, 200, 0);
		init_pair(BACKGROUND, COLOR_WHITE, BG_GREEN);
	}
	else
	{
		init_pair(BACKGROUND, COLOR_WHITE, COLOR_GREEN);
	}
	init_pair(INFO, COLOR_WHITE, COLOR_BLACK);

	refresh();

	table = newwin(rows - 1, cols, 0, 0);
	wbkgd(table, COLOR_PAIR(BACKGROUND));
	box(table, 0, 0);
	wrefresh(table);

	info_bar = newwin(1, cols, rows - 1, 0);
	wbkgd(info_bar, COLOR_PAIR(INFO));
	mvwprintw(info_bar, 0, 1, "v0.1");
	int len = int_len(deal);
	int centre_col = cols / 2;
	mvwprintw(info_bar, 0, centre_col - len / 2 - 3, "Deal: %d", deal);
	wrefresh(info_bar);

	return 0;
}


int update_screen()
{
	wrefresh(table);
	wrefresh(info_bar);
	getch(); 
	endwin();
}
