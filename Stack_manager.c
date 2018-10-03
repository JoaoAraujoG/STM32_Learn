#include "Stack_manager.h"

unsigned char top_index = 0;

unsigned char pop(unsigned char* stack)
{
	
	if(top_index != 0)
	{
		unsigned char ID = *(stack + top_index);
		top_index--;
		return ID;
	}
	
	return 0;
}

void push(unsigned char* stack, unsigned char ID)
{
	top_index++;
	*(stack + top_index) = ID;
}

unsigned char peek(unsigned char* stack)
{
	if(top_index != 0)
	{
		return *(stack + top_index);
	}
	
	else return 0;
}
