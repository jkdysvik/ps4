#include <vslc.h>

/* Global symbol table and string list */
symbol_table_t *global_symbols;
char **string_list;
size_t string_list_len;
size_t string_list_capacity;

static void find_globals ( void );
static void bind_names ( symbol_table_t *local_symbols, node_t *root );
static void print_symbol_table ( symbol_table_t *table, int nesting );
static void destroy_symbol_tables ( void );

static size_t add_string ( char* string );
static void print_string_list ( void );
static void destroy_string_list ( void );

/* External interface */

/* Creates a global symbol table, and local symbol tables for each function.
 * While building the symbol tables:
 *  - All usages of symbols are bound to their symbol table entries.
 *  - All strings are entered into the string_list
 */
void create_tables ( void )
{

    global_symbols = find_globals();
    for(size_t i = 0; i < syntax_tree->n_children; i++) {
        node_t *child = syntax_tree->children[i];
        if(child->type == FUNCTION) {
            bind_names(child->entry, child);
        }
    }

    // TODO:
    // First use find_globals() to create the global symbol table.
    // As global symbols are added, function symbols get their own local symbol tables as well.
    //
    // Once all global symbols are added, go through all functions bodies.
    // All references to variables or functions should get a pointer to their corresponding symbol in the table.
    //
    // Binding should performed by bind_names ( function symbol table, function body AST node ).
    // It also handles adding local variables to the local symbol table, and pushing and popping scopes.
    // A final task performed by bind_names (), is adding strings to the global string list
}

/* Prints the global symbol table, and the local symbol tables for each function.
 * Also prints the global string list.
 * Finally prints out the AST again, with bound symbols.
 */
void print_tables ( void )
{
    print_symbol_table ( global_symbols, 0 );
    printf("\n == STRING LIST == \n");
    print_string_list ();
    printf("\n == BOUND SYNTAX TREE == \n");
    print_syntax_tree ();
}

/* Destroys all symbol tables and the global string list */
void destroy_tables ( void )
{
    destroy_symbol_tables ( );
    destroy_string_list ( );
}

/* Internal matters */

/* Goes through all global declarations in the syntax tree, adding them to the global symbol table.
 * When adding functions, local symbol tables are created, and symbols for the functions parameters are added.
 */
static void find_globals ( void )
{
    for(size_t i = 0; i < syntax_tree->n_children; i++) {
        node_t *child = syntax_tree->children[i];
        if(child->type == GLOBAL) {
            for(size_t j = 0; j < child->n_children; j++) {
                node_t *global = child->children[j];
                symbol_t *symbol = malloc(sizeof(symbol_t));
                symbol->name = global->data;
                symbol->type = SYMBOL_GLOBAL;
                symbol->stack_offset = 0;
                symbol->data = NULL;
                symbol->node = global;
                symbol_table_insert(global_symbols, symbol);
            }
        }
    }
    return global_symbols;

    // TODO: Create symbols for all global defintions (global variables, arrays and functions), and add them to the global symbol table
    // Functions can also get their local symbol tables created here, and symbols for all its parameters
}

/* A recursive function that traverses the body of a function, and:
 *  - Adds variable declarations to the function's local symbol table.
 *  - Pushes and pops local variable scopes when entering and leaving blocks.
 *  - Binds all other IDENTIFIER_DATA nodes to the symbol it references.
 *  - Moves STRING_DATA nodes' data into the global string list,
 *    and replaces the node with a STRING_LIST_REFERENCE node.
 *    This node's data is the string's position in the list casted to a void*
 */
static void bind_names ( symbol_table_t *local_symbols, node_t *node )
{
    // TODO: Implement bind_names, doing all the things described above
    // Tip: See symbol_hashmap_init () in symbol_table.h, to make new hashmaps for new scopes.
    // Remember the symbol_hashmap_t's backup pointer, allowing you to make linked lists of backup hashmaps.

    // Tip: After inserting a string into the string list using add_string, its position is returned.
    // Cast this position to a void* to store it in the data field using (void*) position;

    if(node->type == FUNCTION_DATA) {
        symbol_table_t *function_symbols = symbol_table_init();
        node_t *parameters = node->children[0];
        for(size_t i = 0; i < parameters->n_children; i++) {
            node_t *parameter = parameters->children[i];
            symbol_t *symbol = malloc(sizeof(symbol_t));
            symbol->name = parameter->data;
            symbol->type = SYMBOL_LOCAL;
            symbol->stack_offset = 0;
            symbol->data = NULL;
            symbol->node = parameter;
            symbol_table_insert(function_symbols, symbol);
        }
        node->entry = function_symbols;
    };

    if(node->type == BLOCK) {
        symbol_table_t *backup = local_symbols->backup;
        local_symbols->backup = symbol_table_init();
        local_symbols->backup->backup = backup;
    };

    if(node->type == IDENTIFIER_DATA) {
        symbol_t *symbol = symbol_table_find(local_symbols, node->data);
        if(symbol == NULL) {
            symbol = symbol_table_find(global_symbols, node->data);
        }
        node->entry = symbol;
    };
}

/* Prints the given symbol table, with sequence number, symbol names and types.
 * When printing function symbols, its local symbol table is recursively printed, with indentation.
 */
static void print_symbol_table ( symbol_table_t *table, int nesting )
{
    // TODO: Output the given symbol table

    // TIP: Use SYMBOL_TYPE_NAMES[ my_sybmol->type ] to get a human readable string for each symbol type
}

/* Frees up the memory used by the global symbol table, all local symbol tables, and their symbols */
static void destroy_symbol_tables ( void )
{
    // TODO: Implement cleanup. All symbols in the program are owned by exactly one symbol table.
}

/* Adds the given string to the global string list, resizing if needed.
 * Takes ownership of the string, and returns its position in the string list.
 */
static size_t add_string ( char *string )
{
    if ( string_list_len == string_list_capacity ) {
        string_list_capacity *= 2;
        string_list = realloc ( string_list, string_list_capacity * sizeof ( char* ) );
    }
    string_list[ string_list_len ] = string;
    return string_list_len++;
}

/* Prints all strings added to the global string list */
static void print_string_list ( void )
{
    for(size_t i = 0; i < string_list_len; i++) {
        printf("%ld: %s\n", i, string_list[i]);
    };
}

/* Frees all strings in the global string list, and the string list itself */
static void destroy_string_list ( void )
{
    
    // TODO: Called during cleanup, free strings, and the memory used by the string list itself
}
