#include "Fifo.h"
#include "string.h"

void init_fifo(fifo* f)
{
	f->m_head = 0;
	f->m_tail = 0;
}

int is_fifo_empty(fifo* f)
{
	return f->m_head == f->m_tail;
}

void enqueue(fifo* f, char* string)
{
	strcpy(f->m_string[f->m_head], string);
	f->m_head++;
}

char* unqueue(fifo* f)
{
	
	f->m_tail++;
	return f->m_string[f->m_tail - 1];
}
