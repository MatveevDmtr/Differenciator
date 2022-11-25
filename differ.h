#ifndef DIFFER_H_INCLUDED
#define DIFFER_H_INCLUDED

#define StructTreeInit(name)                              \
        StructureTreeInit(#name,                          \
                           __PRETTY_FUNCTION__,           \
                           __FILE__,                      \
                           __LINE__);


#define allocate_array(type, num_elems)                             \
({                                                                  \
    type* location__ = (type*) calloc (num_elems, sizeof (type));   \
    log ("Type_calloc: %s\n", #type);                               \
    log ("Sizeof type: %d\n", sizeof (type*));                      \
    Assert(location__ == NULL);                                     \
    log("Calloc returned: %p\n", location__);                       \
    location__;                                                     \
})

// START DUMP  DSL

#define dumpline(...)  fprintf(dump_file, __VA_ARGS__);
#define dumphtml(...)  fprintf(html_file, __VA_ARGS__);
// FINISH DUMP DSL

#define MAX_LEN_ELEM 100// to insert in scanf


typedef unsigned long long bird_t;

typedef int elem_t;

enum OPERATIONS
{
    OP_ADD = 'DAA',
    OP_SUB = 'BUS',
    OP_MUL = 'LUM',
    OP_DIV = 'VID',
    OP_DEG = 'GED'
};

enum NUM_SONS
{
    LEFT  = 1,
    RIGHT = 2
};

enum NODE_TYPES
{
    NODE_OP  = 'PO',
    NODE_VAR = 'RAV',
    NODE_VAL = 'LAV'
};

union value_t
{
    char     var_val;
    int      num_val;
    size_t   op_val;
};

typedef struct struct_elem
{
    size_t       type;

    value_t value;

    struct struct_elem* parent;
    struct struct_elem* left;
    struct struct_elem* right;
} elem_s;


typedef struct struct_def_node
{
    elem_s* Node;
    size_t  Branch;
} def_node;

typedef struct struct_queue
{
    def_node* Ptr;
    size_t    Head;
    size_t    Tail;

} queue_t;

typedef struct tree_info
{
    elem_s*            Ptr;
    size_t             Size;

    bool               DeadInside;
} tree_t;

elem_s* CreateNode(size_t type, union value_t value);

int ConnectNodes(elem_s* parent, elem_s* son, size_t num_son);





tree_t StructureTreeInit(const char* name,
                           const char* func,
                           const char* file,
                           int line);

int TreeCtor(tree_t* tree);

int MainMenu(tree_t* tree, queue_t* way_down_d, queue_t* way_down_c1, queue_t* way_down_c2);

int Guess(tree_t* tree, elem_s* node);

void SpeakAndPrint(const char* str...);

int GiveDefinition(tree_t* tree, queue_t* way_down);

int ReadNewNode(tree_t* tree, elem_s* node);

void WriteSpaces(int num_spaces, FILE* db);

int ScanNode(tree_t* tree, elem_s* node, elem_s* parent, FILE* db, size_t num_son);

int ScanDataBase(tree_t* tree);

char ReadPrefix(FILE* db);

int InsertNode(tree_t* tree, elem_s* parent, char* ins_elem, size_t num_son);

int ConfigNode(elem_s** son_field, elem_s* parent, char* ins_elem);

int PrintQuality(queue_t* way_down, size_t i);

int WriteDataBase(elem_s* root);

int PrintDif(const char* name, queue_t* way_down, size_t i_dif);

int PrintComparison(tree_t* tree, queue_t* way_down1, queue_t* way_down2);

int FindNode(tree_t* tree, elem_s* node, const char* name, queue_t* way_down, size_t branch);

int PrintDefinition(tree_t* tree, queue_t* way_down, const char* name);

int InsertSon(tree_t* tree, elem_s* parent, char* ins_elem, const char* quest, size_t num_son);

int InsertBeforeElem(tree_t* tree, size_t i_anc, elem_t elem);

int InsertAfterElem (tree_t* tree, size_t i_anc, elem_t elem);

int TreeVerify(tree_t* tree);

int TreeAppend(tree_t* tree, elem_t elem);

elem_t TreePop(tree_t* tree);

elem_t DeleteElem(tree_t* tree, size_t i_del);

void GraphTreeDump(const tree_t* tree, const char* picname);

int HTMLDump(const tree_t* tree, const char* occasion);

int LogCritError(int errcode, const char* func, int line);

int TreeRecalloc(tree_t* tree, size_t mode);

int YesOrNo();

void PrintBinQuest();

#endif //guard
