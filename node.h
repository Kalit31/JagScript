#ifndef GRAMMER_NODE
#define GRAMMER_NODE

struct node
{
    char *element;
    struct node *nxt;
};

typedef struct node Node;

#endif