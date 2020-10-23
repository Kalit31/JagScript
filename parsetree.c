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
    TreeNode *child = node->nodeType.nonLeafNode.child;
    while (child != NULL)
    {
        TreeNode *prev = child;
        child = child->next;
        /*if (prev->isLeaf)
        {
            printf("DELETING %s\n", TOKENS[prev->nodeType.leafNode.terminal]);
        }
        else
        {
            printf("DELETING %s\n", NONTERMINALS[prev->nodeType.nonLeafNode.nonterminal]);
        }*/
        free(prev);
    }
    node->nodeType.nonLeafNode.child = NULL;
    return;
}

int build(TreeNode *curr, Stack *st, Token *s, Node *g)
{
    StackNode *t = top(st);
    if (s == NULL && t == NULL)
    {
        return 1;
    }
    else if (s == NULL || t == NULL)
    {
        return 0;
    }

    if (t->terminal)
    {
        int tokenIdTopStack = t->id;
        int tokenIdTokenStream = s->tokenName;
        if (tokenIdTopStack == tokenIdTokenStream)
        {
            //printf("CURR TREE NODE: %s\n", TOKENS[curr->nodeType.leafNode.terminal]);
            //printf("POPPING %s\n", TOKENS[tokenIdTopStack]);
            pop(st);
            curr->nodeType.leafNode.tok = s;
            if (curr->next == NULL)
            {
                TreeNode *temp = curr->parent;
                while (temp->next == NULL)
                {
                    if (temp->parent == NULL)
                    {
                        return 1;
                    }
                    temp = temp->parent;
                }
                return build(temp->next, st, s->next, g);
            }
            return build(curr->next, st, s->next, g);
        }
        else
        {
            //printf("Could not match %s with %s\n", TOKENS[tokenIdTopStack], TOKENS[tokenIdTokenStream]);
            return 0;
        }
    }
    else
    {
        int nonTerminalID = t->id;

        int firstRuleNo = findRuleIndex(nonTerminalID, g);
        int lastRuleNo = findLastRuleIndex(nonTerminalID, g);
        //printf("POPPING %s\n", NONTERMINALS[st->head->val->nodeType.nonLeafNode.nonterminal]);
        pop(st);

        int initialStackSize = st->size;
        for (int rule = firstRuleNo; rule <= lastRuleNo; rule++)
        {
            int rhsSize = g[rule].ruleSize;
            Node *list[rhsSize];

            int ptrInd = rhsSize - 1;
            Node *c = g[rule].nxt;
            for (; ptrInd >= 0; ptrInd--)
            {
                list[ptrInd] = c;
                c = c->nxt;
            }

            TreeNode *childrenList[rhsSize];
            TreeNode *child = curr->nodeType.nonLeafNode.child;
            TreeNode *currSibling = child;
            int ptr = 0;
            for (int i = rhsSize - 1; i >= 0; i--)
            {
                TreeNode *n;

                if (list[i]->element[0] >= 65 && list[i]->element[0] <= 90)
                {
                    n = createLeafNode(findTerminal(list[i]->element));
                }
                else
                {
                    n = createNonLeafNode(findNonTerminal(list[i]->element));
                }
                if (child == NULL)
                {
                    child = n;
                    currSibling = n;
                    currSibling->next = NULL;
                }
                else
                {
                    currSibling->next = n;
                    currSibling = currSibling->next;
                }
                n->parent = curr;
                childrenList[ptr] = n;
                ptr++;
            }
            curr->nodeType.nonLeafNode.child = child;

            for (int i = rhsSize - 1; i >= 0; i--)
            {
                push(st, childrenList[i]);
            }

            if (build(child, st, s, g))
            {
                curr->nodeType.nonLeafNode.ruleNo = rule;
                return 1;
            }
            else
            {
                // printf("CURR RULE: %d\n", rule);
                // printf("LAST RULE: %d\n", lastRuleNo);
                // printf("CURRENT NODE: %s\n", s->lexeme);

                while (st->size > initialStackSize)
                {
                    //printf("POPPING IN WHILE LOOP\n");
                    if (pop(st) == NULL)
                    {
                        return 0;
                    };
                }
                deleteChildren(curr);
            }
        }
    }
    return 0;
}

TreeNode *createParseTree(TreeNode *t, Token *s, Node *g)
{
    int error = 0;
    Stack *st = createStack();

    TreeNode *root = initialiseParseTree();
    push(st, root);

    build(root, st, s, g);
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
        TreeNode *declareVarsNode = (root->nodeType.nonLeafNode.child)->next;
        TreeNode *declareJagged = (declareVarsNode->next)->next;
        JaggedArray jaggArr;
        // TODO
        traverseParseTree(declareJagged, table);
        traverseParseTree(declareVarsNode, table);
        break;
    }
    case declare_jagged:
    {
        TreeNode *child = root->nodeType.nonLeafNode.child;
        traverseParseTree(child, table);
        break;
    }
    case declare_twod_jagged:{
        TreeNode *child = root->nodeType.nonLeafNode.child;
        root->nodeType.nonLeafNode.type = JAGGED_ARRAY;
        root->nodeType.nonLeafNode.expression = root->parent->nodeType.nonLeafNode.expression;
        JaggedArray jaggedArray = root->nodeType.nonLeafNode.expression.jaggedType;
        jaggedArray.r1Low = convertStringToInteger(nthChild(root, 4)->nodeType.leafNode.tok->lexeme);
        jaggedArray.r1High = convertStringToInteger(nthChild(root, 6)->nodeType.leafNode.tok->lexeme);
        jaggedArray.is2D = 1;
        jaggedArray.type.twod_array.x=0;
        jaggedArray.type.twod_array.y=0;
        jaggedArray.type.twod_array.size = malloc(sizeof(int)*(jaggedArray.r1High-jaggedArray.r1Low));
        jaggedArray.type.twod_array.r2 = (int **)malloc(sizeof(int *)*(jaggedArray.r1High-jaggedArray.r1Low));
        root->nodeType.nonLeafNode.expression.jaggedType = jaggedArray;
        traverseParseTree(nthChild(root, 12), table);
        break;
    }
    case twod_jagged_statements:{
        root->nodeType.nonLeafNode.expression = root->parent->nodeType.nonLeafNode.expression;
        traverseParseTree(nthChild(root, 0), table);
        traverseParseTree(nthChild(root, 1), table);
        break;
    }
    case twod_jagged_statement:{
        root->nodeType.nonLeafNode.expression = root->parent->nodeType.nonLeafNode.expression;
        JaggedArray jaggArr = root->nodeType.nonLeafNode.expression.jaggedType;
        int size = convertStringToInteger(nthChild(root, 5)->nodeType.leafNode.tok->lexeme);
        jaggArr.type.twod_array.size[jaggArr.type.twod_array.x] = size;
        jaggArr.type.twod_array.r2[jaggArr.type.twod_array.x] = malloc(sizeof(int)*size);
        root->nodeType.nonLeafNode.expression.jaggedType = jaggArr;
        traverseParseTree(nthChild(root, 10), table);
        root->nodeType.nonLeafNode.expression.jaggedType.type.twod_array.x++;
        traverseParseTree(nthChild(root, 1), table);
        break;
    }
    case twod_values:{
        root->nodeType.nonLeafNode.expression = root->parent->nodeType.nonLeafNode.expression;
        JaggedArray jaggArr = root->nodeType.nonLeafNode.expression.jaggedType;
        (jaggArr.type.twod_array.r2[jaggArr.type.twod_array.x])[jaggArr.type.twod_array.y] = convertStringToInteger(nthChild(root, 0)->nodeType.leafNode.tok->lexeme);
        jaggArr.type.twod_array.y++;
        root->nodeType.nonLeafNode.expression.jaggedType = jaggArr;
        break;
    }
    case declare_threed_jagged:{
        TreeNode *child = root->nodeType.nonLeafNode.child;
        root->nodeType.nonLeafNode.type = JAGGED_ARRAY;
        root->nodeType.nonLeafNode.expression = root->parent->nodeType.nonLeafNode.expression;
        JaggedArray jaggedArray;
        jaggedArray.r1Low = convertStringToInteger(nthChild(root, 4)->nodeType.leafNode.tok->lexeme);
        jaggedArray.r1High = convertStringToInteger(nthChild(root, 6)->nodeType.leafNode.tok->lexeme);
        jaggedArray.is2D = 0;
        root->nodeType.nonLeafNode.expression.jaggedType = jaggedArray;
        traverseParseTree(nthChild(root, 14), table);
        break;
    }
    case threed_jagged_statements:{
        root->nodeType.nonLeafNode.expression = root->parent->nodeType.nonLeafNode.expression;
        traverseParseTree(nthChild(root, 0), table);
        traverseParseTree(nthChild(root, 1), table);
        traverseParseTree(nthChild(root, 10), table);
        break;
    }
    case threed_jagged_statement:{
        int size = convertStringToInteger(nthChild(root, 5)->nodeType.leafNode.tok->lexeme);
        break;
    }
    case threed_values:{
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
    }
}
