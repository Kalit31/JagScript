#include <stdio.h>
#include <stdlib.h>
#include "parsetree.h"
#include "typeExpression.h"
#include "naryTree.c"
#include "stack.c"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

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
    printf("COULD NOT FIND RULE\n");
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

int build(TreeNode *curr, Stack *st, Node *g)
{
    StackNode *t = top(st);
    if (globalTokenPtr == NULL && t == NULL)
    {
        return 1;
    }
    else if (globalTokenPtr == NULL || t == NULL)
    {
        return 0;
    }

    if (t->val->isLeaf)
    {
        int tokenIdTopStack = t->val->terminal;
        if (tokenIdTopStack == EPS)
        {
            return 1;
        }
        int tokenIdTokenStream = globalTokenPtr->tokenName;
        if (tokenIdTopStack == tokenIdTokenStream)
        {
            //printf("CURR TREE NODE: %s - %s\n", TOKENS[curr->terminal], globalTokenPtr->lexeme);
            pop(st);
            curr->tok = globalTokenPtr;
            globalTokenPtr = globalTokenPtr->next;
            return 1;
        }
        else
        {
            //printf("Could not match %s with %s\n", TOKENS[tokenIdTopStack], TOKENS[tokenIdTokenStream]);
            return 0;
        }
    }
    else
    {
        int nonTerminalID = t->val->nonterminal;

        int firstRuleNo = findRuleIndex(nonTerminalID, g);
        int lastRuleNo = findLastRuleIndex(nonTerminalID, g);
        pop(st);
        Token *beforeApplying = globalTokenPtr;
        int initialStackSize = st->size;
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
                n->parent = t->val;
                n->next = NULL;
                if (currSibling == NULL)
                {
                    t->val->child = n;
                }
                else
                {
                    currSibling->next = n;
                }
                currSibling = n;

                push(st, n);
                if (build(n, st, g) == 0)
                {
                    possible = 0;
                    break;
                }
                r = r->nxt;
            }

            if (possible == 1)
            {
                curr->ruleNo = rule;
                return 1;
            }
            else
            {
                globalTokenPtr = beforeApplying;
                while (st->size > initialStackSize)
                {
                    pop(st);
                }
                deleteChildren(curr);
                //printf("TRYING DIFF RULE\n");
            }
        }
    }
    return 0;
}

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

    if (nonTerminalID == rhs)
    {
        printf("RHS\n");
        printf("FIRST: %d\n", firstRuleNo);
        printf("LAST: %d\n", lastRuleNo);
        printf("TOKEN: %s\n", globalTokenPtr->lexeme);
    }

    for (int rule = firstRuleNo; rule <= lastRuleNo; rule++)
    {
        if (nonTerminalID == rhs)
        {
            printf("RULE: %d\n", rule);
        }
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
                if (rule == 40)
                {
                    printf("RULE %d\n", rule);
                }
                possible = 0;
                break;
            }
            r = r->nxt;
        }
        if (possible)
        {
            if (rule == 46)
                printf("CURR TOKEN: %s\n", globalTokenPtr->lexeme);
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
    Stack *st = createStack();
    TreeNode *root = initialiseParseTree();
    push(st, root);
    globalTokenPtr = s;
    build(root, st, g);
    //root = buildNew(root, g);
    printf("\n------------------------------------------------PARSE TREE IS CREATED SUCCESSFULLY--------------------------------------------\n\n");

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
    table[currentTableEntry].name = root->tok->lexeme;
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

void populateNodeWithTypeExpression(TreeNode *node, TypeExprEntry *table, TreeNode *term)
{
    if (term->terminal == NUM)
    {
        node->type = PRIMITIVE;
        node->expression.primitiveType = PRIM_INTEGER;
        node->tok = term->tok;
        return;
    }
    int i = 0;
    for (; i < currentTableEntry; i++)
    {
        if (strcmp(table[i].name, term->tok->lexeme) == 0)
        {
            break;
        }
    }
    node->type = table[i].type;
    node->expression = table[i].typeExpr;
    node->tok = term->tok; //lexeme : v1, line no : 4
}

void populateNodeFromNode(TreeNode *a, TreeNode *b)
{
    a->type = b->type;
    a->expression = b->expression;
    a->tok = b->tok;
}

void printTypeCheckError(TreeNode *a, TreeNode *b, Terminal operator)
{
    printf(ANSI_COLOR_RED  "%d\t\t\tASSIGNMENT %20s %20s %12d %20s %12d" ANSI_COLOR_RESET "\n", a->tok->lineNo,TOKENS[operator],a->tok->lexeme
    ,a->expression.primitiveType, b->tok->lexeme, b->expression.primitiveType);
}

void checkArrayCompatibility(TreeNode *node1, TreeNode *node2)
{
    // Check if both arrays are of same dimensions
    if (node1->type == node2->type)
    {
        if (node1->type == RECTANGULAR_ARRAY)
        {
            if (node1->expression.rectType.currDimensions != node2->expression.rectType.currDimensions)
            {
                // Raise incompatibility error
            }
            else
            {
                int curDimensions = node1->expression.rectType.currDimensions;
                while (curDimensions--)
                {
                    if (node1->expression.rectType.dimenArray[curDimensions][0] != node2->expression.rectType.dimenArray[curDimensions][0] ||
                        node1->expression.rectType.dimenArray[curDimensions][1] != node2->expression.rectType.dimenArray[curDimensions][1])
                    {
                        // Dimensions incompatible
                    }
                }
            }
        }
        else if (node1->type == JAGGED_ARRAY)
        {
            if (node1->expression.jaggedType.is2D != node2->expression.jaggedType.is2D)
            {
                // Raise error... Array types not compatible
            }
            else
            {
                if (node1->expression.jaggedType.r1Low != node2->expression.jaggedType.r1Low ||
                    node1->expression.jaggedType.r1High != node2->expression.jaggedType.r1High)
                {
                    // Incompatible dimensions
                }
                if (node1->expression.jaggedType.is2D)
                {
                    // 2D Jagged Array
                    for (int i = 0; i < node1->expression.jaggedType.r1High - node1->expression.jaggedType.r1Low + 1; i++)
                    {
                        if (node1->expression.jaggedType.type.twod_array.size[i] != node2->expression.jaggedType.type.twod_array.size[i])
                        {
                            // Array dimensions incompatible
                        }
                    }
                }
                else
                {
                    // 3D Jagged Array
                    // Check compatibility for 2nd dimension
                    int incomaptibilityFlag2ndDim = 0;
                    for (int i = 0; i < node1->expression.jaggedType.r1High - node1->expression.jaggedType.r1Low + 1; i++)
                    {
                        if (node1->expression.jaggedType.type.threed_array.sizeR2[i] != node2->expression.jaggedType.type.threed_array.sizeR2[i])
                        {
                            // Array dimensions incompatible
                            incomaptibilityFlag2ndDim = 1;
                        }
                    }
                    // check compatibility for 3rd dimension
                    if (!incomaptibilityFlag2ndDim)
                    {
                        for (int i = 0; i < node1->expression.jaggedType.r1High - node1->expression.jaggedType.r1Low + 1; i++)
                        {
                            for (int j = 0; j < node1->expression.jaggedType.type.threed_array.sizeR2[i]; i++)
                            {
                                // Array dimensions incompatible
                                if (node1->expression.jaggedType.type.threed_array.size[i][j] != node2->expression.jaggedType.type.threed_array.size[i][j])
                                {
                                    // Array dimensions incompatible
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        // Raise error... Array types not compatible
        // I assume that rectangular arrays aren't compatible with jagged arrays even if they are of same size
    }
}

void performTypeChecking(TreeNode *root, TreeNode *rightExpr, TreeNode *operation, TypeExprEntry *table)
{
    Terminal op = operation->terminal;
    int typeExprPrimOfParent = root->expression.primitiveType;
    int typeExprPrimOfRightExpr = rightExpr->expression.primitiveType;

    switch (op)
    {
    case PLUS:
    {
        if (typeExprPrimOfParent != typeExprPrimOfRightExpr)
        {
            printTypeCheckError(root, rightExpr, op);
        }
        else
        {
            if (typeExprPrimOfParent == PRIM_BOOLEAN)
            {
                printf("2 BOOLEAN VARIABLES CANNOT BE ADDED\n");
            }
        }
        break;
    }
    case MINUS:
    {
        if (typeExprPrimOfParent != typeExprPrimOfRightExpr)
        {
            printTypeCheckError(root, rightExpr, op);
        }
        else
        {
            if (typeExprPrimOfParent == PRIM_BOOLEAN)
            {
                printf("2 BOOLEAN VARIABLES CANNOT BE SUBTRACTED\n");
            }
        }
        break;
    }
    case MULT:
    {
        if (typeExprPrimOfParent != typeExprPrimOfRightExpr)
        {
            printTypeCheckError(root, rightExpr, op);
        }
        else
        {
            if (typeExprPrimOfParent == PRIM_BOOLEAN)
            {
                printf("2 BOOLEAN VARIABLES CANNOT BE MULTIPLIED\n");
            }
        }
        break;
    }
    case DIV:
    {
        if (typeExprPrimOfParent != typeExprPrimOfRightExpr)
        {
            printTypeCheckError(root, rightExpr, op);
        }
        else
        {
            if (typeExprPrimOfParent == PRIM_BOOLEAN)
            {
                printf("2 BOOLEAN VARIABLES CANNOT BE DIVIDED\n");
                return;
            }
            root->expression.primitiveType = PRIM_REAL;
        }
        break;
    }
    case EQUALS:
    {
        if (typeExprPrimOfParent != typeExprPrimOfRightExpr)
        {
            printTypeCheckError(root, rightExpr, op);
        }
        break;
    }
    case OR:
    {
        if (typeExprPrimOfParent != typeExprPrimOfRightExpr)
        {
            printTypeCheckError(root, rightExpr, op);
        }
        else
        {
            if (typeExprPrimOfParent == PRIM_INTEGER || typeExprPrimOfParent == PRIM_REAL)
            {
                printf("OR CAN BE DONE BETWEEN TWO BOOLEAN VARIABLES ONLY\n");
            }
        }
        break;
    }
    case AND:
    {
        if (typeExprPrimOfParent != typeExprPrimOfRightExpr)
        {
            printTypeCheckError(root, rightExpr, op);
        }
        else
        {
            if (typeExprPrimOfParent == PRIM_INTEGER || typeExprPrimOfParent == PRIM_REAL)
            {
                printf("AND CAN BE DONE BETWEEN TWO BOOLEAN VARIABLES ONLY\n");
            }
        }
        break;
    }
    }
}

void traverseParseTree(TreeNode *root, TypeExprEntry *table)
{
    if (root->isLeaf == 1 && root->terminal == ID)
    {
        populateTypeExpressionTable(table, root);
        return;
    }
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
                if (primDeclChild->nonterminal == declare_vars)
                {
                    declareVarsNode = primDeclChild;
                }
                else if (primDeclChild->nonterminal == primitive_type)
                {
                    TreeNode *leaf = primDeclChild->child;
                    if (leaf->terminal == INTEGER)
                    {
                        primType = PRIM_INTEGER;
                    }
                    else if (leaf->terminal == REAL)
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
                            rectArr.dynamicDimenArray[rectArr.currDimensions][0] = term->tok->lexeme;
                        }
                        else
                        {
                            // idx is a NUM
                            rectArr.dimenArray[rectArr.currDimensions][0] = convertStringToInteger(term->tok->lexeme);
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
                            rectArr.dynamicDimenArray[rectArr.currDimensions][1] = term->tok->lexeme;
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
        jaggedArray.r1Low = convertStringToInteger(nthChild(root, 3)->tok->lexeme);
        jaggedArray.r1High = convertStringToInteger(nthChild(root, 5)->tok->lexeme);
        jaggedArray.is2D = 1;

        jaggedArray.type.twod_array.size = malloc(sizeof(int) * (jaggedArray.r1High - jaggedArray.r1Low + 1));
        root->expression.jaggedType = jaggedArray;
        traverseParseTree(nthChild(root, 12), table);
        root->expression = nthChild(root, 12)->expression;
        break;
    }
    case twod_jagged_statements:
    {
        root->expression = root->parent->expression;
        TreeNode *statement = root->child;
        JaggedArray jaggArr = root->expression.jaggedType;
        int x=0, y=0, r1Low, r1High, r1Len;
        r1Low = jaggArr.r1Low;
        r1High = jaggArr.r1High;
        r1Len = r1High-r1Low+1;

        while(statement!= NULL){
            if(x>=r1Len){
                printf("Recieved More rows than expected in Jagged Array declaration\n");
                break;
            }
            statement->expression = statement->parent->expression;
            int index = convertStringToInteger(nthChild(statement, 2)->tok->lexeme);
            if(index!=r1Low+x){
                printf("Incorrect order/index of rows for Jagged Arrays declaration\n");
            }
            int size = convertStringToInteger(nthChild(statement, 6)->tok->lexeme);
            jaggArr.type.twod_array.size[x] = size;
            statement->expression.jaggedType = jaggArr;
            TreeNode *twod_values = nthChild(statement, 10);
            y=0;
            while(twod_values != NULL){
                twod_values->expression = twod_values->parent->expression;
                y++;
                if(nthChild(twod_values, 1)!=NULL) twod_values = nthChild(twod_values, 2);
                else break;
            }
            if(y!=size){
                printf("Expected %d values in declaration, but got %d values\n", size, y);
            }
            x++;
            if(statement->next!=NULL){
                statement = statement->next->child;
            }
            else break;
        }
        if(x<r1Len){
            printf("Expected %d number of rows in jagged array declaration, but obtained %d rows\n", r1Len, x);
        }
        root->expression.jaggedType = jaggArr;
        break;
    }
    case declare_threed_jagged:
    {
        root->type = JAGGED_ARRAY;
        root->expression = root->parent->expression;
        JaggedArray jaggedArray = root->expression.jaggedType;
        jaggedArray.r1Low = convertStringToInteger(nthChild(root, 3)->tok->lexeme);
        jaggedArray.r1High = convertStringToInteger(nthChild(root, 5)->tok->lexeme);
        jaggedArray.is2D = 0;
        jaggedArray.type.threed_array.sizeR2 = malloc(sizeof(int) * (jaggedArray.r1High - jaggedArray.r1Low + 1));
        jaggedArray.type.threed_array.size = malloc(sizeof(int *) * (jaggedArray.r1High - jaggedArray.r1Low + 1));
        root->expression.jaggedType = jaggedArray;
        traverseParseTree(nthChild(root, 14), table);
        root->expression = nthChild(root, 14)->expression;
        break;
    }
    case threed_jagged_statements:
    {
        root->expression = root->parent->expression;
        TreeNode *statement = root->child;
        int x=0, y=0, r1Low, r1High, r1Len;
        JaggedArray jaggArr = root->expression.jaggedType;
        r1Low = jaggArr.r1Low;
        r1High = jaggArr.r1High;
        r1Len = r1High-r1Low+1;
        while(statement!=NULL){
            if(x>=r1Len){
                printf("Recieved More rows than expected in Jagged Array declaration\n");
                break;
            }
            statement->expression = statement->parent->expression;
            int index = convertStringToInteger(nthChild(statement, 2)->tok->lexeme);
            if(index!=r1Low+x){
                printf("Incorrect order/index of rows for Jagged Arrays declaration\n");
            }
            int size = convertStringToInteger(nthChild(statement, 6)->tok->lexeme);
            jaggArr.type.threed_array.sizeR2[x] = size;
            statement->expression.jaggedType = jaggArr;
            TreeNode *threed_values = nthChild(statement, 10);

            jaggArr.type.threed_array.sizeR2[x] = size;
            jaggArr.type.threed_array.size[x] = malloc(sizeof(int) * size);

            while(threed_values!=NULL){
                y=0;
                threed_values->expression = threed_values->parent->expression;
                jaggArr.type.threed_array.size[x][y] = 0;
                root->expression.jaggedType = jaggArr;
                TreeNode *threed_list = nthChild(threed_values, 0);
                while(threed_list!=NULL){
                    threed_list->expression = threed_list->parent->expression;
                    jaggArr.type.threed_array.size[x][y]++;
                    if(threed_list->next!=NULL){
                        threed_list = nthChild(threed_list, 1);
                    } else break;
                }
                y++;
                if(threed_values->next!=NULL) threed_values = nthChild(threed_values, 2);
                else break;
            }
            x++;
            if(statement->next!=NULL){
                statement = statement->next->child;
            }
            else break;
        }
        root->expression.jaggedType = jaggArr;
        break;
    }
    case declare_vars:
    {
        // curr node is declare_vars node
        // parent is primitive_decl/rect_array_decl/jagged_array_decl
        root->type = root->parent->type;
        root->expression = root->parent->expression;
        TreeNode *child = root->child;
        if (child->next == NULL)
        {
            // single variable(ID)
            traverseParseTree(child, table);
        }
        else
        {
            // LIST OF VARIABLES var_name_list
            // list of variables
            traverseParseTree(child, table);
        }
        break;
    }
    case var_name_list:
    {
        // curr node is var_name_list
        TreeNode *parent = root->parent; // Parent is declare_vars or var_names_list
        root->type = parent->type;
        root->expression = parent->expression;

        TreeNode *child = root->child;
        traverseParseTree(child, table);

        if (child->next != NULL)
        {
            // next node is var_name_list
            TreeNode *varNameListNode = child->next;
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

        TreeNode *operation = lhsNode->next;
        TreeNode *rhsNode = operation->next;
        //Traverse RHS
        traverseParseTree(rhsNode, table);
        performTypeChecking(lhsNode, rhsNode, operation, table);
        break;
    }
    case lhs:
    {
        // curr node is lhs
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
            root->type = tEntry->type;
            root->expression = tEntry->typeExpr;
            root->tok = child->tok;
        }
        else
        {
            // lhs is array element
            traverseParseTree(child, table);
        }
        break;
    }
    case rhs:
    {
        // curr node is rhs
        // child is arithmetic expression or boolean expression
        TreeNode *child = root->child;
        traverseParseTree(child, table);
        populateNodeFromNode(root, child);
        break;
    }
    case arithmetic_expr:
    {
        // curr node is arithmetic expression
        TreeNode *arithmeticTerm = root->child;
        traverseParseTree(arithmeticTerm, table);
        populateNodeFromNode(root, arithmeticTerm);

        if (arithmeticTerm->next == NULL)
        {
        }
        else
        {
            // op1 would be either PLUS or MINUS or OR node
            TreeNode *op1 = arithmeticTerm->next;
            TreeNode *arithExpr = op1->next;
            traverseParseTree(arithExpr, table);

            // do type checking for arithTerm and arithExpr
            performTypeChecking(root, arithExpr, op1->child, table);
        }
        break;
    }
    case arithmetic_term:
    {
        // parent is arith_expr or arith_term
        TreeNode *parent = root->parent;
        TreeNode *idx = root->child;
        traverseParseTree(idx, table);

        if (idx->next == NULL)
        {
            if (parent->child == root)
            {
                populateNodeFromNode(parent, root);
            }
        }

        if (idx->next != NULL)
        {
            //op2 would be either MULT or DIV node
            TreeNode *op2 = idx->next;
            TreeNode *arithTerm = op2->next;
            traverseParseTree(arithTerm, table);
            //do type checking for idx and arithTerm;
            performTypeChecking(root, arithTerm, op2->child, table);
            populateNodeFromNode(parent, root);
        }
        break;
    }
    case idx:
    {
        TreeNode *term = root->child; // ID Node
        populateNodeWithTypeExpression(root, table, term);
        populateNodeFromNode(root->parent, root);
        break;
    }
    case array_ele:
    {
        char *lex = root->child->tok->lexeme;
        TypeExprEntry *tEntry;
        for (int i = 0; i < currentTableEntry; i++)
        {
            if (strcmp(lex, table[i].name) == 0)
            {
                tEntry = &table[i];
            }
        }
        if (tEntry->rectArrayType == DYNAMIC)
        {
            // Raise error as per pg 11, last paragraph
        }
        TreeNode *child = nthChild(root, 2)->child; //list

        if (tEntry->type == RECTANGULAR_ARRAY)
        {
            int i = root->expression.rectType.currDimensions;
            while (1)
            {
                i--;
                if (child->child->terminal == ID)
                {
                    //Raise error as per pg 11
                    break;
                }
                else
                {
                    // It is a Num
                    int idx = convertStringToInteger(child->child->tok->lexeme);
                    if (idx > root->expression.rectType.dimenArray[i][1] ||
                        idx < root->expression.rectType.dimenArray[i][0])
                    {
                        // Index out of Bounds...Raise Error
                    }
                    if (child->next != NULL)
                        child = child->next->child;
                    else
                        break;
                }
            }
            if (i != 0)
            {
                // Error BC!!
            }
        }
        else if (tEntry->type == JAGGED_ARRAY && tEntry->typeExpr.jaggedType.is2D)
        {
            if (child->next == NULL || child->next->child->next == NULL)
            {
                // Error...More dimensions supplied than acceptable by 2d jagged array
            }
            if (child->child->terminal == ID || child->next->child->child->terminal == ID)
            {
                // Error
                break;
            }
            else
            {
                int x1 = convertStringToInteger(child->child->tok->lexeme);
                int x2 = convertStringToInteger(child->next->child->child->tok->lexeme);
                if (x1 > tEntry->typeExpr.jaggedType.r1High || x1 < tEntry->typeExpr.jaggedType.r1Low)
                {
                    // 1st dimension out of bounds
                }
                else if (x2 > tEntry->typeExpr.jaggedType.type.twod_array.size[x1 - tEntry->typeExpr.jaggedType.r1Low] || x2 < 1)
                {
                    // 2nd dimension out of bounds
                }
            }
        }
        else if (tEntry->type == JAGGED_ARRAY && !tEntry->typeExpr.jaggedType.is2D)
        {
            if (child->next == NULL || child->next->child->next == NULL || child->next->child->next->child->next == NULL)
            {
                // Error...More dimensions supplied than acceptable by 2d jagged array
            }
            if (child->child->terminal == ID || child->next->child->child->terminal == ID)
            {
                // Error
                break;
            }
            else
            {
                int x1 = convertStringToInteger(child->child->tok->lexeme);
                int x2 = convertStringToInteger(child->next->child->child->tok->lexeme);
                int x3 = convertStringToInteger(child->next->child->next->child->child->tok->lexeme);
                if (x1 > tEntry->typeExpr.jaggedType.r1High || x1 < tEntry->typeExpr.jaggedType.r1Low)
                {
                    // 1st dimension out of bounds
                }
                else if (x2 > tEntry->typeExpr.jaggedType.type.threed_array.sizeR2[x1 - tEntry->typeExpr.jaggedType.r1Low] || x2 < 1)
                {
                    // 2nd dimension out of bounds
                }
                else if (x3 > tEntry->typeExpr.jaggedType.type.threed_array.size[x1 - tEntry->typeExpr.jaggedType.r1Low][x2 - 1] || x3 < 1)
                {
                    // 3rd dimension out of bounds
                }
            }
        }
        root->type = PRIMITIVE; //Proxy for array element
        root->expression.primitiveType = PRIM_INTEGER;
        populateNodeFromNode(root->parent, root);
        break;
    }
    default:
    {
        printf("You shouldn't be here\n");
    }
    }
}
