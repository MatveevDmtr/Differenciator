#include <stdio.h>


#include "differ.h"
#include "recursive_descent.h"

const char* s = NULL;

int GetG(tree_t* tree, const char* str)
{
    s = str;

    elem_s* root = GetE();

    if (*s == '\0')
    {
        printf("The end\n");
    }
    else
    {
        printf("Syntax error: %c\n", *s);
    }

    tree->Ptr = root;

    return 0;
}

elem_s* GetN()
{
    int val = 0;

    const char* prev_s = s;

    while ('0' <= *s && *s <= '9')
    {
        val = val * 10 + (*s - '0');
        s++;
    }

    if (s == prev_s)
    {
        return GetX();
    }

    return CreateNode(NODE_VAL, {.num_val = val});
}

elem_s* GetX()
{
    if (*s == 'x')
    {
        s++;

        return CreateNode(NODE_VAR, {.var_val = 'x'});
    }
    else
    {
        printf("x not found\n");

        return NULL;
    }
}

elem_s* GetDeg()
{
    elem_s* op_node = GetP();

    while (*s == '^')
    {
        char op = *s;
        s++;

        elem_s* r_node = GetP();
        elem_s* l_node = op_node;

        op_node = CreateNode(NODE_OP, {.op_val = OP_DEG});

        ConnectNodes(op_node, l_node, LEFT);
        ConnectNodes(op_node, r_node, RIGHT);
    }

    return op_node;
}

elem_s* GetE()
{
    elem_s* op_node = GetT();

    while (*s == '+' || *s == '-')
    {
        char op = *s;
        s++;

        elem_s* r_node = GetT();
        elem_s* l_node = op_node;

        if (op == '+')
        {
            op_node = CreateNode(NODE_OP, {.op_val = OP_ADD});
        }
        else
        {
            op_node = CreateNode(NODE_OP, {.op_val = OP_SUB});
        }

        ConnectNodes(op_node, l_node, LEFT);
        ConnectNodes(op_node, r_node, RIGHT);
    }

    return op_node;
}

elem_s* GetT()
{
    elem_s* op_node = GetDeg();

    while (*s == '*' || *s == '/')
    {
        char op = *s;
        s++;

        elem_s* r_node = GetDeg();
        elem_s* l_node = op_node;
        if (op == '*')
        {
            op_node = CreateNode(NODE_OP, {.op_val = OP_MUL});
        }
        else
        {
            op_node = CreateNode(NODE_OP, {.op_val = OP_DIV});
        }

        ConnectNodes(op_node, l_node, LEFT);
        ConnectNodes(op_node, r_node, RIGHT);
    }

    return op_node;
}

elem_s* GetP()
{
    elem_s* node = 0;

    if (*s == '(')
    {
        s++;

        node = GetE();

        if (*s == ')')
        {
            printf("found closing bracket\n");
        }
        else
        {
            printf("syntax error: closing bracket not found\n");
        }

        s++;
    }
    else
    {
        node = GetN();
    }

    return node;
}
