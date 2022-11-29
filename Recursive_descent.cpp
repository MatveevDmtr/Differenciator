#include <stdio.h>
#include <string.h>


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

elem_s* GetTrig()
{
    elem_s* op_node = GetDeg();

    size_t op_val = 0;

    while (op_val = TypeOfTrigF(&s))
    {
        elem_s* r_node = GetDeg();
        elem_s* l_node = NewNum(0);

        op_node = NewOp(op_val);

        ConnectNodes(op_node, l_node, LEFT);
        ConnectNodes(op_node, r_node, RIGHT);
    }

    return op_node;
}

size_t TypeOfTrigF(const char** s)
{
    if      (!strncmp(*s, "sin", 3))
    {
        *s += 3;
        return OP_SIN;
    }
    else if (!strncmp(*s, "cos", 3))
    {
        *s += 3;
        return OP_COS;
    }
    else if (!strncmp(*s, "tg", 2))
    {
        *s += 2;
        return OP_TG;
    }
    else if (!strncmp(*s, "ctg", 3))
    {
        *s += 3;
        return OP_CTG;
    }
    else if (!strncmp(*s, "sh", 2))
    {
        *s += 2;
        return OP_SH;
    }
    else if (!strncmp(*s, "ch", 2))
    {
        *s += 2;
        return OP_CH;
    }
    else if (!strncmp(*s, "arcsin", 6))
    {
        *s += 6;
        return OP_ARCSIN;
    }
    else if (!strncmp(*s, "arccos", 6))
    {
        *s += 6;
        return OP_ARCCOS;
    }
    else if (!strncmp(*s, "arctg", 5))
    {
        *s += 5;
        return OP_ARCTG;
    }
    else if (!strncmp(*s, "arcctg", 6))
    {
        *s += 6;
        return OP_ARCCTG;
    }
    else    return 0;
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
    elem_s* op_node = GetTrig();

    while (*s == '*' || *s == '/')
    {
        char op = *s;
        s++;

        elem_s* r_node = GetTrig();
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
