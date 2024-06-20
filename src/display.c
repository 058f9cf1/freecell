#include <ncurses.h>

#define BACKGROUND 1
#define INFO 2
#define HELP 3
#define UNSELECTED_BLACK_CARD 4
#define UNSELECTED_RED_CARD 5
#define SELECTED_BLACK_CARD 6
#define SELECTED_RED_CARD 7

WINDOW *table;
WINDOW *info_bar;
WINDOW *help_box;

char const suits[4] = {'C', 'D', 'H', 'S'};
char const ranks[13] = {'A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K'};
	
int rows, cols;


//Determine the order of magnitude of an integer
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


void init_screen(int deal, int invert)
{
	//Initialise curses
	initscr();
	curs_set(0);
	noecho();
	keypad(stdscr, TRUE);
	getmaxyx(stdscr, rows, cols);
	refresh();

	//Initialise colours
	start_color();
	use_default_colors();
	if(can_change_color() && COLORS >= 16)
	{
		init_color(COLOR_BLACK, 0, 0, 0);
		init_color(COLOR_WHITE, 1000, 1000, 1000);
		init_color(COLOR_RED, 700, 0, 0);
		init_color(COLOR_GREEN, 0, 300, 0);
	}

	//Window colours
	init_pair(BACKGROUND, COLOR_WHITE, COLOR_GREEN);
	init_pair(INFO, COLOR_WHITE, COLOR_BLACK);
	init_pair(HELP, COLOR_RED, COLOR_WHITE);

	//Card colours
	if(invert)
	{
		init_pair(UNSELECTED_BLACK_CARD, COLOR_WHITE, COLOR_BLACK);
		init_pair(UNSELECTED_RED_CARD, COLOR_WHITE, COLOR_RED);
	}
	else
	{
		init_pair(UNSELECTED_BLACK_CARD, COLOR_BLACK, COLOR_WHITE);
		init_pair(UNSELECTED_RED_CARD, COLOR_RED, COLOR_WHITE);
	}

	init_pair(SELECTED_BLACK_CARD, COLOR_BLACK, COLOR_CYAN);
	init_pair(SELECTED_RED_CARD, COLOR_RED, COLOR_CYAN);

	//Table
	table = newwin(rows - 1, cols, 0, 0);
	wbkgd(table, COLOR_PAIR(BACKGROUND));
	box(table, 0, 0);
	mvwprintw(table, 4, cols / 2 - 7, "F R E E C E L L");
	wrefresh(table);

	//Info bar
	info_bar = newwin(1, cols, rows - 1, 0);
	wbkgd(info_bar, COLOR_PAIR(INFO));
	mvwprintw(info_bar, 0, 1, "v0.1");
	int len = int_len(deal);
	mvwprintw(info_bar, 0, cols / 2 - len / 2 - 3, "Deal: %d", deal);
	mvwprintw(info_bar, 0, cols - 15, "F1 for controls");
	wrefresh(info_bar);
	
	//Help box
	int const HELP_HEIGHT = 10;
	int const HELP_WIDTH = 40;
	help_box = newwin(HELP_HEIGHT, HELP_WIDTH, (rows - HELP_HEIGHT) / 2, (cols - HELP_WIDTH) / 2);
	wbkgd(help_box, COLOR_PAIR(HELP));
	box(help_box, 0, 0);
	mvwprintw(help_box, 1, HELP_WIDTH / 2 - 4, "FREECELL");
	mvwprintw(help_box, 2, HELP_WIDTH / 2 - 17, "Send all cards to the foundations!");
	mvwprintw(help_box, 4, 1, "     [q] to quit");
	mvwprintw(help_box, 5, 1, "   [1-8] to change columns");
	mvwprintw(help_box, 6, 1, "  [asdf] to change to freecells");
	mvwprintw(help_box, 7, 1, "     [h] to send to foundation");
	mvwprintw(help_box, 8, 1, "[return] to select cascade/card");
}


void display_card(int card, int selected, int cascades, int x, int y)
{
	char rank = ' ';
	char suit = ' ';

	//Set card value and colour
	if(card == -1 && x >= cascades)
	{
		wattron(table, COLOR_PAIR(UNSELECTED_BLACK_CARD));
	}
	else if(card == -1)
	{
		wattron(table, COLOR_PAIR(BACKGROUND));
	}
	else
	{
		rank = ranks[card / 4];
		suit = suits[card % 4];

		if(selected == 0)
		{
			if(suit == 'C' || suit == 'S')
			{
				wattron(table, COLOR_PAIR(UNSELECTED_BLACK_CARD));
			}
			else if(suit == 'D' || suit == 'H')
			{
				wattron(table, COLOR_PAIR(UNSELECTED_RED_CARD));
			}
		}
		else if(selected >= 1)
		{
			if(suit == 'C' || suit == 'S')
			{
				wattron(table, COLOR_PAIR(SELECTED_BLACK_CARD));
			}
			else if(suit == 'D' || suit == 'H')
			{
				wattron(table, COLOR_PAIR(SELECTED_RED_CARD));
			}
		}
	}
	wattron(table, A_BOLD);

	//Display card at position on the screen
	if(x < cascades)//Board
	{
		mvwprintw(table, y + 10, (x * 3 + 1) + cols / 2 - 12, "%c%c", rank, suit);
	}
	else if(x == cascades)//Freecells
	{
		x -= cascades;
		mvwprintw(table, y + 10, (x * 3 + 1) + cols / 2 - 22, "%c%c", rank, suit);
	}
	else if(x == cascades + 1)//Foundations
	{
		x -= cascades - 1;
		mvwprintw(table, y + 10, (x * 3 + 1) + cols / 2 + 13, "%c%c", rank, suit);
	}
	wattroff(table, A_BOLD);
	wattroff(table, COLOR_PAIR(UNSELECTED_BLACK_CARD));
	wattroff(table, COLOR_PAIR(UNSELECTED_RED_CARD));
	wattroff(table, COLOR_PAIR(SELECTED_BLACK_CARD));
	wattroff(table, COLOR_PAIR(SELECTED_RED_CARD));
}


void display_cursor(int cascades, int x, int y, char c)
{
	if(x < cascades)//Board
	{
		mvwprintw(table, y + 10, (x * 3 + 1) + cols / 2 - 12, "%c", c);
	}
	else if(x >= cascades)//Freecells
	{
		x -= cascades;
		mvwprintw(table, x + 10, cols / 2 - 18, "%c", c);
	}
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
	wrefresh(table);
	wrefresh(info_bar);
}


void exit_game()
{
	endwin();
}
