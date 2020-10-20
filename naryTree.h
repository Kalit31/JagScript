#ifndef NARYTREE
#define NARYTREE

#include "token.h"
#define NUM_NON_TERMINALS 37

static char *NONTERMINALS[NUM_NON_TERMINALS] = {
    "s",
    "list_of_statements",
    "declaration_statements",
    "assignment_statements",
    "declaration",
    "primitive_decl",
    "jagged_array_decl",
    "declare_vars",
    "declaration_type",
    "declare_jagged",
    "var_name_list",
    "primitive",
    "rect_array",
    "array_dim",
    "idx",
    "declare_twod_jagged",
    "declare_threed_jagged",
    "twod_jagged_statements",
    "twod_jagged_statement",
    "twod_values",
    "threed_jagged_statements",
    "threed_jagged_statement",
    "threed_values",
    "list_num",
    "assignment",
    "lhs",
    "rhs",
    "array_ele",
    "list",
    "arithmetic_expr",
    "boolean_expr",
    "arithmetic_term",
    "op1",
    "op2",
    "boolean_expr",
    "boolean_term",
    "boolean_factor"};

typedef enum NonTerminal
{
    s,
    list_of_statements,
    declaration_statements,
    assignment_statements,
    declaration,
    primitive_decl,
    jagged_array_decl,
    declare_vars,
    declaration_type,
    declare_jagged,
    var_name_list,
    primitive,
    rect_array,
    array_dim,
    idx,
    declare_twod_jagged,
    declare_threed_jagged,
    twod_jagged_statements,
    twod_jagged_statement,
    twod_values,
    threed_jagged_statements,
    threed_jagged_statement,
    threed_values,
    list_num,
    assignment,
    lhs,
    rhs,
    array_ele,
    list,
    arithmetic_expr,
    boolean_expr,
    arithmetic_term,
    op1,
    op2,
    boolean_expr,
    boolean_term,
    boolean_factor
} NonTerminal;

typedef TokenName Terminal;

typedef union
{
    Terminal terminal;
    NonTerminal nonterminal;
} NodeType;

struct treenodeinfo
{
    int terminal;
    NodeType type;
};
typedef struct treenodeinfo TreeNodeInfo;

struct treenode
{
    TreeNodeInfo *info;
    struct treenode *child; // pointer to the leftmost child;
    struct treenode *next;  // pointer to the next node in the same level
};

typedef struct treenode TreeNode;

struct parseTree
{
    TreeNode *root;
};

typedef struct parseTree ParseTree;

#endif