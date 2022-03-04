#include "stdio.h"
#include "stdlib.h"
#define M_PI 3.14159265358979323846
#define M_F 1.618033987
#define M_E 2.718281828459
#define SIZE 20
#define APPROX 23

typedef struct bin_tree
{
    int value;
    struct bin_tree *right;
    struct bin_tree *left;
} bin_tree;

bin_tree *build_bin_search_tree(int *arr, size_t size)
{
    bin_tree *head = malloc(sizeof(bin_tree));
    head->value = arr[0];
    head->left = NULL;
    head->right = NULL;
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
                    curr->left = malloc(sizeof(bin_tree));
                    curr = curr->left;
                    curr->value = arr[i];
                    curr->left = NULL;
                    curr->right = NULL;
                    curr = head;
                    break;
                } 
            }
            else if (arr[i] > curr->value)
            {
                if (curr->right) curr = curr->right;
                else
                {
                    curr->right = malloc(sizeof(bin_tree));
                    curr = curr->right;
                    curr->value = arr[i];
                    curr->left = NULL;
                    curr->right = NULL;
                    curr = head;
                    break;
                }
            }
        }
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


// Stern-Brokot tree.
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
    int isLeft;
} sbf;

sbt *get_sb_subtree(int depth, sbf data)
{
    if (!depth) return NULL;
    sbt *head = malloc(sizeof(sbt));
    head->num = data.numL + data.numR;
    head->den = data.denL + data.denR;
    sbf left = {data.numL, data.denL, head->num, head->den, 1};
    head->left = get_sb_subtree(depth - 1, left);
    sbf right = {head->num, head->den, data.numR, data.denR, 0};
    head->right = get_sb_subtree(depth - 1, right);
    return head;
}

sbt *get_sbt(int depth)
{
    sbt *head = malloc(sizeof(sbt));
    head->den = 1;
    head->num = 1;
    sbf left = {0, 1, 1, 1, 1};
    sbf right = {1, 1, 1, 0, 0};
    head->left = get_sb_subtree(depth - 1, left);
    head->right = get_sb_subtree(depth - 1, right);
    return head;
}


void sb_number(double r, size_t len)
{
    sbt *tree = get_sbt(len);
    sbt *curr = tree;
    while (--len)
    {
        double f = (double)curr->num / curr->den;
        if (r < f) 
        {
            curr = curr->left;
            printf("%s", "L");
        }
        else if (r > f)
        {
            curr = curr->right;
            printf("%s", "R");
        }
    }
    printf("\n%f ~ %d/%d\n", (double)curr->num / curr->den, curr->num, curr->den);
}

int main()
{
    //sort_test(sort_bin_tree);
    //sbt *tree = get_sbt(3);
    sb_number(M_PI, APPROX);
    sb_number(M_F, APPROX);
    sb_number(M_E, APPROX);
    return 0;
}