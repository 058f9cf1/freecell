#include <ncurses.h>

#define BG_GREEN 10

#define BACKGROUND 1
#define INFO 2
#define HELP 3

WINDOW *table;
WINDOW *info_bar;
WINDOW *help_box;


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


void init_screen(int deal)
{
	//Initialise curses
	int rows, cols;

	initscr();
	curs_set(0);
	noecho();
	keypad(stdscr, TRUE);
	getmaxyx(stdscr, rows, cols);

	//Initialise colours
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
	init_pair(HELP, COLOR_RED, COLOR_WHITE);

	//Refresh stdscr
	refresh();

	//Main play area
	table = newwin(rows - 1, cols, 0, 0);
	wbkgd(table, COLOR_PAIR(BACKGROUND));
	box(table, 0, 0);
	wrefresh(table);

	//Info bar
	info_bar = newwin(1, cols, rows - 1, 0);
	wbkgd(info_bar, COLOR_PAIR(INFO));
	mvwprintw(info_bar, 0, 1, "v0.1");
	int len = int_len(deal);
	int centre_col = cols / 2;
	mvwprintw(info_bar, 0, centre_col - len / 2 - 3, "Deal: %d", deal);
	mvwprintw(info_bar, 0, cols - 15, "F1 for controls");
	wrefresh(info_bar);
	
	//Help box
	int const HELP_HEIGHT = 10;
	int const HELP_WIDTH = 40;

	help_box = newwin(HELP_HEIGHT, HELP_WIDTH, (rows-HELP_HEIGHT)/2, (cols-HELP_WIDTH)/2);
	wbkgd(help_box, COLOR_PAIR(HELP));
	box(help_box, 0, 0);
	mvwprintw(help_box, 1, 1, "FREECELL");
	mvwprintw(help_box, 2, 1, "Send all cards to the foundations!");
	mvwprintw(help_box, 4, 1, "q to quit");
}


int toggle_help(int visible)
{
	if(!visible)
	{
		touchwin(help_box);
		wrefresh(help_box);
	}
	else if(visible)
	{
		touchwin(table);
		wrefresh(table);
	}
	return !visible;
}


void update_screen()
{
	endwin();
}
