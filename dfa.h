/*  VERTEX STATUS  */

#define INTER 0
#define FINAL 1
#define BROKEN 2

#define NO_ARC -1

#define TRUE 1
#define FALSE 0

#define INCORRECT_ARG (-1)


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


int add_arc_0(dfa *g, int a, int b);

int add_arc_1(dfa *g, int a, int b);

void dfa_free(dfa *a);

int dfa_check(dfa *a, int x);

void dfa_print(dfa *a, int n);


dfa *int_dfa(int x);

/* {01, 10} */
dfa *L1_init();

/* recognizes numbers = 2^(n-1) */
dfa *L2_init();


dfa *dfa_complement(dfa* a);

dfa *dfa_intersection(dfa *a1, dfa *a2);

dfa *dfa_union(dfa *a1, dfa *a2);

dfa *dfa_difference(dfa *a1, dfa *a2);