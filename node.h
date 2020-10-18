#include <stdio.h>
#include <stdlib.h>

struct node
{
    char *block;
    struct node *next;
};

typedef struct node Node;