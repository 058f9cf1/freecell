#include <ncurses.h>
#include "cards.h"
#include "display.h"


#define DECK_SIZE 52
#define CASCADE_COUNT 8
#define FREECELL_COUNT 4
#define FOUNDATION_COUNT (DECK_SIZE - 1) / 52 * 4 + 4 
#define CASCADE_DEPTH 20


#define FREECELLS CASCADE_COUNT
#define FOUNDATIONS CASCADE_COUNT + 1

#define UNSELECTED 0
#define SELECTED 1

//Initialise board, deck and the deal number
int board[CASCADE_COUNT + 2][CASCADE_DEPTH];
int deck[DECK_SIZE];
int deal = 2147483642;


int main()
{
	//Shuffle the deck
	generate_deal(deck, DECK_SIZE, deal);

	//Deal the deck
	for(int i = 0; i < CASCADE_COUNT * CASCADE_DEPTH; i++)
	{
		if(i < DECK_SIZE)
		{
			board[i % CASCADE_COUNT][i / CASCADE_COUNT] = deck[DECK_SIZE - 1 - i];
		}
		else
		{
			board[i % CASCADE_COUNT][i / CASCADE_COUNT] = -1;
		}
	}

	//Fill out freecell and foundation columns with empty values
	for(int i = 0; i < CASCADE_DEPTH; i++)
	{
		board[FREECELLS][i] = -1;
		board[FOUNDATIONS][i] = -1;
	}

	//Set up initial board display
	init_screen(deal, 1);

	//Display main board
	for(int i = 0; i < CASCADE_COUNT; i++)
	{
		for(int j = 0; j < CASCADE_DEPTH; j++)
		{
			if(board[i][j] != -1)
			{
				display_card(board[i][j], UNSELECTED, CASCADE_COUNT, i, j);
			}
		}
	}

	//Display freecells
	for(int i = 0; i < FREECELL_COUNT; i++)
	{
		display_card(board[FREECELLS][i], UNSELECTED, CASCADE_COUNT, FREECELLS, i);
	}

	//Display foundations
	for(int i = 0; i < FOUNDATION_COUNT; i++)
	{
		display_card(board[FOUNDATIONS][i], UNSELECTED, CASCADE_COUNT, FOUNDATIONS, i);
	}

	//Initialise variables used in the main game loop
	int ch;
	int help_visible = 0;
	int cursor = 0;
	int selected = -1;
	int height = DECK_SIZE / CASCADE_COUNT + (cursor < DECK_SIZE % CASCADE_COUNT);
	int top;
	int free_spaces = FREECELL_COUNT;
	int total = 0;

	//Display cursor
	display_cursor(CASCADE_COUNT, cursor, height + 1, '*');

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
		else if(ch >= 49 && ch <= 57 && ch - 49 < CASCADE_COUNT)//Number key
		{
			display_cursor(CASCADE_COUNT, cursor, height + 1, ' ');
			cursor = ch - 49;
			height = cascade_height(board[cursor], CASCADE_DEPTH);
			display_cursor(CASCADE_COUNT, cursor, height + 1, '*');
			update_screen();
		}
		else if(ch == KEY_LEFT)
		{
			display_cursor(CASCADE_COUNT, cursor, height + 1, ' ');
			cursor = (cursor - 1) % CASCADE_COUNT;
			if(cursor == -1)
			{
				cursor = CASCADE_COUNT - 1;
			}
			height = cascade_height(board[cursor], CASCADE_DEPTH);
			display_cursor(CASCADE_COUNT, cursor, height + 1, '*');
			update_screen();
		}
		else if(ch == KEY_RIGHT)
		{
			display_cursor(CASCADE_COUNT, cursor, height + 1, ' ');
			cursor = (cursor + 1) % CASCADE_COUNT;
			height = cascade_height(board[cursor], CASCADE_DEPTH);
			display_cursor(CASCADE_COUNT, cursor, height + 1, '*');
			update_screen();
		}

		//Move cursor to frecell from the pressed key
		else if(ch == 97)//a
		{
			display_cursor(CASCADE_COUNT, cursor, height + 1, ' ');
			cursor = CASCADE_COUNT;
			height = cascade_height(board[selected], CASCADE_DEPTH);
			display_cursor(CASCADE_COUNT, cursor, 0, '*');
			update_screen();
		}
		else if(ch == 115)//s
		{
			display_cursor(CASCADE_COUNT, cursor, height + 1, ' ');
			cursor = CASCADE_COUNT + 1;
			height = cascade_height(board[selected], CASCADE_DEPTH);
			display_cursor(CASCADE_COUNT, cursor, 0, '*');
			update_screen();
		}
		else if(ch == 100)//d
		{
			display_cursor(CASCADE_COUNT, cursor, height + 1, ' ');
			cursor = CASCADE_COUNT + 2;
			height = cascade_height(board[selected], CASCADE_DEPTH);
			display_cursor(CASCADE_COUNT, cursor, 0, '*');
			update_screen();
		}
		else if(ch == 102)//f
		{
			display_cursor(CASCADE_COUNT, cursor, height + 1, ' ');
			cursor = CASCADE_COUNT + 3;
			height = cascade_height(board[selected], CASCADE_DEPTH);
			display_cursor(CASCADE_COUNT, cursor, 0, '*');
			update_screen();
		}

		//Select card
		else if(ch == 10)//Enter
		{
			//If no card is selected
			if(selected == -1)
			{
				//If a cascade is selected and there is at least one card in the cascade
				if(cursor >= 0 && cursor < CASCADE_COUNT && height > 0)
				{
					//Show what cards are selected
					top = stack_top(board[cursor], CASCADE_DEPTH);
					for(int i = top; i < height; i++)
					{
						display_card(board[cursor][i], SELECTED, CASCADE_COUNT, cursor, i);
					}

					//Save the selected cascade
					selected = cursor;
				}

				//If a non-empty freecell is selected
				else if(board[FREECELLS][cursor - CASCADE_COUNT] != -1)
				{
					//Show what card is selected
					display_card(board[FREECELLS][cursor - CASCADE_COUNT], SELECTED, CASCADE_COUNT, FREECELLS, cursor - CASCADE_COUNT);

					//Save the selected cascade
					selected = cursor;
				}
			}

			//If the selected card is selected again
			else if(selected == cursor)
			{
				//If a cascade is selected
				if(cursor >= 0 && cursor < CASCADE_COUNT)
				{
					//Unshow selection on cascade
					top = stack_top(board[cursor], CASCADE_DEPTH);
					for(int i = top; i < height; i++)
					{
						display_card(board[cursor][i], UNSELECTED, CASCADE_COUNT, cursor, i);
					}
				}
				else
				{
					//Unshow selection on freecell
					display_card(board[FREECELLS][cursor - CASCADE_COUNT], UNSELECTED, CASCADE_COUNT, FREECELLS, cursor - CASCADE_COUNT);
				}

				//Unselect previous selection
				selected = -1;
			}

			//Two different cells selected
			else
			{
				//Move to freecell if freecell is empty
				if(cursor >= CASCADE_COUNT && board[FREECELLS][cursor - CASCADE_COUNT] == -1)
				{
					//From board to freecell
					if(selected < CASCADE_COUNT)
					{
						//Unshow selection on cascade
						top = stack_top(board[selected], CASCADE_DEPTH);
						for(int i = top; i < height; i++)
						{
							display_card(board[selected][i], UNSELECTED, CASCADE_COUNT, selected, i);
						}

						//Move card to freecell
						display_card(board[selected][height - 1], UNSELECTED, CASCADE_COUNT, FREECELLS, cursor - CASCADE_COUNT);
						board[FREECELLS][cursor - CASCADE_COUNT] = board[selected][height - 1];

						//Remove card from board
						display_card(-1, UNSELECTED, CASCADE_COUNT, selected, height - 1);
						board[selected][height - 1] = -1;

						//Don't subtract from free_spaces if an empty column is created
						if(height != 1)
						{
							free_spaces--;
						}
					}

					//From freecell to freecell
					else
					{
						//Move card to freecell
						display_card(board[FREECELLS][selected - CASCADE_COUNT], UNSELECTED, CASCADE_COUNT, FREECELLS, cursor - CASCADE_COUNT);
						board[FREECELLS][cursor - CASCADE_COUNT] = board[FREECELLS][selected - CASCADE_COUNT];

						//Remove card from freecell
						display_card(-1, UNSELECTED, CASCADE_COUNT, FREECELLS, selected - CASCADE_COUNT);
						board[FREECELLS][selected - CASCADE_COUNT] = -1;
					}

					//Unselect previous selection
					selected = -1;
				}

				//Move to board
				else if(cursor < CASCADE_COUNT)
				{
					//From board to board
					if(selected < CASCADE_COUNT)
					{
						//Try to find a matching pair in the stack
						top = stack_top(board[selected], CASCADE_DEPTH);
						int selected_height = cascade_height(board[selected], CASCADE_DEPTH);
						while(top != selected_height && !valid_pair(board[selected][top], board[cursor][height - 1]))
						{
							top++;
						}

						//Determine if a match was found
						if(top != selected_height)
						{
							//Determine if a stack can be moved legally
							if(selected_height - top <= free_spaces + 1)
							{
								//TODO
								display_card(board[selected][top], UNSELECTED, CASCADE_COUNT, FREECELLS, 5);
							}
						}
					}

					//From freecell to board
					else if(valid_pair(board[FREECELLS][selected - CASCADE_COUNT], board[cursor][height - 1]))
					{
						//Move cursor up a space
						height++;
						display_cursor(CASCADE_COUNT, cursor, height, ' ');
						display_cursor(CASCADE_COUNT, cursor, height + 1, '*');

						//Move card to board
						display_card(board[FREECELLS][selected - CASCADE_COUNT], UNSELECTED, CASCADE_COUNT, cursor, height - 1);
						board[cursor][height - 1] = board[FREECELLS][selected - CASCADE_COUNT];

						//Remove card from freecell
						display_card(-1, UNSELECTED, CASCADE_COUNT, FREECELLS, selected - CASCADE_COUNT);
						board[FREECELLS][selected - CASCADE_COUNT] = -1;

						//Unselect previous selection
						selected = -1;

						//Don't add to free_spaces if moving to empty column
						if(height != 1)
						{
							free_spaces++;
						}
					}
				}
			}

			//Refresh the screen
			update_screen();
		}

		//Try to send card to foundations
		else if(ch == 104 && selected == -1)//h
		{
			int card = -1;
			if(cursor < CASCADE_COUNT)
			{
				if(height != 0)
				{
					card = board[cursor][height - 1];
				}
			}
			else
			{
				card = board[FREECELLS][cursor - CASCADE_COUNT];
			}

////TODO: Add support for more than 1 of the same suit of freecells

			//Is the move to the foundations valid?
			if(card != -1 && card / 4 == board[FOUNDATIONS][card % 4] + 1)
			{
				display_card(card, UNSELECTED, CASCADE_COUNT, FOUNDATIONS, card % 4);
				board[FOUNDATIONS][card % 4]++;
////

				//From board to foundation
				if(cursor < CASCADE_COUNT)
				{
					//Remove card from board
					display_card(-1, UNSELECTED, CASCADE_COUNT, cursor, height - 1);
					board[cursor][height - 1] = -1;

					//Move cursor up a space
					display_cursor(CASCADE_COUNT, cursor, height + 1, ' ');
					display_cursor(CASCADE_COUNT, cursor, height, '*');
					height--;

					//Add to free_spaces if an empty column is created
					if(height == 0)
					{
						free_spaces++;
					}
				}

				//From freecell to foundation
				else
				{
					//Remove card from freecell
					display_card(-1, UNSELECTED, CASCADE_COUNT, FREECELLS, cursor - CASCADE_COUNT);
					board[FREECELLS][cursor - CASCADE_COUNT] = -1;

					free_spaces++;
				}

				//Increment the total number of cards in the foundations
				total++;

				//If all of the cards have been moved to the foundations
				if(total == DECK_SIZE)
				{
					end_screen();
				}
			}

			update_screen();
		}
	}

	//Exit game when q is pressed
	exit_game();

	return 0;
}
