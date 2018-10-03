
typedef struct fifo
{
	char m_string[16][128];
	int m_head;
	int m_tail;
}fifo;

void init_fifo(fifo* f);

int is_fifo_empty(fifo* f);

void enqueue(fifo* f, char* string);

char* unqueue(fifo* f);

