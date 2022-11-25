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

const char* DUMP_FILE_NAME = "dump.dot";
const char* PICNAME        = "graphCode";
const char* FILE_EXTENSION = ".png";
const char* HTML_FILE_NAME = "htmldump.html";

const size_t MAX_TREE_HIGHT      = 50;
const size_t MAX_LEN_LINE        = 40;
const char*  POISONED_ELEM       = "Xz";
const size_t MAX_LEN_PICNAME     = 15;
const size_t MAX_LEN_CONSOLE_CMD = 50;
const size_t MAX_LEN_NUM_DUMP    = 10;
const size_t TAB                 = 4;
const int    PREV_FOR_FREE       = -1;

const char   QUEST_PREFIX = '?';
const char   LEAF_PREFIX  = '#';

int main()
{
    printf("start\n");

    tree_t tree = StructTreeInit(tree);

    TreeCtor(&tree);

    log("Tree created\n");

    def_node* temp_ptr_wayd_d  = allocate_array(def_node, MAX_TREE_HIGHT);
    def_node* temp_ptr_wayd_c1 = allocate_array(def_node, MAX_TREE_HIGHT);
    def_node* temp_ptr_wayd_c2 = allocate_array(def_node, MAX_TREE_HIGHT);

    Assert(temp_ptr_wayd_d == NULL);
    Assert(temp_ptr_wayd_c1 == NULL);
    Assert(temp_ptr_wayd_c2 == NULL);

    static queue_t way_down    = {.Ptr = temp_ptr_wayd_d,  .Head = 0, .Tail = 0};
    static queue_t way_down_c1 = {.Ptr = temp_ptr_wayd_c1, .Head = 0, .Tail = 0};
    static queue_t way_down_c2 = {.Ptr = temp_ptr_wayd_c2, .Head = 0, .Tail = 0};

    GetG(&tree, "(20+10)*30");

    HTMLDump(&tree, "After reading expression");
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
    /*
    switch (num_son)
    {
        case LEFT:
            parent->left = temp_ptr;
            break;


        case RIGHT:
            parent->right = temp_ptr;
            break;

        default:
            log("Invalid number of son\n");
    }*/

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
int MainMenu(tree_t* tree, queue_t* way_down_d, queue_t* way_down_c1, queue_t* way_down_c2)
{
    char line[MAX_LEN_LINE] = "";

    char sym = 0;

    while (true)
    {
        SpeakAndPrint("Type \'e\' to exit, \'g\' to guess, \'d\' to see the definition of an object, \'c\' to compare objects\n");

        char sym = 0;

        while (sym != 'e' && sym != 'g' && sym != 'd' && sym != 'c')
        {
            sym = getchar();
        }

        switch(sym)
        {
            case 'g':
                Guess(tree, tree->Ptr);
                break;

            case 'd':
                GiveDefinition(tree, way_down_d);
                break;

            case 'c':
                PrintComparison(tree, way_down_c1, way_down_c2);
                break;

            case 'e':
                SpeakAndPrint("Goodbye!\n");

                WriteDataBase(tree->Ptr);

                return 0; // break from cycle

            default:
                SpeakAndPrint("WHATAFUCK, u must type one of 3 symbols\n");
        }
    }
}
*/

/*
int InsertSon(tree_t* tree, elem_s* parent, char* ins_elem, const char* quest, size_t num_son)
{
    //TreeVerify(tree);

    log("start insert\n");

    elem_s* temp_ptr1 = (elem_s*) calloc(1, sizeof(elem_s));
    elem_s* temp_ptr2 = (elem_s*) calloc(1, sizeof(elem_s));

    if (temp_ptr1 == NULL || temp_ptr2 == NULL)
    {
        LogError(REALLOCERROR);

        return REALLOCERROR;
    }

    parent->son1 = (elem_s*) temp_ptr1;
    parent->son2 = (elem_s*) temp_ptr2;

    strcpy(parent->son1->elem, parent->elem);
    strcpy(parent->son2->elem, ins_elem);
    strcpy(parent->elem, quest);

    parent->son1->parent = parent;
    parent->son2->parent = parent;

    if (num_son == 1)
    {
        elem_s* temp_son = parent->son1;
        parent->son1     = parent->son2;
        parent->son2     = temp_son;
    }

    tree->Size++;

    return 0;
}

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
    dumphtml("<\pre>\n");

    num_dump++;

    fclose(html_file);
}



void DrawNode(elem_s* node, FILE* dump_file, const char* branch_label)
{
    dumpline("struct%p [\nlabel = \"{<data>elem: ", node);
    switch(node->type)
    {
        case NODE_OP:
            dumpline("%.4s", &node->value);
            break;

        case NODE_VAL:
            dumpline("%d", node->value);
            break;

        case NODE_VAR:
            dumpline("%c", node->value);
            break;

        default:
            print_log(FRAMED, "Invalid type of node");
    }

    dumpline("|<type>type: %.4s|<parent>parent: %p|<left>left: %p|<right>right: %p}\", style = \"filled\", "
             "color = \"black\", fillcolor = \"aquamarine\" \n];\n", &node->type, node->parent, node->left, node->right);
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
