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

    if (t->val->isLeaf)
    {
        int tokenIdTopStack = t->val->nodeType.leafNode.terminal;
        int tokenIdTokenStream = s->tokenName;
        if (tokenIdTopStack == tokenIdTokenStream)
        {
            //printf("CURR TREE NODE: %s\n", TOKENS[curr->nodeType.leafNode.terminal]);
            //printf("POPPING %s\n", TOKENS[tokenIdTopStack]);
            pop(st);
            t->val->nodeType.leafNode.tok = s;
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
        int nonTerminalID = t->val->nodeType.nonLeafNode.nonterminal;

        int firstRuleNo = findRuleIndex(nonTerminalID, g);
        int lastRuleNo = findLastRuleIndex(nonTerminalID, g);
        //printf("POPPING %s\n", NONTERMINALS[st->head->val->nodeType.nonLeafNode.nonterminal]);
        pop(st);

        int initialStackSize = st->size;
        Token *temp = s;
        for (int rule = firstRuleNo; rule <= lastRuleNo; rule++)
        {
            temp = s;
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
                /*if (childrenList[i]->isLeaf)
                {
                    printf("PUSHING %s\n", TOKENS[childrenList[i]->nodeType.leafNode.terminal]);
                }
                else
                {
                    printf("PUSHING %s\n", NONTERMINALS[childrenList[i]->nodeType.nonLeafNode.nonterminal]);
                }*/
                push(st, childrenList[i]);
            }

            if (build(child, st, temp, g))
            {
                curr->nodeType.nonLeafNode.ruleNo = rule;
                return 1;
            }
            else
            {
                deleteChildren(curr);
                while (st->size > initialStackSize)
                {
                    if (pop(st) == NULL)
                    {
                        return 0;
                    };
                }
            }
        }
    }
    return 0;
}

void typeCheckVariableIsInteger(TypeExprEntry *table, char *varName)
{
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
    if (nonTerminalId == 0)
    {
        // curr node is 's' : Start Symbol
        traverseParseTree(root->nodeType.nonLeafNode.child, table);
    }
    else if (nonTerminalId == 1)
    {
        // curr node is list of statements
        TreeNode *declStmts = root->nodeType.nonLeafNode.child;
        //Traverse Declaration Subtree
        traverseParseTree(declStmts, table);
        //Traverse Assignment Subtree
        TreeNode *assgnStmts = declStmts->next;
        traverseParseTree(assgnStmts, table);
    }
    else if (nonTerminalId == 2)
    {
        // curr node is declaration statements
        TreeNode *singleDeclaration = root->nodeType.nonLeafNode.child;

        //Traverse single declaration statement
        traverseParseTree(singleDeclaration, table);

        if (singleDeclaration->next != NULL)
        {
            traverseParseTree(singleDeclaration->next, table);
        }
    }
    else if (nonTerminalId == 4)
    {
        // curr node is a single declaration statement;
        TreeNode *typeOfDecl = root->nodeType.nonLeafNode.child;
        traverseParseTree(typeOfDecl, table);
    }
    else if (nonTerminalId == 5)
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
    }
    else if (nonTerminalId == 6)
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
                            typeCheckVariableIsInteger(table, term->nodeType.leafNode.tok->lexeme);
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
                            typeCheckVariableIsInteger(table, term->nodeType.leafNode.tok->lexeme);
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
    }
    else if (nonTerminalId == 10)
    {
        // curr node is rect_arr
        // TreeNode *parent = root->parent;
        // root->nodeType.nonLeafNode.type = parent->nodeType.nonLeafNode.type;
        // RectangularArray rectArr;
        // rectArr.currDimensions = 0;
        // traverseParseTree(root->nodeType.nonLeafNode.child, table);
    }
    else if (nonTerminalId == 13)
    {
        // TreeNode *child = root->nodeType.nonLeafNode.child;
        // while (child->next != NULL)
        // {
        //     if (child->isLeaf == 0)
        //     {
        //         if(child)
        //     }
        //     child = child->next;
        // }
    }
    else if (nonTerminalId == 7)
    {
        // curr node is jagged arrayd declaration
        root->nodeType.nonLeafNode.type = JAGGED_ARRAY;
    }
    else if (nonTerminalId == 8)
    {
        // curr node is declare_vars node
        TreeNode *child = root->nodeType.nonLeafNode.child;
        if (child->next == NULL)
        {
            // single variable
            TreeNode *parent = root->parent;
            TreeNode *IDNode = root->nodeType.nonLeafNode.child;
            table[currentTableEntry].name = IDNode->nodeType.leafNode.tok->lexeme;
            table[currentTableEntry].type = parent->nodeType.nonLeafNode.type;
            if (table[currentTableEntry].type == RECTANGULAR_ARRAY)
            {
                // To be done
            }
            else
            {
                table[currentTableEntry].rectArrayType = NOT_APPLICABLE;
            }
            table[currentTableEntry].typeExpr = parent->nodeType.nonLeafNode.expression;
            currentTableEntry++;
        }
        else
        {
            // list of variables
            traverseParseTree(child, table);
        }
    }
    else if (nonTerminalId == 12)
    {
        // curr node is var_name_list
        TreeNode *parent = root->parent; // Parent is declare_vars or var_names_list
        TreeNode *child = root->nodeType.nonLeafNode.child;

        table[currentTableEntry].name = child->nodeType.leafNode.tok->lexeme;
        table[currentTableEntry].type = parent->nodeType.nonLeafNode.type;
        if (table->type == RECTANGULAR_ARRAY)
        {
            // To be done
        }
        else
        {
            table[currentTableEntry].rectArrayType = NOT_APPLICABLE;
        }
        table[currentTableEntry].typeExpr = parent->nodeType.nonLeafNode.expression;
        currentTableEntry++;

        if (child->next != NULL)
        {
            // next node is var_name_list
            TreeNode *varNameListNode = child->next;
            varNameListNode->nodeType.nonLeafNode.type = parent->nodeType.nonLeafNode.type;
            varNameListNode->nodeType.nonLeafNode.expression = parent->nodeType.nonLeafNode.expression;
            traverseParseTree(varNameListNode, table);
        }
    }
    return;
}
