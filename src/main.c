#include <stdio.h>
#include <limits.h>
#include <ncurses.h>
#include "cards.h"
#include "display.h"

#define DECK_SIZE 52
#define CASCADES 8
#define FREECELLS CASCADES
#define FOUNDATIONS CASCADES + 1
#define FREECELL_COUNT 4
#define FOUNDATION_COUNT 4
#define CASCADE_DEPTH 20

#define UNSELECTED 0
#define SELECTED 1

int main()
{
	int board[CASCADES + 2][CASCADE_DEPTH];
	int deck[DECK_SIZE];
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
	//Fill out freecell and foundation columns
	for(int i = 0; i < CASCADE_DEPTH; i++)
	{
		board[FREECELLS][i] = -1;
		board[FOUNDATIONS][i] = -1;
	}



	//Print for debugging
	for(int i = 0; i < CASCADE_DEPTH; i++)
	{
		for(int j = 0; j < CASCADES + 2; j++)
		{
			printf("%02d ", board[j][i]);
		}
		printf("\n");
	}
	
	

	int ch;
	int help_visible = 0;
	int cursor = 0;
	int selected = -1;
	int height = cascade_height(board[cursor], CASCADE_DEPTH);
	int top;
	int total;

	//Set up initial board display
	init_screen(deal);
	for(int i = 0; i < CASCADES; i++)
	{
		for(int j = 0; j < CASCADE_DEPTH; j++)
		{
			if(board[i][j] != -1)
			{
				display_card(board[i][j], UNSELECTED, CASCADES, i, j);
			}
		}
	}
	for(int i = 0; i < FREECELL_COUNT; i++)
	{
		display_card(board[FREECELLS][i], UNSELECTED, CASCADES, FREECELLS, i);
	}
	for(int i = 0; i < FOUNDATION_COUNT; i++)
	{
		display_card(board[FOUNDATIONS][i], UNSELECTED, CASCADES, FOUNDATIONS, i);
	}
	display_cursor(CASCADES, cursor, height + 1, '*');
	update_screen();
	
	//Main game loop	
	while((ch = getch()) != 'q')
	{
		if(ch == KEY_F(1))//F1
		{
			help_visible = toggle_help(help_visible);
		}
		else if(ch >= 49 && ch <= 56)//Number key
		{
			display_cursor(CASCADES, cursor, height + 1, ' ');
			cursor = ch - 49;
			height = cascade_height(board[cursor], CASCADE_DEPTH);
			display_cursor(CASCADES, cursor, height + 1, '*');
			update_screen();
		}
		else if(ch == 97)//a
		{
			display_cursor(CASCADES, cursor, height + 1, ' ');
			cursor = CASCADES;
			display_cursor(CASCADES, cursor, 0, '*');
			update_screen();
		}
		else if(ch == 115)//s
		{
			display_cursor(CASCADES, cursor, height + 1, ' ');
			cursor = CASCADES + 1;
			display_cursor(CASCADES, cursor, 0, '*');
			update_screen();
		}
		else if(ch == 100)//d
		{
			display_cursor(CASCADES, cursor, height + 1, ' ');
			cursor = CASCADES + 2;
			display_cursor(CASCADES, cursor, 0, '*');
			update_screen();
		}
		else if(ch == 102)//f
		{
			display_cursor(CASCADES, cursor, height + 1, ' ');
			cursor = CASCADES + 3;
			display_cursor(CASCADES, cursor, 0, '*');
			update_screen();
		}
		else if(ch == 10)//Enter
		{
			if(selected == -1)
			{
				if(cursor >= 0 && cursor < CASCADES)
				{
					top = stack_top(board[cursor], CASCADE_DEPTH);
					if(top != 0)
					{
						for(int i = top; i < height; i++)
						{
							display_card(board[cursor][i], SELECTED, CASCADES, cursor, i);
						}
						selected = cursor;
					}
				}
				else if(board[FREECELLS][cursor - CASCADES] != -1)
				{
					display_card(board[FREECELLS][cursor - CASCADES], SELECTED, CASCADES, FREECELLS, cursor - CASCADES);
					selected = cursor;
				}
			}
			else if(selected == cursor)
			{
				if(cursor >= 0 && cursor < CASCADES)
				{
					top = stack_top(board[cursor], CASCADE_DEPTH);
					for(int i = top; i < height; i++)
					{
						display_card(board[cursor][i], UNSELECTED, CASCADES, cursor, i);
					}
				}
				else
				{
					display_card(board[FREECELLS][cursor - CASCADES], UNSELECTED, CASCADES, FREECELLS, cursor - CASCADES);
				}
				selected = -1;
			}
			else//Two different cells selected
			{
				if(cursor >= CASCADES)//Move to freecell
				{
					if(board[FREECELLS][cursor - CASCADES] == -1)//If freecell empty
					{
						if(selected < CASCADES)
						{
							top = stack_top(board[selected], CASCADE_DEPTH);
							if(top != 0)
							{
								for(int i = top; i < height; i++)
								{
									display_card(board[selected][i], UNSELECTED, CASCADES, selected, i);
								}
							}

							display_card(board[selected][height - 1], UNSELECTED, CASCADES, FREECELLS, cursor - CASCADES);
							display_card(-1, UNSELECTED, CASCADES, selected, height - 1);

							board[FREECELLS][cursor - CASCADES] = board[selected][height - 1];
							board[selected][height - 1] = -1;
						}
						else
						{
							display_card(board[FREECELLS][selected - CASCADES], UNSELECTED, CASCADES, FREECELLS, cursor - CASCADES);
							display_card(-1, UNSELECTED, CASCADES, FREECELLS, selected - CASCADES);
							board[FREECELLS][cursor - CASCADES] = board[FREECELLS][selected - CASCADES];
							board[FREECELLS][selected - CASCADES] = -1;
						}
						selected = -1;
					}
				}
			}
			
			update_screen();
		}
		else if(ch == 104 && selected == -1)//h
		{
			int card;
			if(cursor < CASCADES)
			{
				card = board[cursor][height - 1];
			}
			else
			{
				card = board[FREECELLS][cursor - CASCADES];
			}
			//Is the move to the foundations valid?
			if(card != -1 && card / 4 == board[FOUNDATIONS][card % 4] + 1)
			{
				display_card(card, UNSELECTED, CASCADES, FOUNDATIONS, card % 4);
				board[FOUNDATIONS][card % 4]++;
				if(cursor < CASCADES)
				{
					display_card(-1, UNSELECTED, CASCADES, cursor, height - 1);
					board[cursor][height - 1] = -1;
					display_cursor(CASCADES, cursor, height + 1, ' ');
					display_cursor(CASCADES, cursor, height, '*');
					height--;
				}
				else
				{
					display_card(-1, UNSELECTED, CASCADES, FREECELLS, cursor - CASCADES);
					board[FREECELLS][cursor - CASCADES] = -1;
				}
				total++;
				display_card(total, UNSELECTED, CASCADES, FOUNDATIONS, 5);
				if(total == DECK_SIZE)
				{
					printf("WIN!\n");
				}
			}

			update_screen();
		}
	}
	exit_game();

	return 0;
}
