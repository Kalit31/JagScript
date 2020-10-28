/* Group 3
Ashrya Agrawal          2018A7PS0210P
Kalit Naresh Inani      2018A7PS0207P
Prajwal Gupta           2018A7PS0231P */



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
