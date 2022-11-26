#include <stdio.h>
#include <math.h>
//#include <TXLib.h>
#define LOGGING

#include "recursive_descent.h"
#include "differ.h"


//#include "Tree.h"


//#include "D:\\Programming\\C\\Ded_course_1_sem\\Stack\\stack.h"

#include "D:\\Programming\\C\\Ded_course_1_sem\\Processor_v4\\GetPoison.h"

#include "D:\\Programming\\C\\Ded_course_1_sem\\Stack\\specificators.h"

#include "D:\\Programming\\C\\Ded_course_1_sem\\Processor_v4\\logging.h"

//start DSL
#define CopyL CopyNode(node->left)
#define CopyR CopyNode(node->right)

#define DiffL Diff(node->left)
#define DiffR Diff(node->right)

#define NewOp(op)   CreateNode(NODE_OP , {.op_val  = op})
#define NewNum(num) CreateNode(NODE_VAL, {.num_val = num})

#define UpdSon(new_node)                                                                   \
{                                                                                          \
    if      (node->parent->left  == node)   ConnectNodes(node->parent, new_node, LEFT);    \
    else if (node->parent->right == node)   ConnectNodes(node->parent, new_node, RIGHT);   \
    else                                    print_log(FRAMED, "Invalid node Connection");  \
}
//finish DSL

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
const size_t MAX_LEN_PICNAME     = 15;
const size_t MAX_LEN_CONSOLE_CMD = 50;
const size_t MAX_LEN_NUM_DUMP    = 10;
const size_t TAB                 = 4;
const int    PREV_FOR_FREE       = -1;

const char   QUEST_PREFIX        = '?';
const char   LEAF_PREFIX         = '#';

int main()
{
    printf("start\n");

    tree_t tree = StructTreeInit(tree);

    tree_t der_tree = StructTreeInit(der_tree);

    TreeCtor(&tree);
    TreeCtor(&der_tree);

    log("Tree created\n");
    /*
    def_node* temp_ptr_wayd_d  = allocate_array(def_node, MAX_TREE_HIGHT);
    def_node* temp_ptr_wayd_c1 = allocate_array(def_node, MAX_TREE_HIGHT);
    def_node* temp_ptr_wayd_c2 = allocate_array(def_node, MAX_TREE_HIGHT);

    Assert(temp_ptr_wayd_d == NULL);
    Assert(temp_ptr_wayd_c1 == NULL);
    Assert(temp_ptr_wayd_c2 == NULL);

    static queue_t way_down    = {.Ptr = temp_ptr_wayd_d,  .Head = 0, .Tail = 0};
    static queue_t way_down_c1 = {.Ptr = temp_ptr_wayd_c1, .Head = 0, .Tail = 0};
    static queue_t way_down_c2 = {.Ptr = temp_ptr_wayd_c2, .Head = 0, .Tail = 0};
    */
    GetG(&tree, "(2*x)^5");

    HTMLDump(&tree, "After reading expression");

    der_tree.Ptr = Diff(tree.Ptr);

    SimplifyMul(der_tree.Ptr);

    CalculateConsts(der_tree.Ptr);

    HTMLDump(&der_tree, "Derivative 1");
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

elem_s* Diff(elem_s* node)
{
    switch(node->type)
    {
        case NODE_VAL:
            log("number in diff\n");
            return CreateNode(NODE_VAL, {.num_val = 0});
            break;

        case NODE_VAR:
            log("variable in diff\n");
            return CreateNode(NODE_VAL, {.num_val = 1});
            break;

        case NODE_OP:
            log("operation in diff");
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
            dest_node->value.op_val = OP_ADD;
            DiffAddSub(node, dest_node);
            break;

        case OP_SUB:
            dest_node->value.op_val = OP_SUB;
            DiffAddSub(node, dest_node);
            break;

        case OP_MUL:
            dest_node->value.op_val = OP_ADD;
            DiffMul(node, dest_node);
            break;

        case OP_DIV:
            dest_node->value.op_val = OP_DIV;
            DiffDiv(node, dest_node);
            break;

        case OP_DEG:
            dest_node->value.op_val = OP_MUL;
            DiffDeg(node, dest_node);
            break;

        default:
            print_log(FRAMED, "Unexpected type of operation");
    }

    return dest_node;
}

elem_s* DiffMul(elem_s* node, elem_s* dest_node)
{
    elem_s* mul1 = NewOp(OP_MUL);
    elem_s* mul2 = NewOp(OP_MUL);;

    ConnectNodes(dest_node, mul1, LEFT);
    ConnectNodes(dest_node, mul2, RIGHT);

    ConnectNodes(mul1, DiffL, LEFT);
    ConnectNodes(mul1, CopyR, RIGHT);

    ConnectNodes(mul2, CopyL, LEFT);
    ConnectNodes(mul2, DiffR, RIGHT);

    return dest_node;
}

elem_s* DiffDiv(elem_s* node, elem_s* dest_node)
{
    elem_s* sub     = NewOp(OP_SUB);
    elem_s* square  = NewOp(OP_DEG);


    elem_s* mul1 = CreateNode(NODE_OP, {.op_val = OP_MUL});
    elem_s* mul2 = CreateNode(NODE_OP, {.op_val = OP_MUL});

    ConnectNodes(sub, mul1, LEFT);
    ConnectNodes(sub, mul2, RIGHT);

    ConnectNodes(mul1, DiffL, LEFT);
    ConnectNodes(mul1, CopyR, RIGHT);

    ConnectNodes(mul2, CopyL, LEFT);
    ConnectNodes(mul2, DiffR, RIGHT);

    elem_s* deg2 = NewNum(2);

    ConnectNodes(square, CopyR, LEFT);
    ConnectNodes(square, deg2 , RIGHT);

    ConnectNodes(dest_node, sub   , LEFT);
    ConnectNodes(dest_node, square, RIGHT);

    return dest_node;
}

elem_s* DiffAddSub(elem_s* node, elem_s* dest_node)
{
    ConnectNodes(dest_node, Diff(node->left) , LEFT);
    ConnectNodes(dest_node, Diff(node->right), RIGHT);

    return dest_node;
}

elem_s* DiffDeg(elem_s* node, elem_s* dest_node)
{
    elem_s* mul_deg  = NewOp(OP_MUL);

    elem_s* deg = NewOp(OP_DEG);
    ConnectNodes(deg, CopyL                                 , LEFT);
    ConnectNodes(deg, NewNum(node->right->value.num_val - 1), RIGHT);

    ConnectNodes(dest_node, NewNum(node->right->value.num_val), LEFT);
    ConnectNodes(dest_node, mul_deg, RIGHT);

    ConnectNodes(mul_deg, deg, LEFT);
    ConnectNodes(mul_deg, DiffL, RIGHT);

    return dest_node;
};

elem_s* CopyNode(elem_s* node)
{
    elem_s* node_copy = CreateNode(node->type, node->value);

    if(node->left)
    {
        ConnectNodes(node_copy, CopyNode(node->left), LEFT);
    }

    if(node->right)
    {
        ConnectNodes(node_copy, CopyNode(node->right), RIGHT);
    }

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

/*
int WriteNode(elem_s* node, FILE* db, size_t num_spaces)
{
    Assert(node == nullptr);
    Assert(db   == nullptr);

    log("start writing node: %s\n", node->elem);

    WriteSpaces(num_spaces, db);

    if (node->son1 && node->son2)
    {
        fprintf(db, "%c%s\n", QUEST_PREFIX, node->elem);

        num_spaces += TAB;

        log("try to write sons of %s\n", node->elem);

        WriteNode(node->son1, db, num_spaces);
        WriteNode(node->son2, db, num_spaces);
    }
    else
    {
        log("try to write leaf %s\n", node->elem);

        fprintf(db, "%c%s\n", LEAF_PREFIX, node->elem);
    }

    return 0;
}

int WriteDataBase(elem_s* root)
{
    log("Start writing db\n");

    Assert(root == nullptr);

    FILE* db = fopen("dataBase.txt", "w");

    if (db == NULL)
    {
        SpeakAndPrint("Can't open dataBase file\n");

        return -1;
    }

    WriteNode(root, db, 0);

    fclose(db);

    log("Finish writing db\n");

    return 0;
}

void WriteSpaces(int num_spaces, FILE* db)
{
    Assert(db == nullptr);

    while (num_spaces-- > 0)
    {
        fprintf(db, " ");
    }
}

*/


elem_s* CreateNode(size_t type, union value_t value)
{
    elem_s* temp_ptr = allocate_array(elem_s, 1);

    *temp_ptr = {.type   = type,
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

/*
int FindNode(tree_t* tree, elem_s* node, const char* name, queue_t* way_down, size_t branch)
{
    static bool if_found = false;

    if_found = false;

    Assert(way_down->Ptr == NULL);

    log("node: %s\n", node->elem);

    way_down->Ptr[way_down->Tail++] = {node, branch};

    log("added to wd: %p\n", way_down->Ptr[way_down->Tail - 1].Node);

    log("wd.tail=%d\n", way_down->Tail);

    for (size_t i = 0; i < way_down->Tail; i++)
    {
        log("question in FindNode: %s\n", way_down->Ptr[i].Node->elem);
        log("branch in FindNode: %d\n", way_down->Ptr[i].Branch);
    }

    if (if_found) return 0;

    if (!strcmp(name, node->elem))
    {
        log("node found\n");

        if_found = true;

        return 0;
    }

    if (node->son1)
    {
        FindNode(tree, node->son1, name, way_down, LEFT);
    }

    if (if_found) return 0;

    if (node->son2)
    {
        FindNode(tree, node->son2, name, way_down, RIGHT);
    }

    if (if_found)   return 0;

    way_down->Tail--;

    return -1;
}

*/

int CalculateConsts(elem_s* node)
{
    if (node->left )  CalculateConsts(node->left);
    if (node->right)  CalculateConsts(node->right);

    if (node->type != NODE_OP         ||
        node->left ->type != NODE_VAL ||
        node->right->type != NODE_VAL)       return 0;

    int val = 0;

    int lval = node->left ->value.num_val;
    int rval = node->right->value.num_val;

    switch(node->value.op_val)
    {
        case OP_ADD:
            val = lval+rval;
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
            print_log(FRAMED, "Unknown Operation");
    }

    UpdSon(NewNum(val));

    return 0;
}

int SimplifyMul(elem_s* node)
{
    if (node->left )  SimplifyMul(node->left);
    if (node->right)  SimplifyMul(node->right);

    if (node->type != NODE_OP || node->value.op_val != OP_MUL)    return 0;

    CheckForConst(node, node->left , node->right);
    CheckForConst(node, node->right, node->left);

    return 0;
}

void CheckForConst(elem_s* node, elem_s* son_check, elem_s* other_son)
{
    if (son_check && son_check->type == NODE_VAL)
    {
        if (son_check->value.num_val == 0)
        {
            elem_s* new_node = NewNum(0);

            UpdSon(new_node);

            FreeNode(node);
        }
        else if (son_check->value.num_val == 1)
        {
            UpdSon(other_son);

            FreeNode(node);
        }
    }
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

    free(node);

    return 0;
}
/*
int FindNode(elem_s* node)
{

    if (node->son1)
    {
        FindNode(tree, node->son1, name, way_down, LEFT);
    }

    if (if_found) return 0;

    if (node->son2)
    {
        FindNode(tree, node->son2, name, way_down, RIGHT);
    }

    if (if_found)   return 0;

    way_down->Tail--;

    return -1;
}
*/
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
