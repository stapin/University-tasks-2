#include "stdlib.h"
#include "stdio.h"
#include "structlib.h"


stack *stack_init(int value)
{
    stack *result = malloc(sizeof(stack));
    result->value = value;
    result->next = NULL;
    return result;
}

stack *stack_push(stack **head, int value)
{
    if (!(*head)) 
    {
        *head = stack_init(value);
        return NULL;
    }
    stack *curr = malloc(sizeof(stack));
    curr->value = value;
    curr->next = *head;
    *head = curr;
    return NULL;
}

void stack_free(stack *s)
{
    stack *curr = s;
    while (curr->next)
    {
        s = curr;
        curr = curr->next;
        free(s);
    }
}

int stack_pop(stack **head)
{
    if (!(*head)) return STACK_EMPTY;
    if ((*head)->next == NULL)
    {
        int res = (*head)->value;
        stack *tmp = *head;
        stack_free(tmp);
        *head = NULL;
        return res;
    }
    stack *tmp = *head;
    int res = (*head)->value;
    *head = (*head)->next;
    free(tmp);
    return res;
}


// _______________________________________



node *create_node(int value)
{
    node *result = malloc(sizeof(node));
    result->info = value;
    result->next = NULL;
    return result;
}

queue *queue_init()
{
    queue *result = malloc(sizeof(queue));
    result->head = NULL;
    result->tail = NULL;
    return result;
}

void queue_free(queue **q)
{
    node *curr = (*q)->head;
    node *tmp = NULL;
    while (curr)
    {
        tmp = curr;
        curr = curr->next;
        free(tmp);
    }
    free(*q);
    *q = NULL;
    return;
}

int enqueue(queue *q, int value)
{
    if (!q) return WRONG_INPUT;
    node *newNode = create_node(value);
    if (q->tail)
    {
        q->tail->next = newNode;
    }
    q->tail = newNode;
    if (!q->head)
        q->head = newNode;
    return SUCCEED;
}

int dequeue(queue *q)
{
    if (!q) return WRONG_INPUT;
    if (!q->head) return WRONG_INPUT;
    int result = q->head->info;
    node *tmp = q->head;
    q->head = q->head->next;
    free(tmp);
    if (!q->head) q->tail = NULL;
    return result;
}

int queue_is_empty(queue *q)
{
    if (q->head) return FALSE;
    return TRUE;
}