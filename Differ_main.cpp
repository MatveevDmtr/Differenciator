//#include <TXLib.h>

#include <stdio.h>
#include <math.h>
#include <string.h>
#define LOGGING

#include "recursive_descent.h"
#include "differ.h"


//#include "Tree.h"


//#include "D:\\Programming\\C\\Ded_course_1_sem\\Stack\\stack.h"

#include "D:\\Programming\\C\\Ded_course_1_sem\\Processor_v4\\GetPoison.h"

#include "D:\\Programming\\C\\Ded_course_1_sem\\Stack\\specificators.h"

#include "D:\\Programming\\C\\Ded_course_1_sem\\Processor_v4\\logging.h"



enum ERRCODES
{
     OK                 , // 0
     SEGFAULT           , // 1
     ZOMBIE             , // 2
     NULLPTR            , // 3
     SIZEPOISONED       , // 4
     NEGCAP             , // 5
     DEBUGINFOERROR     , // 6
     REALLOCERROR       , // 7
     STACKOVERFLOW      , // 8
     FREE_ERROR         , // 9
     CHAINERROR         , // 10
     CONSTR_ERROR         // 11
};

const char*  DUMP_FILE_NAME      = "dump.dot";
const char*  PICNAME             = "graphCode";
const char*  FILE_EXTENSION      = ".png";
const char*  HTML_FILE_NAME      = "htmldump.html";

const size_t MAX_TREE_HIGHT      = 50;
const size_t MAX_LEN_LINE        = 40;
const char*  POISONED_ELEM       = "Xz";
const size_t MAX_LEN_PICNAME     = 20;
const size_t MAX_LEN_CONSOLE_CMD = 70;
const size_t MAX_LEN_NUM_DUMP    = 10;
const size_t TAB                 = 4;
const size_t MAX_LEN_TEXCMD      = 10;

const char* TEX_PREVIEW = "\\documentclass[a4paper, 12pt]{article}\n"
                          "\\usepackage[a4paper,top=1.5cm, bottom=1.5cm, left=1cm, right=1cm]"
                          "{geometry}\n"
                          "\\usepackage[utf8]{inputenc}\n"
                          "\\usepackage{mathtext}                \n"
                          "\\usepackage[english, russian]{babel} \n"
                          "\\title{Matanchik}\n"
                          "\\author{Witcherok, DedSquad}\n"
                          "\\date{\\today}\n";

int main()
{
    printf("start\n");

    printf("sizeof: %d\n", sizeof("meow"));

    tree_t tree = StructTreeInit(tree);

    //tree_t der_tree = StructTreeInit(der_tree);

    TreeCtor(&tree);

    log("Tree created\n");

    GetG(&tree, "ctg(2*x)/(3*x)"); //ctgx/(2x)

    HTMLDump(&tree, "After reading expression");

    static FILE* texfile = TexStart();

    TexPrint("$$f(x) = ");

    TreeToTex(texfile, &tree);

    TexPrint("$$\n");

    GetNDeriv(texfile, &tree, 2);

    TexPrint("$$f(x) = ");

    TaylorPrint(texfile, &tree, 2);

    TexFinish(texfile);
}

tree_t StructureTreeInit(const char* name,
                         const char* func,
                         const char* file,
                         int line)
{
    void*              Ptr             = NULL;
    size_t             Size            = 0;

    tree_t temp_tree =
    {
        .Ptr               = (elem_s*) getPoison(Ptr),
        .Size              = getPoison(Size),
        .DeadInside        = 1
    };

    return temp_tree;
}

FILE* TexStart()
{
    FILE* texfile = fopen("Derivative.tex", "w");

    TexPrint(TEX_PREVIEW);
    TexPrint("\\begin{document}\n");
    TexPrint("\\maketitle\n");

    return texfile;
}

int TexFinish(FILE* texfile)
{
    TexPrint("\\end{document}\n");

    fclose(texfile);

    return 0;
}

int GetNDeriv(FILE* texfile, tree_t* tree, size_t max_deriv)
{
    static size_t num_deriv = 0;

    if (num_deriv >= max_deriv) return 0;

    tree_t der_tree = StructTreeInit(der_tree);

    TreeCtor(&der_tree);

    log("start taking %dth deriv\n", num_deriv);

    der_tree.Ptr = Diff(tree->Ptr);

    log("finish taking %dth deriv\n", num_deriv);

    num_deriv += 1;

    HTMLDump(&der_tree, "Derivative");

    log("start simplify %dth deriv\n", num_deriv - 1);

    TreeSimplify(&der_tree);
    log("finish simplify %dth deriv\n", num_deriv - 1);

    HTMLDump(&der_tree, "After Simplify");

    TexPrint("$$f^{(%d)}(x) = ", num_deriv);

    TreeToTex(texfile, &der_tree);

    TexPrint("$$\n");

    GetNDeriv(texfile, &der_tree, max_deriv);
}

int TaylorPrint(FILE* texfile, tree_t* tree, size_t max_deriv)
{
    static size_t num_deriv = 0;

    if (num_deriv >= max_deriv)
    {
        TexPrint("o(x^{%d})$$\n")
        return 0;
    }

    tree_t der_tree = StructTreeInit(der_tree);

    TreeCtor(&der_tree);

    der_tree.Ptr = Diff(tree->Ptr);

    num_deriv += 1;

    //HTMLDump(&der_tree, "Derivative");

    TreeSimplify(&der_tree);

    //HTMLDump(&der_tree, "After Simplify");

    TexPrint("\\frac{1}{%d!} \\cdot (", num_deriv);

    TreeToTex(texfile, &der_tree);

    TexPrint(") + \n");

    TaylorPrint(texfile, &der_tree, max_deriv);
}

int TreeSimplify(tree_t* tree)
{
    bool if_simple = false;

    while (!if_simple)
    {
        if_simple = true;

        OneIterationSimplify(tree, tree->Ptr, &if_simple);

        CalculateConsts(tree, tree->Ptr, &if_simple);

        HTMLDump(tree, "Iteration simplify");
    }

    return 0;
}

elem_s* Diff(elem_s* node)
{
    switch(node->type)
    {
        case NODE_VAL:
            log("number in diff\n");
            return NewNum(0);
            break;

        case NODE_VAR:
        {
            log("variable in diff\n");
            elem_s* haha_node = NewNum(1);
            log("value in haha_node^ %d\n", haha_node->value.num_val);
            return haha_node;
            break;
        }

        case NODE_OP:
            log("operation in diff\n");
            return DiffOperation(node, CreateNode(NODE_OP, {.op_val = 0}));
            break;

        default:
            print_log(FRAMED, "Invalid type of node");
    }

    return NULL;
}

elem_s* DiffOperation(elem_s* node, elem_s* dest_node)
{
    switch(node->value.op_val)
    {
        case OP_ADD:
            log("diffing add\n");
            dest_node->value.op_val = OP_ADD;
            DiffAddSub(node, dest_node);
            break;

        case OP_SUB:
            log("diffing sub\n");
            dest_node->value.op_val = OP_SUB;
            DiffAddSub(node, dest_node);
            break;

        case OP_MUL:
            log("diffing mul\n");
            dest_node->value.op_val = OP_ADD;
            DiffMul(node, dest_node);
            break;

        case OP_DIV:
            log("diffing div\n");
            dest_node->value.op_val = OP_DIV;
            DiffDiv(node, dest_node);
            break;

        case OP_DEG:
            log("diffing deg\n");
            dest_node->value.op_val = OP_MUL;
            DiffDeg(node, dest_node);
            break;

        case OP_SIN:
            log("diffing sin\n");
            dest_node->value.op_val = OP_MUL;
            DiffSinCos(node, dest_node);
            break;

        case OP_COS:
            log("diffing cos\n");
            dest_node->value.op_val = OP_MUL;
            DiffSinCos(node, dest_node);
            break;

        case OP_TG:
            log("diffing tg\n");
            dest_node->value.op_val = OP_MUL;
            DiffTgCtg(node, dest_node);
            break;

        case OP_CTG:
            log("diffing ctg\n");
            dest_node->value.op_val = OP_MUL;
            DiffTgCtg(node, dest_node);
            break;

        default:
            print_log(FRAMED, "Unexpected type of operation");
    }

    return dest_node;
}

elem_s* DiffSinCos(elem_s* node, elem_s* dest_node)
{
    elem_s* trig_op = NULL;
    elem_s* mul     = NULL;

    if (node->value.op_val == OP_SIN)
    {
        mul     = dest_node;
        trig_op = NewOp(OP_COS);
    }
    else
    {
        trig_op = NewOp(OP_SIN);
        mul     = NewOp(OP_MUL);

        MakeSons(dest_node, NewNum(-1), mul);
    }

    MakeSons(trig_op, NewNum(0), CopyR);

    MakeSons(mul, trig_op, DiffR);

    return dest_node;
}

elem_s* DiffTgCtg(elem_s* node, elem_s* dest_node)
{
    elem_s* trig_op = NULL;
    elem_s* mul     = NULL;
    elem_s* deg     = NewOp(OP_DEG);

    if (node->value.op_val == OP_TG)
    {
        mul     = dest_node;
        trig_op = NewOp(OP_COS);
    }
    else
    {
        trig_op = NewOp(OP_SIN);
        mul     = NewOp(OP_MUL);

        MakeSons(dest_node, NewNum(-1), mul);
    }

    MakeSons(trig_op, NewNum(0), CopyR);
    MakeSons(deg    , trig_op,   NewNum(-2));
    MakeSons(mul    , deg,       DiffR);

    return dest_node;
}

elem_s* DiffMul(elem_s* node, elem_s* dest_node)
{
    elem_s* mul1 = NewOp(OP_MUL);
    elem_s* mul2 = NewOp(OP_MUL);;

    MakeSons(dest_node, mul1, mul2);

    MakeSons(mul1, DiffL, CopyR);
    MakeSons(mul2, CopyL, DiffR);

    return dest_node;
}

int TreeToTex(FILE* texfile, tree_t* tree)
{
    //TexPrint("$$");

    NodeToTex(texfile, tree->Ptr);

    //TexPrint("$$\n");

    return 0;
}

elem_s* DiffDiv(elem_s* node, elem_s* dest_node)
{
    elem_s* sub     = NewOp(OP_SUB);
    elem_s* square  = NewOp(OP_DEG);


    elem_s* mul1 = NewOp(OP_MUL);
    elem_s* mul2 = NewOp(OP_MUL);

    MakeSons(sub, mul1, mul2);

    MakeSons(mul1, DiffL, CopyR);
    MakeSons(mul2, CopyL, DiffR);

    elem_s* deg2 = NewNum(2);

    MakeSons(square, CopyR, deg2);

    MakeSons(dest_node, sub, square);

    return dest_node;
}

elem_s* DiffAddSub(elem_s* node, elem_s* dest_node)
{
    MakeSons(dest_node, DiffL, DiffR);

    return dest_node;
}

elem_s* DiffDeg(elem_s* node, elem_s* dest_node)
{
    elem_s* mul_deg  = NewOp(OP_MUL);
    elem_s* deg      = NewOp(OP_DEG);

    MakeSons(deg, CopyL, NewNum(node->right->value.num_val - 1));

    MakeSons(dest_node, NewNum(node->right->value.num_val), mul_deg);

    MakeSons(mul_deg, deg, DiffL);

    return dest_node;
};

elem_s* CopyNode(elem_s* node)
{
    elem_s* node_copy = CreateNode(node->type, node->value);

    if (node->left)          ConnectNodes(node_copy, CopyL, LEFT);
    if (node->right)         ConnectNodes(node_copy, CopyR, RIGHT);

    return node_copy;
}

int TreeCtor(tree_t* tree)
{
    static size_t num_dump = 0;

    if (tree == NULL || tree == getPoison(tree))
    {
        LogError(SEGFAULT);

        return SEGFAULT;
    }

    if (!(tree->DeadInside))
    {
        LogError(CONSTR_ERROR);

        return CONSTR_ERROR;
    }

    void* temp_ptr = (elem_s*) calloc(1, sizeof(elem_s));

    if (temp_ptr == NULL)
    {
        LogError(REALLOCERROR);

        return REALLOCERROR;
    }

    tree->Ptr                = (elem_s*)(temp_ptr);


    tree->DeadInside         = 0;
    tree->Size               = 0;

    return 0;
}

elem_s* CreateNode(size_t type, union value_t value)
{
    elem_s* temp_ptr = allocate_array(elem_s, 1);

    *temp_ptr = {.type  = type,
                .value  = value,
                .parent = NULL,
                .left   = NULL,
                .right  = NULL};

    return temp_ptr;
}

int ConnectNodes(elem_s* parent, elem_s* son, size_t num_son)
{
    switch(num_son)
    {
        case LEFT:
            parent->left  = son;
            break;

        case RIGHT:
            parent->right = son;
            break;

        default:
            print_log(FRAMED, "Invalid number of son");
    }

    son->parent = parent;

    return 0;
}

int CalculateConsts(tree_t* tree, elem_s* node, bool* if_simple)
{
    if (node->left )  CalculateConsts(tree, node->left, if_simple);
    if (node->right)  CalculateConsts(tree, node->right, if_simple);

    if (node->type != NODE_OP         ||
        node->left ->type != NODE_VAL ||
        node->right->type != NODE_VAL)       return 0;

    *if_simple = false;

    int val = 0;

    int lval = node->left ->value.num_val;
    int rval = node->right->value.num_val;

    bool update_neccessary = true;

    switch(node->value.op_val)
    {
        case OP_ADD:
            val = lval + rval;
            break;

        case OP_SUB:
            val = lval - rval;
            break;

        case OP_MUL:
            val = lval * rval;
            break;

        case OP_DIV:
            val = lval / rval;
            break;

        case OP_DEG:
            val = pow(lval, rval);
            break;

        default:
            update_neccessary = false;
            print_log(FRAMED, "Unknown Operation");
    }

    if (update_neccessary)
    {
        UpdSon(NewNum(val));
        log("try to free in line: %d\n", __LINE__);
        FreeNode(node);
    }

    return 0;
}

void TexPrintf(FILE* texfile, const char* format, ...)
{
    va_list args;

    va_start (args, format);

    vfprintf(texfile, format, args);

    fflush(texfile);

    va_end(args);
}

int OneIterationSimplify(tree_t* tree, elem_s* node, bool* if_simple)
{
    if (node->left )  OneIterationSimplify(tree, node->left, if_simple);
    if (node->right)  OneIterationSimplify(tree, node->right, if_simple);

    if (node->type != NODE_OP)     return 0;

    CheckForConst(tree, node, node->left , node->right, if_simple);
    CheckForConst(tree, node, node->right, node->left,  if_simple);

    return 0;
}

int CheckForConst(tree_t* tree     , elem_s* node,
                  elem_s* son_check, elem_s* other_son,
                  bool* if_simple)
{
    //if (node->parent == NULL)       return -1;

    if (son_check && son_check->type == NODE_VAL)
    {
        if (son_check->value.num_val == 0)
        {
            elem_s* new_node = NewNodeForOpWithZero(node->value.op_val, other_son);

            if (new_node)
            {
                log("new node found: %p\n", new_node);

                *if_simple = false;

                UpdSon(new_node);

                log("try to free in line: %d\n", __LINE__);

                FreeOneNode(node);

                FreeNode(son_check);

            }
        }
        else if (son_check->value.num_val == 1)
        {
            elem_s* new_node = NewNodeForOpWithOne(node->value.op_val, other_son);

            if (new_node)
            {
                *if_simple = false;

                UpdSon(new_node);

                log("try to free in line: %d\n", __LINE__);

                free(node);

                FreeNode(son_check);
            }
        }
    }
}

elem_s* NewNodeForOpWithOne(size_t op_val, elem_s* other_son)
{
    switch(op_val)
    {
        case OP_MUL:
            return other_son;
            break;

        case OP_DIV:
            if (other_son->parent->left == other_son)
            {
                return other_son;
            }
            break;

        case OP_DEG:
            if (other_son->parent->left == other_son)
            {
                return other_son;
            }
            else
            {
                log("try to free in line: %d\n", __LINE__);
                FreeNode(other_son);
                return NewNum(1);
            }
            break;

        default:
            log("No meaning for simplify\n");
    }
    return NULL;
}

int FreeOneNode(elem_s* node)
{
    node->parent = NULL;
    node->left   = NULL;
    node->right  = NULL;
    free(node);
    return 0;
}

elem_s* NewNodeForOpWithZero(size_t op_val, elem_s* other_son)
{
    log("op_val in newNodeFor0: %.4s\n", &op_val);

    switch(op_val)
    {
        case OP_MUL:
            log("try to free in line: %d\n", __LINE__);
            log("try to free other son: %p\n", other_son);
            FreeNode(other_son);
            log("free done\n");
            return NewNum(0);
            break;

        case OP_DIV:
            if (other_son->parent->left == other_son)
            {
                print_log(FRAMED, "ZERO DIVISION ERROR");
            }
            else
            {
                log("try to free in line: %d\n", __LINE__);
                FreeNode(other_son);
                return NewNum(0);
            }
            break;

        case OP_ADD:
            return other_son;
            break;

        case OP_SUB:
            if (other_son->parent->left == other_son)
            {
                return other_son;
            }
            else
            {
                elem_s* mul = NewOp(OP_MUL);
                MakeSons(mul, NewNum(-1), other_son);
                return mul;
            }
            break;

        case OP_DEG:
            if (other_son->parent->left == other_son)
            {
                log("try to free in line: %d\n", __LINE__);
                FreeNode(other_son);
                return NewNum(1);
            }
            else
            {
                log("try to free in line: %d\n", __LINE__);
                FreeNode(other_son);
                return NewNum(0);
            }
            break;

        default:
            print_log(FRAMED, "Operation Error: unexpected operation");
    }
    return NULL;
}

int FreeNode(elem_s* node)
{
    Assert(node == NULL);

    if (node->left)
    {
        FreeNode(node->left);
    }

    if (node->right)
    {
        FreeNode(node->right);
    }

    node->parent = NULL;
    node->left   = NULL;
    node->right  = NULL;
    log("FreeNode: try to use default free for %p\n", node);
    free(node);
    log("FreeNode: finish use default free\n");

    return 0;
}

int LogCritError(int errcode, const char* func, int line)
{
    switch (errcode)
    {
        case OK:
            //print_log(FRAMED, "Everything OK: Stack is all right and can be used");
            break;

        case SEGFAULT:
            print_crit_errors("SEGMENTATION FAULT: Invalid Pointer to Structure of Stack", func, line);
            break;

        case ZOMBIE:
            print_crit_errors("DEADINSIDE ERROR: Stack doesn't exist", func, line);
            break;

        case NULLPTR:
            print_crit_errors("POINTER ERROR: Stack Pointer (pointer to buffer) is NULL", func, line);
            break;

        case SIZEPOISONED:
            print_crit_errors("SIZE ERROR: Stack Size is poisoned", func, line);
            break;

        case NEGCAP:
            print_crit_errors("CAPACITY ERROR: Stack Capacity has a Negative Value", func, line);
            break;

        case STACKOVERFLOW:
            print_crit_errors("STACK OVERFLOW ERROR: Size of Stack is bigger than its Capacity", func, line);
            break;

        case FREE_ERROR:
            print_crit_errors("FREE ERROR: Index of free unit is invalid", func, line);
            break;

        case CHAINERROR:
            print_crit_errors("CHAIN ERROR: Invalid Next-Prev Connection between elements", func, line);
            break;

        case CONSTR_ERROR:
            print_crit_errors("CONSTRUCTION ERROR: Trying to construct an existing Tree", func, line);
            break;

        default:
            print_crit_errors("DECODE ERROR: Unexpected Error Code", func, line);
            return 1;
    }

    return 0;
}

int NodeToTex(FILE* texfile, elem_s* node)
{
    char texcmd[MAX_LEN_TEXCMD] = "";

    if (node->type == NODE_VAR)
    {
        texcmd[0] = node->value.var_val;
        TexPrint(texcmd);
        return 0;
    }
    if (node->type == NODE_VAL)
    {
        if(node->value.num_val <= 0)
        {
            TexPrint("(");
            TexPrint(itoa(node->value.num_val, texcmd, 10));
            TexPrint(")");
        }
        else    TexPrint(itoa(node->value.num_val, texcmd, 10));
        return 0;
    }
    switch(node->value.op_val)
    {
        case OP_ADD:
            InOrdCmd(" + ");
            break;

        case OP_SUB:
            InOrdCmd(" - ");
            break;

        case OP_MUL:
            InOrdCmd(" \\cdot ");
            break;

        case OP_DIV:
            TexPrint(" \\frac{");
            TexLeft;
            TexPrint("}{");
            TexRight;
            TexPrint("} ");
            break;

        case OP_DEG:
            //if ((node->parent->type == NODE_OP        &&
            //    node->parent->value.op_val == OP_DEG))                {BraceDeg("^{");}
            //else                                            {NoBraceDeg("^{");}
            NoBraceDeg("^{");
            break;

        case OP_SIN:
            TexTrigOp("\\sin{");
            break;

        case OP_COS:
            TexTrigOp("\\cos{");
            break;

        case OP_TG:
            TexTrigOp("\\tg{");
            break;

        case OP_CTG:
            TexTrigOp("\\ctg{");
            break;

        default:
            print_log(FRAMED, "Unexpected type of operation");
    }
}

int HTMLDump(const tree_t* tree, const char* occasion)
{
    Assert(tree == NULL);

    log("tree pointer in HTMLDump: %p\n", tree);

    static size_t num_dump;  //init in TreeCtor

    char curr_picname[MAX_LEN_PICNAME] = "";

    char str_num_dump[MAX_LEN_NUM_DUMP] = "";

    itoa(num_dump, str_num_dump, 10);

    strcpy(curr_picname, PICNAME);
    strcat(curr_picname, str_num_dump);
    strcat(curr_picname, FILE_EXTENSION);

    log("picture name: %s\n", curr_picname);
    //picname made

    GraphTreeDump(tree, curr_picname);

    char html_piccmd[MAX_LEN_CONSOLE_CMD] = "<img src=\"";

    strcat(html_piccmd, curr_picname);
    strcat(html_piccmd, "\" alt=\"ERROR: Picture Not Found\">");

    char* r_mode = "a";
    if (num_dump == 0)          r_mode = "w";

    FILE* html_file = fopen(HTML_FILE_NAME, r_mode);

    if (html_file == NULL)
    {
        print_log(FRAMED, "FILE ERROR: can't open HTML file");
    }

    dumphtml("<pre>\n");
    dumphtml("<b>\n");
    dumphtml("<font size=6>");
    dumphtml("Tree dump (");
    dumphtml("%d", num_dump);
    dumphtml(")\n");
    dumphtml("<i>");
    dumphtml("<font size=4>");
    dumphtml("\nOcassion for DUMP: %s\n\n", occasion);
    dumphtml("%s\n\n", html_piccmd);
    dumphtml("</pre>\n");

    num_dump++;

    fclose(html_file);
}



void DrawNode(elem_s* node, FILE* dump_file, const char* branch_label)
{
    dumpline("struct%p [\nlabel = \"{<data>elem: ", node);

    char* color = "";

    switch(node->type)
    {
        case NODE_OP:
            color = "chartreuse2";
            dumpline("%.4s", &node->value);
            break;

        case NODE_VAL:
            color = "aquamarine";
            dumpline("%d", node->value);
            break;

        case NODE_VAR:
            color = "darkgoldenrod1";
            dumpline("%c", node->value);
            break;

        default:
            print_log(FRAMED, "Invalid type of node");
    }

    dumpline("|<type>type: %.4s|<parent>parent: %p|<left>left: %p|<right>right: %p}\", style = \"filled\", "
             "color = \"black\", fillcolor = \"%s\" \n];\n", &node->type, node->parent, node->left, node->right, color);
    if (node->parent != NULL)
    {
        dumpline("struct%p -> struct%p [weight=900 constraint=true color=red];\n", node->parent, node);
    }
    if (node->left != NULL)
    {
        DrawNode(node->left,  dump_file, "yes");
    }
    if (node->right != NULL)
    {
        DrawNode(node->right, dump_file, "no");
    }
}

void GraphTreeDump(const tree_t* tree, const char* picname)
{
    log("tree pointer: %p\n", tree);

    FILE* dump_file = fopen(DUMP_FILE_NAME, "w");

    if (dump_file == NULL)
    {
        print_log(FRAMED, "FILE ERROR: Cant't open file\n");
    }
    dumpline("digraph {\n");
    dumpline("graph [dpi = 100]");
    dumpline("rankdir = TB;\n");
    //dumpline("splines = \"ortho\";\n");
    dumpline("node [ shape=record ];\n");

    dumpline("Tree [label = \"Tree|size: %d\", style = \"filled\", rankdir = TB, fillcolor = \"indigo\", fontcolor = \"yellow\"];\n", tree->Size);

    DrawNode(tree->Ptr, dump_file, "root");

    dumpline("}");

    fclose(dump_file);

    static size_t num_dump;  //init in TreeCtor

    log("start making console cmd\n");

    char console_cmd[MAX_LEN_CONSOLE_CMD] = "";
    strcpy(console_cmd, "dot -T png -o ");
    strcat(console_cmd, picname);
    strcat(console_cmd, " ");
    strcat(console_cmd, DUMP_FILE_NAME);

    log("console cmd: %s\n", console_cmd);

    printf("%d\n", system("cd D:"));
    printf("%d\n", system("cd D:\\Programming\\C\\Ded_course_1_sem\\Differenciator"));
    printf("%d\n", system(console_cmd));
}
