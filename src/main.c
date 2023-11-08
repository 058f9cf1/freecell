#include <stdio.h>
#include <limits.h>
#include <ncurses.h>
#include "cards.h"
#include "display.h"

#define DECK_SIZE 52
#define CASCADES 8
#define CASCADE_DEPTH 20


int main()
{
	int board[CASCADES][CASCADE_DEPTH];
	int deck[DECK_SIZE];
	int freecell[4];
	int foundation[4];
	int deal = INT_MAX - 5;

	//Shuffle the cards
	generate_deal(deck, DECK_SIZE, deal);

	//Deal the deck
	for(int i = 0; i < CASCADES * CASCADE_DEPTH; i++)
	{
		if(i < DECK_SIZE)
		{
			board[i % CASCADES][i / CASCADES] = deck[DECK_SIZE - 1 - i];
		}
		else
		{
			board[i % CASCADES][i / CASCADES] = -1;
		}
	}
	
	

	int ch;
	int help_visible = 0;
	int cursor = 0;
	int selected = -1;
	int height = cascade_height(board[cursor], CASCADE_DEPTH);
	int top;

	init_screen(deal);
	for(int i = 0; i < CASCADES; i++)
	{
		for(int j = 0; j < CASCADE_DEPTH; j++)
		{
			if(board[i][j] != -1)
			{
				display_card(board[i][j], 0, i, j);
			}
		}
	}
	display_cursor(cursor, height + 1, '*');
	update_screen();
	
	
	while((ch = getch()) != 'q')
	{
		if(ch == KEY_F(1))//F1
		{
			help_visible = toggle_help(help_visible);
		}
		else if(ch >= 49 && ch <= 56)//Number key
		{
			display_cursor(cursor, height + 1, ' ');
			cursor = ch - 49;
			height = cascade_height(board[cursor], CASCADE_DEPTH);
			display_cursor(cursor, height + 1, '*');
			update_screen();
		}
		else if(ch == 10)//Enter
		{
			if(selected == -1)
			{
				top = stack_top(board[cursor], CASCADE_DEPTH);
				for(int i = top; i < height; i++)
				{
					display_card(board[cursor][i], 1, cursor, i);
				}
				selected = cursor;
			}
			else if(selected == cursor)
			{
				top = stack_top(board[cursor], CASCADE_DEPTH);
				for(int i = top; i < height; i++)
				{
					display_card(board[cursor][i], 0, cursor, i);
				}
				selected = -1;
			}
			update_screen();
		}
	}
	exit_game();

	return 0;
}
