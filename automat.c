#include "stdio.h"
#include "stdlib.h"

/* _________ VERTEX STATUS _________ */

#define INIT 0
#define INTER 1
#define FINAL 2
#define INIT_FINAL 3
#define BROKEN 4

/* __________________________________ */

#define NO_ARC -1


#ifndef STRUCTLIB_H

#define STACK_EMPTY (-5)
#define QUEUE_EMPTY (-4)
#define NO_SUCH_ELEMENT (-3)
#define INCORRECT_ARG (-2)
#define WRONG_INPUT (-1)
#define UNKNOWN_ERROR 0
#define SUCCEED 1
#define FALSE 0
#define TRUE 1
#endif


typedef struct state
{
    int ver_status;
    int adj_0;
    int adj_1;
} state;


typedef struct dfa
{
    int state_num;
    state **adj_list;
} dfa;

int add_arc_0(dfa *g, int a, int b)
{
    if (!g) return 0;
    if (g->state_num < a || a < 0 || b < 0 || g->state_num < b) return WRONG_INPUT;
    g->adj_list[a]->adj_0 = b;
    return SUCCEED;
}

int add_arc_1(dfa *g, int a, int b)
{
    if (!g) return 0;
    if (g->state_num < a || a < 0 || b < 0 || g->state_num < b) return WRONG_INPUT;
    g->adj_list[a]->adj_1 = b;
    return SUCCEED;
}

void dfa_free(dfa *a)
{
    for (int i = 0; i < a->state_num; i++)
    {
        free(a->adj_list[i]);
    }
    free(a->adj_list);
    free(a);
    return;
}

int dfa_check(dfa *a, int x)
{
    int cur_state = INIT;
    int cur_status = INIT;
    while (x && cur_status != BROKEN)
    {
        if (x & 1)
        {
            cur_state = a->adj_list[cur_state]->adj_1;
            cur_status = a->adj_list[cur_state]->ver_status;
        }
        else
        {
            cur_state = a->adj_list[cur_state]->adj_0;
            cur_status = a->adj_list[cur_state]->ver_status;
        }
        x >>= 1;
    }
    if (cur_status == FINAL || cur_status == INIT_FINAL)
        return TRUE;
    return FALSE;
}


void dfa_print(dfa *a, int n)
{
    printf("[");
    for (int i = 0; i < n; i++)
    {
        if (dfa_check(a, i))
            printf("%d ", i);
    }
    printf("]\n");
}


/* ______________ DFA BLOCK ________________ */

dfa *int_dfa(int x)
{
    dfa *res = malloc(sizeof(dfa));
    int n = x;
    res->state_num = 1;
    while (x)
    {
        x >>= 1;
        ++res->state_num;
    }
    res->state_num += 1;
    res->adj_list = malloc(sizeof(state*) * res->state_num);
    for (int i = 0; i < res->state_num; i++)
    {
        res->adj_list[i] = malloc(sizeof(state));
        if (n & (1 << i)) 
        {
            add_arc_1(res, i, i + 1);
            add_arc_0(res, i, res->state_num - 1);
        }
        else
        {
            add_arc_0(res, i, i + 1);
            add_arc_1(res, i, res->state_num - 1);
        }
        res->adj_list[i]->ver_status = INTER;
    }
    res->adj_list[0]->ver_status = INTER;
    res->adj_list[res->state_num - 2]->ver_status = FINAL;
    res->adj_list[res->state_num - 1]->ver_status = BROKEN;
    res->adj_list[res->state_num - 1]->adj_0 = res->state_num - 1;
    res->adj_list[res->state_num - 1]->adj_1 = res->state_num - 1;
    
    return res;
}

/* {01, 10} */
dfa *L1_init()
{
    dfa *res = malloc(sizeof(dfa));
    res->state_num = 4;
    res->adj_list = malloc(sizeof(state*) * res->state_num);

    for (int i = 0; i < res->state_num; i++)
    {
        res->adj_list[i] = malloc(sizeof(state));
        res->adj_list[i]->adj_0 = NO_ARC;
        res->adj_list[i]->adj_1 = NO_ARC;
    }
    
    // add status
    res->adj_list[0]->ver_status = FINAL;
    res->adj_list[1]->ver_status = INTER;
    res->adj_list[2]->ver_status = INTER;
    res->adj_list[3]->ver_status = BROKEN;

    // add arcs
    add_arc_0(res, 0, 1);
    add_arc_1(res, 0, 2);
    add_arc_0(res, 1, 3);
    add_arc_1(res, 1, 0);
    add_arc_0(res, 2, 0); 
    add_arc_1(res, 2, 3);
    add_arc_0(res, 3, 3); 
    add_arc_1(res, 3, 3); 

    return res;
}

/* recognizes numbers = 2^(n-1) */
dfa *L2_init()
{
    dfa *res = malloc(sizeof(dfa));
    res->state_num = 2;
    res->adj_list = malloc(sizeof(state*) * res->state_num);

    for (int i = 0; i < res->state_num; i++)
    {
        res->adj_list[i] = malloc(sizeof(state));
        res->adj_list[i]->adj_0 = NO_ARC;
        res->adj_list[i]->adj_1 = NO_ARC;
    }
    
    // add status
    res->adj_list[0]->ver_status = FINAL;
    res->adj_list[1]->ver_status = BROKEN;

    // add arcs
    add_arc_1(res, 0, 0);
    add_arc_0(res, 0, 1);
    add_arc_0(res, 1, 1);
    add_arc_1(res, 1, 1); 

    return res;
}


/* ______________ OPERATIONS ________________ */

dfa *dfa_complement(dfa* a)
{
    dfa *res = malloc(sizeof(dfa));
    res->state_num = a->state_num;
    res->adj_list = malloc(sizeof(state*) * res->state_num);
    state *curr, *curr2;
    for (int i = 0; i < res->state_num; i++)
    {
        res->adj_list[i] = malloc(sizeof(state));
        curr = res->adj_list[i];
        curr2 = a->adj_list[i];
        curr->adj_0 = curr2->adj_0;
        curr->adj_1 = curr2->adj_1;
        if (curr2->ver_status == INTER || curr2->ver_status == BROKEN)
            curr->ver_status = FINAL;
        else if (curr2->ver_status == FINAL && curr2->adj_0 == i && curr2->adj_1 == i)
            curr->ver_status = BROKEN;
        else if (curr2->ver_status == INIT)
            curr->ver_status = INIT_FINAL;
        else if (curr2->ver_status == INIT_FINAL)
            curr->ver_status = INIT;
        else
            curr->ver_status = INTER;
         
    }
    return res;
}

dfa *dfa_intersection(dfa *a1, dfa *a2)
{
    dfa *res = malloc(sizeof(dfa));
    res->state_num = a1->state_num * a2->state_num;
    res->adj_list = malloc(sizeof(state*) * res->state_num);
    state **j1 = a1->adj_list;
    state **j2 = a2->adj_list;
    for (int i = 0; i < a1->state_num; i++)
    {
        int n = a2->state_num;
        for (int j = 0; j < a2->state_num; j++)
        {
            res->adj_list[i * n + j] = malloc(sizeof(state));

            if (j1[i]->ver_status == FINAL && j2[j]->ver_status == FINAL)
            {
                res->adj_list[i * n + j]->ver_status = FINAL;
            }
            else if (j1[i]->ver_status == BROKEN || j2[j]->ver_status == BROKEN)
            {
                res->adj_list[i * n + j]->ver_status = BROKEN;
            }
            else
            {
                res->adj_list[i * n + j]->ver_status = INTER;
            }

            add_arc_0(res, i * n + j, j1[i]->adj_0 * n + j2[j]->adj_0);
            add_arc_1(res, i * n + j, j1[i]->adj_1 * n + j2[j]->adj_1);
        }
    }
    return res;
}

dfa *dfa_union(dfa *a1, dfa *a2)
{
    dfa *res = malloc(sizeof(dfa));
    res->state_num = a1->state_num * a2->state_num;
    res->adj_list = malloc(sizeof(state*) * res->state_num);
    state **j1 = a1->adj_list;
    state **j2 = a2->adj_list;
    for (int i = 0; i < a1->state_num; i++)
    {
        int n = a2->state_num;
        for (int j = 0; j < a2->state_num; j++)
        {
            res->adj_list[i * n + j] = malloc(sizeof(state));

            if (j1[i]->ver_status == FINAL || j2[j]->ver_status == FINAL)
            {
                res->adj_list[i * n + j]->ver_status = FINAL;
            }
            else if (j1[i]->ver_status == BROKEN && j2[j]->ver_status == BROKEN)
            {
                res->adj_list[i * n + j]->ver_status = BROKEN;
            }
            else
            {
                res->adj_list[i * n + j]->ver_status = INTER;
            }

            add_arc_0(res, i * n + j, j1[i]->adj_0 * n + j2[j]->adj_0);
            add_arc_1(res, i * n + j, j1[i]->adj_1 * n + j2[j]->adj_1);
        }
    }
    return res;
}

dfa *dfa_difference(dfa *a1, dfa *a2)
{
    dfa *res = malloc(sizeof(dfa));
    res->state_num = a1->state_num * a2->state_num;
    res->adj_list = malloc(sizeof(state*) * res->state_num);
    state **j1 = a1->adj_list;
    state **j2 = a2->adj_list;
    for (int i = 0; i < a1->state_num; i++)
    {
        int n = a2->state_num;
        for (int j = 0; j < a2->state_num; j++)
        {
            res->adj_list[i * n + j] = malloc(sizeof(state));

            if ((j1[i]->ver_status == FINAL) ^ (j2[j]->ver_status == FINAL))
            {
                res->adj_list[i * n + j]->ver_status = FINAL;
            }
            else if ((j1[i]->ver_status == BROKEN) ^ (j2[j]->ver_status == BROKEN))
            {
                res->adj_list[i * n + j]->ver_status = BROKEN;
            }
            else
            {
                res->adj_list[i * n + j]->ver_status = INTER;
            }

            add_arc_0(res, i * n + j, j1[i]->adj_0 * n + j2[j]->adj_0);
            add_arc_1(res, i * n + j, j1[i]->adj_1 * n + j2[j]->adj_1);
        }
    }
    return res;
}


/* _________ TEST BLOCK __________ */

void dfa_test()
{
    dfa *l2 = L2_init();
    dfa_print(l2, 100);

    dfa *idfa = int_dfa(7);
    dfa_print(idfa, 20);

    dfa *cdfa = dfa_complement(idfa);
    dfa_print(cdfa, 20);

    dfa *ccdfa = dfa_complement(cdfa);
    dfa_print(ccdfa, 20);

    dfa *l1 = L1_init();
    dfa_print(l1, 20);

    dfa *l1_comp = dfa_complement(l1);
    dfa_print(l1_comp, 20);

    dfa *inter = dfa_intersection(l2, idfa);
    dfa_print(inter, 20);

    dfa *uni = dfa_union(l2, idfa);
    dfa_print(uni, 20);

    dfa *diff = dfa_difference(idfa, l2);
    dfa_print(diff, 20);
}


int main()
{
    dfa_test();
    return 0;
}