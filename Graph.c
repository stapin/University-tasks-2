#include "stdio.h"
#include "stdlib.h"
#include "structlib.h"

#define NO_NODE (-2)
#define EMPTY_PTR (-1)


typedef struct list
{
    node *head;
} list;

typedef struct graph
{
    int ver_num;
    list **adj_list;
} graph;

graph *graph_init(int n)
{
    graph *result = malloc(sizeof(graph));
    result->ver_num = n;
    result->adj_list = malloc(n*sizeof(list));
    for (int i = 0; i < n; i++)
    {
        result->adj_list[i] = (list *)malloc(sizeof(list));
        result->adj_list[i]->head = NULL;
    }
    return result;
}

int add_arc(graph *g, int a, int b)
{
    if (!g) return EMPTY_PTR;
    if (g->ver_num < a) return WRONG_INPUT;
    if (g->adj_list[a]->head == NULL)
    {
        g->adj_list[a]->head = create_node(b);
        return SUCCEED;
    }
    node *curr = g->adj_list[a]->head;
    while (curr->next) curr = curr->next;
    curr->next = create_node(b);
    return SUCCEED;
}

int add_edge(graph *g, int a, int b)
{
    int err_code = add_arc(g, a, b);
    if (err_code != SUCCEED) return err_code;
    err_code = add_arc(g, b, a);
    return err_code;
}

int del_arc(graph *g, int a, int b)
{
    if (g->ver_num < a) return NO_SUCH_ELEMENT;
    node *curr = g->adj_list[a]->head;
    node *prev = NULL;
    while (curr && (curr->info != b))
    {
        prev = curr;
        curr = curr->next;
    }
    if (!curr) return NO_SUCH_ELEMENT;
    if (!prev) 
    {
        g->adj_list[a]->head = NULL;
        free(curr);
        return SUCCEED;
    }
    prev->next = curr->next;
    free(curr);
    return SUCCEED;
}

int del_edge(graph *g, int a, int b)
{
    int err_code = del_arc(g, a, b);
    if (err_code != SUCCEED) return err_code;
    err_code = del_arc(g, b, a);
    return err_code;
}

int graph_free(graph **g)
{
    if (g == NULL) return EMPTY_PTR;
    if (*g == NULL) return SUCCEED;
    list *curr = NULL;
    node *ver = NULL;
    node *tmp = NULL;
    for (size_t i = 0; i < (size_t)(*g)->ver_num; i++)
    {
        curr = (*g)->adj_list[i];
        ver = curr->head;
        while (ver)
        {
            tmp = ver;
            ver = ver->next;
            free(tmp);
        }
        free(curr);
    }
    free(*g);
    *g = NULL;
    return SUCCEED;
}

void graph_print(graph *g)
{
    if (g == NULL)
    {
        puts("NULL was given. [graph_print]");
        return;
    }
    node *curr = NULL;
    for (size_t i = 0; i < (size_t)g->ver_num; i++)
    {
        printf("%lld: ", i);
        curr = g->adj_list[i]->head;
        while (curr)
        {
            printf("%d->", curr->info);
            curr = curr->next;
        }
        puts("NULL");
    }
}

int is_biparted_DFS(graph *g)
{
    if (g == NULL) return EMPTY_PTR;
    if (g->ver_num <= 0) return FALSE;
    int *color = calloc(g->ver_num, sizeof(int));
    color[0] = 1;
    stack *vertices = stack_init(0);
    node *ver;
    int curr;
    while (vertices)
    {
        curr = stack_pop(&vertices);
        ver = g->adj_list[curr]->head;
        while (ver && color[ver->info])
        {
            if (color[ver->info] == color[curr])
                return FALSE;
            ver = ver->next;
        }
        if (ver)
        {
            color[ver->info] = -color[curr];
            stack_push(&vertices, curr);
            stack_push(&vertices, ver->info);
        }
    }
    return TRUE;
}

int is_biparted_BFS(graph *g)
{
    if (g == NULL) return EMPTY_PTR;
    if (g->ver_num <= 0) return FALSE;
    int *color = calloc(g->ver_num, sizeof(int));
    queue *vertices = queue_init();
    enqueue(vertices, 0);
    color[0] = 1;
    int curr;
    node *ver = NULL;
    while (!queue_is_empty(vertices))
    {
        curr = dequeue(vertices);
        ver = g->adj_list[curr]->head;
        while (ver)
        {
            if (color[ver->info] == color[curr])
            {
                queue_free(&vertices);
                return FALSE;
            }
            if (!color[ver->info])
            {
                color[ver->info] = -color[curr];
                enqueue(vertices, ver->info);
            }
            ver = ver->next;
        }
    }
    queue_free(&vertices);
    return TRUE;
}

void test()
{
    graph *g = graph_init(5);
    add_arc(g, 1, 2);
    add_edge(g, 1, 3);
    graph_print(g);
    del_edge(g, 1, 3);
    del_arc(g, 1, 3);
    graph_print(g);
    graph_free(&g);
    graph_print(g);
}

void test2()
{
    graph *g = graph_init(5);
    add_arc(g, 0, 1);
    add_arc(g, 1, 2);
    add_arc(g, 2, 3);
    add_arc(g, 3, 0);
    //add_arc(g, 0, 2);
    add_edge(g, 4, 1);
    graph_print(g);
    printf("%d\n", is_biparted_DFS(g));
    printf("%d", is_biparted_BFS(g));
}

int main()
{
    test2();
    return 0;
}