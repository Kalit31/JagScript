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
    TreeNode *child = node->child;
    while (child != NULL)
    {
        TreeNode *prev = child;
        child = child->next;
        deleteChildren(prev);
        free(prev);
    }
    node->child = NULL;
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
        int tokenId = t->terminal;
        int tokenIdTokenStream = globalTokenPtr->tokenName;
        if (tokenId == tokenIdTokenStream)
        {
            t->tok = globalTokenPtr;
            globalTokenPtr = globalTokenPtr->next;
            return t;
        }
        else
        {

            return NULL;
        }
    }

    Token *beforeApplyingRule = globalTokenPtr;
    int nonTerminalID = t->nonterminal;
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
                t->child = n;
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
            t->ruleNo = rule;
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
    TreeNode *node = root->child;
    table[currentTableEntry].name = node->tok->lexeme;
    table[currentTableEntry].type = parent->type;
    if (table[currentTableEntry].type == RECTANGULAR_ARRAY)
    {
        RectangularArray rectArr = parent->expression.rectType;
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
    table[currentTableEntry].typeExpr = parent->expression;
    currentTableEntry++;
}

void populateParentNodeWithTypeExpression(TreeNode *parent, TypeExprEntry *table, TreeNode *term)
{
    int i = 0;
    for (; i < currentTableEntry; i++)
    {
        if (strcmp(table[i].name, term->tok->lexeme) == 0)
        {
            break;
        }
    }
    parent->type = table[i].type;
    parent->expression = table[i].typeExpr;
    parent->tok = term->tok;
}

void performTypeChecking(TreeNode *parent, TreeNode *rightExpr, TreeNode *operation, TypeExprEntry *table)
{
    Terminal op = operation->terminal;
    switch (op)
    {
    case PLUS:
    {
        int typeExprPrimOfParent = parent->expression.primitiveType;
        int typeExprPrimOfRightExpr = rightExpr->expression.primitiveType;
        if (typeExprPrimOfParent != typeExprPrimOfRightExpr)
        {
            printf("TYPE ERROR\n");
            printf("LINE NUMBER: %d\n", parent->tok->lineNo);
            printf("STATEMENT TYPE: ASSIGNMENT STATEMENT\n");
            printf("OPERATOR: %s\n", TOKENS[op]);
            printf("LEXEME OF FIRST OPERAND: %s\n", parent->tok->lexeme);
            printf("TYPE OF FIRST OPERAND: %d\n", typeExprPrimOfParent);
            printf("LEXEME OF FIRST OPERAND: %s\n", rightExpr->tok->lexeme);
            printf("TYPE OF FIRST OPERAND: %d\n", typeExprPrimOfRightExpr);
        }
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

    int nonTerminalId = root->nonterminal;
    switch (nonTerminalId)
    {
    case s:
    {
        // curr node is 's' : Start Symbol
        traverseParseTree(root->child, table);
        break;
    }
    case list_of_statements:
    {
        // curr node is list of statements
        TreeNode *declStmts = root->child;
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
        TreeNode *singleDeclaration = root->child;

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
        TreeNode *typeOfDecl = root->child;
        traverseParseTree(typeOfDecl, table);
        break;
    }
    case primitive_decl:
    {
        // curr node is primitive declaration
        root->type = PRIMITIVE;
        TreeNode *primDeclChild = root->child;
        TreeNode *declareVarsNode = NULL;

        PrimType primType;

        while (primDeclChild->next != NULL)
        {
            if (primDeclChild->isLeaf == 0)
            {
                if (primDeclChild->nonterminal == 8)
                {
                    declareVarsNode = primDeclChild;
                }
                else if (primDeclChild->nonterminal == 9)
                {
                    TreeNode *leaf = primDeclChild->child;
                    if (leaf->terminal == 9)
                    {
                        primType = PRIM_INTEGER;
                    }
                    else if (leaf->terminal == 10)
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

        root->expression.primitiveType = primType;
        traverseParseTree(declareVarsNode, table);
        break;
    }
    case rect_array_decl:
    {
        // curr node is rectangular array declaration
        root->type = RECTANGULAR_ARRAY;
        RectangularArray rectArr;
        rectArr.currDimensions = 0;
        root->expression.rectType = rectArr;
        TreeNode *rectDeclChild = root->child;
        TreeNode *declareVarsNode = NULL;

        while (rectDeclChild->next != NULL)
        {
            if (rectDeclChild->isLeaf == 0)
            {
                if (rectDeclChild->nonterminal == 8)
                {
                    declareVarsNode = rectDeclChild;
                }
                else if (rectDeclChild->nonterminal == 10)
                {
                    // rectArrChild points to node ARRAY(Terminal)
                    TreeNode *rectArrChild = rectDeclChild->child;
                    // now, rectArrChild points to node array_dim
                    rectArrChild = rectArrChild->next;
                    // dimenChild points to SBOP
                    TreeNode *dimenChild = rectArrChild->child;
                    while (1)
                    {
                        //dimenChild now points to idx
                        dimenChild = dimenChild->next;

                        TreeNode *term = dimenChild->child;
                        if (term->terminal == 29)
                        {
                            // idx is a ID
                            checkArrayVariableIsInteger(table, term->tok->lexeme);
                            rectArr.dimenArray[rectArr.currDimensions][0] = -1;
                        }
                        else
                        {
                            // idx is a NUM
                            rectArr.dimenArray[rectArr.currDimensions][0] = convertStringToInteger(term->tok->lexeme);
                            printf("D: %d\n", rectArr.dimenArray[rectArr.currDimensions][0]);
                        }

                        //dimenChild now points to ..
                        dimenChild = dimenChild->next;
                        //dimenChild now points to idx
                        dimenChild = dimenChild->next;

                        term = dimenChild->child;
                        if (term->terminal == 29)
                        {
                            // idx is a ID
                            checkArrayVariableIsInteger(table, term->tok->lexeme);
                            rectArr.dimenArray[rectArr.currDimensions][1] = -1;
                        }
                        else
                        {
                            // idx is a NUM
                            rectArr.dimenArray[rectArr.currDimensions][1] = convertStringToInteger(term->tok->lexeme);
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
                            dimenChild = dimenChild->child;
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
        root->type = JAGGED_ARRAY;
        TreeNode *declareVarsNode = nthChild(root, 1);
        TreeNode *declareJagged = nthChild(root, 3);
        traverseParseTree(declareJagged, table);
        root->expression = nthChild(root, 1)->expression;
        traverseParseTree(declareVarsNode, table);
        break;
    }
    case declare_jagged:
    {
        TreeNode *child = root->child;
        traverseParseTree(child, table);
        root->expression = nthChild(root, 0)->expression;
        break;
    }
    case declare_twod_jagged:
    {
        TreeNode *child = root->child;
        root->type = JAGGED_ARRAY;
        root->expression = root->parent->expression;
        JaggedArray jaggedArray = root->expression.jaggedType;
        jaggedArray.r1Low = convertStringToInteger(nthChild(root, 4)->tok->lexeme);
        jaggedArray.r1High = convertStringToInteger(nthChild(root, 6)->tok->lexeme);
        jaggedArray.is2D = 1;

        jaggedArray.type.twod_array.x = 0;
        jaggedArray.type.twod_array.y = 0;
        jaggedArray.type.twod_array.size = malloc(sizeof(int) * (jaggedArray.r1High - jaggedArray.r1Low + 1));
        jaggedArray.type.twod_array.r2 = (int **)malloc(sizeof(int *) * (jaggedArray.r1High - jaggedArray.r1Low + 1));
        root->expression.jaggedType = jaggedArray;
        traverseParseTree(nthChild(root, 12), table);
        root->expression = nthChild(root, 12)->expression;
        break;
    }
    case twod_jagged_statements:
    {
        root->expression = root->parent->expression;
        traverseParseTree(nthChild(root, 0), table);
        if (nthChild(root, 1) != NULL)
        {
            traverseParseTree(nthChild(root, 1), table);
            root->expression = nthChild(root, 1)->expression;
        }
        break;
    }
    case twod_jagged_statement:
    {
        root->expression = root->parent->expression;
        JaggedArray jaggArr = root->expression.jaggedType;
        int size = convertStringToInteger(nthChild(root, 5)->tok->lexeme);
        jaggArr.type.twod_array.size[jaggArr.type.twod_array.x] = size;
        jaggArr.type.twod_array.r2[jaggArr.type.twod_array.x] = malloc(sizeof(int) * size);
        root->expression.jaggedType = jaggArr;
        traverseParseTree(nthChild(root, 10), table);
        root->expression = nthChild(root, 10)->expression;
        root->expression.jaggedType.type.twod_array.x++;
        root->expression.jaggedType.type.twod_array.y = 0;
        break;
    }
    case twod_values:
    {
        root->expression = root->parent->expression;
        JaggedArray jaggArr = root->expression.jaggedType;
        (jaggArr.type.twod_array.r2[jaggArr.type.twod_array.x])[jaggArr.type.twod_array.y] = convertStringToInteger(nthChild(root, 0)->tok->lexeme);
        jaggArr.type.twod_array.y++;
        root->expression.jaggedType = jaggArr;
        if (nthChild(root, 1) != NULL)
        {
            traverseParseTree(nthChild(root, 1), table);
            root->expression = nthChild(root, 1)->expression;
        }
        break;
    }
    case declare_threed_jagged:
    {
        TreeNode *child = root->child;
        root->type = JAGGED_ARRAY;
        root->expression = root->parent->expression;
        JaggedArray jaggedArray = root->expression.jaggedType;
        jaggedArray.r1Low = convertStringToInteger(nthChild(root, 4)->tok->lexeme);
        jaggedArray.r1High = convertStringToInteger(nthChild(root, 6)->tok->lexeme);
        jaggedArray.is2D = 0;
        jaggedArray.type.threed_array.x = 0;
        jaggedArray.type.threed_array.y = 0;
        jaggedArray.type.threed_array.z = 0;
        jaggedArray.type.threed_array.sizeR2 = malloc(sizeof(int) * (jaggedArray.r1High - jaggedArray.r1Low + 1));
        jaggedArray.type.threed_array.size = malloc(sizeof(int *) * (jaggedArray.r1High - jaggedArray.r1Low + 1));
        jaggedArray.type.threed_array.dimen = (int ***)malloc(sizeof(int **) * (jaggedArray.r1High - jaggedArray.r1Low + 1));
        root->expression.jaggedType = jaggedArray;
        traverseParseTree(nthChild(root, 14), table);
        root->expression = nthChild(root, 14)->expression;
        break;
    }
    case threed_jagged_statements:
    {
        root->expression = root->parent->expression;
        traverseParseTree(nthChild(root, 0), table);
        if (nthChild(root, 1) != NULL)
        {
            traverseParseTree(nthChild(root, 1), table);
            root->expression = nthChild(root, 1)->expression;
        }
        break;
    }
    case threed_jagged_statement:
    {
        root->expression = root->parent->expression;
        JaggedArray jaggArr = root->expression.jaggedType;
        int size = convertStringToInteger(nthChild(root, 5)->tok->lexeme);
        jaggArr.type.threed_array.sizeR2[jaggArr.type.threed_array.x] = size;
        // TODO: Populate this size array declared below
        jaggArr.type.threed_array.size[jaggArr.type.threed_array.x] = malloc(sizeof(int) * size);
        jaggArr.type.threed_array.dimen[jaggArr.type.threed_array.x] = malloc(sizeof(int *) * size);
        root->expression.jaggedType = jaggArr;
        traverseParseTree(nthChild(root, 10), table);
        root->expression = nthChild(root, 10)->expression;
        root->expression.jaggedType.type.threed_array.x++;
        root->expression.jaggedType.type.threed_array.y = 0;
        root->expression.jaggedType.type.threed_array.z = 0;
        break;
    }
    case threed_values:
    {
        root->expression = root->parent->expression;
        JaggedArray jaggArr = root->expression.jaggedType;
        int MAX_INT = 100; // TODO: Do something to accomodate dynamic size of the threeD array when the sizes are not specifie for 3rd dimension
        (jaggArr.type.threed_array.dimen[jaggArr.type.threed_array.x])[jaggArr.type.threed_array.y] = malloc(sizeof(int) * MAX_INT);
        jaggArr.type.threed_array.y++;
        jaggArr.type.threed_array.z = 0;
        jaggArr.type.threed_array.size[jaggArr.type.threed_array.x][jaggArr.type.threed_array.y] = 0;
        root->expression.jaggedType = jaggArr;
        if (nthChild(root, 1) != NULL)
        {
            traverseParseTree(nthChild(root, 1), table);
            root->expression = nthChild(root, 1)->expression;
        }
        break;
    }
    case list_num:
    {
        // TODO: take care of the epsilon thing. Currently, I assume that every subarray has at least 1 element
        // TODO: Maintain count of the size of the third dimension somewhere
        root->expression = root->parent->expression;
        JaggedArray jaggArr = root->expression.jaggedType;
        (jaggArr.type.threed_array.dimen[jaggArr.type.threed_array.x])[jaggArr.type.threed_array.y][jaggArr.type.threed_array.z] =
            convertStringToInteger(nthChild(root, 0)->tok->lexeme);
        jaggArr.type.threed_array.z++;
        jaggArr.type.threed_array.size[jaggArr.type.threed_array.x][jaggArr.type.threed_array.y]++;
        root->expression.jaggedType = jaggArr;
        if (nthChild(root, 1) != NULL)
        {
            traverseParseTree(nthChild(root, 1), table);
            root->expression = nthChild(root, 1)->expression;
        }
        break;
    }
    case declare_vars:
    {
        // curr node is declare_vars node
        TreeNode *child = root->child;
        if (child->next == NULL)
        {
            // single variable
            populateTypeExpressionTable(table, root);
        }
        else
        {
            TreeNode *parent = root->parent;
            root->type = parent->type;
            root->expression = parent->expression;
            // list of variables
            traverseParseTree(child, table);
        }
        break;
    }
    case var_name_list:
    {
        // curr node is var_name_list
        TreeNode *parent = root->parent; // Parent is declare_vars or var_names_list
        TreeNode *child = root->child;
        populateTypeExpressionTable(table, root);

        if (child->next != NULL)
        {
            // next node is var_name_list
            TreeNode *varNameListNode = child->next;
            varNameListNode->type = parent->type;
            varNameListNode->expression = parent->expression;
            traverseParseTree(varNameListNode, table);
        }
        break;
    }
    case assignment_statements:
    {
        // curr node is assignment statements
        TreeNode *singleAssignment = root->child;

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
        // curr node is assignment
        TreeNode *lhsNode = root->child;

        //Traverse LHS
        traverseParseTree(lhsNode, table);
        //Traverse RHS
        traverseParseTree(lhsNode->next, table);
        break;
    }
    case lhs:
    {
        // curr node is lhs
        TreeNode *parent = root->parent;
        TreeNode *child = root->child;

        if (child->isLeaf)
        {
            // lhs is a single variable
            char *lex = child->tok->lexeme;
            TypeExprEntry *tEntry;
            for (int i = 0; i < currentTableEntry; i++)
            {
                if (strcmp(lex, table[i].name) == 0)
                {
                    tEntry = &table[i];
                }
            }
            parent->type = tEntry->type;
            parent->expression = tEntry->typeExpr;
        }
        else
        {
            // lhs is array element
        }
        break;
    }
    case rhs:
    {
        // curr node is rhs
        // child is arithmetic expression
        TreeNode *child = root->child;
        traverseParseTree(child, table);
        break;
    }
    case arithmetic_expr:
    {
        // curr node is arithmetic expression

        TreeNode *arithmeticTerm = root->child;
        traverseParseTree(arithmeticTerm, table);
        if (arithmeticTerm->next == NULL)
        {
        }
        else
        {
            // op1 would be either PLUS or MINUS node
            TreeNode *op1 = arithmeticTerm->next;
            TreeNode *arithExpr = op1->next;
            traverseParseTree(arithExpr, table);
            // do type checking for arithTerm and arithExpr
            performTypeChecking(root, arithExpr, op1->child, table);
        }
        break;
    }
    case boolean_expr:
    {
        break;
    }
    case arithmetic_term:
    {
        // parent is arith_expr or arith_term
        TreeNode *parent = root->parent;
        TreeNode *idx = root->child;

        TreeNode *term = idx->child;
        if (term->terminal == 29)
        {
            // idx is a ID
            populateParentNodeWithTypeExpression(parent, table, term);
        }
        else
        {
            // idx is a NUM        printf("PLUS\n");
        }

        if (idx->next != NULL)
        {
            //op2 would be either MULT or DIV node
            TreeNode *op2 = idx->next;
            TreeNode *arithTerm = op2->next;
            traverseParseTree(arithTerm, table);
            // performTypeChecking(parent, op2->child, table);
            //do type checking for idx and arithTerm;
        }
        break;
    }
    }
}
