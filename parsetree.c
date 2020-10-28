#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parsetree.h"
#include "typeExpression.h"
#include "naryTree.h"
#include "stack.h"
#include "print.h"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

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

int build(TreeNode *curr, Stack *st, Node *g, int depth)
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
                n->depth = depth;
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
                if (build(n, st, g, depth + 1) == 0)
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
    Stack *st = createStack();
    TreeNode *root = initialiseParseTree();
    push(st, root);
    globalTokenPtr = s;
    int success = build(root, st, g, 1);
    //root = buildNew(root, g);
    if (success)
        printf("\n------------------------------------------------PARSE TREE IS CREATED SUCCESSFULLY--------------------------------------------\n\n");
    else
    {
        printf("\n--------------------------------------------Syntax Error : Failed to Create Parse Tree----------------------------------------\n\n");
    }

    return root;
}

void printArrayError(char *name, int type, char *message, int lineno)
{
    char *typeName;
    switch (type)
    {
    case 0:
    {
        typeName = "PRIMITIVE";
        break;
    }
    case 1:
    {
        typeName = "RECTANGULAR ARRAY";
        break;
    }
    case 2:
    {
        typeName = "JAGGED ARRAY";
        break;
    }
    }
    printf("%d\t\tDECLARATION\t\t     ***%15s%22s\t\t    ***\t\t\t  ***\t\t  ***\t%25s"
           "\n",
           lineno,
           name, typeName, message);
}

void printTypeCheckError(TreeNode *a, TreeNode *b, Terminal operator, char * message)
{
    char *typeName1;
    char *typeName2;
    switch (a->type)
    {
    case 0:
    {
        typeName1 = "PRIMITIVE";
        break;
    }
    case 1:
    {
        typeName1 = "RECTANGULAR ARRAY";
        break;
    }
    case 2:
    {
        typeName1 = "JAGGED ARRAY";
        break;
    }
    }
    switch (b->type)
    {
    case 0:
    {
        typeName2 = "PRIMITIVE";
        break;
    }
    case 1:
    {
        typeName2 = "RECTANGULAR ARRAY";
        break;
    }
    case 2:
    {
        typeName2 = "JAGGED ARRAY";
        break;
    }
    }
    printf("%d\t\tASSIGNMENT%22s%15s%22s%18s%22s\t%13d\t%25s"  "\n", a->tok->lineNo,TOKENS[operator],a->tok->lexeme
    ,typeName1, b->tok->lexeme,typeName2,b->depth,message);
}

void checkArrayVariableIsInteger(Token *tok)
{
    char *varName = tok->lexeme;
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
        if (table[i].typeExpr.primitiveType == PRIM_BOOLEAN)
        {
            printArrayError(table[i].name, table[i].type, "Array index var is BOOLEAN", tok->lineNo);
        }
        else if (table[i].typeExpr.primitiveType == PRIM_REAL)
        {
            printArrayError(table[i].name, table[i].type, "Array index var is REAL", tok->lineNo);
        }
    }
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

void populateTypeExpressionTable(TreeNode *root)
{
    if (TYPETABLESIZE == 0)
    {
        table = (TypeExprEntry *)malloc(sizeof(TypeExprEntry) * 1);
        TYPETABLESIZE = 1;
    }
    else if (currentTableEntry == TYPETABLESIZE)
    {
        TYPETABLESIZE = TYPETABLESIZE * 2;
        table = (TypeExprEntry *)realloc(table, sizeof(TypeExprEntry) * TYPETABLESIZE);
    }

    TreeNode *parent = root->parent;
    table[currentTableEntry].name = (char *)malloc(sizeof(char) * (strlen(root->tok->lexeme) + 1));
    strcpy(table[currentTableEntry].name, root->tok->lexeme);

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

void populateNodeWithTypeExpression(TreeNode *node, TreeNode *term)
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
    node->typeInformationStored = 1;
    node->type = table[i].type;
    node->expression = table[i].typeExpr;
    node->tok = term->tok; //lexeme : v1, line no : 4
}

void populateNodeFromNode(TreeNode *a, TreeNode *b)
{
    a->typeInformationStored = 1;
    a->type = b->type;
    a->expression = b->expression;
    a->tok = b->tok;
}

void checkArrayCompatibility(TreeNode *node1, TreeNode *node2, Terminal operation)
{
    // Check if both arrays are of same dimensions
    if (node1->type == node2->type)
    {
        if (node1->type == RECTANGULAR_ARRAY)
        {
            if (node1->expression.rectType.currDimensions != node2->expression.rectType.currDimensions)
            {
                // Raise incompatibility error
                printTypeCheckError(node1, node2, operation, "Arrays are incompatible");
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
                        printTypeCheckError(node1, node2, operation, "Array dimensions do not match");
                    }
                }
            }
        }
        else if (node1->type == JAGGED_ARRAY)
        {
            if (!node1->expression.jaggedType.isValid || !node2->expression.jaggedType.isValid)
            {
                // There was some issue with declaration earlier
                printTypeCheckError(node1, node2, operation, "Type definition carries error");
            }
            else
            {
                if (node1->expression.jaggedType.is2D != node2->expression.jaggedType.is2D)
                {
                    // Raise error... Array types not compatible
                    printTypeCheckError(node1, node2, operation, "Arrays are incompatible");
                }
                else
                {
                    if (node1->expression.jaggedType.r1Low != node2->expression.jaggedType.r1Low ||
                        node1->expression.jaggedType.r1High != node2->expression.jaggedType.r1High)
                    {
                        // Incompatible dimensions
                        printTypeCheckError(node1, node2, operation, "JA size mismatch");
                    }
                    else
                    {
                        if (node1->expression.jaggedType.is2D)
                        {
                            // 2D Jagged Array
                            for (int i = 0;
                                 i <
                                 node1->expression.jaggedType.r1High - node1->expression.jaggedType.r1Low + 1;
                                 i++)
                            {
                                if (node1->expression.jaggedType.type.twod_array.size[i] !=
                                    node2->expression.jaggedType.type.twod_array.size[i])
                                {
                                    // Array dimensions incompatible
                                    printTypeCheckError(node1, node2, operation, "2D JA size mismatch");
                                }
                            }
                        }
                        else
                        {
                            // 3D Jagged Array
                            // Check compatibility for 2nd dimension
                            int incomaptibilityFlag2ndDim = 0;
                            for (int i = 0;
                                 i <
                                 node1->expression.jaggedType.r1High - node1->expression.jaggedType.r1Low + 1;
                                 i++)
                            {
                                if (node1->expression.jaggedType.type.threed_array.sizeR2[i] !=
                                    node2->expression.jaggedType.type.threed_array.sizeR2[i])
                                {
                                    // Array dimensions incompatible
                                    incomaptibilityFlag2ndDim = 1;
                                }
                            }
                            // check compatibility for 3rd dimension
                            if (!incomaptibilityFlag2ndDim)
                            {
                                for (int i = 0; i <
                                                node1->expression.jaggedType.r1High -
                                                node1->expression.jaggedType.r1Low +
                                                1; i++) {
                                    for (int j = 0; j < node1->expression.jaggedType.type.threed_array.sizeR2[i]; j++) {
                                        // Array dimensions incompatible
                                        if (node1->expression.jaggedType.type.threed_array.size[i][j] !=
                                            node2->expression.jaggedType.type.threed_array.size[i][j])
                                        {
                                            // Array dimensions incompatible
                                            printTypeCheckError(node1, node2, operation, "3D JA size mismatch");
                                        }
                                    }
                                }
                            }
                            else
                            {
                                // Mismatch in second dimension of 3D Jagged Array
                                printTypeCheckError(node1, node2, operation, "3D JA size mismatch");
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
        printTypeCheckError(node1, node2, operation, "Arrays are incompatible");
    }
}

void performTypeChecking(TreeNode *root, TreeNode *rightExpr, TreeNode *operation)
{
    Terminal op = operation->terminal;

    int typeParent = root->type;
    int typeRightExpr = rightExpr->type;

    if (typeParent != typeRightExpr)
    {
        printTypeCheckError(root, rightExpr, op, "Type Mismatch");
    }
    else
    {
        switch (typeParent)
        {
        case PRIMITIVE:
        {
            int typeExprPrimOfParent = root->expression.primitiveType;
            int typeExprPrimOfRightExpr = rightExpr->expression.primitiveType;

            if (typeExprPrimOfParent != typeExprPrimOfRightExpr)
            {
                printTypeCheckError(root, rightExpr, op, "Type Mismatch");
            }
            switch (op)
            {
            case PLUS:
            {
                if (typeExprPrimOfParent == PRIM_BOOLEAN)
                {
                    printTypeCheckError(root, rightExpr, op, "Bool var cannot be added");
                }
                break;
            }
            case MINUS:
            {
                if (typeExprPrimOfParent == PRIM_BOOLEAN)
                {
                    printTypeCheckError(root, rightExpr, op, "Bool var cannot be subtracted");
                }
                break;
            }
            case MULT:
            {
                if (typeExprPrimOfParent == PRIM_BOOLEAN)
                {
                    printTypeCheckError(root, rightExpr, op, "Bool var cannot be multiplied");
                }
                break;
            }
            case DIV:
            {
                if (typeExprPrimOfParent == PRIM_BOOLEAN)
                {
                    printTypeCheckError(root, rightExpr, op, "Bool var cannot be divided");
                    return;
                }
                root->expression.primitiveType = PRIM_REAL;
                break;
            }
            case EQUALS:
            {
                break;
            }
            case OR:
            {
                if (typeExprPrimOfParent == PRIM_INTEGER)
                {
                    printTypeCheckError(root, rightExpr, op, "OR operation on integer var");
                }
                else if (typeExprPrimOfParent == PRIM_REAL)
                {
                    printTypeCheckError(root, rightExpr, op, "OR operation on real var");
                }
                break;
            }
            case AND:
            {
                if (typeExprPrimOfParent == PRIM_INTEGER)
                {
                    printTypeCheckError(root, rightExpr, op, "AND operation on integer var");
                }
                else if (typeExprPrimOfParent == PRIM_REAL)
                {
                    printTypeCheckError(root, rightExpr, op, "AND operation on real var");
                }
                break;
            }
            }
            break;
        }
        case RECTANGULAR_ARRAY:
        {
            checkArrayCompatibility(root, rightExpr, op);
            break;
        }
        case JAGGED_ARRAY:
        {
            checkArrayCompatibility(root, rightExpr, op);
            break;
        }
        }
    }
}

void traverseParseTree(TreeNode *root)
{
    if (root->isLeaf == 1 && root->terminal == ID)
    {
        populateTypeExpressionTable(root);
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
        traverseParseTree(root->child);
        break;
    }
    case list_of_statements:
    {
        // curr node is list of statements
        TreeNode *declStmts = root->child;
        //Traverse Declaration Subtree
        traverseParseTree(declStmts);
        //Traverse Assignment Subtree
        TreeNode *assgnStmts = declStmts->next;
        traverseParseTree(assgnStmts);
        break;
    }
    case declaration_statements:
    {
        // curr node is declaration statements
        TreeNode *singleDeclaration = root->child;

        //Traverse single declaration statement
        traverseParseTree(singleDeclaration);

        if (singleDeclaration->next != NULL)
        {
            traverseParseTree(singleDeclaration->next);
        }
        break;
    }
    case declaration:
    {
        // curr node is a single declaration statement;
        TreeNode *typeOfDecl = root->child;
        traverseParseTree(typeOfDecl);
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
        root->typeInformationStored = 1;
        root->expression.primitiveType = primType;
        // Traverse declare_vars node
        traverseParseTree(declareVarsNode);
        break;
    }
    case rect_array_decl:
    {
        // curr node is rectangular array declaration
        root->typeInformationStored = 1;
        root->type = RECTANGULAR_ARRAY;
        RectangularArray rectArr;
        rectArr.currDimensions = 0;
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
                            checkArrayVariableIsInteger(term->tok);
                            rectArr.dimenArray[rectArr.currDimensions][0] = -1;
                            rectArr.dynamicDimenArray[rectArr.currDimensions][0] = term->tok->lexeme;
                        }
                        else
                        {
                            // idx is a NUM
                            int num = convertStringToInteger(term->tok->lexeme);
                            rectArr.dimenArray[rectArr.currDimensions][0] = num;
                        }

                        //dimenChild now points to ..
                        dimenChild = dimenChild->next;
                        //dimenChild now points to idx
                        dimenChild = dimenChild->next;

                        term = dimenChild->child;
                        if (term->terminal == 29)
                        {
                            // idx is a ID
                            checkArrayVariableIsInteger(term->tok);
                            rectArr.dimenArray[rectArr.currDimensions][1] = -1;
                            rectArr.dynamicDimenArray[rectArr.currDimensions][1] = term->tok->lexeme;
                        }
                        else
                        {
                            // idx is a NUM
                            rectArr.dimenArray[rectArr.currDimensions][1] = convertStringToInteger(term->tok->lexeme);
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
                }
            }
            rectDeclChild = rectDeclChild->next;
        }
        rectArr.currDimensions++;
        root->expression.rectType = rectArr;
        traverseParseTree(declareVarsNode);
        break;
    }
    case jagged_array_decl:
    {
        // curr node is jagged arrayd declaration
        root->type = JAGGED_ARRAY;
        TreeNode *declareVarsNode = nthChild(root, 1);
        TreeNode *declareJagged = nthChild(root, 3);
        traverseParseTree(declareJagged);
        root->expression = nthChild(root, 3)->expression;
        traverseParseTree(declareVarsNode);
        break;
    }
    case declare_jagged:
    {
        TreeNode *child = root->child;
        traverseParseTree(child);
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
        jaggedArray.isValid = 1;

        jaggedArray.type.twod_array.size = malloc(sizeof(int) * (jaggedArray.r1High - jaggedArray.r1Low + 1));
        root->expression.jaggedType = jaggedArray;
        traverseParseTree(nthChild(root, 12));
        root->expression = nthChild(root, 12)->expression;
        break;
    }
    case twod_jagged_statements:
    {
        root->expression = root->parent->expression;
        root->tok = root->parent->tok;
        TreeNode *statement = root->child;
        JaggedArray jaggArr = root->expression.jaggedType;
        int x = 0, y = 0, r1Low, r1High, r1Len;
        r1Low = jaggArr.r1Low;
        r1High = jaggArr.r1High;
        r1Len = r1High - r1Low + 1;

        while (statement != NULL)
        {
            if (x >= r1Len)
            {
                printArrayError("***", 2, "More rows than expected", statement->child->tok->lineNo);
                jaggArr.isValid = 0;
                // printf("Recieved More rows than expected in Jagged Array declaration\n");
                break;
            }
            statement->expression = statement->parent->expression;
            int index = convertStringToInteger(nthChild(statement, 2)->tok->lexeme);
            if (index != r1Low + x)
            {
                printArrayError("***", 2, "Incorrect order of rows", statement->child->tok->lineNo);
                jaggArr.isValid = 0;
                //printf("Incorrect order/index of rows for Jagged Arrays declaration\n");
            }
            int size = convertStringToInteger(nthChild(statement, 6)->tok->lexeme);
            jaggArr.type.twod_array.size[x] = size;
            statement->expression.jaggedType = jaggArr;

            TreeNode *twod_values = nthChild(statement, 10);
            y = 0;
            while (1)
            {
                twod_values->expression = twod_values->parent->expression;
                if (twod_values->child->child->next->child->terminal != EPS)
                {
                    printArrayError("***", 2, "Type Definition Error", statement->child->tok->lineNo);
                    jaggArr.isValid = 0;
                    //printf("Type Definition Error for 2D Jagged Array\n");
                }
                y++;
                if (nthChild(twod_values, 1) != NULL)
                    twod_values = nthChild(twod_values, 2);
                else
                    break;
            }
            if (y != size)
            {
                printArrayError("***", 2, "Expected values not found", statement->child->tok->lineNo);
                jaggArr.isValid = 0;
                // printf("Expected %d values in declaration, but got %d values\n", size, y);
            }
            x++;
            if (statement->next != NULL)
            {
                statement = statement->next->child;
            }
            else
                break;
        }
        if (x < r1Len)
        {
            printArrayError("***", 2, "Expected values not found", root->child->child->tok->lineNo);
            jaggArr.isValid = 0;
            //printf("Expected %d number of rows in jagged array declaration, but obtained %d rows\n", r1Len, x);
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
        jaggedArray.isValid = 1;
        jaggedArray.type.threed_array.sizeR2 = malloc(sizeof(int) * (jaggedArray.r1High - jaggedArray.r1Low + 1));
        jaggedArray.type.threed_array.size = malloc(sizeof(int *) * (jaggedArray.r1High - jaggedArray.r1Low + 1));
        root->expression.jaggedType = jaggedArray;
        traverseParseTree(nthChild(root, 14));
        root->expression = nthChild(root, 14)->expression;
        break;
    }
    case threed_jagged_statements:
    {
        root->expression = root->parent->expression;
        root->tok = root->parent->tok;
        TreeNode *statement = root->child;
        int x = 0, y = 0, r1Low, r1High, r1Len;
        JaggedArray jaggArr = root->expression.jaggedType;
        r1Low = jaggArr.r1Low;
        r1High = jaggArr.r1High;
        r1Len = r1High - r1Low + 1;
        while (statement != NULL)
        {
            if (x >= r1Len)
            {
                printArrayError("***", 2, "More rows than expected", statement->child->tok->lineNo);
                jaggArr.isValid = 0;
                break;
            }
            statement->expression = statement->parent->expression;
            int index = convertStringToInteger(nthChild(statement, 2)->tok->lexeme);
            if (index != r1Low + x)
            {

                printArrayError("***", 2, "Incorrect order of rows", statement->child->tok->lineNo);
                jaggArr.isValid = 0;
            }
            int size = convertStringToInteger(nthChild(statement, 6)->tok->lexeme);
            jaggArr.type.threed_array.sizeR2[x] = size;
            statement->expression.jaggedType = jaggArr;

            TreeNode *threed_values = nthChild(statement, 10);

            jaggArr.type.threed_array.size[x] = malloc(sizeof(int) * size);

            y = 0;
            while (threed_values != NULL)
            {
                threed_values->expression = threed_values->parent->expression;
                jaggArr.type.threed_array.size[x][y] = 0;
                TreeNode *threed_list = nthChild(threed_values, 0);
                while (threed_list->child->terminal != EPS)
                {
                    threed_list->expression = threed_list->parent->expression;
                    jaggArr.type.threed_array.size[x][y]++;
                    if (threed_list->child->next != NULL)
                    {

                        threed_list = nthChild(threed_list, 1);
                    }
                    else
                        break;
                }
                y++;
                if (nthChild(threed_values, 1) != NULL)
                    threed_values = nthChild(threed_values, 2);
                else
                    break;
            }
            if (y != size)
            {

                printArrayError("***", 2, "Expected values not found", statement->child->tok->lineNo);
                jaggArr.isValid = 0;
                //printf("Expected %d values in declaration, but got %d values\n", size, y);
            }
            x++;
            if (statement->next != NULL)
            {
                statement = statement->next->child;
            }
            else
                break;
        }
        if (x < r1Len)
        {

            printArrayError("***", 2, "Expected values not found", root->child->child->tok->lineNo);
            jaggArr.isValid = 0;
            //printf("Expected %d number of rows in jagged array declaration, but obtained %d rows\n", r1Len, x);
        }
        root->expression.jaggedType = jaggArr;
        break;
    }
    case declare_vars:
    {
        // curr node is declare_vars node
        // parent is primitive_decl/rect_array_decl/jagged_array_decl
        root->typeInformationStored = 1;
        root->type = root->parent->type;
        root->expression = root->parent->expression;
        TreeNode *child = root->child;
        if (child->next == NULL)
        {
            // single variable(ID)
            traverseParseTree(child);
        }
        else
        {
            // LIST OF VARIABLES var_name_list
            // list of variables
            traverseParseTree(child);
        }
        break;
    }
    case var_name_list:
    {
        // curr node is var_name_list
        TreeNode *parent = root->parent; // Parent is declare_vars or var_names_list
        root->typeInformationStored = 1;
        root->type = parent->type;
        root->expression = parent->expression;

        TreeNode *child = root->child;
        traverseParseTree(child);

        if (child->next != NULL)
        {
            // next node is var_name_list
            TreeNode *varNameListNode = child->next;
            traverseParseTree(varNameListNode);
        }
        break;
    }
    case assignment_statements:
    {
        // curr node is assignment statements
        TreeNode *singleAssignment = root->child;

        //Traverse single assignment statement
        traverseParseTree(singleAssignment);

        if (singleAssignment->next != NULL)
        {
            traverseParseTree(singleAssignment->next);
        }
        break;
    }
    case assignment:
    {
        // curr node is assignment
        TreeNode *lhsNode = root->child;
        //Traverse LHS
        traverseParseTree(lhsNode);

        TreeNode *operation = lhsNode->next;
        TreeNode *rhsNode = operation->next;
        //Traverse RHS
        traverseParseTree(rhsNode);
        performTypeChecking(lhsNode, rhsNode, operation);
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
            root->typeInformationStored = 1;
            root->type = tEntry->type;
            root->expression = tEntry->typeExpr;
            root->tok = child->tok;
        }
        else
        {
            // lhs is array element
            root->typeInformationStored = 1;
            root->type = PRIMITIVE;
            root->expression.primitiveType = PRIM_INTEGER;
            traverseParseTree(child);
        }
        break;
    }
    case rhs:
    {
        // curr node is rhs
        // child is arithmetic expression or boolean expression
        TreeNode *child = root->child;
        traverseParseTree(child);
        populateNodeFromNode(root, child);
        break;
    }
    case arithmetic_expr:
    {
        // curr node is arithmetic expression
        TreeNode *arithmeticTerm = root->child;
        traverseParseTree(arithmeticTerm);
        populateNodeFromNode(root, arithmeticTerm);

        if (arithmeticTerm->next == NULL)
        {
        }
        else
        {
            // op1 would be either PLUS or MINUS or OR node
            TreeNode *op1 = arithmeticTerm->next;
            TreeNode *arithExpr = op1->next;
            traverseParseTree(arithExpr);

            // do type checking for arithTerm and arithExpr
            performTypeChecking(root, arithExpr, op1->child);
        }
        break;
    }
    case arithmetic_term:
    {
        // parent is arith_expr or arith_term
        TreeNode *parent = root->parent;
        TreeNode *idexpr = root->child;
        traverseParseTree(idexpr);

        if (idexpr->next == NULL)
        {
            if (parent->child == root)
            {
                populateNodeFromNode(parent, root);
            }
        }

        if (idexpr->next != NULL)
        {
            //op2 would be either MULT or DIV node
            TreeNode *op2 = idexpr->next;
            TreeNode *arithTerm = op2->next;
            traverseParseTree(arithTerm);
            //do type checking for idx and arithTerm;
            performTypeChecking(root, arithTerm, op2->child);
            populateNodeFromNode(parent, root);
        }
        break;
    }
    case idexpr:
    {
        TreeNode *child = root->child;
        if (child->isLeaf)
        {
            // ID Node or NUM Node
            populateNodeWithTypeExpression(root, child);
            populateNodeFromNode(root->parent, root);
        }
        else
        {
            // Array Element
            // Check whether we are correctly accessing the array
            traverseParseTree(child);
            populateNodeFromNode(root->parent, root); //root->parent->tok = root->tok;
        }
        break;
    }
    case array_ele:
    {
        char *lex = root->child->tok->lexeme;
        root->tok = root->child->tok;
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
            if (root->tok == NULL)
                printArrayError("**", 2, "Array is static", -1);
            else
                printArrayError(root->tok->lexeme, tEntry->type, "Array is static", root->tok->lineNo);
        }
        TreeNode *child = nthChild(root, 2)->child; //list

        if (tEntry->type == RECTANGULAR_ARRAY)
        {
            int i = tEntry->typeExpr.rectType.currDimensions;

            while (1)
            {
                i--;
                if (child->child->terminal == ID)
                {
                    checkArrayVariableIsInteger(root->tok);
                    //Raise error as per pg 11
                    if (root->tok == NULL)
                        printArrayError("**", 2, "Array is dynamic", -1);
                    else
                        printArrayError(root->tok->lexeme, tEntry->type, "Array is dynamic", root->tok->lineNo);
                    break;
                }
                else
                {
                    // It is a Num
                    int idx = convertStringToInteger(child->child->tok->lexeme);
                    if (idx > tEntry->typeExpr.rectType.dimenArray[i][1] ||
                        idx < tEntry->typeExpr.rectType.dimenArray[i][0])
                    {
                        // Index out of Bounds...Raise Error
                        if (root->tok == NULL)
                            printArrayError("**", 2, "Array index out of bounds", -1);
                        else
                            printArrayError(root->tok->lexeme, tEntry->type, "Array index out of bounds", root->tok->lineNo);
                    }
                    if (child->next != NULL)
                        child = child->next->child;
                    else
                        break;
                }
            }
            if (i != 0)
            {
                // Error!!
                if (root->tok == NULL)
                    printArrayError("**", 2, "Array error", -1);
                else
                    printArrayError(root->tok->lexeme, tEntry->type, "Array error", root->tok->lineNo);
            }
        }
        else if (tEntry->type == JAGGED_ARRAY && tEntry->typeExpr.jaggedType.is2D)
        {
            if (child->next == NULL || child->next->child->next != NULL)
            {
                // Error...More dimensions supplied than acceptable by 2d jagged array
                if (root->tok == NULL)
                    printArrayError("**", 2, "Incompatible dimensions", -1);
                else
                    printArrayError(root->tok->lexeme, tEntry->type, "Incompatible dimensions", root->tok->lineNo);
            }
            if (child->child->terminal == ID || child->next->child->child->terminal == ID)
            {
                checkArrayVariableIsInteger(root->tok);
                // Error
                if (root->tok == NULL)
                    printArrayError("**", 2, "Array access error", -1);
                else
                    printArrayError(root->tok->lexeme, tEntry->type, "Array access error", root->tok->lineNo);
                break;
            }
            else
            {
                int x1 = convertStringToInteger(child->child->tok->lexeme);
                int x2 = convertStringToInteger(child->next->child->child->tok->lexeme);
                if (x1 > tEntry->typeExpr.jaggedType.r1High || x1 < tEntry->typeExpr.jaggedType.r1Low)
                {
                    // 1st dimension out of bounds
                    if (root->tok == NULL)
                        printArrayError("**", 2, "Dimension 1 out of bounds", -1);
                    else
                        printArrayError(root->tok->lexeme, tEntry->type, "Dimension 1 out of bounds", root->tok->lineNo);
                }
                else if (x2 > tEntry->typeExpr.jaggedType.type.twod_array.size[x1 - tEntry->typeExpr.jaggedType.r1Low] || x2 < 1)
                {
                    // 2nd dimension out of bounds
                    if (root->tok == NULL)
                        printArrayError("**", 2, "Dimension 2 out of bounds", -1);
                    else
                        printArrayError(root->tok->lexeme, tEntry->type, "Dimension 2 out of bounds", root->tok->lineNo);
                }
            }
        }
        else if (tEntry->type == JAGGED_ARRAY && !tEntry->typeExpr.jaggedType.is2D)
        {
            if (child->next == NULL || child->next->child->next == NULL || child->next->child->next->child->next != NULL)
            {
                // Error...More dimensions supplied than acceptable by 2d jagged array
                if (root->tok == NULL)
                    printArrayError("**", 2, "Incompatible dimensions", -1);
                else
                    printArrayError(root->tok->lexeme, tEntry->type, "Incompatible dimensions", root->tok->lineNo);
            }
            if (child->child->terminal == ID || child->next->child->child->terminal == ID)
            {
                checkArrayVariableIsInteger(root->tok);
                // Error
                if (root->tok == NULL)
                    printArrayError("**", 2, "Array access error", -1);
                else
                    printArrayError(root->tok->lexeme, tEntry->type, "Array access error", root->tok->lineNo);
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
                    if (root->tok == NULL)
                        printArrayError("**", 2, "Dimension 1 out of bounds", -1);
                    else
                        printArrayError(root->tok->lexeme, tEntry->type, "Dimension 1 out of bounds", root->tok->lineNo);
                }
                else if (x2 > tEntry->typeExpr.jaggedType.type.threed_array.sizeR2[x1 - tEntry->typeExpr.jaggedType.r1Low] || x2 < 1)
                {
                    // 2nd dimension out of bounds
                    if (root->tok == NULL)
                        printArrayError("**", 2, "Dimension 2 out of bounds", -1);
                    else
                        printArrayError(root->tok->lexeme, tEntry->type, "Dimension 2 out of bounds", root->tok->lineNo);
                }
                else if (x3 > tEntry->typeExpr.jaggedType.type.threed_array.size[x1 - tEntry->typeExpr.jaggedType.r1Low][x2 - 1] || x3 < 1)
                {
                    // 3rd dimension out of bounds
                    if (root->tok == NULL)
                        printArrayError("**", 2, "Dimension 3 out of bounds", -1);
                    else
                        printArrayError(root->tok->lexeme, tEntry->type, "Dimension 3 out of bounds", root->tok->lineNo);
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

void printTypeExpressionTable()
{
    printf("CURR ENTRIES: %d\n", currentTableEntry);
    printf("Field1\t\t\tField2\t\tField3\t\t\tField4\n\n");
    for (int i = 0; i < currentTableEntry; i++)
    {
        printf("%-10s%20s\t\t%-20s\t", table[i].name, printType(table[i].type), printArrayType(table[i].rectArrayType));
        printTypeExpression(table[i].type, table[i].typeExpr);
        printf("\n");
    }
    printf("\n-----------------------------------------------PRINTING TYPE EXPRESSION TABLE COMPLETED---------------------------------------\n\n");
}