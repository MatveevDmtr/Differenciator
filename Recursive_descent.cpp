#include <stdio.h>

const char* s = NULL;

int GetG(const char* str);

int GetN();

int GetE();

int GetT();

int GetP();



int main()
{
    GetG("11*((50-30)/10)");

    return 0;
}

int GetG(const char* str)
{
    s = str;

    int val = GetE();

    if (*s == '\0')
    {
        printf("The end\n");
    }
    else
    {
        printf("Syntax error: %c\n", *s);
    }

    printf("RESULT: %d\n", val);
}

int GetN()
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
        printf("N fail\n");
    }

    return val;
}

int GetE()
{
    int val = GetT();

    while (*s == '+' || *s == '-')
    {
        char op = *s;
        s++;

        int val2 = GetT();

        if (op == '+')
        {
            val += val2;
        }
        else
        {
            val -= val2;
        }
    }

    return val;
}

int GetT()
{
    int val = GetP();

    while (*s == '*' || *s == '/')
    {
        char op = *s;
        s++;

        int val2 = GetP();

        if (op == '*')
        {
            val *= val2;
        }
        else
        {
            val /= val2;
        }
    }

    return val;
}

int GetP()
{
    int val = 0;

    if (*s == '(')
    {
        s++;

        val = GetE();

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
        val = GetN();
    }

    return val;
}
