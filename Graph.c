#include "stdio.h"
#include "stdlib.h"



typedef struct adj_list
{
    int name;
    struct adj_list *next;
} adj_list;

typedef struct node
{
    int name;
    adj_list *adjList;
    struct node *next;
} node;

typedef struct graph
{
    int count;
    node *head;

} graph;

graph *get_graph(size_t n)
{
    graph *result = malloc(sizeof(graph));
    result->count = n;
    result->head = NULL;
    if (n == 0) return result;
    result->head = malloc(sizeof(node));
    node *curr = result->head;
    size_t size = --n;
    while (n--)
    {
        curr->name = size - n;
        curr->next = NULL;
        curr = curr->next; 
    }
    
}



int main()
{

    return 0;
}