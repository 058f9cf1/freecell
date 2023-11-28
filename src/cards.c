int value;


//Microsoft freecell random number generator
int rand()
{
	value = (value * 214013 + 2531011) & 0x7FFFFFFF;
	return value >> 16;
}


//Microsoft freecell shuffling algorithm
void generate_deal(int deck[], int size, int deal)
{
	//Set the inital seed
	value = deal;
	
	//Place the deck in order
	for(int i = 0; i < size; i++)
	{
		deck[i] = i % 52;
	}

	//Shuffle
	for(int i = size - 1; i >= 0; i--)
	{
		int card_index = rand() % (i + 1);
		int temp = deck[card_index];
		deck[card_index] = deck[i];
		deck[i] = temp;
	}
}


int cascade_height(int cascade[], int depth)
{
	int height = 0;
	while(cascade[height] != -1 && height < depth)
	{
		height++;
	}
	return height;
}


int valid_pair(int card1, int card2)
{
	int opposite_suits;
	int same_rank = card1 / 4 + 1 == card2 / 4;

	if(card1 % 4 == 0 || card1 % 4 == 3)//Black card
	{
		opposite_suits = card2 % 4 == 1 || card2 % 4 == 2;
	}
	else
	{
		opposite_suits = card2 % 4 == 0 || card2 % 4 == 3;
	}
	return same_rank && opposite_suits;
}


int stack_top(int cascade[], int depth)
{
	int height = cascade_height(cascade, depth);
	int size = 0;
	int valid = 1;
	while(valid && (height - size) > 0)
	{
		size++;
		valid = valid_pair(cascade[height - size], cascade[height - size - 1]);
	}
	return height - size;
}
