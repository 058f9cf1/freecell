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
