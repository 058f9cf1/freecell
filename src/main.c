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
#define FOUNDATION_COUNT DECK_SIZE / 13
#define CASCADE_DEPTH 20

#define UNSELECTED 0
#define SELECTED 1

int main()
{
	//Initialise board, deck and the deal number
	int board[CASCADES + 2][CASCADE_DEPTH];
	int deck[DECK_SIZE];
	int deal = INT_MAX - 5;

	//Shuffle the deck
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

	//Fill out freecell and foundation columns with empty values
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



	//Initialise variables used in the main game loop
	int ch;
	int help_visible = 0;
	int cursor = 0;
	int selected = -1;
	int height = cascade_height(board[cursor], CASCADE_DEPTH);
	int top;
	int free_spaces = FREECELL_COUNT;
	int total = 0;

	//Set up initial board display
	init_screen(deal, 1);

	//Display main board
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

	//Display freecells
	for(int i = 0; i < FREECELL_COUNT; i++)
	{
		display_card(board[FREECELLS][i], UNSELECTED, CASCADES, FREECELLS, i);
	}

	//Display foundations
	for(int i = 0; i < FOUNDATION_COUNT; i++)
	{
		display_card(board[FOUNDATIONS][i], UNSELECTED, CASCADES, FOUNDATIONS, i);
	}

	//Display cursor
	display_cursor(CASCADES, cursor, height + 1, '*');

	//Refresh the screen
	update_screen();
	

	//Main game loop, get the pressed key on every loop
	while((ch = getch()) != 'q')
	{

		//Show or hide help
		if(ch == KEY_F(1))//F1
		{
			help_visible = toggle_help(help_visible);
		}

		//Move cursor to the cascade from the pressed key
		else if(ch >= 49 && ch <= 56)//Number key
		{
			display_cursor(CASCADES, cursor, height + 1, ' ');
			cursor = ch - 49;
			height = cascade_height(board[cursor], CASCADE_DEPTH);
			display_cursor(CASCADES, cursor, height + 1, '*');
			update_screen();
		}

		//Move cursor to frecell from the pressed key
		else if(ch == 97)//a
		{
			display_cursor(CASCADES, cursor, height + 1, ' ');
			cursor = CASCADES;
			height = cascade_height(board[selected], CASCADE_DEPTH);
			display_cursor(CASCADES, cursor, 0, '*');
			update_screen();
		}
		else if(ch == 115)//s
		{
			display_cursor(CASCADES, cursor, height + 1, ' ');
			cursor = CASCADES + 1;
			height = cascade_height(board[selected], CASCADE_DEPTH);
			display_cursor(CASCADES, cursor, 0, '*');
			update_screen();
		}
		else if(ch == 100)//d
		{
			display_cursor(CASCADES, cursor, height + 1, ' ');
			cursor = CASCADES + 2;
			height = cascade_height(board[selected], CASCADE_DEPTH);
			display_cursor(CASCADES, cursor, 0, '*');
			update_screen();
		}
		else if(ch == 102)//f
		{
			display_cursor(CASCADES, cursor, height + 1, ' ');
			cursor = CASCADES + 3;
			height = cascade_height(board[selected], CASCADE_DEPTH);
			display_cursor(CASCADES, cursor, 0, '*');
			update_screen();
		}

		//Select card
		else if(ch == 10)//Enter
		{

			//If no card is selected
			if(selected == -1)
			{
				//If a cascade is selected and there is at least one card in the cascade
				if(cursor >= 0 && cursor < CASCADES && height > 0)
				{
					//Show what cards are selected
					top = stack_top(board[cursor], CASCADE_DEPTH);
					for(int i = top; i < height; i++)
					{
						display_card(board[cursor][i], SELECTED, CASCADES, cursor, i);
					}

				}
				//If a non-empty freecell is selected
				else if(board[FREECELLS][cursor - CASCADES] != -1)
				{
					//Show what card is selected
					display_card(board[FREECELLS][cursor - CASCADES], SELECTED, CASCADES, FREECELLS, cursor - CASCADES);
				}

				//Save the selected cascade
				selected = cursor;
			}


			//If the selected card is selected again
			else if(selected == cursor)
			{
				//If a cascade is selected
				if(cursor >= 0 && cursor < CASCADES)
				{
					//Unshow selection on cascade
					top = stack_top(board[cursor], CASCADE_DEPTH);
					for(int i = top; i < height; i++)
					{
						display_card(board[cursor][i], UNSELECTED, CASCADES, cursor, i);
					}
				}
				else
				{
					//Unshow selection on freecell
					display_card(board[FREECELLS][cursor - CASCADES], UNSELECTED, CASCADES, FREECELLS, cursor - CASCADES);
				}

				//Unselect previous selection
				selected = -1;
			}


			//Two different cells selected
			else
			{
				//Move to freecell if freecell is empty
				if(cursor >= CASCADES && board[FREECELLS][cursor - CASCADES] == -1)
				{

					//From board to freecell
					if(selected < CASCADES)
					{
						//Unshow selection on cascade
						top = stack_top(board[selected], CASCADE_DEPTH);
						if(top != 0)
						{
							for(int i = top; i < height; i++)
							{
								display_card(board[selected][i], UNSELECTED, CASCADES, selected, i);
							}
						}

						//Move card to freecell
						display_card(board[selected][height - 1], UNSELECTED, CASCADES, FREECELLS, cursor - CASCADES);
						board[FREECELLS][cursor - CASCADES] = board[selected][height - 1];

						//Remove card from board
						display_card(-1, UNSELECTED, CASCADES, selected, height - 1);
						board[selected][height - 1] = -1;
					}

					//From freecell to freecell
					else
					{
						//Move card to freecell
						display_card(board[FREECELLS][selected - CASCADES], UNSELECTED, CASCADES, FREECELLS, cursor - CASCADES);
						board[FREECELLS][cursor - CASCADES] = board[FREECELLS][selected - CASCADES];

						//Remove card from freecell
						display_card(-1, UNSELECTED, CASCADES, FREECELLS, selected - CASCADES);
						board[FREECELLS][selected - CASCADES] = -1;
					}

					//Unselect previous selection
					selected = -1;
				}

				//Move to board
				else if(cursor < CASCADES)
				{
					//From board to board
					if(selected < CASCADES)
					{
					}

					//From freecell to board
					else
					{
					}
				}
			}

			//Refresh the screen
			update_screen();
		}

		//Try to send card to foundations
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

////TODO: Add support for more than 1 of the same suit of freecells

			//Is the move to the foundations valid?
			if(card != -1 && card / 4 == board[FOUNDATIONS][card % 4] + 1)
			{
				display_card(card, UNSELECTED, CASCADES, FOUNDATIONS, card % 4);
				board[FOUNDATIONS][card % 4]++;

////

				//From board to foundation
				if(cursor < CASCADES)
				{
					//Remove card from board
					display_card(-1, UNSELECTED, CASCADES, cursor, height - 1);
					board[cursor][height - 1] = -1;

					//Move cursor up a space
					display_cursor(CASCADES, cursor, height + 1, ' ');
					display_cursor(CASCADES, cursor, height, '*');
					height--;
				}

				//From freecell to foundation
				else
				{
					//Remove card from freecell
					display_card(-1, UNSELECTED, CASCADES, FREECELLS, cursor - CASCADES);
					board[FREECELLS][cursor - CASCADES] = -1;
				}

				//Increment the total number of cards in the foundations
				total++;

				//If all of the cards have been moved to the foundations
				if(total == DECK_SIZE)
				{
					printf("WIN!\n");
				}
			}

			update_screen();
		}

	}

	//Exit game when q is pressed
	exit_game();

	return 0;
}
