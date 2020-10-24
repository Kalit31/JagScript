#ifndef NARYTREE
#define NARYTREE

#include "token.h"
#define NUM_NON_TERMINALS 35
#define NUM_RULES 60
#define MAX_DIMENSIONS 10

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
};

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
} NonTerminal;

typedef enum Type_Var
{
    PRIMITIVE,
    RECTANGULAR_ARRAY,
    JAGGED_ARRAY
} Type_Var;

typedef enum PrimType
{
    PRIM_INTEGER,
    PRIM_REAL,
    PRIM_BOOLEAN
} PrimType;

typedef struct RectangularArray
{
    int currDimensions;                //2
    int dimenArray[MAX_DIMENSIONS][2]; //(2, 5), (3, 6)
} RectangularArray;

typedef struct TwoDJaggedArray
{
    int *size; // store size of each 1D subbarray
    int x, y;  // store state of array while declaration
    int **r2;  //(3, 6, 2, 4, 1, 5)
} TwoDJaggedArray;

typedef struct ThreeDJaggedArray
{
    int **size, *sizeR2; // store size of each subbarray
    int x, y, z;         // store state of array while declaration
    int ***dimen;        //3 [ 5, 3, 5] , 2 [ 3, 5], 3 [ 5, 4, 3] ,4 [ 2, 5, 4, 4]
} ThreeDJaggedArray;

typedef union JaggedArrayType
{
    TwoDJaggedArray twod_array;
    ThreeDJaggedArray threed_array;
} JaggedArrayType;

typedef struct JaggedArray
{
    int is2D;
    //(4, 7)
    int r1Low;
    int r1High;
    JaggedArrayType type;
} JaggedArray;

typedef union TypeExpression
{
    PrimType primitiveType;
    RectangularArray rectType;
    JaggedArray jaggedType;
} TypeExpression;

typedef TokenName Terminal;

struct treenode
{
    // Tells whether the node is a leaf(terminal) or non-leaf(non-terminal)
    int isLeaf;
    // NonTerminal Enum ID
    NonTerminal nonterminal;
    // Terminal Enum ID
    Terminal terminal;
    // Rule No.
    int ruleNo;
    // Variable type -> { PRIM, RECT, JAGGED }. Serves as the tag for union TypeExpression
    Type_Var type;
    TypeExpression expression;
    // Token containing lexeme, line number
    Token *tok;
    struct treenode *child;
    struct treenode *parent;
    struct treenode *next; //next node in the same level
};

typedef struct treenode TreeNode;

// struct parseTree
// {
//     TreeNode *root;
// };

// typedef struct parseTree ParseTree;

TreeNode *initialiseParseTree();
TreeNode *createLeafNode(Terminal terminal);
TreeNode *createNonLeafNode(NonTerminal nonterminal);
TreeNode *nthSibling(TreeNode *child, int n);
TreeNode *nthChild(TreeNode *root, int n);

#endif