#ifndef GRAMMER_NODE
#define GRAMMER_NODE

struct node
{
    char *element;
    int ruleSize; // only applicable for the lhs of a rule
    struct node *nxt;
};

typedef struct node Node;

#endif