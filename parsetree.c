#include <stdio.h>
#include <stdlib.h>
#include "parsetree.h"
#include "typeExpression.h"
#include "naryTree.c"
#include "stack.c"

static int currentTableEntry = 0;

int findTerminal(char *s)
{
    for (int i = 0; i < NUM_TOKENS; i++)
    {
        if (strcmp(TOKENS[i], s) == 0)
        {
            return i;
        }
    }

    printf("COULD NOT FIND TERMINAL\n");
    return -1;
}

int findNonTerminal(char *s)
{
    for (int i = 0; i < NUM_NON_TERMINALS; i++)
    {
        if (strcmp(NONTERMINALS[i], s) == 0)
        {
            return i;
        }
    }
    printf("COULD NOT FIND NON TERMINAL\n");
    return -1;
}

int findRuleIndex(int nonTerminalInd, Node *g)
{
    for (int i = 0; i < NUM_RULES; i++)
    {
        if (strcmp(g[i].element, NONTERMINALS[nonTerminalInd]) == 0)
        {
            return i;
        }
    }
    return -1;
}

int findLastRuleIndex(int nonTerminalInd, Node *g)
{
    int lastRuleInd = -1;
    for (int i = 0; i < NUM_RULES; i++)
    {
        if (strcmp(g[i].element, NONTERMINALS[nonTerminalInd]) == 0)
        {
            lastRuleInd = i;
        }
    }
    return lastRuleInd;
}

void deleteChildren(TreeNode *node)
{
    if (node == NULL)
    {
        return;
    }
    if (node->isLeaf)
    {
        return;
    }
    TreeNode *child = node->nodeType.nonLeafNode.child;
    while (child != NULL)
    {
        TreeNode *prev = child;
        child = child->next;
        deleteChildren(prev);
        free(prev);
    }
    node->nodeType.nonLeafNode.child = NULL;
    return;
}

Token *globalTokenPtr;

TreeNode *buildNew(TreeNode *t, Node *g)
{
    if (globalTokenPtr == NULL)
    {
        return NULL;
    }

    if (t->isLeaf)
    {
        int tokenId = t->nodeType.leafNode.terminal;
        int tokenIdTokenStream = globalTokenPtr->tokenName;
        if (tokenId == tokenIdTokenStream)
        {
            t->nodeType.leafNode.tok = globalTokenPtr;
            globalTokenPtr = globalTokenPtr->next;
            return t;
        }
        else
        {

            return NULL;
        }
    }

    Token *beforeApplyingRule = globalTokenPtr;
    int nonTerminalID = t->nodeType.nonLeafNode.nonterminal;
    int firstRuleNo = findRuleIndex(nonTerminalID, g);
    int lastRuleNo = findLastRuleIndex(nonTerminalID, g);

    for (int rule = firstRuleNo; rule <= lastRuleNo; rule++)
    {
        int possible = 1;

        Node *r = g[rule].nxt;

        TreeNode *currSibling = NULL;

        while (r != NULL)
        {
            TreeNode *n;
            if (r->element[0] >= 65 && r->element[0] <= 90)
            {
                n = createLeafNode(findTerminal(r->element));
            }
            else
            {
                n = createNonLeafNode(findNonTerminal(r->element));
            }
            n->parent = t;
            n->next = NULL;
            if (currSibling == NULL)
            {
                t->nodeType.nonLeafNode.child = n;
            }
            else
            {
                currSibling->next = n;
            }
            currSibling = n;

            if (buildNew(n, g) == NULL)
            {
                possible = 0;
                break;
            }
            r = r->nxt;
        }
        if (possible)
        {
            t->nodeType.nonLeafNode.ruleNo = rule;
            return t;
        }
        else
        {
            globalTokenPtr = beforeApplyingRule;
            deleteChildren(t);
        }
    }
    return NULL;
}

TreeNode *createParseTree(TreeNode *t, Token *s, Node *g)
{
    TreeNode *root = initialiseParseTree();

    globalTokenPtr = s;

    root = buildNew(root, g);

    printf("---------PARSING COMPLETED-------------\n");
    return root;
}

void checkArrayVariableIsInteger(TypeExprEntry *table, char *varName)
{
    int i = 0;
    for (i = 0; i < currentTableEntry; i++)
    {
        if (strcmp(table[i].name, varName) == 0)
        {
            break;
        }
    }
    if (table[i].typeExpr.primitiveType != PRIM_INTEGER)
    {
        printf("ERROR: ARRAYINDEX VARIABLE SHOULD BE AN INTEGER TYPE\n");
        if (table[i].typeExpr.primitiveType == PRIM_BOOLEAN)
        {
            printf("The array index variable is BOOLEAN\n");
        }
        else if (table[i].typeExpr.primitiveType == PRIM_REAL)
        {
            printf("The array index variable is REAL\n");
        }
    }
    //  return 1;
}

int convertStringToInteger(char *s)
{
    int result = 0;
    int i = 0;
    while (s[i] != '\0')
    {
        result = result * 10 + (s[i] - '0');
        i++;
    }
    return result;
}

void populateTypeExpressionTable(TypeExprEntry *table, TreeNode *root)
{
    TreeNode *parent = root->parent;
    TreeNode *node = root->nodeType.nonLeafNode.child;
    table[currentTableEntry].name = node->nodeType.leafNode.tok->lexeme;
    table[currentTableEntry].type = parent->nodeType.nonLeafNode.type;
    if (table[currentTableEntry].type == RECTANGULAR_ARRAY)
    {
        RectangularArray rectArr = parent->nodeType.nonLeafNode.expression.rectType;
        int dimensions = rectArr.currDimensions;
        int isStatic = 1;
        for (int i = 0; i < dimensions; i++)
        {
            if (rectArr.dimenArray[i][0] == -1 || rectArr.dimenArray[i][1] == -1)
            {
                isStatic = 0;
                break;
            }
        }
        if (isStatic)
        {
            table[currentTableEntry].rectArrayType = STATIC;
        }
        else
        {
            table[currentTableEntry].rectArrayType = DYNAMIC;
        }
    }
    else
    {
        table[currentTableEntry].rectArrayType = NOT_APPLICABLE;
    }
    table[currentTableEntry].typeExpr = parent->nodeType.nonLeafNode.expression;
    currentTableEntry++;
}

void populateParentNodeWithTypeExpression(TreeNode *parent, TypeExprEntry *table, char *lex)
{
    int i = 0;
    for (; i < currentTableEntry; i++)
    {
        if (strcmp(table[i].name, lex) == 0)
        {
            break;
        }
    }
    parent->nodeType.nonLeafNode.type = table[i].type;
    parent->nodeType.nonLeafNode.expression = table[i].typeExpr;
}

void performTypeChecking(TreeNode *parent, TreeNode *operation, TypeExprEntry *table)
{
    Terminal op = operation->nodeType.leafNode.terminal;
    switch (op)
    {
    case PLUS:
    {
        break;
    }
    case MINUS:
    {
        break;
    }
    case MULT:
    {
        break;
    }
    case DIV:
    {
        break;
    }
    }
}

void traverseParseTree(TreeNode *root, TypeExprEntry *table)
{
    while (root->isLeaf == 1 && root->next != NULL)
    {
        root = root->next;
    }
    if (root->isLeaf == 1 && root->next == NULL)
    {
        return;
    }

    int nonTerminalId = root->nodeType.nonLeafNode.nonterminal;
    switch (nonTerminalId)
    {
    case s:
    {
        // curr node is 's' : Start Symbol
        traverseParseTree(root->nodeType.nonLeafNode.child, table);
        break;
    }
    case list_of_statements:
    {
        // curr node is list of statements
        TreeNode *declStmts = root->nodeType.nonLeafNode.child;
        //Traverse Declaration Subtree
        traverseParseTree(declStmts, table);
        //Traverse Assignment Subtree
        TreeNode *assgnStmts = declStmts->next;
        traverseParseTree(assgnStmts, table);
        break;
    }
    case declaration_statements:
    {
        // curr node is declaration statements
        TreeNode *singleDeclaration = root->nodeType.nonLeafNode.child;

        //Traverse single declaration statement
        traverseParseTree(singleDeclaration, table);

        if (singleDeclaration->next != NULL)
        {
            traverseParseTree(singleDeclaration->next, table);
        }
        break;
    }
    case declaration:
    {
        // curr node is a single declaration statement;
        TreeNode *typeOfDecl = root->nodeType.nonLeafNode.child;
        traverseParseTree(typeOfDecl, table);
        break;
    }
    case primitive_decl:
    {
        // curr node is primitive declaration
        root->nodeType.nonLeafNode.type = PRIMITIVE;
        TreeNode *primDeclChild = root->nodeType.nonLeafNode.child;
        TreeNode *declareVarsNode = NULL;

        PrimType primType;

        while (primDeclChild->next != NULL)
        {
            if (primDeclChild->isLeaf == 0)
            {
                if (primDeclChild->nodeType.nonLeafNode.nonterminal == 8)
                {
                    declareVarsNode = primDeclChild;
                }
                else if (primDeclChild->nodeType.nonLeafNode.nonterminal == 9)
                {
                    TreeNode *leaf = primDeclChild->nodeType.nonLeafNode.child;
                    if (leaf->nodeType.leafNode.terminal == 9)
                    {
                        primType = PRIM_INTEGER;
                    }
                    else if (leaf->nodeType.leafNode.terminal == 10)
                    {
                        primType = PRIM_REAL;
                    }
                    else
                    {
                        primType = PRIM_BOOLEAN;
                    }
                }
            }
            primDeclChild = primDeclChild->next;
        }

        root->nodeType.nonLeafNode.expression.primitiveType = primType;
        traverseParseTree(declareVarsNode, table);
        break;
    }
    case rect_array_decl:
    {
        // curr node is rectangular array declaration
        root->nodeType.nonLeafNode.type = RECTANGULAR_ARRAY;
        RectangularArray rectArr;
        rectArr.currDimensions = 0;
        root->nodeType.nonLeafNode.expression.rectType = rectArr;
        TreeNode *rectDeclChild = root->nodeType.nonLeafNode.child;
        TreeNode *declareVarsNode = NULL;

        while (rectDeclChild->next != NULL)
        {
            if (rectDeclChild->isLeaf == 0)
            {
                if (rectDeclChild->nodeType.nonLeafNode.nonterminal == 8)
                {
                    declareVarsNode = rectDeclChild;
                }
                else if (rectDeclChild->nodeType.nonLeafNode.nonterminal == 10)
                {
                    // rectArrChild points to node ARRAY(Terminal)
                    TreeNode *rectArrChild = rectDeclChild->nodeType.nonLeafNode.child;
                    // now, rectArrChild points to node array_dim
                    rectArrChild = rectArrChild->next;
                    // dimenChild points to SBOP
                    TreeNode *dimenChild = rectArrChild->nodeType.nonLeafNode.child;
                    while (1)
                    {
                        //dimenChild now points to idx
                        dimenChild = dimenChild->next;

                        TreeNode *term = dimenChild->nodeType.nonLeafNode.child;
                        if (term->nodeType.leafNode.terminal == 29)
                        {
                            // idx is a ID
                            checkArrayVariableIsInteger(table, term->nodeType.leafNode.tok->lexeme);
                            rectArr.dimenArray[rectArr.currDimensions][0] = -1;
                        }
                        else
                        {
                            // idx is a NUM
                            rectArr.dimenArray[rectArr.currDimensions][0] = convertStringToInteger(term->nodeType.leafNode.tok->lexeme);
                            printf("D: %d\n", rectArr.dimenArray[rectArr.currDimensions][0]);
                        }

                        //dimenChild now points to ..
                        dimenChild = dimenChild->next;
                        //dimenChild now points to idx
                        dimenChild = dimenChild->next;

                        term = dimenChild->nodeType.nonLeafNode.child;
                        if (term->nodeType.leafNode.terminal == 29)
                        {
                            // idx is a ID
                            checkArrayVariableIsInteger(table, term->nodeType.leafNode.tok->lexeme);
                            rectArr.dimenArray[rectArr.currDimensions][1] = -1;
                        }
                        else
                        {
                            // idx is a NUM
                            rectArr.dimenArray[rectArr.currDimensions][1] = convertStringToInteger(term->nodeType.leafNode.tok->lexeme);
                            printf("D: %d\n", rectArr.dimenArray[rectArr.currDimensions][1]);
                        }

                        //dimenChild now points to SBCL
                        dimenChild = dimenChild->next;
                        if (dimenChild->next != NULL)
                        {
                            rectArr.currDimensions++;
                            //dimenChild now points to array_dim
                            dimenChild = dimenChild->next;
                            //dimenChild now points to SBOP
                            dimenChild = dimenChild->nodeType.nonLeafNode.child;
                        }
                        else
                        {
                            break;
                        }
                    }
                    traverseParseTree(rectArrChild, table);
                }
            }
            rectDeclChild = rectDeclChild->next;
        }
        traverseParseTree(declareVarsNode, table);
        break;
    }
    case jagged_array_decl:
    {
        // curr node is jagged arrayd declaration
        root->nodeType.nonLeafNode.type = JAGGED_ARRAY;
        TreeNode *declareVarsNode = nthChild(root, 1);
        TreeNode *declareJagged = nthChild(root, 3);
        traverseParseTree(declareJagged, table);
        root->nodeType.nonLeafNode.expression = nthChild(root, 1)->nodeType.nonLeafNode.expression;
        traverseParseTree(declareVarsNode, table);
        break;
    }
    case declare_jagged:
    {
        TreeNode *child = root->nodeType.nonLeafNode.child;
        traverseParseTree(child, table);
        root->nodeType.nonLeafNode.expression = nthChild(root, 0)->nodeType.nonLeafNode.expression;
        break;
    }
    case declare_twod_jagged:
    {
        TreeNode *child = root->nodeType.nonLeafNode.child;
        root->nodeType.nonLeafNode.type = JAGGED_ARRAY;
        root->nodeType.nonLeafNode.expression = root->parent->nodeType.nonLeafNode.expression;
        JaggedArray jaggedArray = root->nodeType.nonLeafNode.expression.jaggedType;
        jaggedArray.r1Low = convertStringToInteger(nthChild(root, 4)->nodeType.leafNode.tok->lexeme);
        jaggedArray.r1High = convertStringToInteger(nthChild(root, 6)->nodeType.leafNode.tok->lexeme);
        jaggedArray.is2D = 1;

        jaggedArray.type.twod_array.x = 0;
        jaggedArray.type.twod_array.y = 0;
        jaggedArray.type.twod_array.size = malloc(sizeof(int) * (jaggedArray.r1High - jaggedArray.r1Low + 1));
        jaggedArray.type.twod_array.r2 = (int **)malloc(sizeof(int *) * (jaggedArray.r1High - jaggedArray.r1Low + 1));
        root->nodeType.nonLeafNode.expression.jaggedType = jaggedArray;
        traverseParseTree(nthChild(root, 12), table);
        root->nodeType.nonLeafNode.expression = nthChild(root, 12)->nodeType.nonLeafNode.expression;
        break;
    }
    case twod_jagged_statements:
    {
        root->nodeType.nonLeafNode.expression = root->parent->nodeType.nonLeafNode.expression;
        traverseParseTree(nthChild(root, 0), table);
        if (nthChild(root, 1) != NULL)
        {
            traverseParseTree(nthChild(root, 1), table);
            root->nodeType.nonLeafNode.expression = nthChild(root, 1)->nodeType.nonLeafNode.expression;
        }
        break;
    }
    case twod_jagged_statement:
    {
        root->nodeType.nonLeafNode.expression = root->parent->nodeType.nonLeafNode.expression;
        JaggedArray jaggArr = root->nodeType.nonLeafNode.expression.jaggedType;
        int size = convertStringToInteger(nthChild(root, 5)->nodeType.leafNode.tok->lexeme);
        jaggArr.type.twod_array.size[jaggArr.type.twod_array.x] = size;
        jaggArr.type.twod_array.r2[jaggArr.type.twod_array.x] = malloc(sizeof(int) * size);
        root->nodeType.nonLeafNode.expression.jaggedType = jaggArr;
        traverseParseTree(nthChild(root, 10), table);
        root->nodeType.nonLeafNode.expression = nthChild(root, 10)->nodeType.nonLeafNode.expression;
        root->nodeType.nonLeafNode.expression.jaggedType.type.twod_array.x++;
        root->nodeType.nonLeafNode.expression.jaggedType.type.twod_array.y = 0;
        break;
    }
    case twod_values:
    {
        root->nodeType.nonLeafNode.expression = root->parent->nodeType.nonLeafNode.expression;
        JaggedArray jaggArr = root->nodeType.nonLeafNode.expression.jaggedType;
        (jaggArr.type.twod_array.r2[jaggArr.type.twod_array.x])[jaggArr.type.twod_array.y] = convertStringToInteger(nthChild(root, 0)->nodeType.leafNode.tok->lexeme);
        jaggArr.type.twod_array.y++;
        root->nodeType.nonLeafNode.expression.jaggedType = jaggArr;
        if (nthChild(root, 1) != NULL)
        {
            traverseParseTree(nthChild(root, 1), table);
            root->nodeType.nonLeafNode.expression = nthChild(root, 1)->nodeType.nonLeafNode.expression;
        }
        break;
    }
    case declare_threed_jagged:
    {
        TreeNode *child = root->nodeType.nonLeafNode.child;
        root->nodeType.nonLeafNode.type = JAGGED_ARRAY;
        root->nodeType.nonLeafNode.expression = root->parent->nodeType.nonLeafNode.expression;
        JaggedArray jaggedArray = root->nodeType.nonLeafNode.expression.jaggedType;
        jaggedArray.r1Low = convertStringToInteger(nthChild(root, 4)->nodeType.leafNode.tok->lexeme);
        jaggedArray.r1High = convertStringToInteger(nthChild(root, 6)->nodeType.leafNode.tok->lexeme);
        jaggedArray.is2D = 0;
        jaggedArray.type.threed_array.x = 0;
        jaggedArray.type.threed_array.y = 0;
        jaggedArray.type.threed_array.z = 0;
        jaggedArray.type.threed_array.sizeR2 = malloc(sizeof(int) * (jaggedArray.r1High - jaggedArray.r1Low + 1));
        jaggedArray.type.threed_array.size = malloc(sizeof(int *) * (jaggedArray.r1High - jaggedArray.r1Low + 1));
        jaggedArray.type.threed_array.dimen = (int ***)malloc(sizeof(int **) * (jaggedArray.r1High - jaggedArray.r1Low + 1));
        root->nodeType.nonLeafNode.expression.jaggedType = jaggedArray;
        traverseParseTree(nthChild(root, 14), table);
        root->nodeType.nonLeafNode.expression = nthChild(root, 14)->nodeType.nonLeafNode.expression;
        break;
    }
    case threed_jagged_statements:
    {
        root->nodeType.nonLeafNode.expression = root->parent->nodeType.nonLeafNode.expression;
        traverseParseTree(nthChild(root, 0), table);
        if (nthChild(root, 1) != NULL)
        {
            traverseParseTree(nthChild(root, 1), table);
            root->nodeType.nonLeafNode.expression = nthChild(root, 1)->nodeType.nonLeafNode.expression;
        }
        break;
    }
    case threed_jagged_statement:
    {
        root->nodeType.nonLeafNode.expression = root->parent->nodeType.nonLeafNode.expression;
        JaggedArray jaggArr = root->nodeType.nonLeafNode.expression.jaggedType;
        int size = convertStringToInteger(nthChild(root, 5)->nodeType.leafNode.tok->lexeme);
        jaggArr.type.threed_array.sizeR2[jaggArr.type.threed_array.x] = size;
        // TODO: Populate this size array declared below
        jaggArr.type.threed_array.size[jaggArr.type.threed_array.x] = malloc(sizeof(int) * size);
        jaggArr.type.threed_array.dimen[jaggArr.type.threed_array.x] = malloc(sizeof(int *) * size);
        root->nodeType.nonLeafNode.expression.jaggedType = jaggArr;
        traverseParseTree(nthChild(root, 10), table);
        root->nodeType.nonLeafNode.expression = nthChild(root, 10)->nodeType.nonLeafNode.expression;
        root->nodeType.nonLeafNode.expression.jaggedType.type.threed_array.x++;
        root->nodeType.nonLeafNode.expression.jaggedType.type.threed_array.y = 0;
        root->nodeType.nonLeafNode.expression.jaggedType.type.threed_array.z = 0;
        break;
    }
    case threed_values:
    {
        root->nodeType.nonLeafNode.expression = root->parent->nodeType.nonLeafNode.expression;
        JaggedArray jaggArr = root->nodeType.nonLeafNode.expression.jaggedType;
        int MAX_INT = 100; // TODO: Do something to accomodate dynamic size of the threeD array when the sizes are not specifie for 3rd dimension
        (jaggArr.type.threed_array.dimen[jaggArr.type.threed_array.x])[jaggArr.type.threed_array.y] = malloc(sizeof(int) * MAX_INT);
        jaggArr.type.threed_array.y++;
        jaggArr.type.threed_array.z = 0;
        jaggArr.type.threed_array.size[jaggArr.type.threed_array.x][jaggArr.type.threed_array.y] = 0;
        root->nodeType.nonLeafNode.expression.jaggedType = jaggArr;
        if (nthChild(root, 1) != NULL)
        {
            traverseParseTree(nthChild(root, 1), table);
            root->nodeType.nonLeafNode.expression = nthChild(root, 1)->nodeType.nonLeafNode.expression;
        }
        break;
    }
    case list_num:
    {
        // TODO: take care of the epsilon thing. Currently, I assume that every subarray has at least 1 element
        // TODO: Maintain count of the size of the third dimension somewhere
        root->nodeType.nonLeafNode.expression = root->parent->nodeType.nonLeafNode.expression;
        JaggedArray jaggArr = root->nodeType.nonLeafNode.expression.jaggedType;
        (jaggArr.type.threed_array.dimen[jaggArr.type.threed_array.x])[jaggArr.type.threed_array.y][jaggArr.type.threed_array.z] =
            convertStringToInteger(nthChild(root, 0)->nodeType.leafNode.tok->lexeme);
        jaggArr.type.threed_array.z++;
        jaggArr.type.threed_array.size[jaggArr.type.threed_array.x][jaggArr.type.threed_array.y]++;
        root->nodeType.nonLeafNode.expression.jaggedType = jaggArr;
        if (nthChild(root, 1) != NULL)
        {
            traverseParseTree(nthChild(root, 1), table);
            root->nodeType.nonLeafNode.expression = nthChild(root, 1)->nodeType.nonLeafNode.expression;
        }
        break;
    }
    case declare_vars:
    {
        // curr node is declare_vars node
        TreeNode *child = root->nodeType.nonLeafNode.child;
        if (child->next == NULL)
        {
            // single variable
            populateTypeExpressionTable(table, root);
        }
        else
        {
            // list of variables
            traverseParseTree(child, table);
        }
        break;
    }
    case var_name_list:
    {
        // curr node is var_name_list
        TreeNode *parent = root->parent; // Parent is declare_vars or var_names_list
        TreeNode *child = root->nodeType.nonLeafNode.child;

        populateTypeExpressionTable(table, root);

        if (root->next != NULL)
        {
            // next node is var_name_list
            TreeNode *varNameListNode = child->next;
            varNameListNode->nodeType.nonLeafNode.type = parent->nodeType.nonLeafNode.type;
            varNameListNode->nodeType.nonLeafNode.expression = parent->nodeType.nonLeafNode.expression;
            traverseParseTree(varNameListNode, table);
        }
        break;
    }
    case assignment_statements:
    {
        printf("INSIDE ASSGN STMTS\n");
        // curr node is assignment statements
        TreeNode *singleAssignment = root->nodeType.nonLeafNode.child;

        //Traverse single assignment statement
        traverseParseTree(singleAssignment, table);

        if (singleAssignment->next != NULL)
        {
            traverseParseTree(singleAssignment->next, table);
        }
        break;
    }
    case assignment:
    {
        printf("INSIDE ASSGN STMT\n");
        TreeNode *lhsNode = root->nodeType.nonLeafNode.child;

        //Traverse LHS
        traverseParseTree(lhsNode, table);

        break;
    }
    case lhs:
    {
        TreeNode *parent = root->parent;
        TreeNode *child = root->nodeType.nonLeafNode.child;

        if (child->isLeaf)
        {
            // lhs is a single variable
            printf("SINGLE VAR\n");
            char *lex = child->nodeType.leafNode.tok->lexeme;
            TypeExprEntry *tEntry;
            for (int i = 0; i < currentTableEntry; i++)
            {
                if (strcmp(lex, table[i].name) == 0)
                {
                    tEntry = &table[i];
                }
            }
            parent->nodeType.nonLeafNode.type = tEntry->type;
            parent->nodeType.nonLeafNode.expression = tEntry->typeExpr;
        }
        else
        {
        }
        break;
    }
    case rhs:
    {
        TreeNode *child = root->nodeType.nonLeafNode.child;
        traverseParseTree(child, table);
        break;
    }
    case arithmetic_expr:
    {
        TreeNode *arithmeticTerm = root->nodeType.nonLeafNode.child;
        traverseParseTree(arithmeticTerm, table);
        if (arithmeticTerm->next == NULL)
        {
        }
        else
        {
            // op1 would be either PLUS or MINUS node
            TreeNode *op1 = arithmeticTerm->next;
            TreeNode *arithExpr = arithmeticTerm->next->next;
            traverseParseTree(arithExpr, table);
            // do type checking for arithTerm and arithExpr
        }
        break;
    }
    case boolean_expr:
    {
        break;
    }
    case arithmetic_term:
    {
        TreeNode *parent = root->parent;
        TreeNode *idx = root->nodeType.nonLeafNode.child;

        TreeNode *term = idx->nodeType.nonLeafNode.child;
        if (term->nodeType.leafNode.terminal == 29)
        {
            // idx is a ID
            populateParentNodeWithTypeExpression(parent, table, term->nodeType.leafNode.tok->lexeme);
        }
        else
        {
            // idx is a NUM
        }

        if (idx->next != NULL)
        {
            //op2 would be either MULT or DIV node
            TreeNode *op2 = idx->next;
            TreeNode *arithTerm = idx->next->next;
            traverseParseTree(arithTerm, table);
            //do type checking for idx and arithTerm;
        }
        break;
    }
    }
}
