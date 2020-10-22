#ifndef NARYTREE
#define NARYTREE

#include "token.h"
#define NUM_NON_TERMINALS 36
#define NUM_RULES 62

static char *NONTERMINALS[NUM_NON_TERMINALS] = {
    "s",
    "list_of_statements",
    "declaration_statements",
    "assignment_statements",
    "declaration",
    "primitive_decl",
    "rect_array_decl",
    "jagged_array_decl",
    "declare_vars",
    "primitive_type",
    "rect_array",
    "declare_jagged",
    "var_name_list",
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
    rect_array_decl,
    jagged_array_decl,
    declare_vars,
    primitive_type,
    rect_array,
    declare_jagged,
    var_name_list,
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
    boolean_term,
    boolean_factor
} NonTerminal;

typedef struct treenode TreeNode;

struct nonleafnode
{
    NonTerminal nonterminal;
    int ruleNo;
    TreeNode *child;
};
typedef struct nonleafnode NonLeafNode;

typedef TokenName Terminal;

struct leafnode
{
    Terminal terminal;
    Token *tok;
};

typedef struct leafnode LeafNode;

typedef union
{
    LeafNode leafNode;
    NonLeafNode nonLeafNode;
} NodeType;

struct treenode
{
    int isLeaf;
    NodeType nodeType;
    struct treenode *parent;
    struct treenode *next; //next node in the same level
};

// struct parseTree
// {
//     TreeNode *root;
// };

// typedef struct parseTree ParseTree;

TreeNode *initialiseParseTree();
TreeNode *createLeafNode(Terminal terminal);
TreeNode *createNonLeafNode(NonTerminal nonterminal);
TreeNode *createNode(int isLeafNode, int enumid, TreeNode *parent);
void addRuleToParseTree(TreeNode *n);

#endif