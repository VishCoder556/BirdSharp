/*
 * BirdSharp Compiler
 * BirdSharp is a programming language that aims to be similar to C, while having many high level features.
 *
 * Why Use BirdSharp:
 *   1. BirdSharp, as stated earlier, is similar to C, but has nearly no bloat.
 *   2. BirdSharp is extremely fast (although dynamic libraries seem to slow the compile time a lot)
 *   3. BirdSharp supports bytecode and compilation directly into Assembly that can be compiled to create executables.
 *   4. BirdSharp has many features unique to it, such as modes, that allow you to change some stuff about the compiler during runtime, instead of having to rely on compiler flags for such tasks.
 *
 * NOTE: BirdSharp is not finished yet, and hence has a ton of unclean code, and places to improve upon. Do not rely on BirdSharp for complex projects.
 *
 * How to try out BirdSharp:
 *  There are two ways:
 *      1. Bytecode (Has several restrictions, is new):
 *          Just run "./exes/bs.out main.bs -byte" to create your .bt
 *          Then run "./exes/run.out main.bt" to run that
 *      2. Executables (The main version of BirdSharp):
 *          Run "./exes/bs.out main.bs" to create the .s file
 *              (This will automatically assemble it into a .o file and link it into a .out)
 *          Run "./main.out" to run your file!
 *
 *
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <stdbool.h>
#include <wchar.h>
#include <time.h>

char *HELP = 
"Help for AprilScript\n\n"
"-o: Specify the output file\n"
"-h: Print this menu\n"
"-dynamic: Specify dynamic linking (linking with the stdlibrary + a lot more, but at the cost of the program being slow)\n"
"-static: Specify static linking (done by default, no linking with stdlib and other libraries, but the program is fast)\n"
"-...: Anything starting with '-' will be treated as a flag that will be sent to the assembler and linker\n";



enum BirdSharpTypes {
   TYPE_STATIC,
   TYPE_DYNAMIC
};
typedef enum BirdSharpTypes BirdSharpTypes;
BirdSharpTypes _BirdSharpTypes;


enum TokenType {
    TOKEN_EOF,
    TOKEN_STRING,
    TOKEN_LP,
    TOKEN_RP,
    TOKEN_LB,
    TOKEN_RB,
    TOKEN_HASH,
    TOKEN_EXC,
    TOKEN_COMMA,
    TOKEN_INT,
    TOKEN_EQ,
    TOKEN_DOLLAR,
    TOKEN_CHAR,
    TOKEN_ID,
    TOKEN_LT,
    TOKEN_GT,
    TOKEN_PLUS,
    TOKEN_SUB,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_MODULO,
    TOKEN_AMP,
    TOKEN_DOT,
    TOKEN_FLOAT
};



char *token_to_string(int tokentype){
    switch (tokentype){
        case TOKEN_EOF: return "end of file";
        case TOKEN_STRING: return "string";
        case TOKEN_CHAR: return "character";
        case TOKEN_ID: return "identifier";
        case TOKEN_LP: return "'('";
        case TOKEN_RP: return "')'";
        case TOKEN_LB: return "'{'";
        case TOKEN_RB: return "'}'";
        case TOKEN_HASH: return "'#'";
        case TOKEN_EXC: return "'!'";
        case TOKEN_COMMA: return "','";
        case TOKEN_INT: return "integer";
        case TOKEN_EQ: return "'='";
        case TOKEN_DOLLAR: return "'$'";
        case TOKEN_LT: return "'<'";
        case TOKEN_GT: return "'>'";
        case TOKEN_PLUS: return "'+'";
        case TOKEN_SUB: return "'-'";
        case TOKEN_MUL: return "'*'";
    };
    return "unknown";
};

typedef enum{
    AST_UNKNOWN,
    AST_STRING,
    AST_CHAR,
    AST_FUNCALL,
    AST_FUNCDEF,
    AST_FLOAT,
    AST_INT,
    AST_RET,
    AST_ASSIGN,
    AST_VAR,
    AST_DOLLAR,
    AST_LT,
    AST_GT,
    AST_LTE,
    AST_GTE,
    AST_PLUS,
    AST_SUB,
    AST_MUL,
    AST_DIV,
    AST_MODULO,
    AST_EQ,
    AST_NEQ,
    AST_IF,
    AST_WHILE,
    AST_DEREF,
    AST_REF,
    AST_MODE,
    AST_EXPR,
    AST_CAST,
    AST_SYSCALL,
    AST_BREAK,
    AST_MODE_IF,
    AST_TYPEDEF,
    AST_NOT
}AST_Type;

typedef struct AST AST;
typedef struct AST_T AST_T;

typedef struct Arg {
    char *value;
}AST_Arg;
typedef struct {
    char *name;
    AST *args[100];
    int argslen;
} AST_FuncCall;
typedef struct {
    AST *statements[100];
    int statementLen;
    AST *condition;
}Block;
typedef struct {
    AST *condition;
    AST *block[100];
    int blocklen;
    AST *else1[100];
    int elselen;
    Block elseif[100];
    int elseiflen;
} AST_If;
typedef struct {
    AST *condition;
    AST *block[100];
    int blocklen;
} AST_While;
typedef struct {
    AST *from;
    AST *assignto;
} AST_Assign;

typedef struct {
    char *type;
    char ptrnum;
} AST_TypeInfo;

typedef struct {
    AST *ret;
} AST_Return;

typedef struct Argument Argument;

typedef struct {
    char *name;
    struct Argument *args[100];
    int argslen;
    AST *block[100];
    int blocklen;
} AST_FuncDef;

typedef struct {
    AST *left;
    AST *right;
} AST_Expr;

typedef struct {
    char *name;
    char *res;
}AST_Mode;

typedef union {
    AST_FuncDef funcdef;
    AST_FuncCall funcall;
    AST_Assign assign;
    AST_Return ret;
    AST_Arg arg;
    AST_Expr expr;
    AST_If if1;
    AST_While while1;
    AST_Mode mode;
} AST_Data;

struct AST {
    AST_Type type;
    AST_Data data;
    AST_TypeInfo typeinfo;
    int row;
    int col;
};


typedef struct Token{
    int type;
    char value[90];
    int row;
    int col;
    char *name;
}Token;
typedef struct Macro {
    char macro[100];
    Token tokens[500];
    int tokenlen;
}Macro;
typedef struct Tokenizer {
    char *code;
    char *name;
    int cur;
    int line;
    int col;
    Token tokens[9000];
    int tokenlen;
}Tokenizer;
typedef struct Parser {
    AST asts[9000];
    int astlen;
    AST prevAst;
    char *name;
    int cur;
    Token tokens[9000];
    int tokenlen;
}Parser;

typedef struct Argument {
    char *arg;
    AST_TypeInfo type;
}Argument;

typedef struct Variable {
    char *name;
    AST_TypeInfo type_info;
}Variable;

typedef struct Scope {
    Variable variables[100];
    int variablesLen;
}Scope;

typedef struct Function {
    char *name;
    Argument args[200];
    int argslen;
    AST_TypeInfo type;
    Scope scope;
}Function;

typedef enum TranspilerMode {
    MODE_POINTER_ALLOC,
    MODE_POINTER_NOALLOC
}TranspilerMode;


typedef struct Transpiler {
    AST asts[9000];
    int astlen;
    int cur;
    char *asmc;
    char *datac;
    char *bssc;
    char *prec;
    char *prevWhileLoop;
    Function functions[500];
    int functionLen;
    int curfun;
    char *toInclude[25];
    int toIncludeLen;
    Variable variables[500];
    int variablelen;
    unsigned char isTab;
    int datanum;
    char *name;
    int funcnum;
    TranspilerMode transpiler_mode;
}Transpiler;

typedef struct ByteCodeFunction {
    char *name;
    int addr;
}ByteCodeFunction;

typedef struct ByteCode {
    AST asts[6000];
    int current_instr;
    int instr_len;
    int cur;
    int astlen;
    unsigned char bytecode[200];
    char *stringtable[100];
    int stringtablelen;
    ByteCodeFunction func[100];
    int funclen;
    int instr_num;
}ByteCode;

struct Pair {
    char *name;
    int length;
    char *longname;
    int ptrnum;
};

struct Pair types[100] = {
    (struct Pair){"int", 4, "integer", 0},
    (struct Pair){"char", 1, "character", 0},
    (struct Pair){"long", 8, "long integer", 0},
    (struct Pair){"float", 4, "float", 0}
};
int typesLen = 4;

// Utilities:

void error_generate(char *type, char *name){
    printf("\x1b[1;31m%s\x1b[0m: %s\n", type, name);
    exit(-1);
};
void error_generate_parser(char *type, char *name, int l, int c, char *f){
    printf("\x1b[1;37m%s:%d:%d: \x1b[1;31m%s\x1b[0m: %s\n", f, l, c, type, name);
    exit(-1);
};
void warning_generate(char *type, char *name){
    printf("\x1b[1;35m%s\x1b[0m: %s\n", type, name);
};
void warning_generate_parser(char *type, char *name, int l, int c, char *f){
    printf("\x1b[1;37m%s:%d:%d: \x1b[1;35m%s\x1b[0m: %s\n", f, l, c, type, name);
};
char* get_argument_register(int n) {
    switch (n) {
        case 0: return "rdi";
        case 1: return "rsi";
        case 2: return "rdx";
        case 3: return "rcx";
        case 4: return "r8";
        case 5: return "r9";
        default: return "stack";
    }
}

int type_to_len(AST_TypeInfo type){
    if (!type.type){
        return 8;
    }
    if (type.ptrnum != 0){
        return 8;
    };
    if (strcmp(type.type, "float") == 0){
        return 16;
    };
    for (int v=0; v<typesLen; v++){
        if (strcmp(types[v].name, type.type) == 0){
            return types[v].length;
        };
    };
    return -1;
    fprintf(stderr, "type_to_len on line %d: doesn't work", __LINE__);
    exit(0);
    return -1;
};

int fetch_global_variable(Transpiler *transpiler, char *variable){
    for (int v=0; v<transpiler->variablelen; v++){
        if (strcmp(transpiler->variables[v].name, variable) == 0){
            return v;
        };
    }
    return -1;
};

int try_fetch_global_variable_in_scope(Transpiler *transpiler, Scope scope, char *variable){
    for (int v=0; v<scope.variablesLen; v++){
        if (strcmp(scope.variables[v].name, variable) == 0){
            return v;
        };
    }
    return -1;
};

int try_find_local_var(Transpiler *transpiler, char *variable){ 
    if (transpiler->functionLen == 0){
        return -1;
    }
    return try_fetch_global_variable_in_scope(transpiler, transpiler->functions[transpiler->functionLen-1].scope, variable);
}

char *reg_based_on_size(char *reg, int len) {
    char *regs64[] = {"rax", "rbx", "rcx", "rdx", "rsi", "rdi", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};
    char *regs32[] = {"eax", "ebx", "ecx", "edx", "esi", "edi", "r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d"};
    char *regs16[] = {"ax", "bx", "cx", "dx", "si", "di", "r8w", "r9w", "r10w", "r11w", "r12w", "r13w", "r14w", "r15w"};
    char *regs8[] = {"al", "bl", "cl", "dl", "sil", "dil", "r8b", "r9b", "r10b", "r11b", "r12b", "r13b", "r14b", "r15b"};
    char *regs128[] = {"xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7", "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", "xmm14", "xmm15"};

    for (int i = 0; i < sizeof(regs64) / sizeof(regs64[0]); i++) {
        if (strcmp(reg, regs64[i]) == 0) {
            switch (len) {
                case 16: return regs128[i];
                case 8: return regs64[i];
                case 4: return regs32[i];
                case 2: return regs16[i];
                case 1: return regs8[i];
            }
        }
    }
    for (int i = 0; i < sizeof(regs32) / sizeof(regs32[0]); i++) {
        if (strcmp(reg, regs32[i]) == 0) {
            switch (len) {
                case 16: return regs128[i];
                case 8: return regs64[i];
                case 4: return regs32[i];
                case 2: return regs16[i];
                case 1: return regs8[i];
            }
        }
    }
    for (int i = 0; i < sizeof(regs16) / sizeof(regs16[0]); i++) {
        if (strcmp(reg, regs16[i]) == 0) {
            switch (len) {
                case 16: return regs128[i];
                case 8: return regs64[i];
                case 4: return regs32[i];
                case 2: return regs16[i];
                case 1: return regs8[i];
            }
        }
    }
    for (int i = 0; i < sizeof(regs8) / sizeof(regs8[0]); i++) {
        if (strcmp(reg, regs8[i]) == 0) {
            switch (len) {
                case 16: return regs128[i];
                case 8: return regs64[i];
                case 4: return regs32[i];
                case 2: return regs16[i];
                case 1: return regs8[i];
            }
        }
    }
    for (int i = 0; i < sizeof(regs128) / sizeof(regs128[0]); i++) {
        if (strcmp(reg, regs128[i]) == 0) {
            switch (len) {
                case 16: return regs128[i];
                case 8: return regs64[i];
                case 4: return regs32[i];
                case 2: return regs16[i];
                case 1: return regs8[i];
            }
        }
    }

    return reg;
}


char *typeinfo_to_string(AST_TypeInfo typeinfo){
    char a[1000];
    for (int v=0; v<typesLen; v++){
        if (strcmp(types[v].name, typeinfo.type) == 0){
            strncpy(a, types[v].longname, 1000);
        };
    };
    strncat(a, " ", 1);
    for (int v=0; v<typeinfo.ptrnum; v++){
        strncat(a, "pointer ", 9);
    };
    a[strlen(a)-1] = '\0';
    return strdup(a);
}

int get_argument_number(char *a) {
    if (strcmp(a, "rdi") == 0){
        return 0;
    }else if (strcmp(a, "rsi") == 0){
        return 1;
    }else if (strcmp(a, "rdx") == 0){
        return 2;
    }else if (strcmp(a, "rcx") == 0){
        return 3;
    }else if (strcmp(a, "r8") == 0){
        return 4;
    }else if (strcmp(a, "r9") == 0){
        return 5;
    };
    return -1;
}


Scope get_function_scope(Transpiler *transpiler){
    return transpiler->functions[transpiler->functionLen-1].scope;
};

AST_TypeInfo fetch_type(Transpiler *transpiler, AST *ast){
    if (ast->type == AST_FLOAT){
        AST_TypeInfo typeinfo;
        typeinfo.type = "float";
        typeinfo.ptrnum = 0;
        return typeinfo;
    }
    if (ast->type == AST_INT){
        AST_TypeInfo typeinfo;
        typeinfo.type = "null";
        typeinfo.ptrnum = 0;
        return typeinfo;
    }
    if (ast->type == AST_STRING){
        AST_TypeInfo typeinfo;
        typeinfo.type = "char";
        typeinfo.ptrnum = 1;
        return typeinfo;
    }else if (ast->type == AST_CHAR){
        AST_TypeInfo typeinfo;
        typeinfo.type = "char";
        typeinfo.ptrnum = 0;
        return typeinfo;
    };
    if (ast->type == AST_DEREF){
        AST_TypeInfo v7 = fetch_type(transpiler, ast->data.expr.left);
        v7.ptrnum--;
        if (v7.ptrnum == -1){
            error_generate_parser("DerefError", "Only pointers can be dereferenced", ast->row, ast->col, transpiler->name);
            v7.ptrnum = 0;
        }
        return v7;
    };
    if (ast->type == AST_REF){
        AST_TypeInfo v7 = fetch_type(transpiler, ast->data.expr.left);
        v7.ptrnum++;
        return v7;
    };
    if (ast->type == AST_ASSIGN){
        return fetch_type(transpiler, ast->data.assign.from);
    }
    if (ast->type == AST_VAR){
        char c = 0;
        if ((c = fetch_global_variable(transpiler, ast->data.arg.value)) != -1){
            return transpiler->functions[transpiler->functionLen-1].scope.variables[c].type_info;
        };
        for (int n=0; n<transpiler->functions[transpiler->functionLen-1].argslen; n++){
            if (strcmp(transpiler->functions[transpiler->functionLen-1].args[n].arg, ast->data.arg.value) == 0){
                return transpiler->functions[transpiler->functionLen-1].args[n].type;
            };
        }
        if ((c = try_find_local_var(transpiler, ast->data.arg.value)) != -1){
            AST_TypeInfo type_info = get_function_scope(transpiler).variables[c].type_info;
            return type_info;
        }
        if (strcmp(ast->data.arg.value, "argc") == 0){
            return (AST_TypeInfo){"int", 0};
        }else if (strcmp(ast->data.arg.value, "argv") == 0){
            return (AST_TypeInfo){"char", 2};
        }else if(strcmp(ast->data.arg.value, "__LINE__") == 0){
            return (AST_TypeInfo){"int", 0};
        }else if(strcmp(ast->data.arg.value, "__COL__") == 0){
            return (AST_TypeInfo){"int", 0};
        }

        char a[100];
        snprintf(a, 100, "Variable '%s' does not exist", ast->data.arg.value);
        error_generate("VariableError", a);
        exit(0);
    };
    if (ast->type == AST_FUNCALL){
        if (strcmp(ast->data.funcall.name, "string_len") == 0 || strcmp(ast->data.funcall.name, "malloc") == 0){
            return ast->typeinfo;
        };
        for (int i=0; i<transpiler->functionLen; i++){
            if (strcmp(transpiler->functions[i].name, ast->data.funcall.name) == 0){
                return transpiler->functions[i].type;
            };
        };
        return (AST_TypeInfo){"int", 0};
    };
    if (ast->type == AST_PLUS || ast->type == AST_SUB || ast->type == AST_MUL || ast->type == AST_DIV || ast->type == AST_MODULO){
        AST_TypeInfo type = fetch_type(transpiler, ast->data.expr.left);
        return fetch_type(transpiler, ast->data.expr.left);
    };
    return ast->typeinfo;
}

char *len_to_buf(AST_TypeInfo type, int len, char *name){
    char a[100];
    if (len == 1) {
        strncpy(a, "byte [", 100);
        strcat(a, name);
        strcat(a, "]");
        return strdup(a);
    }else if (len == 2) {
        strncpy(a, "word [", 100);
        strcat(a, name);
        strcat(a, "]");
        return strdup(a);
    }else if (len == 4) {
        strncpy(a, "dword [", 100);
        strcat(a, name);
        strcat(a, "]");
        return strdup(a);
    }else if (len == 8) {
        strncpy(a, "qword [", 100);
        strcat(a, name);
        strcat(a, "]");
        return strdup(a);
    }
    assert(0 && "len_to_buf is not working");
    return strdup(a);
};


char *len_to_res(int len){
    if (len == 1) {
        return "resb 1";
    }else if (len == 2) {
        return "resb 2";
    }else if (len == 4) {
        return "resd 1";
    }else if (len == 8) {
        return "resq 1";
    }
    assert(0 && "len_to_buf is not working");
    return "";
};

char *deref_type_as_var(AST_TypeInfo type, char *name){
    int len = -1;
    for (int v=0; v<typesLen; v++){
        if (strcmp(types[v].name, type.type) == 0){
            len = types[v].length;
            break;
        };
    };
    if (type.ptrnum > 0){
        len = 8;
    };
    if (len == -1){
        assert(0 && "Should be unreachable");
    }
    // if (strcmp(type.type, "") == 0){
    //     len = 1;
    // };
    return len_to_buf(type, len, name);
}

char *type_as_var(AST_TypeInfo type, char *name){
    int len = -1;
    for (int v=0; v<typesLen; v++){
        if (type.type == NULL){
            len = 4;
            break;
        }
        if (strcmp(types[v].name, type.type) == 0){
            len = types[v].length;
        };
    };
    if (type.ptrnum > 0){
        len = 8;
    };
    if (len == -1){
        assert(0 && "Should be unreachable");
    }
    // if (strcmp(type.type, "string") == 0){
    //     len = 1;
    // };
    return len_to_buf(type, len, name);
};
char parse_type(Parser *parser, AST_TypeInfo *typeinfo){ // Assumes parser->cur points to the idx of the type 
    /* char c = -1;
    for (int v=0; v<typesLen; v++){
        if (strcmp(types[v].name, parser->tokens[parser->cur].value) == 0){
            c = v;
            break;
        };
    }; */
    typeinfo->type = parser->tokens[parser->cur].value;
    parser->cur++;
    typeinfo->ptrnum = 0;
    while (parser->tokens[parser->cur].type == TOKEN_MUL){
        typeinfo->ptrnum++;
        parser->cur++;
    };
    return 0;
}
char is_type(Parser *parser, Token tok){
    char c = -1;
    for (int v=0; v<typesLen; v++){
        if (strcmp(types[v].name, tok.value) == 0){
            c = v;
            return 0;
        };
    };
    return -1;
};



char can_be_deref(AST_TypeInfo typeinfo){
    if(typeinfo.ptrnum > 0){
        return 0;
    }
    return -1;
};

AST ptr_deref(Transpiler *transpiler, AST ast, AST ast2){
    if (ast2.type == AST_DEREF){
        ast.typeinfo.ptrnum--;
    };
    return ast;
};

char *res_get(Transpiler *transpiler, AST_TypeInfo typeinfo){
    int len = -1;
    for (int v=0; v<typesLen; v++){
        if (strcmp(typeinfo.type, types[v].name) == 0){
            len = types[v].length;
        }
    }
    /* if (len == 1){
        assert(0 && "Unreachable");
    }; */
    if (typeinfo.ptrnum > 0){
        len = 8;
    };
    return len_to_res(len);
}

AST_TypeInfo dec_ptrnum(AST_TypeInfo typeinfo){
    typeinfo.ptrnum--;
    return typeinfo;
};

void initialize_bssc(Transpiler *transpiler, AST_TypeInfo typeinfo, char *name){
    char a[200];
    snprintf(a, 200, "\n\t%s: %s", name, res_get(transpiler, typeinfo));
    strcat(transpiler->bssc, a);
    if (transpiler->transpiler_mode == MODE_POINTER_ALLOC){
        if (typeinfo.ptrnum > 0){
            AST_TypeInfo v2 = typeinfo;
            v2 = dec_ptrnum(v2);
            snprintf(a, 200, "_LBC_%s", name);
            initialize_bssc(transpiler, v2, a);
            char b[200];
            snprintf(b, 200, "lea r9, %s\n\tmov %s, %s\n\t", type_as_var(v2, a), type_as_var(typeinfo, name), reg_based_on_size("r9", type_to_len(typeinfo)));
            strncat(transpiler->prec, b, 200);
        };
    }
};

#define MayCast arith == true ? "Doing arithmetic with" : "Casting"
#define MayInto arith == true ? "and" : "into"

void check_cast(Transpiler *transpiler, AST ast1, AST ast2, int l, int c, char *f, bool arith){
    AST_TypeInfo type1 = fetch_type(transpiler, &ast1);
    AST_TypeInfo type2 = fetch_type(transpiler, &ast2);
    if (type2.type == 0){
        for (int i=0; i<transpiler->variablelen; i++) {
            if (strcmp(transpiler->variables[i].name, ast2.data.arg.value) == 0){
                if (type2.type == 0){
                    type2 = transpiler->variables[i].type_info;
                }
            };
        };
    }
    if (type2.type == 0 || type1.type == 0){return;}
    if (type1.ptrnum > 0){
        if (type2.ptrnum == 0 && (strcmp(type1.type, "char") && type1.ptrnum != 1) && strcmp(type1.type, "null")){
            char a[100];
            snprintf(a, 100, "Instance of type %s cannot be converted into a pointer", type2.type);
            error_generate_parser("CastError", a, l, c, f);
        }
        return;
    }
    char a[1000];
    if (strcmp(type1.type, "long") == 0){
        if (strcmp(type2.type, "int") == 0 && strcmp(type2.type, "char") == 0){
            snprintf(a, 1000, "%s %s %s %s will require zero-extending", MayCast, typeinfo_to_string(type2), MayInto, typeinfo_to_string(type1));
            warning_generate_parser("CastWarning", a, l, c, f);
        }else if (strcmp(type2.type, "long")){
            snprintf(a, 1000, "Instance of type %s cannot be converted into a long", typeinfo_to_string(type2));
            error_generate_parser("CastError", a, l, c, f);
        }
    }else if (strcmp(type1.type, "int") == 0){
        if (strcmp(type2.type, "int") == 0){
        }else if(strcmp(type2.type, "char") == 0){
            snprintf(a, 1000, "%s a character %s an integer will require zero-extending", MayCast, MayInto);
            warning_generate_parser("CastWarning", a, l, c, f);
        }else if (strcmp(type2.type, "null")){
            snprintf(a, 1000, "Instance of type %s cannot be converted into an integer", typeinfo_to_string(type2));
            error_generate_parser("CastError", a, l, c, f);
        }
    }else if (strcmp(type1.type, "char") == 0){
        if (strcmp(type2.type, "int") == 0){
            snprintf(a, 1000, "%s %s %s %s may result in losing some of the %s's data", MayCast, typeinfo_to_string(type2), MayInto, typeinfo_to_string(type1), typeinfo_to_string(type2));
            warning_generate_parser("CastWarning", a, l, c, f);
        }else if (strcmp(type2.type, "char") && strcmp(type2.type, "null")){
            snprintf(a, 1000, "Instance of type %s cannot be converted into a pointer", typeinfo_to_string(type2));
            error_generate_parser("CastError", a, l, c, f);
        }
    }else {
        int ptrnumnum = type1.ptrnum;
        for (int i=0; i<typesLen; i++){
            if (strcmp(types[i].name, type1.type) == 0){
                ptrnumnum = types[i].ptrnum;
            };
        };
        if (ast1.type == AST_DEREF || ast1.type == AST_REF){
            ptrnumnum = type1.ptrnum;
        };
        int ptrnumnum2 = type2.ptrnum;
        for (int i=0; i<typesLen; i++){
            if (strcmp(types[i].name, type2.type) == 0){
                ptrnumnum2 = types[i].ptrnum;
            };
        };
        if (ast2.type == AST_DEREF || ast2.type == AST_REF){
            ptrnumnum2 = type2.ptrnum;
        };
        if (ptrnumnum == -1){
            error_generate("TypeError", "Unsupported Type");
        };
        if (ptrnumnum != ptrnumnum2){
            error_generate("CastError", "You can only set a pointer to another pointer");
        };
    };

};

char *get_value_of_variable(AST *ast){
    if (ast->type == AST_DEREF || ast->type == AST_REF){
        return get_value_of_variable(ast->data.expr.left);
    }
    if (ast->type == AST_PLUS || ast->type == AST_SUB || ast->type == AST_MUL || ast->type == AST_DIV){
        return get_value_of_variable(ast->data.expr.left);
    };
    return ast->data.arg.value;
    error_generate("SyntaxError", "Invalid type");
    return "";
};

char *transpile_newl(Transpiler *transpiler);
char *correct_format_for_var(Transpiler *transpiler, char *var, char *reg, AST_TypeInfo type){
    char c = -1;
    if ((c = fetch_global_variable(transpiler, var)) != -1){
        return type_as_var(transpiler->variables[c].type_info, var);
    }
    if ((c = try_find_local_var(transpiler, var)) != -1){
        char num = 0;
        char c = -1;
        for (int v=0; v<get_function_scope(transpiler).variablesLen; v++){
            if (strcmp(get_function_scope(transpiler).variables[v].name, var) == 0){
                c = v;
                break;
            };
            num += type_to_len(get_function_scope(transpiler).variables[v].type_info);
        };
        num = 32 - num;
        char a[100];
        char b[100];
        snprintf(b, 100, "rbp - %d", num);
        snprintf(a, 100, "%s", type_as_var(type, b));
        return strdup(a);
        if (get_function_scope(transpiler).variables[c].type_info.ptrnum == 0){
            char b[100];
            snprintf(b, 100, "rbp + %d", num);
            snprintf(a, 100, "qword [%s]", b);
            // snprintf(a, 100, "%s", type_as_var(get_function_scope(transpiler).variables[c].type_info, b));
            return strdup(a);
        }
        if (strcmp(reg, "self") == 0){
            snprintf(a, 100, "[rbp + %d]", num);
            // snprintf(a, 100, "r8, rbp%sadd r8, %d%smov r8", transpile_newl(transpiler), num, transpile_newl(transpiler));
        }else {
            snprintf(a, 100, "rbp%sadd %s, %d%s", transpile_newl(transpiler), reg, num, transpile_newl(transpiler));
        }
        return strdup(a);
    }
    return "";
}





// Tokenizer
char tokenizer_token(Tokenizer *tokenizer){
    char c = tokenizer->code[tokenizer->cur];
    int v = tokenizer->col;
    if (c == '\0'){
        tokenizer->tokens[tokenizer->tokenlen++] = (Token){TOKEN_EOF, "\0", tokenizer->line, tokenizer->col, tokenizer->name};
        return -1;
    }
    if (c == '('){
        tokenizer->tokens[tokenizer->tokenlen++] = (Token){TOKEN_LP, "(", tokenizer->line, tokenizer->col, tokenizer->name};
    }else if (c == ')'){
        tokenizer->tokens[tokenizer->tokenlen++] = (Token){TOKEN_RP, ")", tokenizer->line, tokenizer->col, tokenizer->name};
    }else if (c == '{'){
        tokenizer->tokens[tokenizer->tokenlen++] = (Token){TOKEN_LB, "{", tokenizer->line, tokenizer->col, tokenizer->name};
    }else if (c == ','){
        tokenizer->tokens[tokenizer->tokenlen++] = (Token){TOKEN_COMMA, ",", tokenizer->line, tokenizer->col, tokenizer->name};
    }else if (c == '}'){
        tokenizer->tokens[tokenizer->tokenlen++] = (Token){TOKEN_RB, "}", tokenizer->line, tokenizer->col, tokenizer->name};
    }else if (c == '='){
        tokenizer->tokens[tokenizer->tokenlen++] = (Token){TOKEN_EQ, "=", tokenizer->line, tokenizer->col, tokenizer->name};
    }else if (c == ';'){
    }else if (c == '#'){
        tokenizer->tokens[tokenizer->tokenlen++] = (Token){TOKEN_HASH, "#", tokenizer->line, tokenizer->col, tokenizer->name};
    }else if (c == '$'){
        tokenizer->tokens[tokenizer->tokenlen++] = (Token){TOKEN_DOLLAR, "$", tokenizer->line, tokenizer->col, tokenizer->name};
    }else if (c == '<'){
        tokenizer->tokens[tokenizer->tokenlen++] = (Token){TOKEN_LT, "<", tokenizer->line, tokenizer->col, tokenizer->name};
    }else if (c == '>'){
        tokenizer->tokens[tokenizer->tokenlen++] = (Token){TOKEN_GT, ">", tokenizer->line, tokenizer->col, tokenizer->name};
    }else if (c == '+'){
        tokenizer->tokens[tokenizer->tokenlen++] = (Token){TOKEN_PLUS, "+", tokenizer->line, tokenizer->col, tokenizer->name};
    }else if (c == '*'){
        tokenizer->tokens[tokenizer->tokenlen++] = (Token){TOKEN_MUL, "*", tokenizer->line, tokenizer->col, tokenizer->name};
    }else if (c == '/'){
        if (tokenizer->code[tokenizer->cur+1] == '/'){ // Comment
            tokenizer->cur += 2;
            while (tokenizer->code[tokenizer->cur] != '\n' && tokenizer->code[tokenizer->cur] != '\0'){
                tokenizer->cur++;
            };
            tokenizer->line++;
            tokenizer->cur++;
            return tokenizer_token(tokenizer);
        }else if (tokenizer->code[tokenizer->cur+1] == '*'){ // Comment
            tokenizer->cur += 2;
            char c = 0;
            while (tokenizer->code[tokenizer->cur] != '\0'){
                if (tokenizer->code[tokenizer->cur] != '/' && c == 1){
                    c = 0;
                }else if (c == 1){
                    break;
                }
                if (tokenizer->code[tokenizer->cur] == '\n'){
                    tokenizer->line++;
                }

                if (tokenizer->code[tokenizer->cur] == '*'){
                    c = 1;
                };
                tokenizer->cur++;
            };
            tokenizer->cur++;
            return tokenizer_token(tokenizer);
        };
        tokenizer->tokens[tokenizer->tokenlen++] = (Token){TOKEN_DIV, "/", tokenizer->line, tokenizer->col, tokenizer->name};
    }else if (c == '%'){
        tokenizer->tokens[tokenizer->tokenlen++] = (Token){TOKEN_MODULO, "%", tokenizer->line, tokenizer->col, tokenizer->name};
    }else if(c == '.'){
        tokenizer->tokens[tokenizer->tokenlen++] = (Token){TOKEN_DOT, ".", tokenizer->line, tokenizer->col, tokenizer->name};
    }else if(c == '&'){
        tokenizer->tokens[tokenizer->tokenlen++] = (Token){TOKEN_AMP, "&", tokenizer->line, tokenizer->col, tokenizer->name};
    }else if (c == '-'){
        tokenizer->tokens[tokenizer->tokenlen++] = (Token){TOKEN_SUB, "-", tokenizer->line, tokenizer->col, tokenizer->name};
    }else if (c == '!'){
        tokenizer->tokens[tokenizer->tokenlen++] = (Token){TOKEN_EXC, "!", tokenizer->line, tokenizer->col, tokenizer->name};
    } else if (c == '\''){
        tokenizer->cur+=2;
        tokenizer->col += 2;
        char buf[3];
        if (tokenizer->code[tokenizer->cur] != '\0'){
            if (tokenizer->code[tokenizer->cur] != '\''){
                if (tokenizer->code[tokenizer->cur-1] == '\\'){
                    buf[0] = tokenizer->code[tokenizer->cur-1];
                    buf[1] = tokenizer->code[tokenizer->cur];
                    buf[2] = '\0';
                }else {
                    error_generate_parser("AbruptEndError", "Character is too long", tokenizer->line, v, tokenizer->name);
                }
            };
        }else {
            error_generate_parser("AbruptEndError", "Abrupt end", tokenizer->line, v, tokenizer->name);
        }
        if (tokenizer->code[tokenizer->cur] == '\''){
            buf[0] = tokenizer->code[tokenizer->cur-1];
            buf[1] = '\0';
        }else {
            tokenizer->cur++;
            tokenizer->col++;
        }
        tokenizer->tokens[tokenizer->tokenlen].type = TOKEN_CHAR;strcpy(tokenizer->tokens[tokenizer->tokenlen].value, buf);tokenizer->tokens[tokenizer->tokenlen].row = tokenizer->line;tokenizer->tokens[tokenizer->tokenlen].col = v;tokenizer->tokens[tokenizer->tokenlen].name = tokenizer->name;tokenizer->tokenlen++;
    }else if(c == '\"'){
        tokenizer->cur++;
        tokenizer->col++;
        c = tokenizer->code[tokenizer->cur];
        char string[1000];int stringlen = 0;
        while (c != '\"' && c != '\0'){
            if (c == '\n'){
                error_generate_parser("SyntaxError", "Newlines not allowed inside of strings. Use \\n instead", tokenizer->line, v, tokenizer->name);
            };
            string[stringlen++] = c;
            tokenizer->cur++;
            tokenizer->col++;
            if (c == '\\'){
                if (strlen(tokenizer->code) == tokenizer->cur+1){
                    error_generate_parser("SyntaxError", "Found end of string with \\", tokenizer->line, v, tokenizer->name);
                };
                if (tokenizer->code[tokenizer->cur] == '\"'){
                    string[stringlen++] = '\"';
                    tokenizer->cur++;
                    tokenizer->col++;
                    c = tokenizer->code[tokenizer->cur];
                }
            };
            c = tokenizer->code[tokenizer->cur];
        };
        if (c == '\0'){
            error_generate_parser("SyntaxError", "Abrupt end", tokenizer->line, v, tokenizer->name);
        };
        string[stringlen++] = '\0';
        tokenizer->tokens[tokenizer->tokenlen].type = TOKEN_STRING;strcpy(tokenizer->tokens[tokenizer->tokenlen].value, string);
        tokenizer->tokens[tokenizer->tokenlen].row = tokenizer->line;tokenizer->tokens[tokenizer->tokenlen].name = tokenizer->name;
        tokenizer->tokens[tokenizer->tokenlen].col = v;
        tokenizer->tokenlen++;
        if (tokenizer->tokenlen >= 2){
            if (tokenizer->tokens[tokenizer->tokenlen-3].type == TOKEN_HASH && tokenizer->tokens[tokenizer->tokenlen-2].type == TOKEN_ID && strcmp(tokenizer->tokens[tokenizer->tokenlen-2].value, "include") == 0){
                tokenizer->tokenlen -= 3;
                FILE *f = fopen(string, "r");
                if (f == NULL) {
                    char a[100];
                    snprintf(a, 100, "File '%s' does not exist", string);
                    error_generate_parser("IncludeError", a, tokenizer->tokens[tokenizer->tokenlen].row, tokenizer->tokens[tokenizer->tokenlen].col, tokenizer->name);
                };
                Tokenizer *tokenizer2 = malloc(sizeof(Tokenizer));
                tokenizer2->code = malloc(2000);
                int a = fread(tokenizer2->code, 1, 2000, f);
                tokenizer2->code[a] = '\0';
                tokenizer2->cur = 0;
                tokenizer2->tokenlen = 0;
                tokenizer2->name = string;
                tokenizer2->line = 1;
                tokenizer2->col = 0;
                fclose(f);
                while(tokenizer_token(tokenizer2) != -1){};
                for (int n=0; n<tokenizer2->tokenlen; n++){
                    tokenizer->tokens[tokenizer->tokenlen++] = tokenizer2->tokens[n];
                };
                tokenizer->tokenlen--;
                free(tokenizer2);
            };
        };
    }else if(isalpha(c) || c == '_'){
        char res[200];int reslen=0;
        while (isalnum(c) || c == '_'){
            res[reslen++] = c;
            tokenizer->cur++;
            tokenizer->col++;
            c = tokenizer->code[tokenizer->cur];
        };
        res[reslen] = '\0';
        tokenizer->cur--;
        tokenizer->col--;
        tokenizer->tokens[tokenizer->tokenlen].type = TOKEN_ID;strcpy(tokenizer->tokens[tokenizer->tokenlen].value, res);tokenizer->tokens[tokenizer->tokenlen].row = tokenizer->line;tokenizer->tokens[tokenizer->tokenlen].col = v;tokenizer->tokens[tokenizer->tokenlen].name = tokenizer->name;tokenizer->tokenlen++;
        if (strcmp(tokenizer->tokens[tokenizer->tokenlen-1].value, "true") == 0){
            tokenizer->tokens[tokenizer->tokenlen-1].type = TOKEN_INT;
            strcpy(tokenizer->tokens[tokenizer->tokenlen-1].value, "1");
        }else if (strcmp(tokenizer->tokens[tokenizer->tokenlen-1].value, "false") == 0){
            tokenizer->tokens[tokenizer->tokenlen-1].type = TOKEN_INT;
            strcpy(tokenizer->tokens[tokenizer->tokenlen-1].value, "0");
        };
    }else if(isnumber(c)){
        char res[200];int reslen=0;
        while (isnumber(c)){
            res[reslen++] = c;
            tokenizer->cur++;
            tokenizer->col++;
            c = tokenizer->code[tokenizer->cur];
        };
        res[reslen] = '\0';
        if (tokenizer->code[tokenizer->cur] == 'f'){
            tokenizer->tokens[tokenizer->tokenlen].type = TOKEN_FLOAT;strcpy(tokenizer->tokens[tokenizer->tokenlen].value, res);tokenizer->tokens[tokenizer->tokenlen].row = tokenizer->line;tokenizer->tokens[tokenizer->tokenlen].col = v;tokenizer->tokens[tokenizer->tokenlen].name = tokenizer->name;tokenizer->tokenlen++;
        }else {
            tokenizer->cur--;
            tokenizer->col--;
            tokenizer->tokens[tokenizer->tokenlen].type = TOKEN_INT;strcpy(tokenizer->tokens[tokenizer->tokenlen].value, res);tokenizer->tokens[tokenizer->tokenlen].row = tokenizer->line;tokenizer->tokens[tokenizer->tokenlen].col = v;tokenizer->tokens[tokenizer->tokenlen].name = tokenizer->name;tokenizer->tokenlen++;
        }
    }else if (c == '\n') {
        tokenizer->col = 0;
        tokenizer->line++;
    }else if(c == ' ' || c == '\t'){
    } else {
        error_generate_parser("SyntaxError", "Found unknown token", tokenizer->line, v, tokenizer->name);
    };
    tokenizer->tokens[tokenizer->tokenlen-1].col = v;
    tokenizer->tokens[tokenizer->tokenlen-1].row = tokenizer->line;
    tokenizer->cur++;
    tokenizer->col++;
    return 0;
};

void rem_idx(Token arr[], int *len, int index) {
    if (index < 0 || index >= *len) {
        printf("Invalid index\n");
        return;
    }
    
    for (int i = index; i < *len - 1; i++) {
        arr[i] = arr[i + 1];
    }
    (*len)--;
}

void add_idx(Token arr[], int *len, int index) {
    if (index < 0 || index >= *len) {
        printf("Index out of bounds.\n");
        return;
    }

    for (int i = *len - 1; i > index; i--) {
        arr[i] = arr[i - 1];
    }

    arr[index] = (Token){};
    (*len)++;
}

// void parser_peek(Parser *parser){
//     parser->cur++;
// };
void parser_peek(Parser *parser){
    parser->cur++;
    if (parser->cur > parser->tokenlen){
        error_generate_parser("AbruptEndError", "Abrupt end", parser->tokens[parser->tokenlen-1].row, parser->tokens[parser->tokenlen-1].col, parser->tokens[parser->cur].name);
    };
};

void parser_expect_next(Parser *parser, int type){
    if (parser->cur+1 != parser->tokenlen){
        parser_peek(parser);
        if (parser->tokens[parser->cur].type != type){
            char error[100];
            snprintf(error, 100, "Expected %s got %s", token_to_string(type), token_to_string(parser->tokens[parser->cur].type));
            error_generate_parser("ExpectError", error, parser->tokens[parser->cur].row, parser->tokens[parser->cur].col, parser->tokens[parser->cur].name);
        };
        if (parser->cur+2 != parser->tokenlen){
            parser_peek(parser);
        }else {
        }
    }else {
        error_generate_parser("ExpectError", "Didn't expect out of bounds", parser->tokens[parser->cur].row, parser->tokens[parser->cur].col-1, parser->tokens[parser->cur].name);
    };
};
void parser_expect(Parser *parser, int type){
    if (parser->tokens[parser->cur].type != type){
        char error[100];
        snprintf(error, 100, "Expected %s got %s", token_to_string(type), token_to_string(parser->tokens[parser->cur].type));
        error_generate_parser("ExpectError", error, parser->tokens[parser->cur].row, parser->tokens[parser->cur].col, parser->tokens[parser->cur].name);
    };
    if (parser->cur != parser->tokenlen){
        parser_peek(parser);
    }else {
    }
};
AST *parser_eat_expr(Parser *parser);

AST *mode_parse_expr(Parser *parser){
    /* assert(0 && "mode_parse_expr is not implemented yet");
    exit(0); */
    AST *ast = malloc(sizeof(AST));
    ast->type = AST_MODE;
    ast->data.mode.name = malloc(500);
    strcpy(ast->data.mode.name, "");
    while (parser->tokens[parser->cur].type == TOKEN_ID || parser->tokens[parser->cur].type == TOKEN_DOT){
        strncat(ast->data.mode.name, parser->tokens[parser->cur].value, 100);
        parser_peek(parser);
    };
    if (parser->tokens[parser->cur].type == TOKEN_EOF){
        error_generate("ModeError", "Abrupt end to expression of if macro");
    };
    return ast;
};
AST *parser_eat_ast(Parser *parser);
char try_parse_mode(Parser *parser, AST *ast){
    if (parser->tokens[parser->cur].type == TOKEN_HASH){
        if (parser->cur+1 == parser->tokenlen){
            error_generate("HashError", "# not allowed this late into code");
            exit(0);
        }
        parser_peek(parser);
        if (parser->tokens[parser->cur].type == TOKEN_EXC) {
            ast->type = AST_MODE;
            char name[500];
            strcpy(name, "");
            parser->cur++;
            if (strcmp(parser->tokens[parser->cur].value, "if") == 0){ // AST Mode If Statement
                ast->type = AST_MODE_IF;
                parser_peek(parser);
                ast->data.if1 = (AST_If){};
                ast->data.if1.condition = mode_parse_expr(parser);
                parser_expect(parser, TOKEN_LB);
                while (parser->tokens[parser->cur].type != TOKEN_RB){
                    ast->data.if1.block[ast->data.if1.blocklen++] = parser_eat_ast(parser);
                };
                parser_peek(parser);
                return 0;
            };
            while (parser->tokens[parser->cur].type != TOKEN_EOF && parser->tokens[parser->cur].type != TOKEN_EQ){
                strncat(name, parser->tokens[parser->cur].value, 100);
                if (strcmp(name, "extern") == 0){
                    break;
                };
                parser->cur++;
            }
            ast->data.mode.name = strdup(name);
            if (parser->cur+1 == parser->tokenlen){
                error_generate("HashError", "# not allowed this late into code");
                exit(0);
            }
            parser_peek(parser);
            ast->data.mode.res = parser->tokens[parser->cur].value;
            parser_peek(parser);
            return 0;
        }else {
            parser->cur-=2;
        };
    };
    return -1;
};

AST *parser_eat_expr(Parser *parser){
    Token token_1 = parser->tokens[parser->cur];
    int a = parser->cur;
    AST *ast = malloc(sizeof(AST));
    if (try_parse_mode(parser, ast) == 0){
        return ast;
    };
    ast->type = AST_UNKNOWN;
    if (parser->tokens[parser->cur].type == TOKEN_LP){ // Assume it is an expression
        parser->cur++;
        ast->type = AST_EXPR;
        ast->data.expr.left = parser_eat_expr(parser);
        parser_expect(parser, TOKEN_RP);
    };
    if (parser->tokens[parser->cur].type == TOKEN_EXC){
        parser->cur++;
        ast->type = AST_NOT;
        ast->data.expr.left = parser_eat_expr(parser);
    };

    if(parser->tokens[parser->cur].type == TOKEN_AMP){
        parser->cur++;
        ast->type = AST_REF;
        ast->data.expr.left = parser_eat_expr(parser);
        ast->typeinfo = ast->data.expr.left->typeinfo;
        ast->typeinfo.ptrnum++;
        return ast;
    };
    if (parser->tokens[parser->cur].type == TOKEN_STRING){
        ast->type = AST_STRING;
        ast->typeinfo.type = "char";
        ast->typeinfo.ptrnum = 1;
        ast->data.arg.value = parser->tokens[parser->cur].value;
        parser_peek(parser);
    }else if (parser->tokens[parser->cur].type == TOKEN_INT){
        ast->type = AST_INT;
        ast->typeinfo.type = "int";
        ast->data.arg.value = parser->tokens[parser->cur].value;
        parser_peek(parser);
    }else if (parser->tokens[parser->cur].type == TOKEN_FLOAT){
        ast->type = AST_FLOAT;
        ast->typeinfo.type = "float";
        ast->data.arg.value = parser->tokens[parser->cur].value;
        parser_peek(parser);
    }else if (parser->tokens[parser->cur].type == TOKEN_CHAR){
        ast->type = AST_CHAR;
        ast->typeinfo.type = "char";
        ast->data.arg.value = parser->tokens[parser->cur].value;
        parser_peek(parser);
    }else if (parser->tokens[parser->cur].type == TOKEN_ID){
        if (parser->cur+3 != parser->tokenlen){
            if (parser->tokens[parser->cur+1].type == TOKEN_LP){
                if (strcmp(parser->tokens[parser->cur].value, "sizeof") == 0){
                    parser_expect(parser, TOKEN_ID);
                    parser_expect(parser, TOKEN_LP);
                    AST_TypeInfo *type = malloc(sizeof(AST_TypeInfo));
                    parse_type(parser, type);
                    parser_expect(parser, TOKEN_RP);
                    ast->type = AST_INT;
                    ast->typeinfo.ptrnum = 0;
                    ast->typeinfo.type = "int";
                    char abc[100];
                    snprintf(abc, 100, "%d", type_to_len(*type));
                    ast->data.arg.value = strdup(abc);
                }else {
                ast->type = AST_FUNCALL;
                ast->data.funcall = (AST_FuncCall){};
                parser_expect(parser, TOKEN_ID);
                ast->data.funcall.name = parser->tokens[parser->cur-1].value;
                parser_expect(parser, TOKEN_LP);
                if (strcmp(ast->data.funcall.name, "cast") == 0){
                    ast->type = AST_CAST;
                    parse_type(parser, &ast->typeinfo);
                    parser_expect(parser, TOKEN_COMMA);
                    ast->data.expr.left = parser_eat_expr(parser);
                    parser_expect(parser, TOKEN_RP);
                    return ast;
                };
                while (parser->tokens[parser->cur].type != TOKEN_RP && parser->tokens[parser->cur].type != TOKEN_EOF){
                    ast->data.funcall.args[ast->data.funcall.argslen++] = parser_eat_expr(parser);
                    if (parser->tokens[parser->cur].type != TOKEN_RP){
                        parser_expect(parser, TOKEN_COMMA);
                    };
                };
                parser_expect(parser, TOKEN_RP);
                if (strcmp(ast->data.funcall.name, "deref") == 0){
                    ast->type = AST_DEREF;
                    if (ast->data.funcall.argslen != 1){
                        error_generate("DerefError", "Too less or too many arguments to deref function");
                    };
                    ast->data.expr.left = ast->data.funcall.args[0];
                }else if(strcmp(ast->data.funcall.name, "syscall") == 0){
                    ast->type = AST_SYSCALL;
                };
            }
            }else {
                ast->type = AST_VAR;
                ast->data.arg.value = parser->tokens[parser->cur].value;
                parser_peek(parser);
            }
        }else {
            ast->type = AST_VAR;
            ast->data.arg.value = parser->tokens[parser->cur].value;
            parser_peek(parser);
        }
    };
    if (parser->cur == parser->tokenlen){
        error_generate_parser("SuddenStop", "Suddenly stopped expression", parser->tokens[parser->cur].row, parser->tokens[parser->cur].col, parser->tokens[parser->cur].name);
    };
    ast->col = parser->tokens[a].col;
    ast->row = parser->tokens[a].row;
    if (parser->tokens[parser->cur].type == TOKEN_PLUS){
        AST *ast2 = malloc(sizeof(AST));
        ast2->type = AST_PLUS;
        ast2->data.expr.left = ast;
        if (parser->cur + 1 == parser->tokenlen){
            error_generate("AbruptEndError", "Abrupt end");
        };
        parser_peek(parser);
        ast2->data.expr.right = parser_eat_expr(parser);
        ast = ast2;
    }else if (parser->tokens[parser->cur].type == TOKEN_SUB){
        AST *ast2 = malloc(sizeof(AST));
        ast2->type = AST_SUB;
        ast2->data.expr.left = ast;
        if (parser->cur + 1 == parser->tokenlen){
            error_generate("AbruptEndError", "Abrupt end");
        };
        parser_peek(parser);
        ast2->data.expr.right = parser_eat_expr(parser);
        return ast2;
    }else if (parser->tokens[parser->cur].type == TOKEN_MUL){
        AST *ast2 = malloc(sizeof(AST));
        ast2->type = AST_MUL;
        ast2->data.expr.left = ast;
        if (parser->cur + 1 == parser->tokenlen){
            error_generate("AbruptEndError", "Abrupt end in multiplication");
        };
        parser_peek(parser);
        if (parser->tokens[parser->cur+1].type == TOKEN_EQ){ // Only types supported
            parser->cur--;
            free(ast2);
            return ast;
        }
        ast2->data.expr.right = parser_eat_expr(parser);
        return ast2;
    }else if (parser->tokens[parser->cur].type == TOKEN_DIV){
        AST *ast2 = malloc(sizeof(AST));
        ast2->type = AST_DIV;
        ast2->data.expr.left = ast;
        if (parser->cur + 1 == parser->tokenlen){
            error_generate("AbruptEndError", "Abrupt end");
        };
        parser_peek(parser);
        ast2->data.expr.right = parser_eat_expr(parser);
        return ast2;
    }else if (parser->tokens[parser->cur].type == TOKEN_MODULO){
        AST *ast2 = malloc(sizeof(AST));
        ast2->type = AST_MODULO;
        ast2->data.expr.left = ast;
        if (parser->cur + 1 == parser->tokenlen){
            error_generate("AbruptEndError", "Abrupt end");
        };
        parser_peek(parser);
        ast2->data.expr.right = parser_eat_expr(parser);
        return ast2;
    }
    if (parser->tokens[parser->cur].type == TOKEN_LT){
        if (parser->cur + 1 == parser->tokenlen){
            error_generate("AbruptEndError", "Abrupt end");
        };
        parser_peek(parser);
        AST *ast2 = malloc(sizeof(AST));
        ast2->type = AST_LT;
        if (parser->tokens[parser->cur].type == TOKEN_EQ){
            ast2->type = AST_LTE;
        }else {
            parser->cur--;
        };
        ast2->data.expr.left = ast;
        if (parser->cur + 1 == parser->tokenlen){
            error_generate("AbruptEndError", "Abrupt end");
        }
        parser_peek(parser);
        ast2->data.expr.right = parser_eat_expr(parser);
        return ast2;
    }
    if (parser->tokens[parser->cur].type == TOKEN_GT){
        if (parser->cur + 1 == parser->tokenlen){
            error_generate("AbruptEndError", "Abrupt end");
        };
        parser_peek(parser);
        AST *ast2 = malloc(sizeof(AST));
        ast2->type = AST_GT;
        if (parser->tokens[parser->cur].type == TOKEN_EQ){
            ast2->type = AST_GTE;
        }else {
            parser->cur--;
        };
        ast2->data.expr.left = ast;
        if (parser->cur + 1 == parser->tokenlen){
            error_generate("AbruptEndError", "Abrupt end");
        };
        parser_peek(parser);
        ast2->data.expr.right = parser_eat_expr(parser);
        return ast2;
    }
    if (parser->tokens[parser->cur].type == TOKEN_EQ){
        if (parser->cur + 1 == parser->tokenlen){
            error_generate("AbruptEndError", "Abrupt end");
        };
        parser_peek(parser);
        if (parser->tokens[parser->cur].type == TOKEN_EQ){
            AST *ast2 = malloc(sizeof(AST));
            ast2->type = AST_EQ;
            ast2->data.expr.left = ast;
            if (parser->cur + 1 == parser->tokenlen){
                error_generate("AbruptEndError", "Abrupt end");
            };
            parser_peek(parser);
            ast2->data.expr.right = parser_eat_expr(parser);
            ast = ast2;
        }else {
            parser->cur--;
        };
    }else if (parser->tokens[parser->cur].type == TOKEN_EXC){
        if (parser->cur + 1 == parser->tokenlen){
            error_generate("AbruptEndError", "Abrupt end");
        };
        parser_peek(parser);
        if (parser->tokens[parser->cur].type == TOKEN_EQ){
            AST *ast2 = malloc(sizeof(AST));
            ast2->type = AST_NEQ;
            ast2->data.expr.left = ast;
            if (parser->cur + 1 == parser->tokenlen){
                error_generate("AbruptEndError", "Abrupt end");
            };
            parser_peek(parser);
            ast2->data.expr.right = parser_eat_expr(parser);
            ast = ast2;
        };
    }
    ast->row = token_1.row;
    ast->col = token_1.col;
    return ast;
};

AST *parser_eat_ast(Parser *parser){
    int a = parser->cur;
    AST *ast = malloc(sizeof(AST));
    if (try_parse_mode(parser, ast) == 0){
        return ast;
    };
    ast->type = AST_UNKNOWN;
    if (parser->tokens[parser->cur].type == TOKEN_ID){
        {
            if(strcmp(parser->tokens[parser->cur].value, "if") == 0){
                ast->type = AST_IF;
                if (parser->cur + 1 == parser->tokenlen){
                    error_generate_parser("AbruptEnd", "Sudden end to if statement", parser->tokens[parser->cur].row, parser->tokens[parser->cur].col, parser->tokens[parser->cur].name);
                }
                parser_peek(parser);
                ast->data.if1.condition = parser_eat_expr(parser);
                parser_expect(parser, TOKEN_LB);
                Token prev;
                char c = 0;
                ast->data.if1.blocklen = 0;
                ast->data.if1.elselen = 0;
                ast->data.if1.elseiflen = 0;
                while (parser->tokens[parser->cur].type != TOKEN_RB && parser->tokens[parser->cur].type != TOKEN_EOF){
                    if (parser->tokens[parser->cur].type == prev.type && strcmp(parser->tokens[parser->cur].value, prev.value) == 0){
                        c++;
                        if (c > 5){
                            error_generate_parser("SyntaxError", "Something went wrong", parser->tokens[parser->cur].row, parser->tokens[parser->cur].col, parser->tokens[parser->cur].name);
                        }
                    };
                    ast->data.if1.block[ast->data.if1.blocklen++] = parser_eat_ast(parser);
                    prev = parser->tokens[parser->cur];
                };
                parser_expect(parser, TOKEN_RB);
                while (strcmp(parser->tokens[parser->cur].value, "else") == 0){
                    if (parser->cur + 1 == parser->tokenlen){
                        error_generate_parser("AbruptEnd", "Sudden end to if statement", parser->tokens[parser->cur].row, parser->tokens[parser->cur].col, parser->tokens[parser->cur].name);
                    }
                    parser_peek(parser);
                    if (strcmp(parser->tokens[parser->cur].value, "if") == 0){
                        if (parser->cur + 1 == parser->tokenlen){
                            error_generate_parser("AbruptEnd", "Sudden end to if statement", parser->tokens[parser->cur].row, parser->tokens[parser->cur].col, parser->tokens[parser->cur].name);
                        }
                        parser_peek(parser);
                        parser_expect(parser, TOKEN_LP);
                        ast->data.if1.elseif[ast->data.if1.elseiflen].condition = parser_eat_expr(parser);
                        parser_expect(parser, TOKEN_RP);
                        parser_expect(parser, TOKEN_LB);
                        Token prev;
                        char c = 0;
                        ast->data.if1.elseif[ast->data.if1.elseiflen].statementLen = 0;
                        while (parser->tokens[parser->cur].type != TOKEN_RB && parser->tokens[parser->cur].type != TOKEN_EOF){
                            if (parser->tokens[parser->cur].type == prev.type && strcmp(parser->tokens[parser->cur].value, prev.value) == 0){
                                c++;
                                if (c > 5){
                                    error_generate_parser("SyntaxError", "Something went wrong", parser->tokens[parser->cur].row, parser->tokens[parser->cur].col, parser->tokens[parser->cur].name);
                                }
                            };
                            ast->data.if1.elseif[ast->data.if1.elseiflen].statements[ast->data.if1.elseif[ast->data.if1.elseiflen].statementLen++] = parser_eat_ast(parser);
                            prev = parser->tokens[parser->cur];
                        };
                        ast->data.if1.elseiflen++;
                        parser_expect(parser, TOKEN_RB);
                    }else {
                        parser->cur--;
                        c = 0;
                        prev = (Token){};
                        parser_expect_next(parser, TOKEN_LB);
                        while (parser->tokens[parser->cur].type != TOKEN_RB && parser->tokens[parser->cur].type != TOKEN_EOF){
                            if (parser->tokens[parser->cur].type == prev.type && strcmp(parser->tokens[parser->cur].value, prev.value) == 0){
                                c++;
                                if (c > 5){
                                    error_generate_parser("SyntaxError", "Something went wrong", parser->tokens[parser->cur].row, parser->tokens[parser->cur].col, parser->tokens[parser->cur].name);
                                }
                            };
                            ast->data.if1.else1[ast->data.if1.elselen++] = parser_eat_ast(parser);
                            prev = parser->tokens[parser->cur];
                        };
                        parser_expect(parser, TOKEN_RB);
                    };
                }
            }else if(strcmp(parser->tokens[parser->cur].value, "while") == 0){
                ast->type = AST_WHILE;
                if (parser->cur + 1 == parser->tokenlen){
                    error_generate_parser("AbruptEnd", "Sudden end to while statement", parser->tokens[parser->cur].row, parser->tokens[parser->cur].col, parser->tokens[parser->cur].name);
                }
                parser_peek(parser);
                ast->data.while1.condition = parser_eat_expr(parser);
                parser_expect(parser, TOKEN_LB);
                Token prev;
                char c = 0;
                ast->data.while1.blocklen = 0;
                while (parser->tokens[parser->cur].type != TOKEN_RB && parser->tokens[parser->cur].type != TOKEN_EOF){
                    if (parser->tokens[parser->cur].type == prev.type && strcmp(parser->tokens[parser->cur].value, prev.value) == 0){
                        c++;
                        if (c > 5){
                            error_generate_parser("SyntaxError", "Something went wrong in while", parser->tokens[parser->cur].row, parser->tokens[parser->cur].col, parser->tokens[parser->cur].name);
                        }
                    }else {
                        c = 0;
                    };
                    prev = parser->tokens[parser->cur];
                    ast->data.while1.block[ast->data.while1.blocklen++] = parser_eat_ast(parser);
                };
                parser_expect(parser, TOKEN_RB);
            }else if (strcmp(parser->tokens[parser->cur].value, "return") == 0) {
                parser_peek(parser);
                ast->type = AST_RET;
                ast->data.ret.ret = parser_eat_expr(parser);
            }else if((is_type(parser, parser->tokens[parser->cur]) == 0)){
                parse_type(parser, &ast->typeinfo);
                ast->type = AST_ASSIGN;
                parser_expect(parser, TOKEN_ID);
                parser->cur--;
                ast->data.assign.from = parser_eat_expr(parser); // Even though we technically don't expect it to be expression, this is used for consistency
                ast->data.assign.from->typeinfo = ast->typeinfo;
                if (parser->tokens[parser->cur].type == TOKEN_EQ){
                    parser_peek(parser);
                    ast->data.assign.assignto = parser_eat_expr(parser);
                }else {
                    ast->data.assign.assignto = malloc(sizeof(*(ast->data.assign.assignto)));
                }
            }else if (strcmp(parser->tokens[parser->cur].value, "return") == 0) {
                parser_peek(parser);
                ast->type = AST_RET;
                ast->data.ret.ret = parser_eat_expr(parser);
            }else if(strcmp(parser->tokens[parser->cur].value, "break") == 0){
                ast->type = AST_BREAK;
                parser->cur++;
            }else {
                AST *ast_expr = parser_eat_expr(parser);
                if(parser->tokens[parser->cur].type == TOKEN_EQ){
                    ast->type = AST_ASSIGN;
                    ast->row = parser->tokens[parser->cur].row;
                    ast->col = parser->tokens[parser->cur].col;
                    ast->typeinfo.type = "unknown";
                    ast->data.assign.from = ast_expr;
                    parser_peek(parser);;
                    ast->data.assign.assignto = parser_eat_expr(parser);
                    return ast;
                }else if (parser->tokens[parser->cur].type == TOKEN_LP){
                    ast->type = AST_FUNCALL;
                    ast->data.funcall = (AST_FuncCall){};
                    parser_expect(parser, TOKEN_ID);
                    ast->data.funcall.name = parser->tokens[parser->cur-1].value;
                    parser_expect(parser, TOKEN_LP);
                    while (parser->tokens[parser->cur].type != TOKEN_RP && parser->tokens[parser->cur].type != TOKEN_EOF){
                        ast->data.funcall.args[ast->data.funcall.argslen++] = parser_eat_expr(parser);
                        if (parser->tokens[parser->cur].type != TOKEN_COMMA && parser->tokens[parser->cur].type != TOKEN_RP){
                            error_generate("CommaError", "Expected Comma");
                        }else if(parser->tokens[parser->cur].type == TOKEN_COMMA)
                            parser->cur++;
                    };
                    parser_expect(parser, TOKEN_RP);
            }else {
                return ast_expr;
            }
            }
        }
    }else{
        AST *ast_expr = parser_eat_expr(parser);
        if(parser->tokens[parser->cur].type == TOKEN_EQ){
            ast->type = AST_ASSIGN;
            ast->row = parser->tokens[parser->cur].row;
            ast->col = parser->tokens[parser->cur].col;
            ast->typeinfo.type = "unknown";
            ast->data.assign.from = ast_expr;
            parser_peek(parser);
            ast->data.assign.assignto = parser_eat_expr(parser);
            return ast;
        }else if(parser->tokens[parser->cur].type == TOKEN_LP){

        }else {
            error_generate_parser("AbruptEndError", "Abrupt end", parser->tokens[parser->cur+2].row, parser->tokens[parser->cur+2].col, parser->tokens[parser->cur].name);
       }
    }
    ast->col = parser->tokens[a].col;
    ast->row = parser->tokens[a].row;
    return ast;
}

AST parser_eat_body(Parser *parser){
    int av = parser->cur;
    AST ast = (AST){};
    if (try_parse_mode(parser, &ast) == 0){
        return ast;
    };
    ast.type = AST_UNKNOWN;
    if (parser->tokens[parser->cur].type == TOKEN_ID){
        if(is_type(parser, parser->tokens[parser->cur]) == 0){
            ast.type = AST_ASSIGN;
            parse_type(parser, &ast.typeinfo);
            ast.data.assign.from = malloc(sizeof(struct AST));
            ast.data.assign.from->type = AST_VAR;
            ast.data.assign.from->data.arg.value = parser->tokens[parser->cur].value;
            parser_peek(parser);
            if (parser->tokens[parser->cur].type == TOKEN_EQ){
                error_generate_parser("AssignError", "TODO: Do not support setting a global variable yet", parser->tokens[parser->cur].row, parser->tokens[parser->cur].col, parser->tokens[parser->cur].name);
            }else if(parser->tokens[parser->cur].type == TOKEN_LP){
                ast.type = AST_FUNCDEF;
                ast.data.funcdef.name = ast.data.assign.from->data.arg.value;
                parser_peek(parser);
            while (parser->tokens[parser->cur].type != TOKEN_RP && parser->tokens[parser->cur].type != TOKEN_EOF){
                ast.data.funcdef.args[ast.data.funcdef.argslen] = malloc(sizeof(Argument));
                parse_type(parser, &(ast.data.funcdef.args[ast.data.funcdef.argslen]->type));
                ast.data.funcdef.args[ast.data.funcdef.argslen]->arg = parser->tokens[parser->cur].value;
                ast.data.funcdef.argslen++;
                parser_peek(parser);
                if (parser->tokens[parser->cur].type != TOKEN_RP){
                    parser_expect(parser, TOKEN_COMMA);
                };
            };
            parser_expect(parser, TOKEN_RP);
            parser_expect(parser, TOKEN_LB);
            Token prev;
            char c = 0;
            while (parser->tokens[parser->cur].type != TOKEN_RB && parser->tokens[parser->cur].type != TOKEN_EOF){
                if (parser->tokens[parser->cur].type == prev.type && strcmp(parser->tokens[parser->cur].value, prev.value) == 0){
                    c++;
                    if (c > 5){
                        error_generate_parser("SyntaxError", "Something went wrong in function", parser->tokens[parser->cur].row, parser->tokens[parser->cur].col, parser->tokens[parser->cur].name);
                    }
                }else {
                    c = 0;
                };
                ast.data.funcdef.block[ast.data.funcdef.blocklen++] = parser_eat_ast(parser);
                prev = parser->tokens[parser->cur-1];
            };
            parser_expect(parser, TOKEN_RB);
            }
        }else if(strcmp(parser->tokens[parser->cur].value, "typedef") == 0){
            ast.type = AST_TYPEDEF;
            parser_expect_next(parser, TOKEN_ID);
            ast.data.mode.name = parser->tokens[parser->cur-1].value;
            parser_expect(parser, TOKEN_EQ);
            parser_expect(parser, TOKEN_ID);
            parser->cur--;
            parse_type(parser, &ast.typeinfo);
        int len = -1;
        for (int i=0; i<typesLen; i++){
            if (strcmp(types[i].name, ast.typeinfo.type) == 0){
                len = types[i].length;
            };
        };
        if (ast.typeinfo.ptrnum > 0){
            len = 8;
        };
        if (len == -1){
            char a[100];
            snprintf(a, 100, "Type `%s` does not exist", ast.typeinfo.type);
        }
        types[typesLen].length = len;
        types[typesLen].name = ast.data.mode.name;
        types[typesLen].longname = ast.data.mode.name;
        types[typesLen].ptrnum = ast.typeinfo.ptrnum;
        typesLen++;
        }else {
            error_generate_parser("SyntaxError", "Unknown identifier", parser->tokens[av].row, parser->tokens[av].col, parser->name);
        };
    };
    ast.col = parser->tokens[av].col;
    ast.row = parser->tokens[av].row;
    return ast;
}

char parser_eat(Parser *parser){
    if(parser->tokens[parser->cur].type == TOKEN_EOF){
        return -1;
    };
    Token token = parser->tokens[parser->cur];
    parser->asts[parser->astlen++] = parser_eat_body(parser);
    parser->asts[parser->astlen-1].row = token.row;
    parser->asts[parser->astlen-1].col = token.col;
    return 0;
};



char *transpile_newl(Transpiler *transpiler){
    if (transpiler->isTab){
        return "\n\t";
    }else {
        return "\n";
    };
};

char *transpiler_asm_expr(Transpiler *transpiler, AST *expr, char *reg, AST_TypeInfo type_info, char override);

void transpile_func(Transpiler *transpiler, AST *expr){
    char float1 = 0;
    char v = 0;
        for (int i=0; i<expr->data.funcall.argslen; i++){
            strcat(transpiler->asmc, "push ");
            strcat(transpiler->asmc, get_argument_register(i));
            strcat(transpiler->asmc, transpile_newl(transpiler));
            AST_TypeInfo typeinfo = fetch_type(transpiler, expr->data.funcall.args[i]);
            char *reg = reg_based_on_size(get_argument_register(v), type_to_len(typeinfo));
            if (typeinfo.type != NULL){
            if (strcmp(typeinfo.type, "float") == 0){ // There is special functionality for where floats are stored
                char a[100];
                snprintf(a, 100, "xmm%d", float1);
                reg = a;
                float1++;
                v--;
            }
            }
            strcat(transpiler->asmc, transpiler_asm_expr(transpiler, expr->data.funcall.args[i], get_argument_register(i), (AST_TypeInfo){"long", 0}, 0));
            strcat(transpiler->asmc, transpile_newl(transpiler));
            v++;
        };
        char f = 0;
        for (int i=0; i<transpiler->functionLen; i++){
            if (strcmp(transpiler->functions[i].name, expr->data.funcall.name) == 0){
                f = 1;
            };
        };
        if (f == 0){
            if (strcmp(expr->data.funcall.name, "print") && strcmp(expr->data.funcall.name, "exit") && strcmp(expr->data.funcall.name, "malloc") && strcmp(expr->data.funcall.name, "string_len") && strcmp(expr->data.funcall.name, "string_copy") && strcmp(expr->data.funcall.name, "string_eq")){
                char v = 0;
                for (int c=0; c<transpiler->toIncludeLen; c++){
                    if (strcmp(transpiler->toInclude[c], expr->data.funcall.name) == 0){
                        v = 1;
                    }else if(expr->data.funcall.name[0] == '_'){
                        char *copy = strdup(expr->data.funcall.name) + 1;
                        if (strcmp(transpiler->toInclude[c], copy) == 0){
                            v = 1;
                        }
                    };
                };
                if (v == 0) {
                    char a[100];
                    snprintf(a, 100, "Function %s doesn't exist", expr->data.funcall.name);
                    error_generate_parser("FunctionError", a, expr->row, expr->col, transpiler->name);
                }else { 
                    char fullstring[100];
                    if (expr->data.funcall.name[0] != '_'){
                        strcpy(fullstring, "_");
                        strcat(fullstring, expr->data.funcall.name);
                    }else {
                        strcpy(fullstring, expr->data.funcall.name);
                    };
                    expr->data.funcall.name = strdup(fullstring);
                }
            }else {
                if (strcmp(expr->data.funcall.name, "print") == 0) {
                    expr->typeinfo.ptrnum = 0;
                    expr->typeinfo.type = "unknown";
                }else if (strcmp(expr->data.funcall.name, "exit") == 0) {
                    expr->typeinfo.ptrnum = 0;
                    expr->typeinfo.type = "unknown";
                }else if (strcmp(expr->data.funcall.name, "malloc") == 0){
                    expr->typeinfo.ptrnum = 1;
                    expr->typeinfo.type = "null";
                }else if(strcmp(expr->data.funcall.name, "string_len") == 0){
                    expr->typeinfo.ptrnum = 0;
                    expr->typeinfo.type = "long";
                }
                char f = 0;
                for (int i=0; i<transpiler->toIncludeLen; i++){
                    if (strcmp(transpiler->toInclude[i], expr->data.funcall.name) == 0){
                        f = 1;
                    };
                };
                if(f == 0) {transpiler->toInclude[transpiler->toIncludeLen++] = expr->data.funcall.name;} 
            };
        };
        strcat(transpiler->asmc, "call ");
        strcat(transpiler->asmc, expr->data.funcall.name);
        strcat(transpiler->asmc, transpile_newl(transpiler));
        for (int i=expr->data.funcall.argslen; i != 0; i--){
            strcat(transpiler->asmc, "pop ");
            strcat(transpiler->asmc, get_argument_register(expr->data.funcall.argslen - i));
            strcat(transpiler->asmc, transpile_newl(transpiler));
        };
};

void transpile_syscall(Transpiler *transpiler, AST *syscall){
    if (syscall->data.funcall.argslen < 2){
        error_generate("SyscallError", "Need more arguments to syscall");
    };
    for (int i=1; i<syscall->data.funcall.argslen; i++){ 
        strcat(transpiler->asmc, transpiler_asm_expr(transpiler, syscall->data.funcall.args[i], get_argument_register(i - 1), (AST_TypeInfo){}, 0));
        strcat(transpiler->asmc, transpile_newl(transpiler));
    }
    strcat(transpiler->asmc, transpiler_asm_expr(transpiler, syscall->data.funcall.args[0], "rax", (AST_TypeInfo){}, 0));
    strcat(transpiler->asmc, transpile_newl(transpiler));
    strcat(transpiler->asmc, "syscall");
    strcat(transpiler->asmc, transpile_newl(transpiler));
};
void transpile_asm_block(Transpiler *transpiler, AST block);

int try_transpile_mode(Transpiler *transpiler, AST *ast){
    if (ast->type == AST_MODE){
        if (strcmp(ast->data.mode.name, "safety.pointers") == 0){
            if (strcmp(ast->data.mode.res, "auto_alloc") == 0){
                transpiler->transpiler_mode = MODE_POINTER_ALLOC;
            }else if(strcmp(ast->data.mode.res, "manual") == 0){
                transpiler->transpiler_mode = MODE_POINTER_NOALLOC;
            }else {
                error_generate_parser("ModeError", "Mode value for \"safety.pointers\" not recognized. The only options available are auto_alloc for automatic allocation of pointers, and manual for no automatic allocation of pointers", ast->row, ast->col, transpiler->name);
            }
        }else if(strcmp(ast->data.mode.name, "extern") == 0){
            if (_BirdSharpTypes == TYPE_STATIC){
                error_generate("ModeError", "No externs are in static mode");
                exit(0);
            };
           transpiler->toInclude[transpiler->toIncludeLen++] = ast->data.mode.res;
           strcat(transpiler->asmc, "extern _");
           strcat(transpiler->asmc, ast->data.mode.res);
           strcat(transpiler->asmc, "\n");
        }else {
            error_generate_parser("ModeError", "Mode not recognized", ast->row, ast->col, transpiler->name);
        };
        return 0;
    }else if(ast->type == AST_MODE_IF){
        if (strcmp(ast->data.if1.condition->data.mode.name, "compile.static") == 0){
            if (_BirdSharpTypes == TYPE_STATIC){
                for (int v=0; v<ast->data.if1.blocklen; v++){
                    AST *ast2 = (ast->data.if1.block[v]);
                    transpile_asm_block(transpiler, *ast2);
                };
            }
            return 0;
        }else if (strcmp(ast->data.if1.condition->data.mode.name, "compile.dynamic") == 0){
            if (_BirdSharpTypes == TYPE_DYNAMIC){
                for (int v=0; v<ast->data.if1.blocklen; v++){
                    AST *ast2 = (ast->data.if1.block[v]);
                    transpile_asm_block(transpiler, *ast2);
                };
            }
            return 0;
        }else if (strcmp(ast->data.if1.condition->data.mode.name, "safety.pointers.manual") == 0){
            if (transpiler->transpiler_mode == MODE_POINTER_NOALLOC){
                for (int v=0; v<ast->data.if1.blocklen; v++){
                    AST *ast2 = (ast->data.if1.block[v]);
                    transpile_asm_block(transpiler, *ast2);
                };
            }
            return 0;
        }else if (strcmp(ast->data.if1.condition->data.mode.name, "safety.pointers.auto_alloc") == 0){
            if (transpiler->transpiler_mode == MODE_POINTER_ALLOC){
                for (int v=0; v<ast->data.if1.blocklen; v++){
                    AST *ast2 = (ast->data.if1.block[v]);
                    transpile_asm_block(transpiler, *ast2);
                };
            }
            return 0;
        }
    }
    return -1;
};

void string(Transpiler *transpiler, AST *expr, char *a){
        strcat(transpiler->datac, "\n\t");
        strcat(transpiler->datac, a);
        strcat(transpiler->datac, ": db '");
        for (int v=0; v<strlen(expr->data.arg.value); v++){
            if (expr->data.arg.value[v] == '\\' && v+1 != strlen(expr->data.arg.value)){
                if (expr->data.arg.value[v+1] == '0'){
                    strcat(transpiler->datac, "', 0, '");
                }else if (expr->data.arg.value[v+1] == 'n'){
                    strcat(transpiler->datac, "', 10, '");
                } else if (expr->data.arg.value[v+1] == '\"'){
                    strcat(transpiler->datac, "', 34, '");
                } else if (expr->data.arg.value[v+1] == '\''){
                    strcat(transpiler->datac, "', 39, '");
                } else if (expr->data.arg.value[v+1] == 'u'){
                    char vb[10];v++;vb[0] = expr->data.arg.value[v+1];v++;vb[1] = expr->data.arg.value[v+1];v++;vb[2] = expr->data.arg.value[v+1];v++;vb[3] = expr->data.arg.value[v+1];
                    snprintf(vb, 10, "%d", (int)strtol(vb, NULL, 16));
                    strcat(transpiler->datac, "', ");
                    strcat(transpiler->datac, vb);
                    strcat(transpiler->datac, ", '");
                } else if (expr->data.arg.value[v+1] == 'x'){
                    char vb[10];v++;vb[0] = expr->data.arg.value[v+1];v++;vb[1] = expr->data.arg.value[v+1];
                    snprintf(vb, 10, "%d", (int)strtol(vb, NULL, 16));
                    strcat(transpiler->datac, "', ");
                    strcat(transpiler->datac, vb);
                    strcat(transpiler->datac, ", '");
                };
                v++;
            }else {
                char b[2];b[0] = expr->data.arg.value[v];b[1]='\0';
                strcat(transpiler->datac, b);
            };
        };
        if (expr->data.arg.value[strlen(expr->data.arg.value)-1] == '0' && expr->data.arg.value[strlen(expr->data.arg.value)-2] == '\\'){
            strcat(transpiler->datac, "'");
        }else {
            strcat(transpiler->datac, "', 0");
        };
};

AST_TypeInfo get_type(Transpiler *transpiler, AST *ast){
    if (ast->type == AST_VAR){
        char z = fetch_global_variable(transpiler, ast->data.arg.value);
        if (z != -1){
            return transpiler->variables[z].type_info;
        }
        char v = try_find_local_var(transpiler, ast->data.arg.value);
    }else if(ast->type == AST_REF){
        AST_TypeInfo typeinfo = get_type(transpiler, ast->data.expr.left);
        typeinfo.ptrnum++;
        return typeinfo;
    }else if(ast->type == AST_DEREF){
        AST_TypeInfo typeinfo = get_type(transpiler, ast->data.expr.left);
        if (typeinfo.ptrnum == 0){
            error_generate("DerefError", "Only pointers can be dereferenced");
        }
        typeinfo = dec_ptrnum(typeinfo);
        return typeinfo;
    }
    if(ast->type == AST_ASSIGN){
        return ast->data.assign.assignto->typeinfo;
    };
    if (ast->type == AST_FUNCALL){
        for (int i=0; i<transpiler->functionLen; i++){
            if (strcmp(transpiler->functions[i].name, ast->data.funcall.name) == 0){
                return transpiler->functions[i].type;
            };
        };
    };
    if (ast->type == AST_PLUS || ast->type == AST_SUB || ast->type == AST_MUL || ast->type == AST_DIV || ast->type == AST_MODULO){
        return get_type(transpiler, ast->data.expr.left);
    };
    return fetch_type(transpiler, ast);
};

char *transpiler_asm_expr(Transpiler *transpiler, AST *expr, char *reg, AST_TypeInfo type_info, char override){
    if (try_transpile_mode(transpiler, expr) == 0){
        error_generate("ModeError", "Do not support mode expressions");
        return "";
    };
    if (expr->type == AST_NOT){
        strcat(transpiler->asmc, transpiler_asm_expr(transpiler, expr->data.expr.left, reg, type_info, 0));
        strcat(transpiler->asmc, transpile_newl(transpiler));
        strcat(transpiler->asmc, "test ");
        strcat(transpiler->asmc, reg);
        strcat(transpiler->asmc, ", ");
        strcat(transpiler->asmc, reg);
        strcat(transpiler->asmc, transpile_newl(transpiler));
        strcat(transpiler->asmc, "setz ");
        strcat(transpiler->asmc, reg_based_on_size(reg, 1));
        strcat(transpiler->asmc, transpile_newl(transpiler));
        strcat(transpiler->asmc, "movzx ");
        strcat(transpiler->asmc, reg);
        strcat(transpiler->asmc, ", ");
        strcat(transpiler->asmc, reg_based_on_size(reg, 1));
        return "";
    };
    if(expr->type == AST_SYSCALL){
        transpile_syscall(transpiler, expr);
        strcat(transpiler->asmc, "mov ");
        strcat(transpiler->asmc, reg);
        strcat(transpiler->asmc, ", ");
        strcat(transpiler->asmc, "rax");
        strcat(transpiler->asmc, transpile_newl(transpiler));
        return "";
    };
    if (fetch_type(transpiler, expr).type){
        AST_TypeInfo type = fetch_type(transpiler, expr);
        if (override == false){
            reg = reg_based_on_size(reg, type_to_len(type));
        }else {
            reg = reg_based_on_size(reg, type_to_len(type_info));
        }
    };
    if(expr->type != AST_SUB && expr->type != AST_PLUS && expr->type != AST_MUL && expr->type != AST_DIV && expr->type != AST_MODULO && expr->type != AST_LT && expr->type != AST_GT  && expr->type != AST_LTE && expr->type != AST_GTE && expr->type != AST_EQ && expr->type != AST_NEQ && expr->type != AST_FUNCALL && expr->type != AST_REF && expr->type != AST_DEREF && expr->type != AST_EXPR && expr->type != AST_CAST && expr->type != AST_FLOAT && expr->type != AST_STRING && expr->type != AST_VAR && strcmp(reg, "")){
        bool can_be_deref_1 = type_info.ptrnum != 0;
        if (!type_info.type){
            type_info = fetch_type(transpiler, expr);
        };
        if (can_be_deref_1){
            if (strcmp(reg, "rdx") == 0){
                strcat(transpiler->asmc, "lea rdx, ");
            }else if(expr->type == AST_VAR){
            strcat(transpiler->asmc, "mov ");
            strcat(transpiler->asmc, reg);
            strcat(transpiler->asmc, ", ");
            }else{
                strcat(transpiler->asmc, "lea ");
                strcat(transpiler->asmc, reg);
                strcat(transpiler->asmc, ", ");
            }
        }else {
            strcat(transpiler->asmc, "mov ");
            strcat(transpiler->asmc, reg);
            strcat(transpiler->asmc, ", ");
        }
    }
    if (expr->type == AST_CAST){
        if (expr->typeinfo.ptrnum == 1 && strcmp(expr->typeinfo.type, "char")==0 && fetch_type(transpiler, expr->data.expr.left).ptrnum == 0){
            AST_TypeInfo typeinfo = expr->typeinfo;
            ;
            strcat(transpiler->asmc, transpiler_asm_expr(transpiler, expr->data.expr.left, "rdx", typeinfo, 0));
            strcat(transpiler->asmc, transpile_newl(transpiler));
            strcat(transpiler->asmc, "mov ");
            strcat(transpiler->asmc, reg);
            strcat(transpiler->asmc, ", ");
            strcat(transpiler->asmc, "rdx\n\t");
        }else if(expr->typeinfo.ptrnum == 0 && fetch_type(transpiler, expr->data.expr.left).ptrnum == 0 && strcmp(expr->typeinfo.type, "long") == 0 && strcmp(fetch_type(transpiler, expr->data.expr.left).type, "int") == 0){
            strcat(transpiler->asmc, "xor ");
            strcat(transpiler->asmc, reg);
            strcat(transpiler->asmc, ", ");
            strcat(transpiler->asmc, reg);
            strcat(transpiler->asmc, transpile_newl(transpiler));
            char *updated = reg_based_on_size(reg, type_to_len(fetch_type(transpiler, expr->data.expr.left)));
            strcat(transpiler->asmc, transpiler_asm_expr(transpiler, expr->data.expr.left, updated, type_info, 0));
            strcat(transpiler->asmc, transpile_newl(transpiler));
        }else if(expr->typeinfo.ptrnum == 0 && fetch_type(transpiler, expr->data.expr.left).ptrnum == 0 && strcmp(expr->typeinfo.type, "int") == 0 && strcmp(fetch_type(transpiler, expr->data.expr.left).type, "long") == 0){
            char *updated = reg_based_on_size(reg, type_to_len(fetch_type(transpiler, expr->data.expr.left)));
            strcat(transpiler->asmc, transpiler_asm_expr(transpiler, expr->data.expr.left, updated, type_info, 0));
            strcat(transpiler->asmc, transpile_newl(transpiler));
        }else {
            strcat(transpiler->asmc, transpiler_asm_expr(transpiler, expr->data.expr.left,reg, type_info, 0));
            strcat(transpiler->asmc, transpile_newl(transpiler));
        }
    }
    if (expr->type == AST_EXPR){
        return transpiler_asm_expr(transpiler, expr->data.expr.left, reg, type_info, 0);
    }
    if (expr->type == AST_REF){
        strcat(transpiler->asmc, "lea r9, ");
        strcat(transpiler->asmc, transpiler_asm_expr(transpiler, expr->data.expr.left, "", (AST_TypeInfo){"", 1}, 0));
        strcat(transpiler->asmc, transpile_newl(transpiler));
        strcat(transpiler->asmc, "mov ");
        strcat(transpiler->asmc, reg);
        strcat(transpiler->asmc, ", ");
        AST_TypeInfo typeinfo = fetch_type(transpiler, expr);
        strcat(transpiler->asmc, reg_based_on_size("r9", type_to_len(typeinfo)));
        return "";
    }else if(expr->type == AST_DEREF){
        strcat(transpiler->asmc, transpiler_asm_expr(transpiler, expr->data.expr.left, "r9", (AST_TypeInfo){}, 0));
        strcat(transpiler->asmc, transpile_newl(transpiler));
        AST_TypeInfo type_of_expr = fetch_type(transpiler, expr);
        strcat(transpiler->asmc, "mov ");
        strcat(transpiler->asmc, reg_based_on_size("r10", type_to_len(type_of_expr)));
        strcat(transpiler->asmc, ", ");
        strcat(transpiler->asmc, deref_type_as_var(type_of_expr, "r9"));
        strcat(transpiler->asmc, transpile_newl(transpiler));
        if (strcmp(reg, reg_based_on_size(reg, type_to_len(type_of_expr)))){ // Registers is not size expected
            strcat(transpiler->asmc, "movzx ");
            strcat(transpiler->asmc, reg);
            strcat(transpiler->asmc, ", ");
            strcat(transpiler->asmc, reg_based_on_size("r10", type_to_len(type_of_expr)));
        }else {
        strcat(transpiler->asmc, "mov ");
        strcat(transpiler->asmc, reg_based_on_size(reg, type_to_len(type_of_expr)));
        strcat(transpiler->asmc, ", ");
        strcat(transpiler->asmc, reg_based_on_size("r10", type_to_len(type_of_expr)));
        }
    }
    if (expr->type == AST_STRING){
        strcat(transpiler->asmc, "lea r13, ");
        char a[100];snprintf(a, 100, "_LBC%d", transpiler->datanum++);
        string(transpiler, expr, a);
        snprintf(a, 100, "[_LBC%d]", transpiler->datanum-1);
        strcat(transpiler->asmc, a);
        strcat(transpiler->asmc, transpile_newl(transpiler));
        strcat(transpiler->asmc, "mov ");
        strcat(transpiler->asmc, reg);
        strcat(transpiler->asmc, ", ");
        return "r13";
    }else if(expr->type == AST_INT){
        return expr->data.arg.value;
    }else if(expr->type == AST_FLOAT){
        char a[100];
        snprintf(a, 100, "mov r15d, %s%scvtsi2ss %s, r15d", expr->data.arg.value, transpile_newl(transpiler), reg);
        return strdup(a);
    }else if (expr->type == AST_CHAR){
        if (strcmp(expr->data.arg.value, "\\0") == 0){
            return "0";
        }else if(strcmp(expr->data.arg.value, "\\n") == 0){
            return "10";
        };
        char a[100];snprintf(a, 100, "'%s'", expr->data.arg.value);
        return strdup(a);
    }else if(expr->type == AST_VAR){
        char f = -1;
        f = fetch_global_variable(transpiler, expr->data.arg.value);
        if (f == -1){
            char l = -1;
            for (int n=0; n<transpiler->functions[transpiler->functionLen-1].argslen; n++){
                if (strcmp(transpiler->functions[transpiler->functionLen-1].args[n].arg, expr->data.arg.value) == 0){
                    l = n;
                };
            }
            if (l == -1){
                if ((l = try_find_local_var(transpiler, expr->data.arg.value)) != -1){
                    strcat(transpiler->asmc, "mov ");
                    strcat(transpiler->asmc, reg);
                    strcat(transpiler->asmc, ", ");
                    return correct_format_for_var(transpiler, expr->data.arg.value, reg, fetch_type(transpiler, expr));
                }else if (strcmp(expr->data.arg.value, "argc") == 0){
                    strcat(transpiler->asmc, "mov ");
                    strcat(transpiler->asmc, reg);
                    strcat(transpiler->asmc, ", ");
                    if (_BirdSharpTypes == TYPE_DYNAMIC){
                        return "rdi";
                    }
                    return "[rbx]";
                }else if (strcmp(expr->data.arg.value, "argv") == 0){
                    strcat(transpiler->asmc, "mov ");
                    strcat(transpiler->asmc, reg);
                    strcat(transpiler->asmc, ", ");
                    if (_BirdSharpTypes == TYPE_DYNAMIC){
                        return "rsi";
                    }
                    strcat(transpiler->asmc, "rbx\n\t");
                    strcat(transpiler->asmc, "add ");
                    strcat(transpiler->asmc, reg);
                    strcat(transpiler->asmc, ", ");
                    strcat(transpiler->asmc, "8");
                    return "";
                }else if(strcmp(expr->data.arg.value, "__LINE__") == 0){
                    strcat(transpiler->asmc, "mov ");
                    strcat(transpiler->asmc, reg);
                    strcat(transpiler->asmc, ", ");
                    char a[100];
                    snprintf(a, 100, "%d", expr->row);
                    strcat(transpiler->asmc, strdup(a));
                    return "";
                }else if(strcmp(expr->data.arg.value, "__COL__") == 0){
                    strcat(transpiler->asmc, "mov ");
                    strcat(transpiler->asmc, reg);
                    strcat(transpiler->asmc, ", ");
                    char a[100];
                    snprintf(a, 100, "%d", expr->col);
                    strcat(transpiler->asmc, strdup(a));
                    return "";
                }else {
                    error_generate_parser("VariableError", "Variable does not exist", expr->row, expr->col, transpiler->name);
                }
            }else {
                if (strcmp(expr->data.arg.value, "argc") == 0){
                    strcat(transpiler->asmc, "mov ");
                    strcat(transpiler->asmc, reg);
                    strcat(transpiler->asmc, ", ");
                    if (_BirdSharpTypes == TYPE_DYNAMIC){
                        return "rdi";
                    }
                    return "[rbx]";
                }else if (strcmp(expr->data.arg.value, "argv") == 0){
                    strcat(transpiler->asmc, "mov ");
                    strcat(transpiler->asmc, reg);
                    strcat(transpiler->asmc, ", ");
                    if (_BirdSharpTypes == TYPE_DYNAMIC){
                        return "rsi";
                    }
                    strcat(transpiler->asmc, "rbx\n\t");
                    strcat(transpiler->asmc, "add ");
                    strcat(transpiler->asmc, reg);
                    strcat(transpiler->asmc, ", ");
                    strcat(transpiler->asmc, "8");
                    return "";
                }else if(strcmp(expr->data.arg.value, "__LINE__") == 0){
                    strcat(transpiler->asmc, "mov ");
                    strcat(transpiler->asmc, reg);
                    strcat(transpiler->asmc, ", ");
                    char a[100];
                    snprintf(a, 100, "%d", expr->row);
                    strcat(transpiler->asmc, strdup(a));
                    return "";
                }else if(strcmp(expr->data.arg.value, "__COL__") == 0){
                    strcat(transpiler->asmc, "mov ");
                    strcat(transpiler->asmc, reg);
                    strcat(transpiler->asmc, ", ");
                    char a[100];
                    snprintf(a, 100, "%d", expr->col);
                    strcat(transpiler->asmc, strdup(a));
                    return "";
                };
                for (int i=0; i<transpiler->functions[transpiler->functionLen-1].argslen; i++){
                    if (strcmp(expr->data.arg.value, transpiler->functions[transpiler->functionLen-1].args[i].arg) == 0){
                        int number = type_to_len(transpiler->functions[transpiler->functionLen-1].args[l].type);
                        strcat(transpiler->asmc, "mov ");
                        strcat(transpiler->asmc, reg_based_on_size(reg, number));
                        strcat(transpiler->asmc, ", ");
                        return reg_based_on_size(get_argument_register(l), number);
                    };
                };
                strcat(transpiler->asmc, "mov ");
                strcat(transpiler->asmc, reg);
                strcat(transpiler->asmc, ", ");
                return reg_based_on_size(get_argument_register(l), type_to_len(transpiler->functions[transpiler->functionLen-1].args[l].type));
            };
        };
        int number = type_to_len(transpiler->variables[f].type_info);
        strcat(transpiler->asmc, "mov ");
        strcat(transpiler->asmc, reg_based_on_size(reg, number));
        strcat(transpiler->asmc, ", ");
        return type_as_var(transpiler->variables[f].type_info, get_value_of_variable(expr));
    }else if (expr->type == AST_PLUS || expr->type == AST_SUB || expr->type == AST_MUL || expr->type == AST_DIV || expr->type == AST_MODULO || expr->type == AST_LT || expr->type == AST_GT || expr->type == AST_LTE || expr->type == AST_GTE || expr->type == AST_EQ || expr->type == AST_NEQ){
        while (
            ((expr->data.expr.right->type == AST_PLUS || expr->data.expr.right->type == AST_SUB) && (expr->type != AST_EQ && expr->type != AST_NEQ && expr->type != AST_LT && expr->type != AST_GT && expr->type != AST_LTE && expr->type != AST_GTE)) || expr->data.expr.right->type == AST_EQ || expr->data.expr.right->type == AST_NEQ || expr->data.expr.right->type == AST_LT || expr->data.expr.right->type == AST_GT || expr->data.expr.right->type == AST_LTE || expr->data.expr.right->type == AST_GTE){
        
            AST exprr = *(expr->data.expr.right);
            AST exprl = *(expr->data.expr.left);
            expr->data.expr.left->type = expr->type;
            expr->type = exprr.type;
            expr->data.expr.right = exprr.data.expr.right;
            expr->data.expr.left->data.expr.left = malloc(sizeof(AST));
            *(expr->data.expr.left->data.expr.left) = exprl;
            expr->data.expr.left->data.expr.right = exprr.data.expr.left;
            expr->data.expr.right = expr->data.expr.right;
        };
        if(expr->type == AST_PLUS){
            check_cast(transpiler, *expr->data.expr.left, *expr->data.expr.right, expr->row, expr->col, transpiler->name, true);
            expr->typeinfo = fetch_type(transpiler, expr->data.expr.left);
            if (expr->data.expr.left->type == AST_INT && expr->data.expr.right->type == AST_INT){
                char a[100];
                snprintf(a, 100, "%d", atoi(expr->data.expr.left->data.arg.value) + atoi(expr->data.expr.right->data.arg.value));
                strcat(transpiler->asmc, "mov ");
                strcat(transpiler->asmc, reg);
                strcat(transpiler->asmc, ", ");
                strcat(transpiler->asmc, a);
            }else {
                strcat(transpiler->asmc, transpiler_asm_expr(transpiler, expr->data.expr.right, "r12", (AST_TypeInfo){0}, 0));
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, transpiler_asm_expr(transpiler, expr->data.expr.left, "r11", (AST_TypeInfo){0}, 0));
                strcat(transpiler->asmc, transpile_newl(transpiler));
                if (strcmp(reg, "r12")){
                    strcat(transpiler->asmc, "mov ");
                    AST_TypeInfo typeinfo = fetch_type(transpiler, expr->data.expr.left);
                    strcat(transpiler->asmc, reg);
                    strcat(transpiler->asmc, ", ");
                    strcat(transpiler->asmc, reg_based_on_size("r12", type_to_len(fetch_type(transpiler, expr->data.expr.left))));
                    strcat(transpiler->asmc, transpile_newl(transpiler));
                    strcat(transpiler->asmc, "add ");
                    strcat(transpiler->asmc, reg);
                    strcat(transpiler->asmc, ", ");
                    strcat(transpiler->asmc, reg_based_on_size("r11", type_to_len(fetch_type(transpiler, expr->data.expr.left))));
                }else {
                    strcat(transpiler->asmc, "add ");
                    strcat(transpiler->asmc, reg);
                    strcat(transpiler->asmc, ", r11");
                }
            }
        }else if(expr->type == AST_SUB){
            if (expr->data.expr.left->type == AST_INT && expr->data.expr.right->type == AST_INT){
                char a[100];
                snprintf(a, 100, "%d", atoi(expr->data.expr.left->data.arg.value) - atoi(expr->data.expr.right->data.arg.value));
                strcat(transpiler->asmc, "mov ");
                strcat(transpiler->asmc, reg);
                strcat(transpiler->asmc, ", ");
                strcat(transpiler->asmc, a);
            }else {
                strcat(transpiler->asmc, transpiler_asm_expr(transpiler, expr->data.expr.right, "r14", (AST_TypeInfo){0}, 0));
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, transpiler_asm_expr(transpiler, expr->data.expr.left, "r13", (AST_TypeInfo){0}, 0));
                strcat(transpiler->asmc, transpile_newl(transpiler));
                if (strcmp(reg, "r13")){
                    strcat(transpiler->asmc, "mov ");
                    strcat(transpiler->asmc, reg);
                    strcat(transpiler->asmc, ", ");
                    strcat(transpiler->asmc, reg_based_on_size("r13", type_to_len(fetch_type(transpiler, expr->data.expr.left))));
                    strcat(transpiler->asmc, transpile_newl(transpiler));
                    strcat(transpiler->asmc, "sub ");
                    strcat(transpiler->asmc, reg);
                    strcat(transpiler->asmc, ", ");
                    strcat(transpiler->asmc, reg_based_on_size("r14", type_to_len(fetch_type(transpiler, expr->data.expr.left))));
                }else {
                    strcat(transpiler->asmc, "sub ");
                    strcat(transpiler->asmc, reg);
                    strcat(transpiler->asmc, ", r14");
                }
            }
        }else if(expr->type == AST_MUL){
            if (expr->data.expr.left->type == AST_INT && expr->data.expr.right->type == AST_INT){
                char a[100];
                snprintf(a, 100, "%d", atoi(expr->data.expr.left->data.arg.value) * atoi(expr->data.expr.right->data.arg.value));
                strcat(transpiler->asmc, "mov ");
                strcat(transpiler->asmc, reg);
                strcat(transpiler->asmc, ", ");
                strcat(transpiler->asmc, a);
            }else {
                strcat(transpiler->asmc, transpiler_asm_expr(transpiler, expr->data.expr.right, "r14", (AST_TypeInfo){0}, 0));
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, transpiler_asm_expr(transpiler, expr->data.expr.left, "r13", (AST_TypeInfo){0}, 0));
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "imul r13, r14");
                strcat(transpiler->asmc, transpile_newl(transpiler));
                if (strcmp(reg, "r13")){
                    strcat(transpiler->asmc, "mov ");
                    strcat(transpiler->asmc, reg);
                    strcat(transpiler->asmc, ", ");
                    strcat(transpiler->asmc, reg_based_on_size("r13", type_to_len(fetch_type(transpiler, expr))));
                };
            }
        }else if(expr->type == AST_DIV){
            if (expr->data.expr.left->type == AST_INT && expr->data.expr.right->type == AST_INT){
                char a[100];
                snprintf(a, 100, "%d", atoi(expr->data.expr.left->data.arg.value) / atoi(expr->data.expr.right->data.arg.value));
                strcat(transpiler->asmc, "mov ");
                strcat(transpiler->asmc, reg);
                strcat(transpiler->asmc, ", ");
                strcat(transpiler->asmc, a);
            }else {
                strcat(transpiler->asmc, "push rax");
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "push rdx");
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, transpiler_asm_expr(transpiler, expr->data.expr.left, "rax", (AST_TypeInfo){0}, 0));
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, transpiler_asm_expr(transpiler, expr->data.expr.right, "r13", (AST_TypeInfo){0}, 0));
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "cqo");
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "idiv r13");
                if (strcmp(reg, "rax")){
                    strcat(transpiler->asmc, transpile_newl(transpiler));
                    AST_TypeInfo type = fetch_type(transpiler, expr);
                    strcat(transpiler->asmc, "mov ");
                    strcat(transpiler->asmc, reg_based_on_size(reg, type_to_len(type)));
                    strcat(transpiler->asmc, ", ");
                    strcat(transpiler->asmc, reg_based_on_size("rax", type_to_len(type)));
                };
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "pop rdx");
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "pop rax");
            }
        }else if(expr->type == AST_MODULO){

            if (expr->data.expr.left->type == AST_INT && expr->data.expr.right->type == AST_INT){
                char a[100];
                snprintf(a, 100, "%d", atoi(expr->data.expr.left->data.arg.value) % atoi(expr->data.expr.right->data.arg.value));
                strcat(transpiler->asmc, "mov ");
                strcat(transpiler->asmc, reg);
                strcat(transpiler->asmc, ", ");
                strcat(transpiler->asmc, a);
            }else {
                strcat(transpiler->asmc, "push rax");
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "push rdx");
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, transpiler_asm_expr(transpiler, expr->data.expr.left, "rax", (AST_TypeInfo){0}, 0));
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, transpiler_asm_expr(transpiler, expr->data.expr.right, "r13", (AST_TypeInfo){0}, 0));
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "cqo");
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "idiv r13");
                if (strcmp(reg, "rdx")){
                    strcat(transpiler->asmc, transpile_newl(transpiler));
                    AST_TypeInfo typeinfo = fetch_type(transpiler, expr);
                    strcat(transpiler->asmc, "mov ");
                    strcat(transpiler->asmc, reg_based_on_size(reg, type_to_len(typeinfo)));
                    strcat(transpiler->asmc, ", ");
                    strcat(transpiler->asmc, reg_based_on_size("rdx", type_to_len(typeinfo)));
                    // strcat(transpiler->asmc, reg_based_on_size("rdx", type_to_len(type_info)));
                };
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "pop rdx");
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "pop rax");
            }
        }else if (expr->type == AST_LT){
            if (expr->data.expr.left->type == AST_INT && expr->data.expr.right->type == AST_INT){
                char a[100];
                snprintf(a, 100, "%d", atoi(expr->data.expr.left->data.arg.value) < atoi(expr->data.expr.right->data.arg.value));
                strcat(transpiler->asmc, "mov ");
                strcat(transpiler->asmc, reg);
                strcat(transpiler->asmc, ", ");
                strcat(transpiler->asmc, a);
            }else {
                strcat(transpiler->asmc, transpiler_asm_expr(transpiler, expr->data.expr.right, "r9", (AST_TypeInfo){0}, 0));
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, transpiler_asm_expr(transpiler, expr->data.expr.left, "r10", (AST_TypeInfo){0}, 0));
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "cmp r10, r9");
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "setl al");
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "movzx ");
                strcat(transpiler->asmc, reg);
                strcat(transpiler->asmc, ", al");
                strcat(transpiler->asmc, transpile_newl(transpiler));
            }
        }else if (expr->type == AST_GT){
            if (expr->data.expr.left->type == AST_INT && expr->data.expr.right->type == AST_INT){
                char a[100];
                snprintf(a, 100, "%d", atoi(expr->data.expr.left->data.arg.value) > atoi(expr->data.expr.right->data.arg.value));
                strcat(transpiler->asmc, "mov ");
                strcat(transpiler->asmc, reg);
                strcat(transpiler->asmc, ", ");
                strcat(transpiler->asmc, a);
            }else {
                strcat(transpiler->asmc, transpiler_asm_expr(transpiler, expr->data.expr.right, "r9", (AST_TypeInfo){0}, 0));
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, transpiler_asm_expr(transpiler, expr->data.expr.left, "r10", (AST_TypeInfo){0}, 0));
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "cmp r10, r9");
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "setg al");
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "movzx ");
                strcat(transpiler->asmc, reg);
                strcat(transpiler->asmc, ", al");
                strcat(transpiler->asmc, transpile_newl(transpiler));
            }
        }else if (expr->type == AST_LTE){
            if (expr->data.expr.left->type == AST_INT && expr->data.expr.right->type == AST_INT){
                char a[100];
                snprintf(a, 100, "%d", atoi(expr->data.expr.left->data.arg.value) <= atoi(expr->data.expr.right->data.arg.value));
                strcat(transpiler->asmc, "mov ");
                strcat(transpiler->asmc, reg);
                strcat(transpiler->asmc, ", ");
                strcat(transpiler->asmc, a);
            }else {
                strcat(transpiler->asmc, transpiler_asm_expr(transpiler, expr->data.expr.right, "r9", (AST_TypeInfo){0}, 0));
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, transpiler_asm_expr(transpiler, expr->data.expr.left, "r10", (AST_TypeInfo){0}, 0));
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "cmp r10, r9");
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "setle al");
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "movzx ");
                strcat(transpiler->asmc, reg);
                strcat(transpiler->asmc, ", al");
                strcat(transpiler->asmc, transpile_newl(transpiler));
            }
        }else if (expr->type == AST_GTE){
            if (expr->data.expr.left->type == AST_INT && expr->data.expr.right->type == AST_INT){
                char a[100];
                snprintf(a, 100, "%d", atoi(expr->data.expr.left->data.arg.value) >= atoi(expr->data.expr.right->data.arg.value));
                strcat(transpiler->asmc, "mov ");
                strcat(transpiler->asmc, reg);
                strcat(transpiler->asmc, ", ");
                strcat(transpiler->asmc, a);
            }else {
                strcat(transpiler->asmc, transpiler_asm_expr(transpiler, expr->data.expr.right, "r9", (AST_TypeInfo){0}, 0));
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, transpiler_asm_expr(transpiler, expr->data.expr.left, "r10", (AST_TypeInfo){0}, 0));
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "cmp r10, r9");
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "setge al");
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "movzx ");
                strcat(transpiler->asmc, reg);
                strcat(transpiler->asmc, ", al");
                strcat(transpiler->asmc, transpile_newl(transpiler));
            }
        }else if (expr->type == AST_EQ){
            if (expr->data.expr.left->type == AST_INT && expr->data.expr.right->type == AST_INT){
                char a[100];
                snprintf(a, 100, "%d", atoi(expr->data.expr.left->data.arg.value) == atoi(expr->data.expr.right->data.arg.value));
                strcat(transpiler->asmc, "mov ");
                strcat(transpiler->asmc, reg);
                strcat(transpiler->asmc, ", ");
                strcat(transpiler->asmc, a);
            }else {
                strcat(transpiler->asmc, transpiler_asm_expr(transpiler, expr->data.expr.right, reg_based_on_size("r9", type_to_len(fetch_type(transpiler, expr->data.expr.right))), (AST_TypeInfo){0}, 0));
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, transpiler_asm_expr(transpiler, expr->data.expr.left, reg_based_on_size("r10", type_to_len(fetch_type(transpiler, expr->data.expr.left))), (AST_TypeInfo){0}, 0));
                strcat(transpiler->asmc, transpile_newl(transpiler));
                if (strcmp(type_info.type, "if") == 0){
                    strcat(transpiler->asmc, "cmp r10, r9\n");
                    return "ifeq";
                }else {

                strcat(transpiler->asmc, "cmp r10, r9");
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "sete al");
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "movzx ");
                strcat(transpiler->asmc, reg);
                strcat(transpiler->asmc, ", al");
                strcat(transpiler->asmc, transpile_newl(transpiler));
                }
            }
        }else if (expr->type == AST_NEQ){
            if (expr->data.expr.left->type == AST_INT && expr->data.expr.right->type == AST_INT){
                char a[100];
                snprintf(a, 100, "%d", atoi(expr->data.expr.left->data.arg.value) != atoi(expr->data.expr.right->data.arg.value));
                strcat(transpiler->asmc, "mov ");
                strcat(transpiler->asmc, reg);
                strcat(transpiler->asmc, ", ");
                strcat(transpiler->asmc, a);
            }else {
                strcat(transpiler->asmc, transpiler_asm_expr(transpiler, expr->data.expr.right, "r9", (AST_TypeInfo){0}, 0));
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, transpiler_asm_expr(transpiler, expr->data.expr.left, "r10", (AST_TypeInfo){0}, 0));
                strcat(transpiler->asmc, transpile_newl(transpiler));
                goto hi;
                thing: 
                    strcat(transpiler->asmc, "cmp r10, r9");
                    strcat(transpiler->asmc, transpile_newl(transpiler));
                    strcat(transpiler->asmc, "setne al");
                    strcat(transpiler->asmc, transpile_newl(transpiler));
                    strcat(transpiler->asmc, "movzx ");
                    strcat(transpiler->asmc, reg);
                    strcat(transpiler->asmc, ", al");
                    strcat(transpiler->asmc, transpile_newl(transpiler));
                    return "";
                hi: 
                if (!type_info.type){
                    goto thing;
                }else if (strcmp(type_info.type, "if") == 0){
                    strcat(transpiler->asmc, "cmp r10, r9\n");
                    return "ifneq";
                }else {
                    goto thing;
                }
            }
        }
    }else if(expr->type == AST_FUNCALL){
        transpile_func(transpiler, expr);
        strcat(transpiler->asmc, "mov ");
        AST_TypeInfo type = fetch_type(transpiler, expr);
        strcat(transpiler->asmc, reg_based_on_size(reg, type_to_len(type)));
        strcat(transpiler->asmc, ", ");
        strcat(transpiler->asmc, reg_based_on_size("rax", type_to_len(type)));
        strcat(transpiler->asmc, transpile_newl(transpiler));
    };
    return "";
};

void transpile_asm_block(Transpiler *transpiler, AST block){
    if (try_transpile_mode(transpiler, &block) == 0){
        return;
    };
    if (block.type == AST_BREAK){
        if (transpiler->prevWhileLoop){
            strcat(transpiler->asmc, "jmp ");
            strcat(transpiler->asmc, transpiler->prevWhileLoop);
            strcat(transpiler->asmc, transpile_newl(transpiler));
        }
    }else if (block.type == AST_FUNCALL){
        transpile_func(transpiler, &block);
    } else if(block.type == AST_SYSCALL){
        transpile_syscall(transpiler, &block);
    }else if(block.type == AST_ASSIGN){
        char z = -1;
        for (int zi=0; zi<transpiler->variablelen; zi++){
            if (block.data.assign.from){ 
                if (get_value_of_variable(block.data.assign.from) > 0) {
                    if (strcmp(transpiler->variables[zi].name, get_value_of_variable(block.data.assign.from)) == 0) {
                        z = zi;
                    }
                };
            }
        };
        if (z == -1){
            #define current_scope transpiler->functions[transpiler->functionLen-1].scope
            current_scope.variables[current_scope.variablesLen].name = get_value_of_variable(block.data.assign.from);
            current_scope.variables[current_scope.variablesLen++].type_info = block.typeinfo;
            #undef current_scope
            // TODO: Support two kinds of variables: global and local
        };
        if (strcmp(block.typeinfo.type, "unknown") == 0){
            block.typeinfo = transpiler->variables[z].type_info;
        };
        if (block.typeinfo.type == 0 && z != -1){
            block.typeinfo = transpiler->variables[z].type_info;
        }
        AST ast = ptr_deref(transpiler, block, *(block.data.assign.from));
        char *a;
        if (block.typeinfo.type){
            a = correct_format_for_var(transpiler, get_value_of_variable(block.data.assign.from), "self", block.typeinfo);
        }else {
            a = correct_format_for_var(transpiler, get_value_of_variable(block.data.assign.from), "self", fetch_type(transpiler, &block));
        }
        AST_TypeInfo typeinfo = fetch_type(transpiler, &block);
        if (get_type(transpiler, &ast).ptrnum > 0){
            ;
        }else if(block.data.assign.from->type == AST_DEREF){
            // strcat(transpiler->asmc, transpiler_asm_expr(transpiler, block.data.assign.from->data.expr.left, "r8", get_type(transpiler, block.data.assign.from->data.expr.left), 0));
            // strcat(transpiler->asmc, transpile_newl(transpiler));
            typeinfo = fetch_type(transpiler, block.data.assign.from->data.expr.left);
            typeinfo = dec_ptrnum(typeinfo);
            // a = type_as_var(typeinfo, "r8");
            a = reg_based_on_size("r13", type_to_len(typeinfo));
        }
        if (typeinfo.type && !block.typeinfo.type){ 
            block.typeinfo = typeinfo;
        } 
            check_cast(transpiler, *block.data.assign.from, *block.data.assign.assignto, block.row, block.col, transpiler->name, false);
            AST asta = ptr_deref(transpiler, block, *block.data.assign.from->data.expr.left);
            if (fetch_type(transpiler, block.data.assign.assignto).ptrnum == block.typeinfo.ptrnum && block.typeinfo.ptrnum != 0){
                // strncpy(b, get_value_of_variable(block.data.assign.from), 100);
                asta = block;
            };
            if (block.data.assign.assignto->type) {
                strcat(transpiler->asmc, transpiler_asm_expr(transpiler, block.data.assign.assignto, a, fetch_type(transpiler, &asta), 0));
                strcat(transpiler->asmc, transpile_newl(transpiler));
            }else if(transpiler->transpiler_mode == MODE_POINTER_ALLOC){
                strcat(transpiler->asmc, "mov rax, ");
                char v[100];
                snprintf(v, 100, "%d", type_to_len(block.typeinfo));
                strcat(transpiler->asmc, v);
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "call malloc");
                char c = 0;
                for (int i=0; i<transpiler->toIncludeLen; i++){
                    if (strcmp(transpiler->toInclude[i], "malloc") == 0){
                        c = 1;
                    };
                };
                if (c == 0){
                    transpiler->toInclude[transpiler->toIncludeLen++] = "malloc";
                }
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "mov ");
                strcat(transpiler->asmc, a);
                strcat(transpiler->asmc, ", rax");
                strcat(transpiler->asmc, transpile_newl(transpiler));
            }
        if (block.data.assign.from->type == AST_DEREF){
            if (block.typeinfo.type){
                a = correct_format_for_var(transpiler, get_value_of_variable(block.data.assign.from), "self", block.typeinfo);
            }else {
                a = correct_format_for_var(transpiler, get_value_of_variable(block.data.assign.from), "self", fetch_type(transpiler, &block));
            }
            strcat(transpiler->asmc, transpiler_asm_expr(transpiler, block.data.assign.from->data.expr.left, "r8", get_type(transpiler, block.data.assign.from->data.expr.left), 0));
            strcat(transpiler->asmc, transpile_newl(transpiler));
            strcat(transpiler->asmc, "mov ");
            strcat(transpiler->asmc, type_as_var(typeinfo, "r8"));
            strcat(transpiler->asmc, ", ");
            strcat(transpiler->asmc, reg_based_on_size("r13", type_to_len(fetch_type(transpiler, &block))));
            strcat(transpiler->asmc, transpile_newl(transpiler));
            /* strcat(transpiler->asmc, "mov ");
            strcat(transpiler->asmc, a);
            strcat(transpiler->asmc, ", ");
            strcat(transpiler->asmc, reg_based_on_size("r13", type_to_len(fetch_type(transpiler, &block))));
            strcat(transpiler->asmc, transpile_newl(transpiler)); */
        };;
    }else if(block.type == AST_RET){
        if (transpiler->functionLen != 0){
            if (strcmp(transpiler->functions[transpiler->functionLen-1].name, "main") == 0 && _BirdSharpTypes == TYPE_STATIC) {
                AST_TypeInfo typeinfo1 = fetch_type(transpiler, block.data.ret.ret);
                /* if (block.data.ret.ret->type == AST_DEREF){
                    typeinfo1.ptrnum--;
                } */
                if (typeinfo1.ptrnum != 0 || strcmp(typeinfo1.type, "int")){
                    error_generate_parser("MainError", "Return type is not same as main", block.data.ret.ret->row, block.data.ret.ret->col, transpiler->name);
                };
                if (block.data.ret.ret->type == AST_DEREF){
                    AST ast = ptr_deref(transpiler, *(block.data.ret.ret->data.expr.left), *(block.data.ret.ret)); 
                    strcat(transpiler->asmc, transpiler_asm_expr(transpiler, &ast, "rdi", (AST_TypeInfo){0}, 0));
                    strcat(transpiler->asmc, transpile_newl(transpiler));
                    strcat(transpiler->asmc, "mov rdi, ");
                    AST_TypeInfo typeinfo = get_type(transpiler, block.data.ret.ret);
                    ast.typeinfo = typeinfo;
                    strcat(transpiler->asmc, type_as_var(typeinfo, "rdi"));
                }else {
                    AST_TypeInfo typeinfo = fetch_type(transpiler, block.data.ret.ret);
                    strcat(transpiler->asmc, transpiler_asm_expr(transpiler, block.data.ret.ret, reg_based_on_size("rdi", type_to_len(typeinfo)), (AST_TypeInfo){0}, 0));
                }
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "mov rsp, rbp");
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "pop rbp");
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "mov rax, 0x02000001");
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "syscall");
                strcat(transpiler->asmc, transpile_newl(transpiler));
            }else if(strcmp(transpiler->functions[transpiler->functionLen-1].name, "main") == 0){
                strcat(transpiler->asmc, "mov rsp, rbp");
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "pop rbp");
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, transpiler_asm_expr(transpiler, block.data.ret.ret, "rax", (AST_TypeInfo){0}, 0));
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "ret");
                strcat(transpiler->asmc, transpile_newl(transpiler));
            }else {
                if (fetch_type(transpiler, block.data.ret.ret).ptrnum != transpiler->functions[transpiler->functionLen-1].type.ptrnum || strcmp(fetch_type(transpiler, block.data.ret.ret).type, transpiler->functions[transpiler->functionLen-1].type.type) && strcmp(fetch_type(transpiler, block.data.ret.ret).type, "null")){
                    error_generate_parser("MainError", "Return type is not same as function declaration", block.data.ret.ret->row, block.data.ret.ret->col, transpiler->name);
                };
                strcat(transpiler->asmc, transpiler_asm_expr(transpiler, block.data.ret.ret, "rax", (AST_TypeInfo){0}, 0));
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "mov rsp, rbp\n\tpop rbp");
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "ret");
                strcat(transpiler->asmc, transpile_newl(transpiler));
            };
        }else {
            error_generate_parser("SyntaxError", "So far, returns cannot happen outside of a function", block.row, block.col, transpiler->name);
        }
    }else if(block.type == AST_IF){
        char *abc = transpiler_asm_expr(transpiler, block.data.if1.condition, "r13", (AST_TypeInfo){"if", 0}, 0);
        char a[100];snprintf(a, 100, "._LBF%d", transpiler->funcnum++);
        char c[100];snprintf(c, 100, "._LBF%d", transpiler->funcnum++);
        char d[100];snprintf(d, 100, "._LBF%d", transpiler->funcnum++);
        if (abc[0] != 'i' && abc[1] != 'f'){
            strcat(transpiler->asmc, abc);
            strcat(transpiler->asmc, transpile_newl(transpiler));
            strcat(transpiler->asmc, "test r13, r13");
        }
        if (strcmp(abc, "ifeq") == 0){
            strcat(transpiler->asmc, transpile_newl(transpiler));
            strcat(transpiler->asmc, "jne ");
            strcat(transpiler->asmc, d);
        }else if (strcmp(abc, "ifneq") == 0){
            strcat(transpiler->asmc, transpile_newl(transpiler));
            strcat(transpiler->asmc, "je ");
            strcat(transpiler->asmc, d);
        }else {
            strcat(transpiler->asmc, transpile_newl(transpiler));
            strcat(transpiler->asmc, "jz ");
            strcat(transpiler->asmc, d);
        }
        /*
        if (strcmp(transpiler->prevWhileLoop, "") == 0){
            strcat(transpiler->asmc, d);
        }else {
            fprintf(stderr, "{%s}", transpiler->prevWhileLoop);
            strcat(transpiler->asmc, transpiler->prevWhileLoop);
        } */
        strcat(transpiler->asmc, transpile_newl(transpiler));
        strcat(transpiler->asmc, a);
        strcat(transpiler->asmc, ":\n\t");
        for (int i=0; i<block.data.if1.blocklen; i++){
            transpile_asm_block(transpiler, *block.data.if1.block[i]);
        };
        strcat(transpiler->asmc, "jmp ");
        strcat(transpiler->asmc, c);
        strcat(transpiler->asmc, transpile_newl(transpiler));

        strcat(transpiler->asmc, "\n");
        strcat(transpiler->asmc, d);
        strcat(transpiler->asmc, ":");
        char b[100];strncpy(b, "", 100);
        char n[100];strncpy(n, "", 100);
        snprintf(n, 100, "._LBF%d", transpiler->funcnum++);
        for (int i=0; i<block.data.if1.elseiflen; i++){
            snprintf(b, 100, "._LBF%d", transpiler->funcnum++);
            char *abc = transpiler_asm_expr(transpiler, block.data.if1.elseif[i].condition, "r13", (AST_TypeInfo){"if", 0}, 0);
            if (strcmp(abc, "ifcorrect")){
                strcat(transpiler->asmc, abc);
                strcat(transpiler->asmc, transpile_newl(transpiler));
                strcat(transpiler->asmc, "test r13, r13");
                strcat(transpiler->asmc, transpile_newl(transpiler));
            }
            /* strcat(transpiler->asmc, "jnz ");
            strcat(transpiler->asmc, n);
            strcat(transpiler->asmc, transpile_newl(transpiler)); */
            strcat(transpiler->asmc, "jnz ");
            strcat(transpiler->asmc, b);
            strcat(transpiler->asmc, transpile_newl(transpiler));
        };
        strcat(transpiler->asmc, "\n");
        strcat(transpiler->asmc, n);
        strcat(transpiler->asmc, ":");
        transpiler->isTab = 1;
        for (int i=0; i<block.data.if1.elselen; i++){
            transpile_asm_block(transpiler, *block.data.if1.else1[i]);
        };
        transpiler->isTab = 0;
        strcat(transpiler->asmc, "jmp ");
        strcat(transpiler->asmc, c);
        strcat(transpiler->asmc, transpile_newl(transpiler));
        for (int i=0; i<block.data.if1.elseiflen; i++){
            snprintf(b, 100, "._LBF%d", transpiler->funcnum - block.data.if1.elseiflen + i);
            strcat(transpiler->asmc, "\n");
            strcat(transpiler->asmc, b);
            strcat(transpiler->asmc, ":\n\t");
            for (int j=0; j<block.data.if1.elseif[i].statementLen; j++){
                transpile_asm_block(transpiler, *block.data.if1.elseif[i].statements[j]);
            };
            strcat(transpiler->asmc, "jmp ");
            strcat(transpiler->asmc, c);
            strcat(transpiler->asmc, transpile_newl(transpiler));
        };
        strcat(transpiler->asmc, transpile_newl(transpiler));
        strcat(transpiler->asmc, c);
        strcat(transpiler->asmc, ":");
        transpiler->isTab = 1;

        strcat(transpiler->asmc, transpile_newl(transpiler));
    }else if(block.type == AST_WHILE){
        strcat(transpiler->asmc, transpiler_asm_expr(transpiler, block.data.while1.condition, "r13", (AST_TypeInfo){0}, 0));
        strcat(transpiler->asmc, transpile_newl(transpiler));
        strcat(transpiler->asmc, "test r13, r13");
        strcat(transpiler->asmc, transpile_newl(transpiler));
        char a[100];snprintf(a, 100, "._LBF%d", transpiler->funcnum++);
        char c[100];snprintf(c, 100, "._LBF%d", transpiler->funcnum++);
        transpiler->prevWhileLoop = strdup(c);
        strcat(transpiler->asmc, "jz ");
        strcat(transpiler->asmc, c);
        transpiler->isTab = 0;
        strcat(transpiler->asmc, transpile_newl(transpiler));
        strcat(transpiler->asmc, a);
        strcat(transpiler->asmc, ":");
        transpiler->isTab = 1;
        strcat(transpiler->asmc, transpile_newl(transpiler));
        for (int i=0; i<block.data.while1.blocklen; i++){
            transpile_asm_block(transpiler, *block.data.while1.block[i]);
        };
        strcat(transpiler->asmc, transpiler_asm_expr(transpiler, block.data.while1.condition, "r13", (AST_TypeInfo){0}, 0));
        strcat(transpiler->asmc, transpile_newl(transpiler));
        strcat(transpiler->asmc, "cmp r13, 0");
        strcat(transpiler->asmc, transpile_newl(transpiler));
        strcat(transpiler->asmc, "jne ");
        strcat(transpiler->asmc, a);
        strcat(transpiler->asmc, transpile_newl(transpiler));
        strcat(transpiler->asmc, "jmp ");
        strcat(transpiler->asmc, c);
        strcat(transpiler->asmc, transpile_newl(transpiler));
        char b[100];strncpy(b, "", 100);

        transpiler->isTab = 0;
        strcat(transpiler->asmc, transpile_newl(transpiler));

        strcat(transpiler->asmc, c);
        strcat(transpiler->asmc, ":");
        transpiler->isTab = 1;

        strcat(transpiler->asmc, transpile_newl(transpiler));
        transpiler->prevWhileLoop = "";
    };
};
void transpile_asm(Transpiler *transpiler){
    if (try_transpile_mode(transpiler, &(transpiler->asts[transpiler->cur])) == 0){
        transpiler->cur++;
        return;
    };
    if (transpiler->asts[transpiler->cur].type == AST_FUNCDEF){
        transpiler->functions[transpiler->functionLen].name = transpiler->asts[transpiler->cur].data.funcdef.name;
        transpiler->functions[transpiler->functionLen].type = transpiler->asts[transpiler->cur].typeinfo;
        for (int z=0; z<transpiler->asts[transpiler->cur].data.funcdef.argslen; z++){
            transpiler->functions[transpiler->functionLen].args[z] = (Argument){transpiler->asts[transpiler->cur].data.funcdef.args[z]->arg, transpiler->asts[transpiler->cur].data.funcdef.args[z]->type};
            transpiler->functions[transpiler->functionLen].argslen++;
        };
        transpiler->functionLen++;
        strcat(transpiler->asmc, "\n");
        if (strcmp(transpiler->asts[transpiler->cur].data.funcdef.name, "main")){
            strcat(transpiler->asmc, transpiler->asts[transpiler->cur].data.funcdef.name);
        }else {
            if (strcmp(transpiler->asts[transpiler->cur].typeinfo.type, "int") || transpiler->asts[transpiler->cur].typeinfo.ptrnum != 0){
                error_generate_parser("MainError", "Main has to return 'int'", transpiler->asts[transpiler->cur].row, transpiler->asts[transpiler->cur].col, transpiler->name);
            };
            strcat(transpiler->asmc, "align 16\n");
            strcat(transpiler->asmc, (_BirdSharpTypes == TYPE_STATIC) ? "start" : "_main");
        }
        strcat(transpiler->asmc, ":\n\t");
        if (strcmp(transpiler->asts[transpiler->cur].data.funcdef.name, "main") == 0){
            strcat(transpiler->asmc, "mov rbx, rsp\n\t");
        }
        strcat(transpiler->asmc, "push rbp\n\tmov rbp, rsp\n\tsub rsp, 32\n\t");
        if (strcmp(transpiler->asts[transpiler->cur].data.funcdef.name, "main") == 0){
            strcat(transpiler->asmc, "call __pre\n\t");
        }
        if (_BirdSharpTypes == TYPE_DYNAMIC){
            strcat(transpiler->asmc, "mov qword [rbx + 8], rsi\n\tpush rsi\n\t");
        }
        transpiler->isTab = 1;
        for (int i=0; i<transpiler->asts[transpiler->cur].data.funcdef.blocklen; i++){
            transpile_asm_block(transpiler, *transpiler->asts[transpiler->cur].data.funcdef.block[i]);
        };
        transpiler->funcnum = 0;
        if (strcmp(transpiler->asts[transpiler->cur].data.funcdef.name, "main") == 0){
            strcat(transpiler->asmc, "mov rsp, rbp\n\tpop rbp\n\tmov rax, 0x02000001\n\txor rdi, rdi\n\tsyscall");
        }else {
            strcat(transpiler->asmc, "mov rsp, rbp\n\tpop rbp\n\tret");
        }
        transpiler->cur++;
    }else if(transpiler->asts[transpiler->cur].type == AST_ASSIGN){
        char z = -1;
        for (int zi=0; zi<transpiler->variablelen; zi++){
            if (strcmp(transpiler->variables[zi].name, transpiler->asts[transpiler->cur].data.assign.from->data.arg.value) == 0){
                z = zi;
            };
        };
        if (strcmp(transpiler->asts[transpiler->cur].typeinfo.type, "unknown") == 0){
            error_generate_parser("AssignError", "Need to provide type", transpiler->asts[transpiler->cur].row, transpiler->asts[transpiler->cur].col, transpiler->name);
        }else {
            if (z != -1){
                error_generate_parser("AssignError", "Redefinition of variable", transpiler->asts[transpiler->cur].row, transpiler->asts[transpiler->cur].col, transpiler->name);
            };
            z = -1;
        };
        transpiler->variables[transpiler->variablelen].name = transpiler->asts[transpiler->cur].data.assign.from->data.arg.value;
        transpiler->variables[transpiler->variablelen].type_info = transpiler->asts[transpiler->cur].typeinfo;
        transpiler->variablelen++;
        if (transpiler->asts[transpiler->cur].typeinfo.type == 0){
            error_generate_parser("TypeError", "Type does not exist", transpiler->asts[transpiler->cur].row, transpiler->asts[transpiler->cur].col, transpiler->name);
        }
            if (z == -1){
                initialize_bssc(transpiler, transpiler->asts[transpiler->cur].typeinfo, get_value_of_variable(transpiler->asts[transpiler->cur].data.assign.from));
            };
            char b[100];
            strncpy(b, "", 100);
            strncpy(b, type_as_var(fetch_type(transpiler, &transpiler->asts[transpiler->cur]), get_value_of_variable(transpiler->asts[transpiler->cur].data.assign.from)), 100);
        transpiler->cur++;
    }else if(transpiler->asts[transpiler->cur].type == AST_TYPEDEF){
        transpiler->cur++; // Typedefs are handled at parsing stage
    };
};

char transpile_to_asm(Transpiler *transpiler){
    if (transpiler->astlen == transpiler->cur){
        return -1;
    };
    transpile_asm(transpiler);
    return 0;
};

bool StartsWith(char *a, char *b)
{
   if(strncmp(a, b, strlen(b)) == 0) return 1;
   return 0;
};

ByteCode *init_bytecode(Parser *parser){
    ByteCode *bytecode = malloc(sizeof(ByteCode));
    memcpy(bytecode->asts, parser->asts, parser->astlen * sizeof(AST));
    bytecode->astlen = parser->astlen;
    return bytecode;
};

void bytecode_type_match(ByteCode *bytecode, AST_TypeInfo typeinfo){
    char match;
    for (int i=0; i<typesLen; i++){
        if (strcmp(types[i].name, typeinfo.type) == 0){
            match = i;
        }
    };
    if (strcmp(typeinfo.type, "int") == 0){
        match = 0x55;
        if (typeinfo.ptrnum > 2){
            bytecode->bytecode[bytecode->instr_len++] = match;
            bytecode->bytecode[bytecode->instr_len++] = 0x66 + typeinfo.ptrnum - 3;;
        }else {
            bytecode->bytecode[bytecode->instr_len++] = match + typeinfo.ptrnum;
        };
    }else if (strcmp(typeinfo.type, "char") == 0){
        match = 0x58;
        if (typeinfo.ptrnum > 2){
            bytecode->bytecode[bytecode->instr_len++] = match;
            bytecode->bytecode[bytecode->instr_len++] = 0x66 + typeinfo.ptrnum - 3;
        }else {
            bytecode->bytecode[bytecode->instr_len++] = match + typeinfo.ptrnum;
        };
    }else if (strcmp(typeinfo.type, "long") == 0){
        match = 0x5b;
        if (typeinfo.ptrnum > 3){
            bytecode->bytecode[bytecode->instr_len++] = match;
            bytecode->bytecode[bytecode->instr_len++] = 0x66 + typeinfo.ptrnum - 3;
        }else {
            bytecode->bytecode[bytecode->instr_len++] = match + typeinfo.ptrnum;
        };;
    }else {
        ;
    };
    if (match == 0x69){

    }
};

char move_reg(int regn){
    if (regn > 16){
        error_generate("RegisterError", "Found register over 16, invalid");
    }
    return regn - 1;
};


char move_imm_reg(int regn){
    if (regn > 16){
        error_generate("RegisterError", "Found register over 16, invalid");
    }
    return regn + 0x3d;
};

char cmp_reg(int regn){
    if (regn > 16){
        error_generate("RegisterError", "Found register over 16, invalid");
    }
    return regn + 0x24;
};

void move_addr_reg(ByteCode *bytecode, int regn){
    if (regn > 16){
        error_generate("RegisterError", "Found register over 16, invalid");
    }
    bytecode->bytecode[bytecode->instr_len++] = 0x6a;
    bytecode->bytecode[bytecode->instr_len++] = regn;
};

void byte_type_spec(ByteCode *bytecode, AST_TypeInfo type){/*
    if (type.ptrnum > 0){
        bytecode->bytecode[bytecode->instr_len++] = 0x70;
    }else if (strcmp(type.type, "int") == 0){
        bytecode->bytecode[bytecode->instr_len++] = 0x6d;
    }else if (strcmp(type.type, "char") == 0){
        bytecode->bytecode[bytecode->instr_len++] = 0x6e;
    }else if (strcmp(type.type, "string") == 0){
        bytecode->bytecode[bytecode->instr_len++] = 0x6f;
    };*/
};

char *format_string(char *str){
    char *b = strdup(str);
    char h[100];
    int hb = 0;
    while (*b){
        if (*b == '\\'){
            b++;
            if (*b == 'n'){
                h[hb++] = 10;
                b++;
                continue;
            };
        };
        h[hb++] = *b;
        b++;
    };
    return strdup(h);
}

void bytecode_set(ByteCode *bytecode, int reg, AST *ast);
void bytecode_make_expr(ByteCode *bytecode, AST *ast, int reg){
    if (ast->type == AST_STRING){
        byte_type_spec(bytecode, (AST_TypeInfo){"char", 0});
        bytecode->stringtable[bytecode->stringtablelen++] = format_string(ast->data.arg.value);
        int res = bytecode->stringtablelen - 1;
        bytecode->bytecode[bytecode->instr_len++] = res & 0xFF;
        bytecode->bytecode[bytecode->instr_len++] = (res >> 8) & 0xFF;
        bytecode->bytecode[bytecode->instr_len++] = (res >> 16) & 0xFF;
        bytecode->bytecode[bytecode->instr_len++] = (res >> 24) & 0xFF;
    }else if(ast->type == AST_INT){
        byte_type_spec(bytecode, (AST_TypeInfo){"null", 0});
        int res = atoi(ast->data.arg.value);
        bytecode->bytecode[bytecode->instr_len++] = res & 0xFF;
        bytecode->bytecode[bytecode->instr_len++] = (res >> 8) & 0xFF;
        bytecode->bytecode[bytecode->instr_len++] = (res >> 16) & 0xFF;
        bytecode->bytecode[bytecode->instr_len++] = (res >> 24) & 0xFF;
    }else if(ast->type == AST_PLUS){
        bytecode_set(bytecode, 7, ast->data.expr.right);
        bytecode_set(bytecode, reg, ast->data.expr.left);
        bytecode->bytecode[bytecode->instr_len++] = 0x20;
        bytecode->bytecode[bytecode->instr_len++] = reg;
        bytecode->bytecode[bytecode->instr_len++] = 7;
        bytecode->instr_num++;
    }else if(ast->type == AST_SUB){
        bytecode_set(bytecode, reg, ast->data.expr.left);
        bytecode_set(bytecode, 7, ast->data.expr.right);
        bytecode->bytecode[bytecode->instr_len++] = 0x21;
        bytecode->bytecode[bytecode->instr_len++] = reg;
        bytecode->bytecode[bytecode->instr_len++] = 7;
        bytecode->instr_num+=2;
    }else if(ast->type == AST_DIV){
        bytecode_set(bytecode, reg, ast->data.expr.left);
        bytecode_set(bytecode, 7, ast->data.expr.right);
        bytecode->bytecode[bytecode->instr_len++] = 0x23;
        bytecode->bytecode[bytecode->instr_len++] = reg;
        bytecode->bytecode[bytecode->instr_len++] = 7;
        bytecode->instr_num+=2;
    }else if(ast->type == AST_MUL){
        bytecode_set(bytecode, reg, ast->data.expr.left);
        bytecode_set(bytecode, 7, ast->data.expr.right);
        bytecode->bytecode[bytecode->instr_len++] = 0x22;
        bytecode->bytecode[bytecode->instr_len++] = reg;
        bytecode->bytecode[bytecode->instr_len++] = 7;
        bytecode->instr_num+=2;
    }else if(ast->type == AST_MODULO){
        bytecode_set(bytecode, reg, ast->data.expr.left);
        bytecode_set(bytecode, 7, ast->data.expr.right);
        bytecode->bytecode[bytecode->instr_len++] = 0x24;
        bytecode->bytecode[bytecode->instr_len++] = reg;
        bytecode->bytecode[bytecode->instr_len++] = 7;
        bytecode->instr_num+=2;
    }else if(ast->type == AST_FUNCALL){
        bytecode->bytecode[bytecode->instr_len++] = 0x52;
        char addr = -1;
        for (int i=0; i<bytecode->funclen; i++){
            if (strcmp(bytecode->func[i].name, ast->data.funcall.name) == 0){
                addr = bytecode->func[i].addr;
            };
        };
        bytecode->bytecode[bytecode->instr_len++] = addr + 1;
        if (reg != 1) {
            bytecode->bytecode[bytecode->instr_len++] = move_reg(reg);
            bytecode->bytecode[bytecode->instr_len++] = 1;
            bytecode->instr_num++;
        }
    };
};

void bytecode_set(ByteCode *bytecode, int reg, AST *ast){
    if (ast->type == AST_INT){
        bytecode->bytecode[bytecode->instr_len++] = move_imm_reg(reg);
    }else if (ast->type == AST_STRING){
        move_addr_reg(bytecode, reg);
    }else if(ast->type == AST_PLUS || ast->type == AST_SUB || ast->type == AST_MUL || ast->type == AST_DIV || ast->type == AST_MODULO){
        while (ast->data.expr.right->type == AST_PLUS || ast->data.expr.right->type == AST_SUB || ast->data.expr.right->type == AST_MUL || ast->data.expr.right->type == AST_DIV || ast->data.expr.right->type == AST_MODULO) {
            AST exprr = *(ast->data.expr.right);
            AST exprl = *(ast->data.expr.left);
            ast->data.expr.left->type = ast->type;
            ast->type = exprr.type;
            ast->data.expr.right = exprr.data.expr.right;
            ast->data.expr.left->data.expr.left = malloc(sizeof(AST));
            *(ast->data.expr.left->data.expr.left) = exprl;
            ast->data.expr.left->data.expr.right = exprr.data.expr.left;
            ast->data.expr.right = ast->data.expr.right;;
        }
    }else if(ast->type == AST_FUNCALL){
        ;
    }else {
        bytecode->bytecode[bytecode->instr_len++] = move_reg(reg);
        byte_type_spec(bytecode, ast->typeinfo);
    };
    bytecode->instr_num++;
    bytecode_make_expr(bytecode, ast, reg);
};

void bytecode_jz(ByteCode *bytecode, int addr){
    bytecode->bytecode[bytecode->instr_len++] = 0x39;
    bytecode->bytecode[bytecode->instr_len++] = addr;
};

void bytecode_make_body(ByteCode *bytecode, AST *ast){
    if (ast->type == AST_FUNCALL || ast->type == AST_SYSCALL){
        for (int v=0; v<ast->data.funcall.argslen; v++){
            if (v > 5){
                break;
            };
            bytecode_set(bytecode, v + 2, ast->data.funcall.args[v]);
        };
        if (ast->type == AST_FUNCALL) {
            bytecode->bytecode[bytecode->instr_len++] = 0x52;
            char addr = -1;
            for (int i=0; i<bytecode->funclen; i++){
                if (strcmp(bytecode->func[i].name, ast->data.funcall.name) == 0){
                    addr = bytecode->func[i].addr;
                };
            };
            bytecode->bytecode[bytecode->instr_len++] = addr + 1;
        }else {
            bytecode->bytecode[bytecode->instr_len++] = 0x6b;
        }
    }else if(ast->type == AST_RET){
        bytecode_set(bytecode, 1, ast->data.ret.ret);
        bytecode->bytecode[bytecode->instr_len++] = 0x53;
    }else if(ast->type == AST_IF){ // TODO: Add if
        // bytecode_jz(bytecode, bytecode->instr_num);
        for (int i=0; i<ast->data.if1.elselen; i++){
            bytecode_make_body(bytecode, ast->data.if1.else1[i]);
        };
        for (int i=0; i<ast->data.if1.blocklen; i++){
            bytecode_make_body(bytecode, ast->data.if1.block[i]);
        };
    };
};

char bytecode_make(ByteCode *bytecode){
    if (bytecode->cur > bytecode->astlen){
        return -1;
    };
    if (bytecode->asts[bytecode->cur].type == AST_FUNCDEF){
        bytecode->func[bytecode->funclen].name = bytecode->asts[bytecode->cur].data.funcdef.name;
        bytecode->func[bytecode->funclen++].addr = bytecode->instr_num;
        for (int i=0; i<bytecode->asts[bytecode->cur].data.funcdef.blocklen; i++){
            bytecode_make_body(bytecode, bytecode->asts[bytecode->cur].data.funcdef.block[i]);
            bytecode->instr_num++;
        };
    }
    bytecode->cur++;
    return 0;
};

int main(int argc, char **argv){
    struct timespec __begin;
    clock_gettime(CLOCK_MONOTONIC, &__begin);
    argv++;
    char *input_file;
    char *output_file;
    bool dynamic1 = 0;
    char exed = 1;
    char link[100];
    strcpy(link, "");
    while (*argv){
        if (*argv[0] == '-'){ // Settings command
            if(strcmp(*argv, "-o") == 0){
                argv++;
                output_file = *argv;
            }else if(strcmp(*argv, "-h") == 0){
                printf("%s", HELP);
            }else if(StartsWith(*argv, "-entry_point")){
                printf("Found entry point");
            }else if(strcmp(*argv, "-dynamic") == 0){
                dynamic1 = 1;
            }else if(strcmp(*argv, "-byte") == 0){
                exed = 0;
            }else {
                strcat(link, *argv);
                strcat(link, " ");
            }
        } else {
            input_file = *argv;
        };
        argv++;
    };
    if (dynamic1 == 0){
        _BirdSharpTypes = TYPE_STATIC;
    }else {
        _BirdSharpTypes = TYPE_DYNAMIC;
    };
    if (!output_file){
        output_file = "res/main";
    };

    char asm_output_name[100];
    char obj_output_name[100];
    char exec_output_name[100];
    char byte_output_name[100];
    strncpy(asm_output_name, output_file, 100);
    strcat(asm_output_name, ".asm");
    strncpy(obj_output_name, output_file, 100);
    strcat(obj_output_name, ".o");
    strncpy(exec_output_name, output_file, 100);
    strcat(exec_output_name, ".out");
    strncpy(byte_output_name, output_file, 100);
    strcat(byte_output_name, ".bt");

    if (!input_file){
        printf("\x1b[1;31merror\x1b[0m: No input file provided");
        return -1;
    };
    struct timespec __start, __end;

    clock_gettime(CLOCK_MONOTONIC, &__start);

    Tokenizer *tokenizer = malloc(sizeof(Tokenizer));
    FILE *f = fopen(input_file, "r");
    tokenizer->line = 1;
    tokenizer->name = input_file;
    tokenizer->col = 0;
    tokenizer->code = malloc(90000);
    size_t size = fread(tokenizer->code, 1, 90000, f);
    tokenizer->code[size] = '\0';
    fclose(f);
    tokenizer->cur = 0;
    tokenizer->tokenlen = 0;

    while(tokenizer_token(tokenizer) != -1){
    };
    clock_gettime(CLOCK_MONOTONIC, &__end);
    fprintf(stderr, "[INFO] Tokenizer process took %.4f seconds\n", (__end.tv_sec - __start.tv_sec) * 1000.0 +
                        (__end.tv_nsec - __start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &__start);
    Macro macros[100];
    int macroLen = 0;

    for (int i=0; i<tokenizer->tokenlen; i++){
        if (i + 3 < tokenizer->tokenlen){
            if (strcmp(tokenizer->tokens[i].value, "#")==0&&strcmp(tokenizer->tokens[i+1].value, "macro")==0){
                Token orgi = tokenizer->tokens[i];
                rem_idx(tokenizer->tokens, &tokenizer->tokenlen, i);
                rem_idx(tokenizer->tokens, &tokenizer->tokenlen, i);
                strcpy(macros[macroLen].macro, tokenizer->tokens[i].value);
                rem_idx(tokenizer->tokens, &tokenizer->tokenlen, i);
                    while (strcmp(tokenizer->tokens[i].value, "endmacro")){
                        if (i+1 == tokenizer->tokenlen){
                            error_generate_parser("SyntaxError", "No \"endmacro\"", orgi.row, orgi.col, orgi.name);
                        };
                        macros[macroLen].tokens[macros[macroLen].tokenlen++] = tokenizer->tokens[i];
                        rem_idx(tokenizer->tokens, &tokenizer->tokenlen, i);
                    };
                    rem_idx(tokenizer->tokens, &tokenizer->tokenlen, i);
                macroLen++;
                i--;
            };
        }
        if(i+1 < tokenizer->tokenlen){
            if (tokenizer->tokens[i].type == TOKEN_ID){
                if (strcmp(tokenizer->tokens[i].value, "is") == 0){
                    add_idx(tokenizer->tokens, &tokenizer->tokenlen, i);
                    tokenizer->tokens[i].type = TOKEN_EQ;
                    strcpy(tokenizer->tokens[i].value, "=");
                    tokenizer->tokens[i].type = TOKEN_EQ;
                    strcpy(tokenizer->tokens[i+1].value, "=");
                };
                for (int vim=0; vim<macroLen; vim++){
                    if (strcmp(macros[vim].macro, tokenizer->tokens[i].value) == 0){
                        if (macros[vim].tokenlen == 0){
                            warning_generate("MacroExpansionError", "Empty macro");
                        }else {
                            tokenizer->tokens[i].type = macros[vim].tokens[0].type;
                            strncpy(tokenizer->tokens[i].value, macros[vim].tokens[0].value, 15);
                            tokenizer->tokens[i].value[strlen(macros[vim].tokens[0].value)] = '\0';
                            tokenizer->tokens[i].value[15] = '\0';
                            for (int vid=1; vid<macros[vim].tokenlen; vid++){
                                add_idx(tokenizer->tokens, &tokenizer->tokenlen, i+vid);
                                tokenizer->tokens[i+vid].type = macros[vim].tokens[vid].type;
                                strncpy(tokenizer->tokens[i+vid].value, macros[vim].tokens[vid].value, 15);
                                tokenizer->tokens[i+vid].value[strlen(macros[vim].tokens[vid].value)] = '\0';
                                tokenizer->tokens[i+vid].value[15] = '\0';
                            };
                        };
                    };
                };
            };
        }
    };
    clock_gettime(CLOCK_MONOTONIC, &__end);
    fprintf(stderr, "[INFO] Preprocessing process took %.4f seconds\n", (__end.tv_sec - __start.tv_sec) * 1000.0 +
                        (__end.tv_nsec - __start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &__start);
    Parser *parser = malloc(sizeof(Parser));
    parser->astlen = 0;
    parser->tokens[parser->cur].name = tokenizer->name;
    parser->cur = 0;
    memcpy(parser->tokens, tokenizer->tokens, tokenizer->tokenlen * sizeof(Token));
    parser->tokenlen = tokenizer->tokenlen;
    free(tokenizer);
    while (parser_eat(parser) != -1){};
    clock_gettime(CLOCK_MONOTONIC, &__end);
    fprintf(stderr, "[INFO] Parsing process took %.4f seconds\n", (__end.tv_sec - __start.tv_sec) * 1000.0 +
                        (__end.tv_nsec - __start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &__start);
    if (exed == 0){
        ByteCode *bytecode = init_bytecode(parser);
        while (bytecode_make(bytecode) != -1){
            ;
        };
        clock_gettime(CLOCK_MONOTONIC, &__end);
        fprintf(stderr, "[INFO] Bytecode process took %.4f seconds\n", (__end.tv_sec - __start.tv_sec) * 1000.0 +
                            (__end.tv_nsec - __start.tv_nsec) / 1e9);
        FILE *fc = fopen(byte_output_name, "wb");
        long a = 0xF34aCDBA;
        
        fwrite(&a, sizeof(a), 1, fc);
        fwrite(&bytecode->stringtablelen, sizeof(int), 1, fc);
        for (int i=0; i<bytecode->stringtablelen; i++){
            int a = strlen(bytecode->stringtable[i]);
            fwrite(&a, sizeof(int), 1, fc);
            fwrite(bytecode->stringtable[i], strlen(bytecode->stringtable[i]), 1, fc);
        };
        char addr = -1;
        for (int i=0; i<bytecode->funclen; i++){
            if (strcmp(bytecode->func[i].name, "main") == 0){
                addr = bytecode->func[i].addr;
            };
        };
        if (addr == -1){
            assert(0 && "Main is not found");
        };
        a = 0x52;
        addr++;
        fwrite(&a, 1, 1, fc);
        fwrite(&addr, 1, 1, fc);
        for (int i=0; i<bytecode->instr_len; i++){
            fwrite(&bytecode->bytecode[i], 1, 1, fc);
        };
        fclose(fc);
        free(bytecode);
    }else {
        Transpiler *transpiler = malloc(sizeof(Transpiler));
        memcpy(transpiler->asts, parser->asts, parser->astlen * sizeof(AST));
        transpiler->astlen = parser->astlen;
        transpiler->transpiler_mode = MODE_POINTER_ALLOC;
        transpiler->isTab = 0;
        transpiler->asmc = malloc(6000);
        strcpy(transpiler->asmc, "");
        transpiler->datac = malloc(6000);
        strcpy(transpiler->datac, "");
        transpiler->bssc = malloc(6000);;
        strcpy(transpiler->bssc, "");
        transpiler->prec = malloc(6000);
        strncpy(transpiler->prec, "", 6000);
        transpiler->prevWhileLoop = "";
        transpiler->curfun = -1;
        transpiler->name = parser->tokens[parser->cur].name;
        while (transpile_to_asm(transpiler) != -1){}
        f = fopen(asm_output_name, "w");
        char *res = malloc(3000);
        strcpy(res, "default rel\nBITS 64\nsection .bss align=16");
        strcat(res, transpiler->bssc);
        strcat(res, "\nsection .data align=8");
        strcat(res, transpiler->datac);
        strcat(res, "\nsection .text align=16\nglobal ");
        strcat(res, (_BirdSharpTypes == TYPE_STATIC) ? "start" : "_main");
        strcat(res, "\n");
        strcat(res, "__pre:\n\t");
        strcat(res, transpiler->prec);
        strcat(res, "ret\nstring_len:\n\txor rax, rax\n.loop:\n\tcmp byte [rdi], 0\n\tje .done\n\tinc rdi\n\tinc rax\n\tjnz .loop\n.done:\n\tret\n");
        for (int i=0; i<transpiler->toIncludeLen; i++){
            if (strcmp(transpiler->toInclude[i], "exit") == 0){
                strcat(res, "exit:\n\tmov rax, 0x02000001\n\tsyscall\n\tret\n");
            }else if (strcmp(transpiler->toInclude[i], "print") == 0){
                strcat(res, "print:\n\tmov rax, 0\n\tmov rcx, rdi\n\tcall string_len\n\tmov rdi, rcx\n\tmov rdx, rax\n\tmov rsi, rdi\n\tmov rdi, 1\n\tmov rax, 0x02000004\n\tsyscall\n\tret\n");
            }else if (strcmp(transpiler->toInclude[i], "string_copy") == 0){
                strcat(res, "string_copy:\n\tpush rdi\n\tpush rsi\n\tmov rax, 0\n\tcall string_len\n\tmov rcx, rax\n\tpop rsi\n\tpop rdi\n\txor rax, rax\n\trep movsb\n\tret\n");
            }else if (strcmp(transpiler->toInclude[i], "string_eq") == 0){
                strcat(res, "string_eq:\n\tpush rdi\n\tcall string_len\n\tpop rdi\n\tmov rcx, rax\n\tcld\n\trepe cmpsb\n\tsetz al\n\tmovzx rax, al\n\tret\n");
            }else if(strcmp(transpiler->toInclude[i], "malloc") == 0){
                strcat(res, "malloc:\n\tmov rsi, rdi            ; length\n\txor rdi, rdi\n\tmov rdx, 0x3             ; PROT_READ | PROT_WRITE\n\tmov r10, 0x1002          ; MAP_PRIVATE | MAP_ANON\n\tmov r8, -1               ; fd = -1 (not used)\n\txor r9, r9               ; offset = 0\n\tmov rax, 0x20000C5       ; syscall number for mmap\n\tsyscall\n\tret\n");
            };
        };
        strcat(res, transpiler->asmc);
        strcat(res, "\n");
        fwrite(res, 1, strlen(res), f);
        fclose(f);
        free(parser);
        clock_gettime(CLOCK_MONOTONIC, &__end);
        fprintf(stderr, "[INFO] Transpiling process took %.4f seconds\n", (__end.tv_sec - __start.tv_sec) * 1000.0 +
                            (__end.tv_nsec - __start.tv_nsec) / 1e9);

        clock_gettime(CLOCK_MONOTONIC, &__start);
        char asm_command[100];
        snprintf(asm_command, 100, "yasm -f macho64 %s -o %s", asm_output_name, obj_output_name); // TODO: nasm is much more cross-platform, so maybe for the sake of being cross-platform we could use nasm, but yasm is way better for quickly compiling macho64
        system(asm_command);
        clock_gettime(CLOCK_MONOTONIC, &__end);
        fprintf(stderr, "[INFO] Assembler took %.4f seconds\n", (__end.tv_sec - __start.tv_sec) * 1000.0 +
                            (__end.tv_nsec - __start.tv_nsec) / 1e9);

        clock_gettime(CLOCK_MONOTONIC, &__start);
        char ld_command[200];
        snprintf(ld_command, 200, "ld -macosx_version_min 13.00 -o %s %s %s %s", exec_output_name, obj_output_name, (_BirdSharpTypes == TYPE_STATIC) ? "-static" : "-L/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/lib -lSystem", link);
        system(ld_command);
        clock_gettime(CLOCK_MONOTONIC, &__end);
        fprintf(stderr, "[INFO] Linking process took %.4f seconds\n", (__end.tv_sec - __start.tv_sec) * 1000.0 +
                            (__end.tv_nsec - __start.tv_nsec) / 1e9);
    }
    fprintf(stderr, "[INFO] Program took %.4f seconds\n", (__end.tv_sec - __begin.tv_sec) * 1000.0 +
                        (__end.tv_nsec - __begin.tv_nsec) / 1e9);
    
    return 0;
}
