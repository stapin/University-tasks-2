#include "stdio.h"
#include "stdlib.h"
#include "structlib.h"
#include "Windows.h"

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#define SCAN_ERR -5
#define CLOSE_ERR -4
#define OPEN_ERR -3
#define NO_NODE -2
#define EMPTY_PTR -1
#define RED 1
#define BLACK 2

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
    if (g->ver_num < a || a < 0 || b < 0 || g->ver_num < b) return WRONG_INPUT;
    if (g->adj_list[a]->head == NULL)
    {
        g->adj_list[a]->head = create_node(b);
        return SUCCEED;
    }
    node *curr = g->adj_list[a]->head;
    while (curr->next) {
        if(curr->info == b) return SUCCEED;
        curr = curr->next;
    }
    if(curr->info != b) curr->next = create_node(b);
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
    if (g->ver_num < a || a < 0 || b < 0 || g->ver_num < b) return NO_SUCH_ELEMENT;
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
        g->adj_list[a]->head = curr->next;
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


int get_uncolored(int *colors, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (!colors[i]) return i;
    }
    return -1;
}

int is_biparted_DFS(graph *g, int *color, int ind)
{
    if (g == NULL) return EMPTY_PTR;
    if (g->ver_num <= 0) return FALSE;
    node *ver;
    int curr;
    color[ind] = 1;
    stack *vertices = stack_init(ind);
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

int is_biparted_BFS(graph *g, int **part)
{
    if (g == NULL) return EMPTY_PTR;
    if (g->ver_num <= 0) return FALSE;
    int *color = calloc(g->ver_num, sizeof(int));
    queue *vertices = queue_init();
    int curr;
    node *ver = NULL;
    while (1)
    {
        int col = get_uncolored(color, g->ver_num);
        if (col == -1) break;
        color[col] = 1;
        enqueue(vertices, col);
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
    }
    queue_free(&vertices);
    *part = color;
    return TRUE;
}

int is_biparted(graph *g, int **part)
{
    int *colors = calloc(g->ver_num, sizeof(int));
    for (int i = 0; i < g->ver_num; i++)
    {
        if (colors[i] == 0)
        {
            if (is_biparted_DFS(g, colors, i) != TRUE)
            {
                return FALSE;
            }
        }
    }
    *part = colors;
    return SUCCEED;
}


int topological_sort(graph* g, int** result)
{
    if (g == NULL) return EMPTY_PTR;
    if (g->ver_num == 0) return SUCCEED;
    int* res = malloc(sizeof(int) * g->ver_num);
    int ind = g->ver_num - 1;
    stack* s = stack_init(0);
    int curr;
    int* color = calloc(g->ver_num, sizeof(int));
    color[0] = RED;
    node* ver = NULL;
    while (s)
    {
        curr = stack_pop(&s);
        ver = g->adj_list[curr]->head;
        while (ver && color[ver->info])
        {
            if (color[ver->info] == RED)
            {
                free(color);
                free(res);
                *result = NULL;
                return FALSE;
            }
            ver = ver->next;
        }
        if (ver)
        {
            stack_push(&s, curr);
            stack_push(&s, ver->info);
            color[ver->info] = RED;
        }
        else
        {
            color[curr] = BLACK;
            res[ind--] = curr;
        }
    }
    *result = res;
    free(color);
    return SUCCEED;
}


int graph_write(graph *g, const char *path)
{
    if (g == NULL) return EMPTY_PTR;
    FILE *f = fopen(path, "w");
    if (f == NULL) return OPEN_ERR;
    node *curr = NULL;
    fprintf(f, "%d\n", g->ver_num);
    for (int i = 0; i < g->ver_num; i++)
    {
        curr = g->adj_list[i]->head;
        while (curr)
        {
            fprintf(f, "%d ", curr->info);
            curr = curr->next;
        }
        fputs("-1\n", f);
    }
    if (fclose(f) == EOF) return CLOSE_ERR;
    return SUCCEED;
}

int graph_read(graph **g, const char *path)
{
    FILE *f = fopen(path, "r");
    if (f == NULL) return OPEN_ERR;
    int vertex = 0;
    int ver_num = 0;
    if (fscanf(f, "%d\n", &ver_num) <= 0)
        return SCAN_ERR;
    graph *res = graph_init(ver_num);
    for (int i = 0; i < ver_num; i++)
    {
        while ( (fscanf(f, "%d", &vertex) == 1) && (vertex != -1) )
        {
            if (add_arc(res, i, vertex) <= 0) 
                return UNKNOWN_ERROR;
        }
    }
    *g = res;
    return SUCCEED;
}


graph *get_inverted(graph *g)
{
    graph *result = graph_init(g->ver_num);
    node *curr = NULL;
    for (int i = 0; i < g->ver_num; i++)
    {
        curr = g->adj_list[i]->head;
        while (curr)
        {
            add_arc(result, curr->info, i);
            curr = curr->next;
        }
    }
    return result;
}


void arr_print(int *arr, int size)
{
    if (arr == NULL) return;
    for (int i = 0; i < size; i++)
    {
        printf("%d ", arr[i]);
    }
    puts("\n"); 
}


void SCC2_dfs(graph *g, int start_ver, int *component, int *curr_comp)
{
    stack *s = stack_init(start_ver);
    stack *s2 = stack_init(start_ver);
    int time = 1;
    int *in_time = malloc(g->ver_num * sizeof(int));
    int *min_out = malloc(g->ver_num * sizeof(int));
    in_time[start_ver] = time;
    min_out[start_ver] = time++;
    int curr;
    node *adj_ver = NULL;
    while (s)
    {
        curr = stack_pop(&s);
        component[curr] = -1;
        adj_ver = g->adj_list[curr]->head;
        while (adj_ver && component[adj_ver->info] != 0)
        {
            if (component[adj_ver->info] == -1)
                min_out[curr] = min(min_out[curr], min_out[adj_ver->info]);
            adj_ver = adj_ver->next;
        }
        if (adj_ver == NULL)
        {
            if (min_out[curr] == in_time[curr])
            {
                int tmp;
                do
                {
                    tmp = stack_pop(&s2);
                    component[tmp] = *curr_comp;
                }
                while (tmp != curr);
                ++(*curr_comp);
            }
        }
        else
        {
            stack_push(&s, curr);
            stack_push(&s, adj_ver->info);
            in_time[adj_ver->info] = time;
            min_out[adj_ver->info] = time++;
            stack_push(&s2, adj_ver->info);
        }   
    }
}

int *SCC2_decomposition(graph *g)
{
    int *component = calloc(g->ver_num, sizeof(int));
    int curr_comp = 1;
    for (int i = 0; i < g->ver_num; i++)
    {
        if (!component[i])
            SCC2_dfs(g, i, component, &curr_comp);
    }
    return component;
}


void SCC1_dfs1(graph *g, int *visited, int *out_time, int *time, int start_ver)
{
    stack *s = stack_init(start_ver);
    node *adj_ver = NULL;
    int curr;
    while (s)
    {
        curr = stack_pop(&s);
        visited[curr] = 1;
        adj_ver = g->adj_list[curr]->head;
        while (adj_ver && visited[adj_ver->info] != 0)
        {
            adj_ver = adj_ver->next;
        }
        if (adj_ver == NULL)
        {
            out_time[(*time)++] = curr;
        }
        else
        {
            stack_push(&s, curr);
            stack_push(&s, adj_ver->info);
        }
    }
}

void SCC1_dfs2(graph *gi, int start_ver, int *visited, int *result, int *time)
{
    stack *s = stack_init(start_ver);
    node *adj_ver = NULL;
    int curr;
    while (s)
    {
        curr = stack_pop(&s);
        visited[curr] = 2;
        adj_ver = gi->adj_list[curr]->head;
        while (adj_ver && visited[adj_ver->info] == 2)
        {
            adj_ver = adj_ver->next;
        }
        if (adj_ver == NULL)
        {
            result[curr] = *time;
        }
        else
        {
            stack_push(&s, curr);
            stack_push(&s, adj_ver->info);
        }
    }
    ++(*time);
}

int *SCC1_decomposition(graph *g)
{
    int *visited = calloc(g->ver_num, sizeof(int));
    int *out_time = calloc(g->ver_num, sizeof(int));
    int time = 0;
    for (int i = 0; i < g->ver_num; i++)
    {
        if (!visited[i])
            SCC1_dfs1(g, visited, out_time, &time, i);
    }
    time = 1;
    graph *gi = get_inverted(g);
    int *result = malloc(sizeof(int) * g->ver_num);
    for (int i = g->ver_num - 1; i >=0; --i)
    {
        if (visited[out_time[i]] != 2)
            SCC1_dfs2(gi, out_time[i], visited, result, &time);
    }
    free(visited);
    free(out_time);
    graph_free(&gi);
    return result;
}

void SCC1_dfs2_topsort(graph *gi, int start_ver, int *visited, int *result, int *time, int *topsorted, int *ind)
{
    stack *s = stack_init(start_ver);
    node *adj_ver = NULL;
    int curr;
    while (s)
    {
        curr = stack_pop(&s);
        visited[curr] = 2;
        adj_ver = gi->adj_list[curr]->head;
        while (adj_ver && visited[adj_ver->info] == 2)
        {
            adj_ver = adj_ver->next;
        }
        if (adj_ver == NULL)
        {
            result[curr] = *time;
            topsorted[(*ind)++] = curr;
        }
        else
        {
            stack_push(&s, curr);
            stack_push(&s, adj_ver->info);
        }
    }
    ++(*time);
}

int *SCC1_decomposition_topsort(graph *g, int **topsorted)
{
    int *visited = calloc(g->ver_num, sizeof(int));
    int *out_time = calloc(g->ver_num, sizeof(int));
    int time = 0;
    for (int i = 0; i < g->ver_num; i++)
    {
        if (!visited[i])
            SCC1_dfs1(g, visited, out_time, &time, i);
    }
    time = 1;
    graph *gi = get_inverted(g);
    int *result = malloc(sizeof(int) * g->ver_num);
    int ind = 0;
    *topsorted = malloc(sizeof(int) * g->ver_num);
    for (int i = g->ver_num - 1; i >=0; --i)
    {
        if (visited[out_time[i]] != 2)
            SCC1_dfs2_topsort(gi, out_time[i], visited, result, &time, *topsorted, &ind);
    }
    free(visited);
    free(out_time);
    graph_free(&gi);
    return result;
}


// input: [1, -2, -1, 2] = [(x | !y) ^ (!x | y)]
// n variables = [1, ..., n, n+1 (= !1), n+2, ..., 2n (= !n)] 
int *SAT_2(int var_num, int *expr, int len)
{
    // len = number of paired brackets
    graph *g = graph_init(var_num * 2);
    for (int i = 0; i < 2 * len; i += 2)
    {
        // if expr[i] == 0 retrun ERROR;
        int a = expr[i] < 0 ? -expr[i] + var_num : expr[i];
        int b = expr[i + 1] < 0 ? -expr[i + 1] + var_num : expr[i + 1];
        int na = expr[i] < 0 ? -expr[i] : expr[i] + var_num;
        int nb = expr[i + 1] < 0 ? -expr[i + 1] : expr[i + 1] + var_num;
        add_arc(g, na - 1, b - 1);
        add_arc(g, nb - 1, a - 1);
    }
    int *topsorted = NULL;
    int *decomp = SCC1_decomposition_topsort(g, &topsorted);
    graph_free(&g);

    // checking for invalid expression.
    for (int i = 0; i < var_num; i++)
    {
        if (decomp[i] == decomp[i + var_num])
            return NULL;
    }
    int *result = malloc(sizeof(int) * var_num);
    for (int i = 0; i < var_num; i++) result[i] = -1; // not visited
    
    for (int i = var_num * 2 - 1; i >= 0; i--)
    {
        int ind = topsorted[i];
        int is_inverse = 0;
        if (topsorted[i] >= var_num)
        {
            is_inverse = 1;
            ind = topsorted[i] - var_num;
        }
        if (result[ind] == -1)
            result[ind] = is_inverse ? 0 : 1;
    }
    return result;
}


void test_graph()
{
    graph *g = graph_init(5);
    add_arc(g, 1, 2);
    add_arc(g, 1, 4);
    add_edge(g, 1, 3);
    graph_print(g);
    //del_edge(g, 1, 3);
    del_arc(g, 1, 2);
    graph_print(g);
    graph_free(&g);
    graph_print(g);
}

void test_graph2()
{
    graph *g = graph_init(5);
    add_arc(g, 0, 1);
    add_arc(g, 1, 2);
    add_arc(g, 2, 3);
    add_arc(g, 3, 0);
    //add_arc(g, 0, 2);
    //add_edge(g, 4, 1);
    graph_print(g);
    int *parts = NULL;
    //printf("%d\n", is_biparted_DFS(g, &parts));
    arr_print(parts, g->ver_num);
    printf("%d\n", is_biparted_BFS(g, &parts));
    arr_print(parts, g->ver_num);
    
}

void test_is_biparted()
{
    graph *g = graph_init(5);
    add_arc(g, 0, 1);
    add_arc(g, 1, 2);
    add_arc(g, 2, 3);
    add_arc(g, 3, 0);
    //add_arc(g, 0, 2);
    //add_edge(g, 4, 1);
    graph_print(g);
    int *parts = NULL;
    printf("%d\n", is_biparted(g, &parts));
    arr_print(parts, g->ver_num);
}

void test_top_sort()
{
    graph *g = graph_init(5);
    add_arc(g, 4, 1);
    add_arc(g, 1, 2);
    add_arc(g, 4, 3);
    add_arc(g, 3, 2);
    add_arc(g, 0, 4);
    graph_print(g);
    
    int *result = NULL;
    if(topological_sort(g, &result))
    {
        for (int i = 0; i < g->ver_num; i++)
        {
            printf("%d ", result[i]);
        }
    }
    else puts("0");
}

void file_test()
{
    graph *g = graph_init(5);
    add_arc(g, 4, 1);
    add_arc(g, 1, 2);
    add_arc(g, 4, 3);
    add_arc(g, 3, 2);
    add_arc(g, 0, 4);
    //add_edge(g, 2, 4);
    //add_edge(g, 2, 0);
    graph_print(g);    
    printf("write status: %d\n", graph_write(g, "gr.txt"));
    graph *res = NULL;
    printf("read status: %d\n", graph_read(&res, "gr.txt"));
    graph_print(res);
    del_arc(res, 4, 1);
    graph_print(res);
    printf("write status: %d\n", graph_write(res, "gr1.txt"));
}

void write_graph_test()
{
    graph *g = graph_init(6);
    add_arc(g, 1, 5);
    add_arc(g, 5, 0);
    add_arc(g, 5, 2);
    add_arc(g, 2, 1);
    add_arc(g, 4, 2);
    graph_write(g, "gr1.txt");
}

void SCC_test()
{
    graph *g = graph_init(4);
    add_arc(g, 0, 1);
    add_edge(g, 1, 2);
    add_arc(g, 1, 3);
    add_arc(g, 3, 2);
    int *components = SCC1_decomposition(g);
    arr_print(components, 4);
}

void SCC1_test()
{
    graph *g = graph_init(6);
    add_arc(g, 1, 5);
    add_arc(g, 5, 0);
    add_arc(g, 5, 2);
    add_arc(g, 2, 1);
    add_arc(g, 4, 2);
    int *components = SCC1_decomposition(g);
    arr_print(components, 6);
}

void SCC1_topsort_test()
{
    graph *g = graph_init(6);
    add_arc(g, 1, 5);
    add_arc(g, 5, 0);
    add_arc(g, 5, 2);
    add_arc(g, 2, 1);
    add_arc(g, 4, 2);
    int *topsorted = NULL;
    int *components = SCC1_decomposition_topsort(g, &topsorted);
    arr_print(components, 6);
    arr_print(topsorted, 6);
}

void SCC2_test()
{
    graph *g = graph_init(6);
    add_arc(g, 1, 5);
    add_arc(g, 5, 0);
    add_arc(g, 5, 2);
    add_arc(g, 2, 1);
    add_arc(g, 4, 2);
    int *components = SCC2_decomposition(g);
    arr_print(components, 6);
}

void SCC2_test2()
{
    graph *g = graph_init(6);
    add_edge(g, 4, 3);
    add_edge(g, 3, 2);
    add_arc(g, 0, 5);
    add_arc(g, 5, 1);
    add_arc(g, 1, 0);
    int *components = SCC2_decomposition(g);
    arr_print(components, 6);
}

void SAT_2_test()
{
    int expr[] = {1, 2, -2, -1};//{1, -2, 2, 3, -2, -3, -1, 3, 2, -1};
    int *res = SAT_2(2, expr, 2);
    arr_print(res, 2);
}
