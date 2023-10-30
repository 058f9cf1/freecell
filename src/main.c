#include <stdio.h>
#include <limits.h>
#include <ncurses.h>
#include "generate_deal.h"
#include "show.h"

#define DECK_SIZE 52


int main()
{
	int deck[DECK_SIZE];
	int deal = INT_MAX;

	generate_deal(deck, DECK_SIZE, deal);
	
	
	char const suits[4] = {'C', 'D', 'H', 'S'};
	char const ranks[13] = {'A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K'};


	for(int i = 0; i < DECK_SIZE; i++)
	{
		printf("%c%c ", ranks[deck[DECK_SIZE - i - 1] / 4], suits[deck[DECK_SIZE - i - 1] % 4]);
		if(i % 8 == 7)
		{
			printf("\n");
		}
	}
	printf("\n");

	int ch;
	int help_visible = 0;

	init_screen(deal);
	while((ch = getch()) != 'q')
	{
		if(ch == KEY_F(1))
		{
			help_visible = toggle_help(help_visible);
		}
	}
	update_screen();

	return 0;
}
