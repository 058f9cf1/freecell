int value;


int rand()
{
	value = (value * 214013 + 2531011) & 0x7FFFFFFF;
	return value >> 16;
}


void generate_deal(int deck[], int size, int deal)
{
	value = deal;
	
	for(int i = 0; i < size; i++)
	{
		deck[i] = i;
	}

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
	while(cascade[height] != -1)
	{
		height++;
	}
	return height;
}


int stack_top(int cascade[], int depth)
{
	int height = cascade_height(cascade, depth);
	int size = 0;
	int same_rank = 1;
	int opposite_suits = 1;
	while(same_rank && opposite_suits &&  height - size > 0)
	{
		size++;
		same_rank = cascade[height - size] / 4 + 1 == cascade[height - size - 1] / 4;
		if(cascade[height - size] % 4 == 0 || cascade[height - size] % 4 == 3)//Black card
		{
			opposite_suits = cascade[height - size - 1] % 4 == 1 || cascade[height - size - 1] % 4 == 2;
		}
		else
		{
			opposite_suits = cascade[height - size - 1] % 4 == 0 || cascade[height - size - 1] % 4 == 3;
		}
	}
	return height - size;
}
