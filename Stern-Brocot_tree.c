#include "stdio.h"
#include "stdlib.h"
#define M_PI 3.14159265358979323846
#define M_FI 1.618033987
#define M_E 2.718281828459
#define SIZE 20
#define APPROX 23

typedef struct bin_tree
{
    int value;
    struct bin_tree *right;
    struct bin_tree *left;
} bin_tree;

bin_tree *get_node(int val) {
    bin_tree *head = malloc(sizeof(bin_tree));
    head->value = val;
    head->left = NULL;
    head->right = NULL;
    return head;
}

bin_tree *build_bin_search_tree(int *arr, size_t size)
{
    bin_tree *head = get_node(arr[0]);
    bin_tree *curr = head;
    for (size_t i = 1; i < size; i++)
    {
        while (1)        
        {
            if (arr[i] <= curr->value)
            {
                if (curr->left) curr = curr->left;
                else
                {
                    curr->left = get_node(arr[i]);
                    break;
                } 
            }
            else
            {
                if (curr->right) curr = curr->right;
                else
                {
                    curr->right = get_node(arr[i]);
                    break;
                }
            }
        }
        curr = head;
    }
    return head;
}

// size(head) == size(arr)!
void DFS(bin_tree *head, int *arr, size_t *ind)
{
    if (!head) return;
    DFS(head->left, arr, ind);
    arr[*ind] = head->value;
    ++(*ind);
    DFS(head->right, arr, ind);
    return;
}

void sort_bin_tree(int *arr, size_t size)
{
    bin_tree *head = build_bin_search_tree(arr, size);
    size_t ind = 0;
    DFS(head, arr, &ind);
}

void sort_test(void (*s_func)(int*, size_t))
{

    int a[SIZE];
    for (size_t i = 0; i < SIZE; i++)
    {
        a[i] = rand() % 100;
    }
    for (size_t i = 0; i < SIZE; i++)
    {
        printf("%d ", a[i]);
    }
    printf("\n");
    size_t size = SIZE;
    s_func(a, size);

    for (size_t i = 0; i < SIZE; i++)
    {
        printf("%d ", a[i]);
    }
    printf("\n");
}


// Stern-Brocot tree.
typedef struct sbt
{
    int num;
    int den;
    struct sbt *left;
    struct sbt *right;
} sbt;

typedef struct sbf
{
    int numL;
    int denL;
    int numR;
    int denR;
} sbf;

sbt *get_sb_subtree(int depth, sbf data)
{
    if (!depth) return NULL;
    sbt *head = malloc(sizeof(sbt));
    head->num = data.numL + data.numR;
    head->den = data.denL + data.denR;
    sbf left = {data.numL, data.denL, head->num, head->den};
    head->left = get_sb_subtree(depth - 1, left);
    sbf right = {head->num, head->den, data.numR, data.denR};
    head->right = get_sb_subtree(depth - 1, right);
    return head;
}

sbt *get_sbt(int depth)
{
    sbt *head = malloc(sizeof(sbt));
    head->den = 1;
    head->num = 1;
    sbf left = {0, 1, 1, 1};
    sbf right = {1, 1, 1, 0};
    head->left = get_sb_subtree(depth - 1, left);
    head->right = get_sb_subtree(depth - 1, right);
    return head;
}


unsigned char *sb_number(double r, sbt *tree)
{
    int size = APPROX + 1;
    unsigned char *result = malloc(size);
    unsigned char *pRes = result;
    
    sbt *curr = tree;
    while (curr)
    {
        double f = (double)curr->num / curr->den;
        if (pRes - result >= size)
        {
            size *= 2;
            result = realloc(result, size);
        }    
        if (r < f) 
        {
            curr = curr->left;
            *pRes = 'L';
            ++pRes;
        }
        else if (r > f)
        {
            curr = curr->right;
            *pRes = 'R';
            ++pRes;
        }
    }
    *pRes = '\0';
    result = realloc(result, pRes - result + 1);
    return result;
}

void test_sbt()
{
    sbt *tree = get_sbt(APPROX);
    printf("%s\n", sb_number(M_PI, tree));
    printf("%s\n", sb_number(M_FI, tree));
    printf("%s\n", sb_number(M_E, tree));
}
