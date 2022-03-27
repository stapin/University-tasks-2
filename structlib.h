#ifndef STRUCTLIB_H
#define STRUCTLIB_H

#define STACK_EMPTY (-5)
#define QUEUE_EMPTY (-4)
#define NO_SUCH_ELEMENT (-3)
#define INCORRECT_ARG (-2)
#define WRONG_INPUT (-1)
#define UNKNOWN_ERROR 0
#define SUCCEED 1
#define FALSE 0
#define TRUE 1

typedef struct stack
{
    int value;
    struct stack *next;
} stack;

stack *stack_init(int value);

stack *stack_push(stack **head, int value);

void stack_free(stack *s);

int stack_pop(stack **head);

typedef struct node
{
    int info;
    struct node *next;
} node;

typedef struct queue
{
    node *head;
    node *tail;
} queue;

node *create_node(int value);

queue *queue_init();

void queue_free(queue **q);

int enqueue(queue *q, int value);

int dequeue(queue *q);

int queue_is_empty(queue *q);

#endif