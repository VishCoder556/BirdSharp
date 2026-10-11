/* Programming Language Development
 *
 * -- BIRDSHARP --
 *
 *
*/

char *BIRDSHARP_VERSION = "BirdSharp pre-release";

#define STB_LANG_ERROR_IMPLEMENTATION


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ARENA_IMPLEMENTATION
#include "../libraries/arena/arena.h"

#include "../libraries/error/error.h"
#include "../libraries/tokenizer/tokenizer.h"
#include "../libraries/preprocessor/preprocessor.h"
#include "../libraries/parser/parser.h"
#include "../libraries/typeinfo/typeinfo.h"
#include "../libraries/ir/ir.h"
#include "../libraries/codegen/codegen.h"
#include "../libraries/driver/driver.h"
#include "../libraries/regalloc/regalloc.h"
#include "../libraries/optimizer/optimizer.h"



dymarray_typenew(char, 300, 40); // For codegen

char *HELP = "BirdSharp HELP Manual\n\t"
"-help: creates this page\n";



#define CUR_TOKENIZER_NAME Lang_Tokenizer
#define CUR_TOKENIZER_PREFIX lang_tokenizer

STB_LANG_NEW_TOKENIZER(
    STB_LANG_TOKENS(
        TOKEN_LP,
        TOKEN_RP,
        TOKEN_LB,
        TOKEN_RB,
        TOKEN_LSB,
        TOKEN_RSB,
        TOKEN_ID,
        TOKEN_NUM,
        TOKEN_COMMA,
        TOKEN_EQ,
        TOKEN_ADD,
        TOKEN_SUB,
        TOKEN_MUL,
        TOKEN_DIV,
        TOKEN_MODULO,
        TOKEN_STRING,
        TOKEN_GT,
        TOKEN_GTE,
        TOKEN_LT,
        TOKEN_LTE,
        TOKEN_DEQ,
        TOKEN_NEQ,
        TOKEN_NOT,
        TOKEN_AND,
        TOKEN_BAND,
        TOKEN_OR,
        TOKEN_BOR,
        TOKEN_CARET,
        TOKEN_DOT,
        TOKEN_HASH,
        TOKEN_BSHR,
        TOKEN_BSHL,
        TOKEN_DOLLAR
    ),
    STB_LANG_TOKEN_REPR(
        case TOKEN_LP: return "(";
        case TOKEN_RP: return ")";
        case TOKEN_LB: return "{";
        case TOKEN_RB: return "}";
        case TOKEN_LSB: return "[";
        case TOKEN_RSB: return "]";
        case TOKEN_ID: return "identifier";
        case TOKEN_NUM: return "number";
        case TOKEN_COMMA: return ",";
        case TOKEN_EQ: return "=";
        case TOKEN_ADD: return "+";
        case TOKEN_SUB: return "-";
        case TOKEN_MUL: return "*";
        case TOKEN_DIV: return "/";
        case TOKEN_MODULO: return "%";
        case TOKEN_STRING: return "string";
        case TOKEN_GT: return ">";
        case TOKEN_GTE: return ">=";
        case TOKEN_LT: return "<";
        case TOKEN_LTE: return "<=";
        case TOKEN_DEQ: return "==";
        case TOKEN_NEQ: return "!=";
        case TOKEN_NOT: return "!";
        case TOKEN_AND: return "&&";
        case TOKEN_BAND: return "&";
        case TOKEN_OR: return "||";
        case TOKEN_BOR: return "|";
        case TOKEN_CARET: return "^";
        case TOKEN_DOT: return ".";
        case TOKEN_HASH: return "#";
        case TOKEN_BSHR: return ">>";
        case TOKEN_BSHL: return "<<";
        case TOKEN_DOLLAR: return "$";
    ),
    STB_LANG_SIMPLE_CASES(
        STB_LANG_TOKEN_CHAR('(', TOKEN_LP)
        STB_LANG_TOKEN_CHAR(')', TOKEN_RP)
        STB_LANG_TOKEN_CHAR('{', TOKEN_LB)
        STB_LANG_TOKEN_CHAR('}', TOKEN_RB)
        STB_LANG_TOKEN_CHAR('[', TOKEN_LSB)
        STB_LANG_TOKEN_CHAR(']', TOKEN_RSB)
        STB_LANG_TOKEN_CHAR(',', TOKEN_COMMA)
        STB_LANG_TOKEN_CHAR('+', TOKEN_ADD)
        STB_LANG_TOKEN_CHAR('-', TOKEN_SUB)
        STB_LANG_TOKEN_CHAR('*', TOKEN_MUL)
        STB_LANG_TOKEN_CHAR('/', TOKEN_DIV)
        STB_LANG_TOKEN_CHAR('%', TOKEN_MODULO)
        STB_LANG_TOKEN_CHAR('^', TOKEN_CARET)
        STB_LANG_TOKEN_CHAR('.', TOKEN_DOT)
        STB_LANG_TOKEN_CHAR('#', TOKEN_HASH)
        STB_LANG_TOKEN_CHAR('$', TOKEN_DOLLAR)
        STB_LANG_SKIP('\n')
    ),
    STB_LANG_ALPHA(TOKEN_ID)
    STB_LANG_NUM(TOKEN_NUM)
    STB_LANG_STRING('"', TOKEN_STRING)
    STB_LANG_TOKEN_DOUBLE_CHAR_IF("<<", TOKEN_BSHL)
    STB_LANG_TOKEN_DOUBLE_CHAR_IF(">>", TOKEN_BSHR)
    STB_LANG_TOKEN_DOUBLE_CHAR(">=", TOKEN_GTE, TOKEN_GT)
    STB_LANG_TOKEN_DOUBLE_CHAR("<=", TOKEN_LTE, TOKEN_LT)
    STB_LANG_TOKEN_DOUBLE_CHAR("==", TOKEN_DEQ, TOKEN_EQ)
    STB_LANG_TOKEN_DOUBLE_CHAR("!=", TOKEN_NEQ, TOKEN_NOT)
    STB_LANG_TOKEN_DOUBLE_CHAR("&&", TOKEN_AND, TOKEN_BAND)
    STB_LANG_TOKEN_DOUBLE_CHAR("||", TOKEN_OR, TOKEN_BOR)
    
    STB_LANG_TOKEN_COMMENT_LINE("//")
    STB_LANG_SKIP(' ')
)


#define CUR_PREPROCESSOR_NAME Lang_Preprocessor
#define CUR_PREPROCESSOR_PREFIX lang_preprocessor
STB_LANG_NEW_PREPROCESSOR(

STB_LANG_PREPROCESSOR_PROCESS(
    if (token.type == TOKEN_HASH){
        STB_LANG_SAVE(oldcursor, processor->cursor);
        STB_LANG_PROCESSOR_ADVANCE();
        if (token.type == TOKEN_NOT){
            STB_LANG_PROCESSOR_ADVANCE();
            if (token.type == TOKEN_ID){
                if (strcmp(token.value, "include") == 0){
                    STB_LANG_PROCESSOR_ADVANCE();
                    if (token.type == TOKEN_STRING){
                STB_LANG_SAVE(name, token.value);
                        STB_LANG_PROCESSOR_TRIM(oldcursor, processor->cursor+1);
                        processor->cursor = oldcursor;
                        STB_LANG_PROCESSOR_UPDATE();

                        Lang_Tokenizer *_tokenizer = lang_tokenizer_init(lang_tokenizer_file_init(name));
                        while (lang_tokenizer_token(_tokenizer) == 0){
                        }
                        Lang_Preprocessor *_processor = lang_preprocessor_init(_tokenizer, processor->fl+1);
                        while (lang_preprocessor_token(_processor) == 0){
                        }
                        STB_LANG_PROCESSOR_INSERT(oldcursor, _processor);

                        free(_tokenizer);
                        free(_processor);

                    };
                }
            }
        }else {
            stb_lang_error_minor(processor->file.name, processor->file.contents, token.offset, "SyntaxError", "Unexpected token after `#`");
        }
    }
)
)


typedef struct {
    char *name;
    void *ast; // Pointer to the ast to which the label points to
}Lang_Parser_IR_Label;
typedef struct {
    LinkedList(Lang_Parser_IR_Label);
}Lang_Parser_IR_Labels;

#define String char*
dymarray_typenew(String, 10, 1);

#define CUR_TYPEINFO_NAME Lang_TypeInfo
#define CUR_TYPEINFO_PREFIX lang_typeinfo

#define STB_LANG_PARSER_PARSE_MODE() \
while (1){ \
    if (dot == 0){ \
        break; \
    } \
    if (token.type == TOKEN_ID){ \
        strncat(data, token.value, strlen(token.value)); \
        if (dot == 1){dot = 0;} \
        STB_LANG_PARSER_ADVANCE(); \
    } \
    if (token.type == TOKEN_DOT && dot == 0){ \
        strncat(data, ".", 1); \
        dot = 1; \
        STB_LANG_PARSER_ADVANCE(); \
\
        if (parser->cursor + 1 >= parser->tokens.datalen){ \
            break; \
        } \
    }else { \
        if (parser->cursor + 1 >= parser->tokens.datalen){ \
            break; \
        } \
        break; \
    }; \
};

#define STB_LANG_PARSER_MODE() \
STB_LANG_PARSER_ADVANCE(); \
STB_LANG_PARSER_EXPECT(TOKEN_NOT); \
char data[150]; \
strncpy(data, "", 150); \
int dot = 1; \
STB_LANG_PARSER_PARSE_MODE(); \
if (strcmp(data, "scope.flat") == 0){ \
    parser->scope_flat = 1; \
}else if (strcmp(data, "scope.structured") == 0){ \
    parser->scope_flat = 0; \
}else if (strcmp(data, "ir.inline") == 0){ \
    STB_LANG_PARSER_EXPECT(TOKEN_LB); \
    Lang_Parser_ASTList list = (Lang_Parser_ASTList){0}; \
    InitLinkedList(list, Lang_Parser_AST); \
    while (token.type != TOKEN_RB){ \
        Lang_Parser_AST *ast = parser_parse_ir_inline(parser); \
        if (ast != NULL) AppendToLinkedList(list, Lang_Parser_AST, *ast); \
        STB_LANG_PARSER_UPDATE(); \
    }; \
    STB_LANG_PARSER_EXPECT(TOKEN_RB); \
    return STB_LANG_AST(.type=AST_IR_LIST, .typeinfo={.type=-1, .ptrnum=-1}, .value=arena_strdup(&g_arena, data), .left=STB_LANG_LINKED_LIST(list), .right=NULL); \
}else if (strcmp(data, "linker.framework") == 0){ \
    if (token.type == TOKEN_STRING){ \
         dymarray_String_add(&linker_data.frameworks, token.value); \
    }else { \
        STB_LANG_PARSER_ERROR_MINOR(token.offset, token.file, "InlineIRError", "Unexpected argument to mode \"linker.framework\""); \
    }; \
    STB_LANG_PARSER_ADVANCE(); \
}else if (strcmp(data, "linker.library") == 0){ \
    if (token.type == TOKEN_STRING){ \
         dymarray_String_add(&linker_data.libraries, token.value); \
    }else { \
        STB_LANG_PARSER_ERROR_MINOR(token.offset, token.file, "InlineIRError", "Unexpected argument to mode \"linker.library\""); \
    }; \
    STB_LANG_PARSER_ADVANCE(); \
}else if (strcmp(data, "linker.libpath") == 0){ \
    if (token.type == TOKEN_STRING){ \
         dymarray_String_add(&linker_data.libpaths, token.value); \
    }else { \
        STB_LANG_PARSER_ERROR_MINOR(token.offset, token.file, "InlineIRError", "Unexpected argument to mode \"linker.library\""); \
    }; \
    STB_LANG_PARSER_ADVANCE(); \
}else if (strcmp(data, "import") == 0){ \
    if (token.type == TOKEN_ID){ \
        char *str = malloc(200); \
        strncpy(str, "", 200); \
        int grace = 1; \
        while (token.type == TOKEN_ID && grace == 1){ \
            strcat(str, token.value); \
            STB_LANG_PARSER_ADVANCE(); \
            if (token.type == TOKEN_DOT){ \
                strcat(str, "."); \
                grace = 1; \
                STB_LANG_PARSER_ADVANCE(); \
                continue; \
            } \
            if (grace == 1) grace = 0; \
        } \
         dymarray_String_add(&linker_data.modules, str); \
    }else { \
        STB_LANG_PARSER_ERROR_MINOR(token.offset, token.file, "InlineIRError", "Unexpected argument to mode \"import\" (expected identifier)"); \
    }; \
}else if (strcmp(data, "export.global") == 0){ \
    if (token.type == TOKEN_ID){ \
         dymarray_String_add(&linker_data.exports, token.value); \
    }else { \
        STB_LANG_PARSER_ERROR_MINOR(token.offset, token.file, "InlineIRError", "Unexpected argument to mode \"export.global\" (expected identifier)"); \
    }; \
    STB_LANG_PARSER_ADVANCE(); \
}else if (strcmp(data, "export.structure") == 0){ \
    if (token.type == TOKEN_ID){ \
         dymarray_String_add(&linker_data.structexports, token.value); \
    }else { \
        STB_LANG_PARSER_ERROR_MINOR(token.offset, token.file, "InlineIRError", "Unexpected argument to mode \"export.structure\" (expected identifier)"); \
    }; \
    STB_LANG_PARSER_ADVANCE(); \
}else if (strcmp(data, "if") == 0){ \
    char not = 0; \
    if (token.type == TOKEN_NOT){ \
        not = 1; \
        STB_LANG_PARSER_ADVANCE(); \
    }; \
    if (token.type == TOKEN_ID){ \
        strncpy(data, "", 150); \
        dot = 1; \
        STB_LANG_PARSER_PARSE_MODE(); \
        STB_LANG_PARSE_STATEMENT_LIST(stmnts, TOKEN_LB, -1, TOKEN_RB); \
        return STB_LANG_AST(.type=AST_MODE_IF, .typeinfo={.type=-1, .ptrnum=-1}, .value=arena_strdup(&g_arena, data), .left=(void*)(long)(int)not, .right=STB_LANG_LINKED_LIST(stmnts)); \
    }else { \
        STB_LANG_PARSER_ERROR_MINOR(token.offset, token.file, "InlineIRError", "Unexpected argument to mode \"if\" (expected identifier)"); \
    }; \
} \
return STB_LANG_AST(.type=AST_MODE, .typeinfo={.type=-1, .ptrnum=-1}, .value=arena_strdup(&g_arena, data), .left=NULL, .right=NULL);


#define CUR_PARSER_NAME Lang_Parser
#define CUR_PARSER_PREFIX lang_parser


STB_LANG_DEFINE_TYPEINFO(
    AST_TYPE_VOID,
    AST_TYPE_INT,
    AST_TYPE_I32,
    AST_TYPE_I64,
    AST_TYPE_CHAR,
    AST_TYPE_STRING,
    AST_TYPE_ARRAY,
    AST_TYPE_STRUCT,
    AST_TYPE_FLOAT
)

typedef struct {
    dymarray_String frameworks;
    dymarray_String libraries;
    dymarray_String libpaths;
    dymarray_String modules;
    dymarray_String exports;
    dymarray_String structexports;
}Lang_LinkerData;
Lang_LinkerData linker_data = (Lang_LinkerData){0};



STB_LANG_NEW_PARSER(
STB_LANG_BINDING_POWER(
    STB_LANG_MATCH_BINDING_POWER(TOKEN_OR, 1)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_AND, 2)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_BOR, 3)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_CARET, 4)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_BAND, 5)


    STB_LANG_MATCH_BINDING_POWER(TOKEN_LT, 6)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_LTE, 6)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_GT, 6)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_GTE, 6)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_DEQ, 6)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_NEQ, 6)

    STB_LANG_MATCH_BINDING_POWER(TOKEN_BSHL, 8)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_BSHR, 8)


    STB_LANG_MATCH_BINDING_POWER(TOKEN_ADD, 10)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_SUB, 10)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_MUL, 20)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_DIV, 20)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_MODULO, 20)


    STB_LANG_MATCH_BINDING_POWER(TOKEN_LSB, 30)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_DOT, 30)
),
STB_LANG_ASTS(
    AST_FUNCDEF,
    AST_FUNCDECL,
    AST_VAR,
    AST_IR_TEMP,
    AST_INT,
    AST_ASSIGN,
    AST_DECL,
    AST_FUNCALL,
    AST_ADD,
    AST_SUB,
    AST_MUL,
    AST_DIV,
    AST_MODULO,
    AST_IF,
    AST_WHILE,
    AST_LT,
    AST_LTE,
    AST_GT,
    AST_GTE,
    AST_EQ,
    AST_NEQ,
    AST_RET,
    AST_STRING,
    AST_CAST,
    AST_OR,
    AST_AND,
    AST_BOR,
    AST_BAND,
    AST_XOR,
    AST_EXPR,
    AST_REF,
    AST_DEREF,
    AST_STORE,
    AST_INDEX,
    AST_STRUCT,
    AST_ACCESS,
    AST_MODE,
    AST_BSHL,
    AST_BSHR,
    AST_IR_INSTRUCTION,
    AST_IR_LIST,
    AST_FLOAT,
    AST_SIZEOF,
    AST_MODE_IF,
    AST_ASSERT
),
STB_LANG_PARSER_FIELDS(
    int scope_flat;
    Lang_Parser_ASTList flat_scope;
    char *funcname;
),
STB_LANG_PARSER_INIT(
    parser->scope_flat = 0;
    InitLinkedList(parser->flat_scope, Lang_Parser_AST);
    parser->funcname = NULL;
),
STB_LANG_PARSER_SUFFIX(
    if (GetLinkedListHead(parser->flat_scope, Lang_Parser_AST) != NULL){
        if (GetLinkedListLen(parser->flat_scope, Lang_Parser_AST) > 0){
            Lang_Parser_AST *ast = STB_LANG_AST(.type=AST_FUNCDEF, .typeinfo={.type=-1, .ptrnum=-1}, .value="main", .left=NULL, .right=STB_LANG_LINKED_LIST(parser->flat_scope));
            parser->funcname = "main";
            AppendToLinkedList((*parser), STB_CONCAT(CUR_PARSER_NAME, _AST), *ast);
        }
    }
),
STB_LANG_PARSER_FUNCS(
Lang_Parser_AST *parser_parse_ir_inline(Lang_Parser *parser){
    Lang_Tokenizer_Token token = parser->tokens.data[parser->cursor];
    int offset = token.offset;
    int file = token.file;

    char *instr = token.value;


    if (token.type == TOKEN_ID){
        if (strcmp(instr, "mov") == 0){
            STB_LANG_PARSER_ADVANCE();

            STB_LANG_GET_AST_EXPR(a, 10);

            STB_LANG_PARSER_EXPECT(TOKEN_COMMA);

            STB_LANG_GET_AST_EXPR(b, 10);

            return STB_LANG_AST(.type=AST_IR_INSTRUCTION, .typeinfo={.type=-1, .ptrnum=-1}, .value=instr, .left=STB_LANG_AS_AST(a), .right=STB_LANG_AS_AST(b));
        }else if (strcmp(instr, "add") == 0 || strcmp(instr, "sub") == 0 || strcmp(instr, "mul") == 0 || strcmp(instr, "div") == 0 || strcmp(instr, "mod") == 0|| strcmp(instr, "setlt") == 0 || strcmp(instr, "setle") == 0 || strcmp(instr, "setgt") == 0 || strcmp(instr, "setge") == 0 || strcmp(instr, "seteq") == 0 || strcmp(instr, "setneq") == 0 || strcmp(instr, "bor") == 0 || strcmp(instr, "band") == 0 || strcmp(instr, "and") == 0 || strcmp(instr, "or") == 0 || strcmp(instr, "xor") == 0 || strcmp(instr, "bshl") == 0 || strcmp(instr, "bshr") == 0){

            STB_LANG_GET_AST_EXPR(a, 10);

            STB_LANG_PARSER_EXPECT(TOKEN_COMMA);

            STB_LANG_GET_AST_EXPR(b, 10);
            STB_LANG_PARSER_EXPECT(TOKEN_COMMA);

            STB_LANG_GET_AST_EXPR(c, 10);

            Lang_Parser_ASTList list = (Lang_Parser_ASTList){0};
            InitLinkedList(list, Lang_Parser_AST);
            AppendToLinkedList(list, Lang_Parser_AST, *a);
            AppendToLinkedList(list, Lang_Parser_AST, *b);

            if (token.type == TOKEN_COMMA){
                STB_LANG_PARSER_ERROR_MINOR(token.offset, token.file, "InlineIRError", "Should not find commas after `%s` IR instruction", instr);
            }

            return STB_LANG_AST(.type=AST_IR_INSTRUCTION, .typeinfo={.type=-1, .ptrnum=-1}, .value=instr, .left=STB_LANG_LINKED_LIST(list), .right=STB_LANG_AS_AST(c));
        }else if (strcmp(instr, "addr") == 0 || strcmp(instr, "load") == 0 || strcmp(instr, "store") == 0){

            STB_LANG_PARSER_ADVANCE();


            STB_LANG_GET_AST_EXPR(a, 10);

            STB_LANG_PARSER_EXPECT(TOKEN_COMMA);

            STB_LANG_GET_AST_EXPR(b, 10);

            if (token.type == TOKEN_COMMA){
                STB_LANG_PARSER_ERROR_MINOR(token.offset, token.file, "InlineIRError", "Should not find commas after `%s` IR instruction", instr);
            }

            return STB_LANG_AST(.type=AST_IR_INSTRUCTION, .typeinfo={.type=-1, .ptrnum=-1}, .value=instr, .left=STB_LANG_AS_AST(a), .right=STB_LANG_AS_AST(b));
        }else if (strcmp(instr, "call") == 0){
            STB_LANG_PARSER_ADVANCE();
            char *name = token.value;
            STB_LANG_PARSER_EXPECT(TOKEN_ID);



            Lang_Parser_ASTList list = (Lang_Parser_ASTList){0};
            InitLinkedList(list, Lang_Parser_AST);
            STB_LANG_PARSER_EXPECT(TOKEN_LP);
            while (token.type != TOKEN_RP){
                STB_LANG_GET_AST_EXPR(a, 10);
                AppendToLinkedList(list, Lang_Parser_AST, *a);
                if (token.type != TOKEN_RP){
                    STB_LANG_PARSER_EXPECT(TOKEN_COMMA);
                };
            }
            STB_LANG_PARSER_EXPECT(TOKEN_RP);
            return STB_LANG_AST(.type=AST_IR_INSTRUCTION, .typeinfo={.type=-1, .ptrnum=-1}, .value=instr, .left=STB_LANG_LINKED_LIST(list), .right=STB_LANG_AS_AST(name));
            // STB_LANG_GET_AST_EXPR(a, 10);
        }else if (strcmp(instr, "ret") == 0){
            STB_LANG_PARSER_ADVANCE();
            STB_LANG_GET_AST_EXPR(a, 10);
            return STB_LANG_AST(.type=AST_IR_INSTRUCTION, .typeinfo={.type=-1, .ptrnum=-1}, .value=instr, .left=STB_LANG_AS_AST(a), .right=NULL);
        }else if (strcmp(instr, "syscall3") == 0){
            STB_LANG_PARSER_ADVANCE();
            STB_LANG_SAVE(tknval, token.value);
            STB_LANG_PARSER_EXPECT(TOKEN_NUM);
            return STB_LANG_AST(.type=AST_IR_INSTRUCTION, .typeinfo={.type=-1, .ptrnum=-1}, .value=instr, .left=(void*)tknval, .right=NULL);
        }

    }else if (token.type == TOKEN_NOT){
        STB_LANG_PARSER_ADVANCE();

        if (token.type == TOKEN_ID){
            if (strcmp(token.value, "label") == 0){
                STB_LANG_PARSER_ADVANCE();
                char *lblname = token.value;
                (void)lblname;
                if (token.type != TOKEN_ID){
                    STB_LANG_PARSER_ERROR_MINOR(token.offset, token.file, "InlineIRError", "Expected label name to be identifier");
                    return NULL;
                }

                STB_LANG_PARSER_ADVANCE();
                // printf("%s\n", lblname);
            }else {
                goto err;
            }
        }else {
            goto err;
        }
    }else {
    err:
        STB_LANG_PARSER_ERROR_MINOR(offset, file, "InlineIRError", "Could not parse IR instruction");
        return NULL;
    }

    return NULL;
}
),
STB_LANG_PARSE_BODY(
    STB_LANG_IF_TOKEN(TOKEN_HASH,
        if (parser->scope_flat == 0){
            STB_LANG_PARSER_MODE();
        } 
        // Otherwise, leave it to AST level in flat mode so that it's picked into the entry point
    )

    STB_LANG_IF_VALUE(TOKEN_ID, "struct",
        STB_LANG_PARSER_ADVANCE();
        STB_LANG_SAVE(struct_name, token);
        STB_LANG_PARSER_ADVANCE();
        STB_CONCAT(CUR_PARSER_NAME, _ASTList) fields = (STB_CONCAT(CUR_PARSER_NAME, _ASTList)){0};
        InitLinkedList(fields, STB_CONCAT(CUR_PARSER_NAME, _AST));
        if (token.type == TOKEN_LB){
        STB_LANG_PARSE_CUSTOM_LIST(TOKEN_LB, -1, TOKEN_RB,
            STB_LANG_GET_TYPEINFO(argt){
                STB_LANG_IF_TOKEN(TOKEN_ID,
                    STB_CONCAT(CUR_PARSER_NAME, _AST) ast = (STB_CONCAT(CUR_PARSER_NAME, _AST)){.type=AST_VAR, .typeinfo=argt, .value=match_token.value, .offset=offset, .left=(void*)1};

                    AppendToLinkedList(fields, STB_CONCAT(CUR_PARSER_NAME, _AST), ast);
                )
                STB_LANG_PARSER_ADVANCE();
            }
        )
        

        Lang_TypeInfo_Typeinfo typeinfo = STB_LANG_TYPEINFO(.type=AST_TYPE_STRUCT, .ptrnum=0);
        typeinfo.data.struct1.name = struct_name.value;
        typeinfo.data.struct1.symbol = NULL;
        typeinfo.data.struct1.size = -1;
        return STB_LANG_AST(.type=AST_STRUCT, .typeinfo=typeinfo, .value=struct_name.value, .left=STB_LANG_LINKED_LIST(fields), .right=NULL);
        }else {
            STB_LANG_PARSER_BACK();
            STB_LANG_PARSER_BACK(); // Undo cursor to initial stage
        }
    )
    STB_LANG_GET_TYPEINFO(typeinfo){

        STB_LANG_IF_TOKEN(TOKEN_ID, // Function name
            STB_LANG_PARSER_ADVANCE();
            STB_LANG_SAVE(func_name, match_token);

            STB_CONCAT(CUR_PARSER_NAME, _ASTList) params = (STB_CONCAT(CUR_PARSER_NAME, _ASTList)){0};
            InitLinkedList(params, STB_CONCAT(CUR_PARSER_NAME, _AST));
        if (token.type == TOKEN_LP){
            int depth = 0;
            int oldcursor = parser->cursor;
            STB_LANG_PARSE_CUSTOM_LIST(TOKEN_LP, TOKEN_COMMA, TOKEN_RP,
                if (depth >= 100 && oldcursor == parser->cursor){
                    STB_LANG_PARSER_ERROR_MINOR(token.offset, token.file, "FunctionError", "Could not parse parameter of function");
                }
                depth++; oldcursor = parser->cursor;
                STB_LANG_GET_TYPEINFO(argt){
                    STB_LANG_IF_TOKEN(TOKEN_ID,
                        STB_CONCAT(CUR_PARSER_NAME, _AST) ast = (STB_CONCAT(CUR_PARSER_NAME, _AST)){.type=AST_VAR, .typeinfo=argt, .value=match_token.value, .offset=offset};
                        AppendToLinkedList(params, STB_CONCAT(CUR_PARSER_NAME, _AST), ast);
                    )
                    STB_LANG_PARSER_ADVANCE();
                }else {
                    STB_LANG_IF_TOKEN(TOKEN_DOT,
                        STB_LANG_PARSER_ADVANCE()
                        STB_LANG_IF_TOKEN(TOKEN_DOT,
                            STB_LANG_PARSER_ADVANCE()
                            STB_LANG_IF_TOKEN(TOKEN_DOT,
                                STB_LANG_PARSER_ADVANCE()

                                STB_CONCAT(CUR_PARSER_NAME, _AST) ast = (STB_CONCAT(CUR_PARSER_NAME, _AST)){.type=STB_LANG_AST_NONE, .typeinfo={.type = STB_LANG_TYPEINFO_VARIADIC, .ptrnum=-1}, .value=NULL, .offset=match_token.offset};
                                AppendToLinkedList(params, STB_CONCAT(CUR_PARSER_NAME, _AST), ast);
                            )
                        )
                    )
                }
            )


        }else {
            parser->cursor = initial_cursor;
            STB_LANG_PARSER_UPDATE();
            goto _exit;
        }

            if (token.type == TOKEN_LB) {
                parser->funcname = "main";
                STB_LANG_PARSE_STATEMENT_LIST(stmnts, TOKEN_LB, -1, TOKEN_RB)
                return STB_LANG_AST(.type=AST_FUNCDEF, .typeinfo=typeinfo, .value=func_name.value, .left=STB_LANG_LINKED_LIST(params), .right=STB_LANG_LINKED_LIST(stmnts));
            }else {
                return STB_LANG_AST(.type=AST_FUNCDECL, .typeinfo=typeinfo, .value=func_name.value, .left=STB_LANG_LINKED_LIST(params), .right=NULL);
            }
        )

    }
    _exit:
        if (parser->scope_flat == 1){
            AppendToLinkedList(parser->flat_scope, Lang_Parser_AST, *STB_LANG_PARSE_STATEMENT());
            STB_LANG_PARSER_UPDATE();
            return STB_LANG_PARSE_TOP();
        }

),
STB_LANG_PARSE_AST(
    STB_LANG_IF_TOKEN(TOKEN_HASH,
        STB_LANG_PARSER_MODE();
    )

    STB_LANG_IF_VALUE(TOKEN_ID, "if", 
        STB_LANG_PARSER_ADVANCE();
        STB_LANG_PARSER_EXPECT(TOKEN_LP)
        STB_LANG_OPERAND(ifexpr, lang_parser_parse_expr(parser, 0));
        STB_LANG_PARSER_EXPECT(TOKEN_RP)
        STB_LANG_PARSE_STATEMENT_LIST(stmnts, TOKEN_LB, -1, TOKEN_RB)

        Lang_Parser_AST *ast = STB_LANG_AST(.type=AST_IF, .value=NULL, .left=STB_LANG_AS_AST(ifexpr), .middle=NULL, .right=STB_LANG_LINKED_LIST(stmnts));

        STB_LANG_IF_VALUE(TOKEN_ID, "else",
            STB_LANG_PARSER_ADVANCE();
            STB_LANG_PARSE_STATEMENT_LIST(else1, TOKEN_LB, -1, TOKEN_RB)
            ast->middle = STB_LANG_LINKED_LIST(else1);
        )
        return ast;
    )
    STB_LANG_IF_VALUE(TOKEN_ID, "while", 
        STB_LANG_PARSER_ADVANCE();
        STB_LANG_PARSER_EXPECT(TOKEN_LP)
        STB_LANG_OPERAND(ifexpr, lang_parser_parse_expr(parser, 0));
        STB_LANG_PARSER_EXPECT(TOKEN_RP)
        STB_LANG_PARSE_STATEMENT_LIST(stmnts, TOKEN_LB, -1, TOKEN_RB)
        return STB_LANG_AST(.type=AST_WHILE, .value=NULL, .left=STB_LANG_AS_AST(ifexpr), .middle=NULL, .right=STB_LANG_LINKED_LIST(stmnts));
    )
    STB_LANG_IF_VALUE(TOKEN_ID, "return", 
        STB_LANG_PARSER_ADVANCE();
        STB_LANG_OPERAND(expr, lang_parser_parse_expr(parser, 0));
        return STB_LANG_AST(.type=AST_RET, .value=NULL, .left=STB_LANG_AS_AST(expr), .middle=NULL, .right=NULL);
    )
    STB_LANG_IF_TOKEN(TOKEN_DOLLAR,
        STB_LANG_PARSER_ADVANCE();
        STB_LANG_IF_VALUE(TOKEN_ID, "assert", 
            STB_LANG_PARSER_ADVANCE();
            STB_LANG_PARSER_EXPECT(TOKEN_LP);
            STB_LANG_SAVE(err, token.value);
            STB_LANG_PARSER_EXPECT(TOKEN_STRING);
            STB_LANG_PARSER_EXPECT(TOKEN_RP);

            return STB_LANG_AST(.type=AST_ASSERT, .value=err, .left=NULL, .middle=NULL, .right=NULL);
        )
    )
    STB_LANG_GET_TYPEINFO(typeinfo){
        goto assign_decl_end;
    maybe_assign:
        if (token.value != NULL){
            if (strcmp(token.value, "deref") == 0){goto not_funcall;}
        }
        STB_LANG_SAVE(varia, token)
        STB_LANG_PARSER_ADVANCE()
        STB_LANG_IF_TOKEN(TOKEN_LP,
            STB_LANG_PARSE_EXPR_LIST(args, TOKEN_LP, TOKEN_COMMA, TOKEN_RP);
            return STB_LANG_AST_FUNCALL(AST_FUNCALL, varia, args)
        ) STB_LANG_ELSE (
            STB_LANG_PARSER_BACK();
not_funcall:
            ;
            STB_LANG_GET_AST_EXPR(lhs, 0);
            STB_LANG_IF_TOKEN(TOKEN_EQ,
                STB_LANG_PARSER_ADVANCE();
                STB_LANG_OPERAND(assign, lang_parser_parse_expr(parser, 0));
                if (lhs->type == AST_INDEX || lhs->type == AST_ACCESS || lhs->type == AST_DEREF){
                    return STB_LANG_AST(.type=AST_STORE, .typeinfo=typeinfo, .value = NULL, .left=STB_LANG_AS_AST(lhs), .middle=NULL, .right=STB_LANG_AS_AST(assign));
                }
                return STB_LANG_AST(.type=AST_ASSIGN, .typeinfo=typeinfo, .value = NULL, .left=STB_LANG_AS_AST(lhs), .middle=NULL, .right=STB_LANG_AS_AST(assign));
            )
        );
    assign_decl_end:
        STB_LANG_IF_TOKEN(TOKEN_ID, 
            STB_LANG_PARSER_ADVANCE()
            STB_LANG_SAVE(varia, match_token)
            STB_LANG_IF_TOKEN(TOKEN_EQ,
                STB_LANG_PARSER_ADVANCE();
                STB_LANG_OPERAND(assign, lang_parser_parse_expr(parser, 0));
                return STB_LANG_AST(.type=AST_DECL, .typeinfo=typeinfo, .value = varia.value, .left=NULL, .middle=NULL, .right=STB_LANG_AS_AST(assign));
            ) STB_LANG_ELSE(
                STB_LANG_IF_TOKEN(TOKEN_LSB,
                    STB_LANG_PARSER_ERROR_MINOR(token.offset, token.file, "SyntaxError", "Square bracket at wrong place in declaration (try `int[3] x`)"); \
                )
                if (typeinfo.type != -1){
                    return STB_LANG_AST(.type=AST_DECL, .typeinfo=typeinfo, .value = varia.value, .left=NULL, .middle=NULL, .right=NULL);
                }
            )
        )
    }else {
        goto maybe_assign;
    }
),
STB_LANG_PARSE_EXPR(
    STB_LANG_MATCH_TOKEN(TOKEN_DOLLAR,  
        STB_LANG_PARSER_ADVANCE();
        STB_LANG_SAVE(typid, token.value);
        left = STB_LANG_AST_LITERAL(AST_STRING, token);
        STB_LANG_PARSER_EXPECT(TOKEN_ID);
        if (strcmp(typid, "platform") == 0){
            left->value = arena_alloc(&g_arena, 100);

        #if defined(_WIN32)
            snprintf(left->value, 100, "Windows");
        #elif defined(_WIN64)
            snprintf(left->value, 100, "Windows");
        #elif defined(__APPLE__) || defined(__MACH__)
            snprintf(left->value, 100, "MacOS");
        #elif defined(__linux__)
            snprintf(left->value, 100, "Linux");
        #elif defined(__unix__) || defined(__unix)
            snprintf(left->value, 100, "POSIX");
        #else
            snprintf(left->value, 100, "Unknown Operating System");
        #endif
            goto skip;
        }else if (strcmp(typid, "version") == 0){
            left->value = BIRDSHARP_VERSION;
        }else if (strcmp(typid, "file") == 0){
            left->value = parser->files.data[match_token.file].name;
        }else if (strcmp(typid, "line") == 0){
            left->type = AST_INT;

            char *str = arena_alloc(&g_arena, 100);
            snprintf(str, 100, "%d", stb_lang_get_position(parser->files.data[match_token.file].contents, match_token.offset, NULL).row);
            left->value = str;
        }else if (strcmp(typid, "col") == 0){
            left->type = AST_INT;

            char *str = arena_alloc(&g_arena, 100);
            snprintf(str, 100, "%d", stb_lang_get_position(parser->files.data[match_token.file].contents, match_token.offset, NULL).col);
            left->value = str;
        }else if (strcmp(typid, "function") == 0){
            if (parser->funcname == NULL){
                left->value = "(unknown)";
            }else {
                left->value = parser->funcname;
            }
        }else {
            STB_LANG_PARSER_ERROR_MINOR(match_token.offset, match_token.file, "IntrinsicError", "Unknown Intrinsic");
        }
    )
    STB_LANG_MATCH_TOKEN(TOKEN_BAND,  
        STB_LANG_PARSER_ERROR_MINOR(token.offset, token.file, "SyntaxError", "Wrong operator for referencing (use `ref(x)` instead)");
    )
    STB_LANG_MATCH_TOKEN(TOKEN_MUL,  
        STB_LANG_PARSER_ERROR_MINOR(token.offset, token.file, "SyntaxError", "Wrong operator for dereferencing (use `deref(x)` instead)");
    )
    STB_LANG_MATCH_TOKEN(TOKEN_DOT,  
        STB_LANG_PARSER_ADVANCE();
        left = STB_LANG_AST_LITERAL(AST_IR_TEMP, token);
        STB_LANG_PARSER_ADVANCE();
        goto skip;
    )
    STB_LANG_MATCH_TOKEN(TOKEN_ID,  
        if (strcmp(match_token.value, "cast") == 0){
            STB_LANG_PARSER_ADVANCE();
            STB_LANG_PARSER_EXPECT(TOKEN_LP);
            STB_LANG_GET_TYPEINFO(typeinfo){
                STB_LANG_PARSER_EXPECT(TOKEN_COMMA);
                STB_LANG_GET_AST_EXPR(exp, 0);
                STB_LANG_PARSER_EXPECT(TOKEN_RP);
                left = STB_LANG_AST(.type = AST_CAST, .typeinfo = typeinfo, .left = STB_LANG_AS_AST(exp), .right=NULL, .middle=NULL);
                goto skip;
            }
        }

        if (strcmp(match_token.value, "ref") == 0){
            STB_LANG_PARSER_ADVANCE();
            STB_LANG_PARSER_EXPECT(TOKEN_LP);
            STB_LANG_GET_AST_EXPR(exp, 0);
            STB_LANG_PARSER_EXPECT(TOKEN_RP);
            left = STB_LANG_AST(.type = AST_REF, .left = STB_LANG_AS_AST(exp), .right=NULL, .middle=NULL);
            goto skip;
        }

        if (strcmp(match_token.value, "deref") == 0){
            STB_LANG_PARSER_ADVANCE();
            STB_LANG_PARSER_EXPECT(TOKEN_LP);
            STB_LANG_GET_AST_EXPR(exp, 0);
            STB_LANG_PARSER_EXPECT(TOKEN_RP);
            left = STB_LANG_AST(.type = AST_DEREF, .left = STB_LANG_AS_AST(exp), .right=NULL, .middle=NULL);
            goto skip;
        }
        if (strcmp(match_token.value, "sizeof") == 0){
            STB_LANG_PARSER_ADVANCE();
            STB_LANG_PARSER_EXPECT(TOKEN_LP);
            STB_LANG_GET_TYPEINFO(old){
            };
            STB_LANG_PARSER_EXPECT(TOKEN_RP);
            left = STB_LANG_AST(.type = AST_SIZEOF, .left = NULL, .right=NULL, .middle=NULL, .typeinfo=old);
            goto skip;
        }

        STB_LANG_SAVE(name, match_token)
        if (!STB_LANG_PARSER_IN_BOUNDS()){
            return STB_LANG_AST_LITERAL(AST_VAR, name);
        }
        STB_LANG_PARSER_ADVANCE();
        STB_LANG_IF_TOKEN(TOKEN_LP,
            STB_LANG_PARSE_EXPR_LIST(args, TOKEN_LP, TOKEN_COMMA, TOKEN_RP);
            left = STB_LANG_AST_FUNCALL(AST_FUNCALL, name, args)
        ) STB_LANG_ELSE(
            left = STB_LANG_AST_LITERAL(AST_VAR, name);
            goto skip;
        )
    )
    STB_LANG_MATCH_TOKEN(TOKEN_LP,  
        STB_LANG_PARSER_ADVANCE();
        STB_LANG_GET_AST_EXPR(l, 2);
        STB_LANG_PARSER_EXPECT(TOKEN_RP);
        left = STB_LANG_AST(.type = AST_EXPR, .left = STB_LANG_AS_AST(l))
    )
    STB_LANG_MATCH_TOKEN(TOKEN_NUM,  
        STB_LANG_SAVE(prev_t_token, token);
        STB_LANG_PARSER_ADVANCE();
        STB_LANG_IF_TOKEN(TOKEN_DOT, 
            STB_LANG_PARSER_ADVANCE();
        STB_LANG_IF_TOKEN(TOKEN_NUM, 
            int str2len = strlen(prev_t_token.value) + strlen(token.value) + 2;
            char *str2 = arena_alloc(&g_arena, str2len);
            snprintf(str2, str2len, "%s.%s", prev_t_token.value, token.value);
            free(prev_t_token.value);
            free(token.value);
            left = STB_LANG_AST(.type=AST_FLOAT, .typeinfo={.type=AST_TYPE_FLOAT, .ptrnum=0}, .value=str2, .left=NULL, .right=NULL);
            STB_LANG_PARSER_ADVANCE();
            goto skip;
        )
        )
        left = STB_LANG_AST_LITERAL(AST_INT, match_token);
        if (token.type == TOKEN_ID){
            if (token.value[0] == 'x'){
                int str2len = (int)strlen(token.value) * 8/5;
                char *str2 = arena_alloc(&g_arena, str2len);
                snprintf(str2, str2len, "%ld", strtol(token.value+1, NULL, 16));
                left->value = str2;
                STB_LANG_PARSER_ADVANCE();
            }
        }
    )
    STB_LANG_MATCH_TOKEN(TOKEN_STRING,  
        STB_LANG_PARSER_ADVANCE();
        left = STB_LANG_AST_LITERAL(AST_STRING, match_token);
    )
skip:
    STB_LANG_PRATT_PARSER(
        STB_LANG_TOKEN_MATCH_AST(TOKEN_ADD, AST_ADD)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_SUB, AST_SUB)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_MUL, AST_MUL)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_DIV, AST_DIV)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_MODULO, AST_MODULO)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_LT, AST_LT)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_LTE, AST_LTE)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_GT, AST_GT)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_GTE, AST_GTE)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_DEQ, AST_EQ)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_NEQ, AST_NEQ)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_OR, AST_OR)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_BOR, AST_BOR)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_AND, AST_AND)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_BAND, AST_BAND)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_CARET, AST_XOR)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_BSHL, AST_BSHL)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_BSHR, AST_BSHR)
        STB_LANG_TOKEN_MATCH_AST_CUSTOM(TOKEN_LSB, AST_INDEX,
            STB_LANG_PARSER_ADVANCE();
            STB_LANG_GET_AST_EXPR(new, 0);
            parent->right = STB_LANG_AS_AST(new);
            STB_LANG_PARSER_EXPECT(TOKEN_RSB);
        )
        STB_LANG_TOKEN_MATCH_AST_CUSTOM(TOKEN_DOT, AST_ACCESS,
            STB_LANG_PARSER_ADVANCE();
            parent->value = token.value;
            STB_LANG_PARSER_ADVANCE();
        )
    )
),
STB_LANG_PARSE_TYPEINFO(
    Lang_TypeInfo_Typeinfo typeinfo = (Lang_TypeInfo_Typeinfo){.type=STB_LANG_TYPEINFO_NONE, .ptrnum=0};
    STB_LANG_IF_VALUE(TOKEN_ID, "unsigned",
        STB_LANG_PARSER_ADVANCE();
    )
    STB_LANG_IF_VALUE(TOKEN_ID, "ptr",
        STB_LANG_PARSER_ADVANCE();
        STB_LANG_PARSER_EXPECT(TOKEN_LT);
        STB_LANG_GET_TYPEINFO(old){
            old.ptrnum++;
        };
        STB_LANG_PARSER_EXPECT(TOKEN_GT);
        return old;
    )
    STB_LANG_IF_VALUE(TOKEN_ID, "struct",
        typeinfo.type = AST_TYPE_STRUCT;
        STB_LANG_PARSER_ADVANCE();
        typeinfo.data.struct1.name = token.value;
        typeinfo.data.struct1.symbol = NULL;
        typeinfo.data.struct1.size = -1;
        STB_LANG_PARSER_ADVANCE();
        return typeinfo;
    )
    STB_LANG_IF_VALUE(TOKEN_ID, "void",
        typeinfo.type = AST_TYPE_VOID;
        STB_LANG_PARSER_ADVANCE();
    )
    STB_LANG_IF_VALUE(TOKEN_ID, "int",
        typeinfo.type = AST_TYPE_INT;
        STB_LANG_PARSER_ADVANCE();
    )
    STB_LANG_IF_VALUE(TOKEN_ID, "i32",
        typeinfo.type = AST_TYPE_I32;
        STB_LANG_PARSER_ADVANCE();
    )
    STB_LANG_IF_VALUE(TOKEN_ID, "i64",
        typeinfo.type = AST_TYPE_I64;
        STB_LANG_PARSER_ADVANCE();
    )
    STB_LANG_IF_VALUE(TOKEN_ID, "char",
        typeinfo.type = AST_TYPE_CHAR;
        STB_LANG_PARSER_ADVANCE();
    )
    STB_LANG_IF_VALUE(TOKEN_ID, "float",
        typeinfo.type = AST_TYPE_FLOAT;
        STB_LANG_PARSER_ADVANCE();
    )
    STB_LANG_IF_VALUE(TOKEN_ID, "string",
        typeinfo.type = AST_TYPE_STRING;
        STB_LANG_PARSER_ADVANCE();
    )
    STB_LANG_IF_TOKEN(TOKEN_LSB,
start_parse_bracket:
        STB_LANG_PARSER_EXPECT(TOKEN_LSB);
        STB_LANG_SAVE(num, token)
        STB_LANG_PARSER_EXPECT(TOKEN_NUM);
        Lang_TypeInfo_Typeinfo typeinf = (Lang_TypeInfo_Typeinfo){.type=AST_TYPE_ARRAY, .ptrnum=0};
        typeinf.data.array.size = atoi(num.value);
        typeinf.data.array.elem_type = arena_alloc(&g_arena, sizeof(Lang_TypeInfo_Typeinfo));
        *(Lang_TypeInfo_Typeinfo*)typeinf.data.array.elem_type = typeinfo;
        STB_LANG_PARSER_EXPECT(TOKEN_RSB);
        if (token.type == TOKEN_LSB){
            typeinfo = typeinf;
            goto start_parse_bracket;
        }
        return typeinf;
    )
    return typeinfo;
)
)


STB_LANG_TYPEINFO_SIZE(
    if (typeinfo.ptrnum > 0) return 8;
    switch(typeinfo.type){
        case AST_TYPE_VOID: return 0;
        case AST_TYPE_INT: return 8;
        case AST_TYPE_I32: return 4;
        case AST_TYPE_I64: return 8;
        case AST_TYPE_FLOAT: return 4;
        case AST_TYPE_CHAR: return 1;
        case AST_TYPE_STRING: return 8;
        case AST_TYPE_ARRAY: return STB_LANG_LOOKUP_SIZE(root_scope, typeinfo.data.array.elem_type) * typeinfo.data.array.size;
        case AST_TYPE_STRUCT: {
            STB_LANG_FIND_DATA(root_scope, typeinfo.data.struct1.name,
                int size = 0;
                Lang_Parser_AST *structdef = STB_LANG_LHS(STB_LANG_GET_AST(symnew.data.struct1.structdef));
                STB_LANG_ITERATE_LINKED_LIST(structdef, field, Lang_Parser_AST,
                    size += STB_LANG_LOOKUP_SIZE(root_scope, &field->typeinfo);
                )
                return size;
            );
        }
        default: stb_lang_error_major_global("TypeError", "Size of type '%d' is unknown", typeinfo.type); return -1;
    }
)


STB_LANG_NEW_TYPEINFO(
    STB_LANG_TYPEINFO_FIELDS(
        char casting;
        char decl_auto;

        char cast_implicit;
        char cast_pointer;
        char mode_intrp;
        char scope_flat;

        char os_windows;
        char os_mac;
        char os_linux;
        char os_unknown;
        char os_posix;

            // declaration.var.infer, [declaration.var.explicit]
            // casting.strict, [casting.implicit], casting.pointer
            // mode.interpreted, [mode.compiled]
            // scope.flat, [scope.structured]
    ),
    STB_LANG_TYPEINFO_INIT(
        checker->decl_auto = 0;
        checker->casting = 1; // Default is implicit
// 0: default, 1: implicit, 2: pointer, 3: pointer and implicit

        checker->cast_implicit = 1;
        checker->cast_pointer = 0;
        checker->mode_intrp = 0;
        checker->scope_flat = 0;

        checker->os_windows = 0;
        checker->os_mac = 0;
        checker->os_linux = 0;
        checker->os_unknown = 0;
        checker->os_posix = 0;


    #if defined(_WIN32)
        checker->os_windows = 1;
    #elif defined(_WIN64)
        checker->os_windows = 1;
    #elif defined(__APPLE__) || defined(__MACH__)
        checker->os_mac = 1;
    #elif defined(__linux__)
        checker->os_linux = 1;
    #elif defined(__unix__) || defined(__unix)
        checker->os_posix = 1;
    #else
        checker->os_unknown = 1;
    #endif
    ),
    STB_LANG_TYPEINFO_SUFFIX(
        
    ),
    STB_LANG_TYPEINFO_CHECK_TYPES(
        if (left->typeinfo.ptrnum == 0 && right->typeinfo.ptrnum == 0){
            if (checker->casting == 1 || checker->casting == 3){
                return; // Allow any implicit casting
            }
        }else if (left->typeinfo.ptrnum == right->typeinfo.ptrnum){
            if (checker->casting == 2 || checker->casting == 3){
                return; // Allow same pointer type casting (ptr<char> -> ptr<int>)
            }
        }
    ),
    STB_LANG_TYPEINFO_CASES(
        STB_LANG_TYPEINFO_CASE(AST_ASSERT,
            STB_LANG_TYPEINFO_ERROR_MINOR(ast->offset, ast->file, "AssertError", "Assert Failed: %s", ast->value);
        )
        STB_LANG_TYPEINFO_CASE(AST_MODE,
            if (ast->value != NULL){
                if (strcmp(ast->value, "declaration.var.infer") == 0){
                    checker->decl_auto = 1;
                }else if (strcmp(ast->value, "declaration.var.explicit") == 0){
                    checker->decl_auto = 0;
                }else if (strcmp(ast->value, "casting.strict") == 0){
                    checker->casting = 0;
                    checker->cast_implicit = 0;
                    checker->cast_pointer = 0;
                }else if (strcmp(ast->value, "casting.implicit") == 0){
                    checker->cast_implicit = 1;
                    if (checker->casting == 2 || checker->casting == 3){
                        checker->casting = 3;
                    }else {
                        checker->casting = 1;
                    }
                }else if (strcmp(ast->value, "casting.pointer") == 0){
                    checker->cast_pointer = 1;
                    if (checker->casting == 1 || checker->casting == 3){
                        checker->casting = 3;
                    }else {
                        checker->casting = 2;
                    }
                }else if (strcmp(ast->value, "mode.interpreted") == 0){
                    checker->mode_intrp = 1;
                }else if (strcmp(ast->value, "mode.compiled") == 0){
                    checker->mode_intrp = 0;
                }else if (strcmp(ast->value, "scope.flat") == 0){
                    checker->scope_flat = 1;
                }else if (strcmp(ast->value, "scope.structured") == 0){
                    checker->scope_flat = 0;
                }

            }
        )
        STB_LANG_TYPEINFO_CASE(AST_MODE_IF, 
            int expand = 0;
            char *data = ast->value;

            if (strcmp(data, "declaration.var.infer") == 0){
                expand = checker->decl_auto;
            }else if (strcmp(data, "declaration.var.explicit") == 0){
                expand = !checker->decl_auto;
            }else if (strcmp(data, "casting.strict") == 0){
                expand = !(checker->cast_implicit && checker->cast_pointer);
            }else if (strcmp(data, "casting.implicit") == 0){
                expand = checker->cast_implicit;
            }else if (strcmp(data, "casting.pointer") == 0){
                expand = checker->cast_pointer;
            }else if (strcmp(data, "mode.interpreted") == 0){
                expand = checker->mode_intrp;
            }else if (strcmp(data, "mode.compiled") == 0){
                expand = !checker->mode_intrp;
            }else if (strcmp(data, "scope.flat") == 0){
                expand = checker->scope_flat;
            }else if (strcmp(data, "scope.structured") == 0){
                expand = checker->scope_flat;
            }else if (strcmp(data, "platform.windows") == 0){
                expand = checker->os_windows;
            }else if (strcmp(data, "platform.mac") == 0){
                expand = checker->os_mac;
            }else if (strcmp(data, "platform.linux") == 0){
                expand = checker->os_linux;
            }else if (strcmp(data, "platform.posix") == 0){
                expand = checker->os_posix;
            }else if (strcmp(data, "platform.unknown") == 0){
                expand = checker->os_unknown;
            }else {
                STB_LANG_TYPEINFO_ERROR_MINOR(ast->offset, ast->file, "ModeIfError", "Attempting to mode if on non-existent mode");
            }

            if (ast->left == (void*)1){
                expand = !expand;
            }

            if (expand == 1){
                ast->typeinfo.type = 100;
                STB_LANG_EXPAND_BLOCK();
            }

        )
        STB_LANG_TYPEINFO_CASE(AST_FUNCDECL, 
            STB_LANG_MAKE_SCOPE(ast->value);
            STB_LANG_ADD_FUNCTION(ast->value,
                STB_LANG_FUNCTION_RETURN(ast->typeinfo);
                STB_LANG_FUNCTION_ADD_PARAMS(STB_LANG_GET_AST(ast->left));
            )
            STB_LANG_EXPAND_PARAMS();
        )
        STB_LANG_TYPEINFO_CASE(AST_FUNCDEF, 
            STB_LANG_MAKE_SCOPE(ast->value);
            STB_LANG_ADD_FUNCTION(ast->value,
                STB_LANG_FUNCTION_RETURN(ast->typeinfo);
                STB_LANG_FUNCTION_ADD_PARAMS(STB_LANG_GET_AST(ast->left));
            )
            STB_LANG_EXPAND_PARAMS();
            STB_LANG_EXPAND_BLOCK();
        )
        STB_LANG_TYPEINFO_CASE(AST_STRUCT, 



            int size = 0;
            STB_LANG_ITERATE_LINKED_LIST(STB_LANG_GET_AST(ast->left), head, Lang_Parser_AST,


                size += STB_LANG_LOOKUP_SIZE(checker->root_scope, &head->typeinfo);
            )
            ast->typeinfo.data.struct1.size = size;
            ast->typeinfo.type = AST_TYPE_STRUCT;

            STB_LANG_ADD_DATA(ast->value, 
                // STB_LANG_FUNCTION_ADD_PARAMS(STB_LANG_GET_AST(ast->left));
                symnew.data.struct1.structdef = STB_LANG_AS_AST(ast);
                STB_LANG_SET_SYMBOL(ast->typeinfo.data.struct1.symbol, STB_LANG_CURRENT_SYMBOL());
            )



        )
        STB_LANG_TYPEINFO_CASE(AST_STORE, 
            STB_LANG_EXPAND_LHS();
            STB_LANG_EXPAND_RHS();
            STB_LANG_INFER_TYPE(ast->value);
            ast->typeinfo = STB_LANG_LHS(ast)->typeinfo;
            int typ = STB_LANG_LHS(ast)->type;
            if (typ == AST_ACCESS || typ == AST_DEREF || typ == AST_INDEX){
                ast->typeinfo = STB_LANG_LHS(STB_LANG_LHS(ast))->typeinfo;
            }
            if (ast->typeinfo.ptrnum != 0){
                ast->typeinfo.ptrnum--;
            }else if (ast->typeinfo.type == AST_TYPE_ARRAY){
                ast->typeinfo = *(Lang_TypeInfo_Typeinfo*)(ast->typeinfo.data.array.elem_type);
                if (STB_LANG_LHS(ast)->type == AST_DEREF){
                    STB_LANG_LHS(ast)->typeinfo = ast->typeinfo;
                }else {
                }
            }else if (STB_LANG_LHS(ast)->type == AST_ACCESS){
                ast->typeinfo = STB_LANG_LHS(ast)->typeinfo;
            }else if (STB_LANG_LHS(ast)->type == AST_DEREF){
                // Deref already did the typeinfo--
            }else {
                STB_LANG_TYPEINFO_ERROR_MINOR(ast->offset, ast->file, "DerefError", "Could not dereference anything that's not a pointer or array");
            };
            if (STB_LANG_LHS(ast)->type == AST_INDEX || STB_LANG_LHS(ast)->type == AST_ACCESS){
                // STB_LANG_EXPECT_TYPE_EQ(STB_LANG_LHS(ast), STB_LANG_RHS(ast));
            }else {
                STB_LANG_EXPECT_TYPE_EQ(ast, STB_LANG_RHS(ast));
            }
        )
        STB_LANG_TYPEINFO_CASE(AST_IR_TEMP, 
            ;
        )
        STB_LANG_TYPEINFO_CASE(AST_ASSIGN, 
            STB_LANG_EXPAND_LHS();
            STB_LANG_EXPAND_RHS();
            if (STB_LANG_OF_AST(ast->left, type) == AST_VAR){
                STB_LANG_INFER_TYPE(STB_LANG_OF_AST(ast->left, value));
                if (ast->typeinfo.type == -1 || ast->typeinfo.type == 0){
                    if (checker->decl_auto == 0){
                        STB_LANG_TYPEINFO_ERROR_MINOR(ast->offset, ast->file, "AssignError", "Variable \"%s\" has not been declared before being assigned", STB_LANG_OF_AST(ast->left, value));
                    }else {
                        ast->typeinfo = STB_LANG_RHS(ast)->typeinfo;
                        STB_LANG_LHS(ast)->typeinfo = ast->typeinfo;
                    }
                }
                STB_LANG_REGISTER_VARIABLE(STB_LANG_OF_AST(ast->left, value), ast->typeinfo)
            }
            STB_LANG_EXPECT_TYPE_EQ(STB_LANG_LHS(ast), STB_LANG_RHS(ast));
        )
        STB_LANG_TYPEINFO_CASE(AST_DECL,
            if (STB_LANG_RHS(ast) != NULL){
                STB_LANG_EXPAND_RHS();
                STB_LANG_EXPECT_TYPE_EQ(ast, STB_LANG_RHS(ast));
            }
            STB_LANG_TYPEINFO_ASSUME_TYPE(STB_LANG_LHS(ast)->typeinfo);
            STB_LANG_VARIABLE(ast);
        )
        STB_LANG_TYPEINFO_CASE(AST_VAR,
            if (ast->left != (void*)1){
                STB_LANG_INFER_TYPE(ast->value);
            }
        )
        STB_LANG_TYPEINFO_CASE(AST_INT,
            STB_LANG_TYPEINFO_ASSUME_TYPE(STB_LANG_TYPEINFO(.type=AST_TYPE_INT, .ptrnum=0));
        )
        STB_LANG_TYPEINFO_CASE(AST_FLOAT,
            STB_LANG_TYPEINFO_ASSUME_TYPE(STB_LANG_TYPEINFO(.type=AST_TYPE_FLOAT, .ptrnum=0));
        )
        STB_LANG_TYPEINFO_CASE(AST_STRING,
            STB_LANG_TYPEINFO_ASSUME_TYPE(STB_LANG_TYPEINFO(.type=AST_TYPE_STRING, .ptrnum=0));
        )
        STB_LANG_TYPEINFO_6CASES(AST_LT, AST_LTE, AST_GT, AST_GTE, AST_EQ, AST_NEQ,
            STB_LANG_EXPAND_LHS();
            STB_LANG_EXPAND_RHS();
            STB_LANG_TYPEINFO_ASSUME_TYPE(STB_LANG_LHS(ast)->typeinfo);
            STB_LANG_EXPECT_TYPE_EQ(ast, STB_LANG_RHS(ast));
        )
        STB_LANG_TYPEINFO_5CASES(AST_ADD, AST_SUB, AST_MUL, AST_DIV, AST_MODULO,
            STB_LANG_EXPAND_LHS();
            STB_LANG_EXPAND_RHS();
            STB_LANG_TYPEINFO_ASSUME_TYPE(STB_LANG_LHS(ast)->typeinfo);
            STB_LANG_EXPECT_TYPE_EQ(ast, STB_LANG_RHS(ast));
        )
        STB_LANG_TYPEINFO_5CASES(AST_AND, AST_OR, AST_BAND, AST_BOR, AST_XOR,
            STB_LANG_EXPAND_LHS();
            STB_LANG_EXPAND_RHS();
            STB_LANG_TYPEINFO_ASSUME_TYPE(STB_LANG_RHS(ast)->typeinfo);
            STB_LANG_EXPECT_TYPE_EQ(ast, STB_LANG_RHS(ast));
        )
        STB_LANG_TYPEINFO_2CASES(AST_BSHL, AST_BSHR,
            STB_LANG_EXPAND_RHS();
            STB_LANG_TYPEINFO_ASSUME_TYPE(STB_LANG_RHS(ast)->typeinfo);
            STB_LANG_EXPECT_TYPE_EQ(ast, STB_LANG_RHS(ast));
        )
        STB_LANG_TYPEINFO_CASE(AST_SIZEOF,
            char *size = arena_alloc(&g_arena, 100);
            snprintf(size, 100, "%d", STB_LANG_LOOKUP_SIZE(checker->root_scope, &ast->typeinfo));
            ast->type = AST_INT;
            ast->value = size;
        )
        STB_LANG_TYPEINFO_CASE(AST_FUNCALL,
            STB_LANG_EXPAND_ARGS();
            STB_LANG_FIND_FUNCTION(checker->root_scope, ast->value, 
                STB_LANG_TYPEINFO_ASSUME_TYPE(symnew.typeinfo);
                STB_LANG_FUNCTION_CHECK_LIST(ast->left);
            )
        )
        STB_LANG_TYPEINFO_CASE(AST_IF,
            STB_LANG_EXPAND_ARGS();
            STB_LANG_EXPAND_BLOCK();
            STB_LANG_EXPAND_LIST(ast->middle);
        )
        STB_LANG_TYPEINFO_CASE(AST_WHILE,
            STB_LANG_EXPAND_ARGS();
            STB_LANG_EXPAND_BLOCK();
        )
        STB_LANG_TYPEINFO_CASE(AST_RET,
            STB_LANG_EXPAND_ARGS();
        )
        STB_LANG_TYPEINFO_CASE(AST_CAST,
            STB_LANG_EXPAND_LHS();
        )
        STB_LANG_TYPEINFO_CASE(AST_REF,
            STB_LANG_EXPAND_LHS();

            Lang_TypeInfo_Typeinfo typinf = STB_LANG_OF_AST(ast->left, typeinfo);
            typinf.ptrnum++;


            ast->typeinfo = typinf;
        )
        STB_LANG_TYPEINFO_CASE(AST_DEREF,
            STB_LANG_EXPAND_LHS();

            Lang_TypeInfo_Typeinfo typinf = STB_LANG_OF_AST(ast->left, typeinfo);
            if (typinf.ptrnum == 0 && typinf.type != AST_TYPE_ARRAY){
                STB_LANG_TYPEINFO_ERROR_MINOR(ast->offset, ast->file, "DerefError", "Could not dereference anything that's not a pointer or array");
            }else if (typinf.type == AST_TYPE_ARRAY){
                if (typinf.data.array.elem_type != NULL){
                    ast->typeinfo = *(Lang_TypeInfo_Typeinfo*)(typinf.data.array.elem_type);
                }
            }
            if (typinf.ptrnum != 0){
                typinf.ptrnum--;
                ast->typeinfo = typinf;
            }
        )
        STB_LANG_TYPEINFO_CASE(AST_INDEX,
            STB_LANG_EXPAND_LHS();

            Lang_TypeInfo_Typeinfo typinf = STB_LANG_OF_AST(ast->left, typeinfo);
            ast->typeinfo = typinf;
            if (ast->typeinfo.ptrnum != 0){
                ast->typeinfo.ptrnum--;
            }else if (ast->typeinfo.type == AST_TYPE_ARRAY){
                ast->typeinfo = *(Lang_TypeInfo_Typeinfo*)(ast->typeinfo.data.array.elem_type);
            }else {
                STB_LANG_TYPEINFO_ERROR_MINOR(ast->offset, ast->file, "DerefError", "Could not dereference anything that's not a pointer or array");
            };
        )
        STB_LANG_TYPEINFO_CASE(AST_ACCESS,
            // printf("[%d\n", a->type == AST_ACCESS);

            STB_LANG_EXPAND_LHS();
            STB_LANG_TYPEINFO_ASSUME_TYPE(STB_LANG_LHS(ast)->typeinfo);


            // printf("{%s}\n", STB_LANG_LHS(ast)->typeinfo.data.struct1.name);
            STB_LANG_FIND_DATA(checker->root_scope, STB_LANG_LHS(ast)->typeinfo.data.struct1.name,
                STB_LANG_FIND_DATA(checker->root_scope, STB_LANG_LHS(ast)->typeinfo.data.struct1.name,
                    Lang_Parser_AST *structdef = STB_LANG_LHS(STB_LANG_GET_AST(symnew.data.struct1.structdef));
                    STB_LANG_ITERATE_LINKED_LIST(structdef, field, Lang_Parser_AST,
                        if (field->type == AST_VAR){
                            if (strcmp(field->value, ast->value) == 0){
                                ast->typeinfo = field->typeinfo;
                            }
                        }
                    )
                )

                Lang_TypeInfo_Typeinfo *typeinfo = &(ast->typeinfo);
                STB_LANG_SET_SYMBOL(typeinfo->data.struct1.symbol, symnew);

            )
        )
        STB_LANG_TYPEINFO_CASE(AST_EXPR,
            STB_LANG_EXPAND_LHS();
        )
        STB_LANG_TYPEINFO_CASE(AST_IR_INSTRUCTION,
            char *instr = ast->value;
            if (strcmp(ast->value, "ret") == 0){
                STB_LANG_EXPAND_LHS();
            }else if (strcmp(instr, "add") == 0 || strcmp(instr, "sub") == 0 || strcmp(instr, "mul") == 0 || strcmp(instr, "div") == 0 || strcmp(instr, "mod") == 0|| strcmp(instr, "setlt") == 0 || strcmp(instr, "setle") == 0 || strcmp(instr, "setgt") == 0 || strcmp(instr, "setge") == 0 || strcmp(instr, "seteq") == 0 || strcmp(instr, "setneq") == 0 || strcmp(instr, "bor") == 0 || strcmp(instr, "band") == 0 || strcmp(instr, "and") == 0 || strcmp(instr, "or") == 0 || strcmp(instr, "xor") == 0 || strcmp(instr, "bshl") == 0 || strcmp(instr, "bshr") == 0){
                STB_LANG_ITERATE_LINKED_LIST(ast->left, lefts, Lang_Parser_AST,
                    STB_LANG_EXPAND(lefts);
                )
                STB_LANG_EXPAND_RHS();
            }else if (strcmp(instr, "addr") == 0 || strcmp(instr, "load") == 0 || strcmp(instr, "store") == 0){
                STB_LANG_EXPAND_LHS();
                STB_LANG_EXPAND_RHS();
            }else if (strcmp(instr, "mov") == 0){
                STB_LANG_EXPAND_LHS();
                STB_LANG_EXPAND_RHS();

            if (STB_LANG_OF_AST(ast->left, type) == AST_VAR){
                STB_LANG_INFER_TYPE(STB_LANG_OF_AST(ast->left, value));
                if (ast->typeinfo.type == -1 || ast->typeinfo.type == 0){
                    if (checker->decl_auto == 0){
                        STB_LANG_TYPEINFO_ERROR_MINOR_UNDERLYING(ast->offset, ast->file, "AssignError", "Variable \"%s\" has not been declared before being assigned", STB_LANG_OF_AST(ast->left, value));

                        stb_lang_error_hint("if you're trying to access an IR register, use `.a0`", "mov .a0, 5");
                        stb_lang_exit(-1);
                    }else {
                        ast->typeinfo = STB_LANG_RHS(ast)->typeinfo;
                        STB_LANG_LHS(ast)->typeinfo = ast->typeinfo;
                    }
                }
                STB_LANG_REGISTER_VARIABLE(STB_LANG_OF_AST(ast->left, value), ast->typeinfo)
            }
            }else if (strcmp(instr, "call") && strcmp(instr, "syscall3")){
                STB_LANG_EXPAND_LHS();
                STB_LANG_EXPAND_RHS();
            }
        )

        STB_LANG_TYPEINFO_CASE(AST_IR_LIST,
            STB_LANG_ITERATE_LINKED_LIST(ast->left, a_instr, Lang_Parser_AST,
                STB_LANG_EXPAND(a_instr);
            )
        )
    )
)



#define CUR_IR_NAME Lang_IR
#define CUR_IR_PREFIX lang_ir

STB_LANG_NEW_IR(
    STB_LANG_IR_FIELDS(
        int interpreted;
    ),
    STB_LANG_IR_INIT(
        ir->interpreted = 0;
    ),
    STB_LANG_IR_OPERANDS(
        IR_INT,
        IR_FLOAT,
        IR_VAR,
        IR_REG,
        IR_MEM
    ),
    STB_LANG_IR_INSTRS(
        IR_NOP,
        IR_EXTERN,
        IR_FUNCDEF_BEGIN,
        IR_FUNCDEF_END,
        IR_ASSIGN,
        IR_DECL,
        IR_POP, // pop the stack
        IR_PUSH,
        IR_CALL,
        IR_ADD,
        IR_SUB,
        IR_MUL,
        IR_DIV,
        IR_MOD,
        IR_LT,
        IR_LTE,
        IR_GT,
        IR_GTE,
        IR_EQ,
        IR_NEQ,
        IR_JUMP_IF_FALSE,
        IR_JUMP,
        IR_LABEL,
        IR_RET,
        IR_BOR,
        IR_BAND,
        IR_AND,
        IR_OR,
        IR_XOR,
        IR_ADDR,
        IR_LOAD,
        IR_STORE,
        IR_BSHL,
        IR_BSHR,
        IR_SYSCALL3
    ),
    STB_LANG_IR_CASES(
        STB_LANG_IR_CASE(AST_FUNCDECL,
            STB_LANG_IR_EMIT(IR_EXTERN, STB_LANG_IR_OPERAND(IR_VAR, ast->value), NULL, NULL);
            // Skip
        )
        STB_LANG_IR_CASE(AST_MODE,
            if (ast->value != NULL){
                if (strcmp(ast->value, "mode.interpreted") == 0){
                    ir->interpreted = 1;
                }else if (strcmp(ast->value, "mode.compiled") == 0){
                    ir->interpreted = 0;
                }
            }
        )
        STB_LANG_IR_CASE(AST_MODE_IF,
            if (ast->typeinfo.type == 100){
                STB_LANG_IR_BLOCK()
            }
        )
        STB_LANG_IR_CASE(AST_FUNCDEF,
            STB_LANG_IR_EMIT(IR_FUNCDEF_BEGIN, STB_LANG_IR_OPERAND(IR_VAR, ast->value), NULL, NULL);
int idx = 0;
int argidx = 0;
STB_LANG_ITERATE_LINKED_LIST(ast->left, _args, Lang_Parser_AST,
    if (_args->flags != STB_LANG_TYPEINFO_VARIADIC) {
        char str[10];snprintf(str, 10, "a%d", idx++);
        STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND(IR_VAR, _args->value), STB_LANG_IR_OPERAND(IR_REG, arena_strdup(&g_arena, str)), NULL);
    }else {
        char str[10];snprintf(str, 10, ".arg%d", argidx++);
        STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND(IR_VAR, _args->value), STB_LANG_IR_OPERAND(IR_VAR, _args->value), NULL);
    }
)
            STB_LANG_IR_BLOCK()
            STB_LANG_IR_EMIT(IR_FUNCDEF_END, STB_LANG_IR_OPERAND(IR_VAR, ast->value), NULL, NULL);
        )
        STB_LANG_IR_CASE(AST_ASSIGN,
            STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_LHS_EXTRA(ast, 1), STB_LANG_IR_RHS(ast), NULL);
        )
        STB_LANG_IR_CASE(AST_STRUCT,
        )

        STB_LANG_IR_CASE(AST_IR_INSTRUCTION,
            char *instr = ast->value;
            if (strcmp(ast->value, "mov") == 0){
                if (STB_LANG_LHS(ast)->type == AST_ACCESS){
                    Lang_TypeInfo_Symbol *symbol = STB_LANG_GET_SYMBOL(STB_LANG_LHS(ast)->typeinfo.data.struct1.symbol);
                    Lang_Parser_AST *structdef = STB_LANG_GET_AST(symbol->data.struct1.structdef);
                    int offset = 0;
                    int found = 0;
                    STB_LANG_ITERATE_LINKED_LIST(STB_LANG_LHS(structdef), field, Lang_Parser_AST,
                        if (field->type == AST_VAR){
                            if (strcmp(field->value, STB_LANG_LHS(ast)->value) == 0){
                                ast->typeinfo = field->typeinfo;
                                found = 1;
                                break;
                            }else {
                                offset += STB_LANG_LOOKUP_SIZE(ir->root_scope, &field->typeinfo);
                            }
                        }
                    )

                    if (found == 0){
                        STB_LANG_IR_ERROR_MINOR(ast->offset, ast->file, "StructError", "Could not find field \"%s\" in \"struct %s\"", STB_LANG_LHS(ast)->value, symbol->name);
                    }
                    STB_LANG_IR_NEW_TEMP(addr_reg);
                    STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_LHS_EXTRA(STB_LANG_LHS(ast), 1), NULL);
                    char str[32]; snprintf(str, 32, "%d", offset);
                    STB_LANG_IR_EMIT(IR_ADD, STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_OPERAND(IR_INT, arena_strdup(&g_arena, str)));
                    STB_LANG_IR_EMIT(IR_STORE, STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_RHS(ast), NULL, .typeinfo=STB_LANG_LHS(ast)->typeinfo);
                }
            else if (STB_LANG_LHS(ast)->type == AST_DEREF) {
                ast->typeinfo = STB_LANG_LHS(ast)->typeinfo;
                if (ast->typeinfo.type == AST_TYPE_ARRAY){
                    ast->typeinfo = *(Lang_TypeInfo_Typeinfo*)(ast->typeinfo.data.array.elem_type);
                    if (STB_LANG_LHS(ast)->type == AST_DEREF){
                        STB_LANG_LHS(ast)->typeinfo = ast->typeinfo;
                    }else {
                    }
                }
                STB_LANG_IR_NEW_TEMP(secure_addr_reg);
                // printf("%d\n", STB_LANG_LOOKUP_SIZE(ir->root_scope, &ast->typeinfo));

                Lang_Parser_AST *ll = STB_LANG_LHS(STB_LANG_LHS(ast));
                if (STB_LANG_LHS(STB_LANG_LHS(ast))->typeinfo.ptrnum != 0){
                    Lang_IR_Operand *op = STB_LANG_IR(ll);
                    STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND(IR_REG, secure_addr_reg), op, NULL);
                }else if (ll->type == AST_VAR){
                    Lang_IR_Operand *op = STB_LANG_IR(ll);
                    STB_LANG_IR_EMIT(IR_ADDR, STB_LANG_IR_OPERAND(IR_REG, secure_addr_reg), op, NULL);
                }else if (ll->type == AST_ADD){
                    Lang_IR_Operand *op = STB_LANG_IR_LHS(ll);
                    Lang_IR_Operand *opl = STB_LANG_IR_RHS(ll);
                    STB_LANG_IR_EMIT(IR_ADDR, STB_LANG_IR_OPERAND(IR_REG, secure_addr_reg), op, NULL);
                    STB_LANG_IR_EMIT(IR_ADD, STB_LANG_IR_OPERAND(IR_REG, secure_addr_reg), STB_LANG_IR_OPERAND(IR_REG, secure_addr_reg), opl);
                }else if (ll->type == AST_SUB){
                    Lang_IR_Operand *op = STB_LANG_IR_LHS(ll);
                    Lang_IR_Operand *opl = STB_LANG_IR_RHS(ll);
                    STB_LANG_IR_EMIT(IR_ADDR, STB_LANG_IR_OPERAND(IR_REG, secure_addr_reg), op, NULL);
                    STB_LANG_IR_EMIT(IR_SUB, STB_LANG_IR_OPERAND(IR_REG, secure_addr_reg), STB_LANG_IR_OPERAND(IR_REG, secure_addr_reg), opl);
                }else {
                    STB_LANG_IR_ERROR_MINOR_UNDERLYING(ast->offset, ast->file, "DerefError", "Deref type not supported");
                    stb_lang_error_hint("These are the only supported types of derefs:", "deref(a)\nderef(a + ...)\nderef(a - ...)");
                };
                STB_LANG_IR_EMIT(IR_STORE, STB_LANG_IR_OPERAND(IR_REG, secure_addr_reg), STB_LANG_IR_RHS(ast), NULL, .typeinfo=ast->typeinfo);
            }else if (STB_LANG_LHS(ast)->type == AST_INDEX){
                STB_LANG_IR_NEW_TEMP(addr_reg);
                STB_LANG_IR_NEW_TEMP(offset_reg);

                int size = STB_LANG_LOOKUP_SIZE(ir->root_scope, &STB_LANG_LHS(ast)->typeinfo);
                char str[32]; snprintf(str, 32, "%d", size);
                // a[0] = 5
                STB_LANG_IR_EMIT(IR_MUL, STB_LANG_IR_OPERAND(IR_REG, offset_reg), STB_LANG_IR_RHS(STB_LANG_LHS(ast)), STB_LANG_IR_OPERAND(IR_INT, arena_strdup(&g_arena, str)));
                if (STB_LANG_LHS(STB_LANG_LHS(ast))->typeinfo.ptrnum == 0){
                    STB_LANG_IR_EMIT(IR_ADDR, STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_LHS(STB_LANG_LHS(ast)), NULL);
                }else {
                    STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_LHS(STB_LANG_LHS(ast)), NULL);
                }
                STB_LANG_IR_EMIT(IR_ADD, STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_OPERAND(IR_REG, offset_reg));
                STB_LANG_IR_EMIT(IR_STORE, STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_RHS(ast), NULL, .typeinfo=STB_LANG_LHS(ast)->typeinfo);
            }
                else {
                    STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_LHS_EXTRA(ast, 1), STB_LANG_IR_RHS(ast), NULL);
                }
            }else if (strcmp(ast->value, "call") == 0){
                char *funcname = (char*)STB_LANG_RHS(ast);
                Lang_Parser_AST *params[64]; int paramslen = 0;
                STB_LANG_ITERATE_LINKED_LIST(ast->left, arg, Lang_Parser_AST,
                    params[paramslen++] = arg;
                )
                int idx = -1;
                int varidx = -1;
                for (int i = 0; i < paramslen; i++) {
                    if (params[i]->flags != STB_LANG_TYPEINFO_VARIADIC) {
                        idx++;
                    } else {
                        varidx++;
                    }
                }

                long origidx = (long)idx;


                for (int i = paramslen - 1; i >= 0; i--) {
                    Lang_Parser_AST *param = params[i];
                    if (param->type != AST_IR_TEMP){
                        STB_LANG_IR_ERROR_MINOR(ast->offset, ast->file, "InlineIRCallError", "Only argument registers (`a0, a1, ...`) are allowed to be arguments in inline IR");
                    }

                    STB_CONCAT(CUR_IR_NAME, _Operand) *operand = lang_ir_ast(ir, param, 0);
                    if (param->flags != STB_LANG_TYPEINFO_VARIADIC) {
                        char str[32];
                        snprintf(str, 32, "a%d", idx--); 
                        STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND(IR_REG, arena_strdup(&g_arena, str)), operand, NULL);
                    } else {
                        char str[32];
                        snprintf(str, 32, ".arg%d", varidx--);
                        STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND(IR_REG, arena_strdup(&g_arena, str)), operand, NULL);
                    }
                }
                STB_LANG_IR_EMIT(IR_CALL, STB_LANG_IR_OPERAND(IR_VAR, funcname), NULL, NULL, .typeinfo=(Lang_TypeInfo_Typeinfo){.type = origidx});
            }else if (strcmp(ast->value, "syscall3") == 0){
                STB_LANG_IR_EMIT(IR_SYSCALL3, NULL, STB_LANG_IR_OPERAND(IR_INT, (char*)ast->left), NULL);
            }else if (strcmp(ast->value, "ret") == 0){
                STB_CONCAT(CUR_IR_NAME, _Operand) *operand = STB_LANG_IR_LHS(ast);
                STB_LANG_IR_EMIT(IR_RET, NULL, operand, NULL);
            }else if (strcmp(instr, "add") == 0 || strcmp(instr, "sub") == 0 || strcmp(instr, "mul") == 0 || strcmp(instr, "div") == 0 || strcmp(instr, "mod") == 0|| strcmp(instr, "setlt") == 0 || strcmp(instr, "setle") == 0 || strcmp(instr, "setgt") == 0 || strcmp(instr, "setge") == 0 || strcmp(instr, "seteq") == 0 || strcmp(instr, "setneq") == 0 || strcmp(instr, "bor") == 0 || strcmp(instr, "band") == 0 || strcmp(instr, "and") == 0 || strcmp(instr, "or") == 0 || strcmp(instr, "xor") == 0 || strcmp(instr, "bshl") == 0 || strcmp(instr, "bshr") == 0){
                Lang_Parser_AST *ast1 = (Lang_Parser_AST*)ast->left;
                Lang_Parser_AST *ast2 = (Lang_Parser_AST*)((Lang_Parser_AST*)ast->left)->next;
                int ir_instr = -1;
                
                if (strcmp(ast->value, "add") == 0){
                    ir_instr = IR_ADD;
                }else if (strcmp(ast->value, "sub") == 0){
                    ir_instr = IR_SUB;
                }else if (strcmp(ast->value, "mul") == 0){
                    ir_instr = IR_MUL;
                }else if (strcmp(ast->value, "div") == 0){
                    ir_instr = IR_DIV;
                }else if (strcmp(ast->value, "mod") == 0){
                    ir_instr = IR_MOD;
                }else if (strcmp(ast->value, "setlt") == 0){
                    ir_instr = IR_LT;
                }else if (strcmp(ast->value, "setle") == 0){
                    ir_instr = IR_LTE;
                }else if (strcmp(ast->value, "setgt") == 0){
                    ir_instr = IR_GT;
                }else if (strcmp(ast->value, "setge") == 0){
                    ir_instr = IR_GTE;
                }else if (strcmp(ast->value, "seteq") == 0){
                    ir_instr = IR_EQ;
                }else if (strcmp(ast->value, "setneq") == 0){
                    ir_instr = IR_NEQ;
                }else if (strcmp(ast->value, "bor") == 0){
                    ir_instr = IR_BOR;
                }else if (strcmp(ast->value, "band") == 0){
                    ir_instr = IR_BAND;
                }else if (strcmp(ast->value, "and") == 0){
                    ir_instr = IR_AND;
                }else if (strcmp(ast->value, "or") == 0){
                    ir_instr = IR_OR;
                }else if (strcmp(ast->value, "xor") == 0){
                    ir_instr = IR_XOR;
                }else if (strcmp(ast->value, "bshl") == 0){
                    ir_instr = IR_BSHL;
                }else if (strcmp(ast->value, "bshr") == 0){
                    ir_instr = IR_BSHR;
                }
                if (ir_instr != -1){
                    STB_LANG_IR_EMIT(ir_instr, STB_LANG_IR(ast1), STB_LANG_IR(ast2), STB_LANG_IR_RHS(ast));
                }
            }else if (strcmp(instr, "addr") == 0){
                STB_LANG_IR_EMIT(IR_ADDR, STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast), NULL);
            }else if (strcmp(instr, "load") == 0){
                STB_LANG_IR_EMIT(IR_LOAD, STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast), NULL);
            }else if (strcmp(instr, "store") == 0){
                STB_LANG_IR_EMIT(IR_STORE, STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast), NULL);
            }
        )
        STB_LANG_IR_CASE(AST_IR_LIST,
            STB_LANG_ITERATE_LINKED_LIST(ast->left, instr, Lang_Parser_AST,
                STB_LANG_IR(instr);
                instr->typeinfo.type = ir->temp_number;
            )
        )

        STB_LANG_IR_CASE(AST_STORE,
            if (STB_LANG_LHS(ast)->type == AST_INDEX){
                STB_LANG_IR_NEW_TEMP(addr_reg);
                STB_LANG_IR_NEW_TEMP(offset_reg);

                int size = STB_LANG_LOOKUP_SIZE(ir->root_scope, &STB_LANG_LHS(ast)->typeinfo);
                char str[32]; snprintf(str, 32, "%d", size);
                // a[0] = 5
                STB_LANG_IR_EMIT(IR_MUL, STB_LANG_IR_OPERAND(IR_REG, offset_reg), STB_LANG_IR_RHS(STB_LANG_LHS(ast)), STB_LANG_IR_OPERAND(IR_INT, arena_strdup(&g_arena, str)));
                if (STB_LANG_LHS(STB_LANG_LHS(ast))->typeinfo.ptrnum == 0){
                    STB_LANG_IR_EMIT(IR_ADDR, STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_LHS(STB_LANG_LHS(ast)), NULL);
                }else {
                    STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_LHS(STB_LANG_LHS(ast)), NULL);
                }
                STB_LANG_IR_EMIT(IR_ADD, STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_OPERAND(IR_REG, offset_reg));
                STB_LANG_IR_EMIT(IR_STORE, STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_RHS(ast), NULL, .typeinfo=STB_LANG_LHS(ast)->typeinfo);
            }else if (STB_LANG_LHS(ast)->type == AST_ACCESS) {
                Lang_TypeInfo_Symbol *symbol = STB_LANG_GET_SYMBOL(STB_LANG_LHS(ast)->typeinfo.data.struct1.symbol);
                Lang_Parser_AST *structdef = STB_LANG_GET_AST(symbol->data.struct1.structdef);
                int offset = 0;
                int found = 0;
                STB_LANG_ITERATE_LINKED_LIST(STB_LANG_LHS(structdef), field, Lang_Parser_AST,
                    if (field->type == AST_VAR){
                        if (strcmp(field->value, STB_LANG_LHS(ast)->value) == 0){
                            ast->typeinfo = field->typeinfo;
                            found = 1;
                            break;
                        }else {
                            offset += STB_LANG_LOOKUP_SIZE(ir->root_scope, &field->typeinfo);
                        }
                    }
                )

                if (found == 0){
                    STB_LANG_IR_ERROR_MINOR(ast->offset, ast->file, "StructError", "Could not find field \"%s\" in \"struct %s\"", STB_LANG_LHS(ast)->value, symbol->name);
                }

                STB_LANG_IR_NEW_TEMP(addr_reg);
                STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND_EXTRA(IR_REG, addr_reg, 1), STB_LANG_IR_LHS_EXTRA(STB_LANG_LHS(ast), 1), NULL);
                char str[32]; snprintf(str, 32, "%d", offset);
                STB_LANG_IR_EMIT(IR_ADD, STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_OPERAND(IR_INT, arena_strdup(&g_arena, str)));
                STB_LANG_IR_EMIT(IR_STORE, STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_RHS(ast), NULL, .typeinfo=STB_LANG_LHS(ast)->typeinfo);
            }else if (STB_LANG_LHS(ast)->type == AST_DEREF) {
                STB_LANG_IR_NEW_TEMP(secure_addr_reg);

                Lang_Parser_AST *ll = STB_LANG_LHS(STB_LANG_LHS(ast));
                if (STB_LANG_LHS(STB_LANG_LHS(ast))->typeinfo.ptrnum != 0){
                    Lang_IR_Operand *op = STB_LANG_IR(ll);
                    STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND(IR_REG, secure_addr_reg), op, NULL);
                }else if (ll->type == AST_VAR){
                    Lang_IR_Operand *op = STB_LANG_IR(ll);
                    STB_LANG_IR_EMIT(IR_ADDR, STB_LANG_IR_OPERAND(IR_REG, secure_addr_reg), op, NULL);
                }else if (ll->type == AST_ADD){
                    Lang_IR_Operand *op = STB_LANG_IR_LHS(ll);
                    Lang_IR_Operand *opl = STB_LANG_IR_RHS(ll);
                    STB_LANG_IR_EMIT(IR_ADDR, STB_LANG_IR_OPERAND(IR_REG, secure_addr_reg), op, NULL);
                    STB_LANG_IR_EMIT(IR_ADD, STB_LANG_IR_OPERAND(IR_REG, secure_addr_reg), STB_LANG_IR_OPERAND(IR_REG, secure_addr_reg), opl);
                }else if (ll->type == AST_SUB){
                    Lang_IR_Operand *op = STB_LANG_IR_LHS(ll);
                    Lang_IR_Operand *opl = STB_LANG_IR_RHS(ll);
                    STB_LANG_IR_EMIT(IR_ADDR, STB_LANG_IR_OPERAND(IR_REG, secure_addr_reg), op, NULL);
                    STB_LANG_IR_EMIT(IR_SUB, STB_LANG_IR_OPERAND(IR_REG, secure_addr_reg), STB_LANG_IR_OPERAND(IR_REG, secure_addr_reg), opl);
                }else {
                    STB_LANG_IR_ERROR_MINOR_UNDERLYING(ast->offset, ast->file, "DerefError", "Deref type not supported");
                    stb_lang_error_hint("These are the only supported types of derefs:", "deref(a)\nderef(a + ...)\nderef(a - ...)");
                };
                STB_LANG_IR_EMIT(IR_STORE, STB_LANG_IR_OPERAND(IR_REG, secure_addr_reg), STB_LANG_IR_RHS(ast), NULL, .typeinfo=ast->typeinfo);
            }else {
                STB_LANG_IR_EMIT(IR_STORE, STB_LANG_IR_LHS_EXTRA(ast, 1), STB_LANG_IR_RHS(ast), NULL, .typeinfo=STB_LANG_LHS(ast)->typeinfo);
            }
        )

        STB_LANG_IR_CASE(AST_ACCESS,
            Lang_TypeInfo_Symbol *symbol = STB_LANG_GET_SYMBOL(ast->typeinfo.data.struct1.symbol);
            if (symbol == NULL){
                fprintf(stderr, "Symbol error thing\n");
                stb_lang_exit(0);
            }
            Lang_Parser_AST *structdef = STB_LANG_GET_AST(symbol->data.struct1.structdef);
            int offset = 0;
            int found = 0;
            STB_LANG_ITERATE_LINKED_LIST(STB_LANG_LHS(structdef), field, Lang_Parser_AST,
                if (field->type == AST_VAR){
                    if (strcmp(field->value, ast->value) == 0){
                        ast->typeinfo = field->typeinfo;
                        found = 1;
                        break;
                    }else {
                        offset += STB_LANG_LOOKUP_SIZE(ir->root_scope, &field->typeinfo);
                    }
                }
            )
            if (found == 0){
                STB_LANG_IR_ERROR_MINOR(ast->offset, ast->file, "StructError", "Could not find field \"%s\" in \"struct %s\"", ast->value, symbol->name);
            }
            STB_LANG_IR_NEW_TEMP(addr_reg);
            STB_LANG_IR_NEW_TEMP(dest_reg);
            STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_LHS_EXTRA(ast, 1), NULL);
            char str[32]; snprintf(str, 32, "%d", offset);
            STB_LANG_IR_EMIT(IR_ADD, STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_OPERAND(IR_INT, arena_strdup(&g_arena, str)));
            STB_LANG_IR_EMIT(IR_LOAD, STB_LANG_IR_OPERAND(IR_REG, dest_reg), STB_LANG_IR_OPERAND(IR_REG, addr_reg), NULL);
            return STB_LANG_IR_OPERAND(IR_REG, dest_reg);
        )

        STB_LANG_IR_CASE(AST_VAR,
            return STB_LANG_IR_OPERAND(IR_VAR, ast->value);
        )
        STB_LANG_IR_CASE(AST_IR_TEMP,
            if (ast->value[0] == 'a'){
                char value[50];
                value[0] = '.';
                strncpy(value+1, ast->value, 49);
                return STB_LANG_IR_OPERAND(IR_REG, arena_strdup(&g_arena, value));
                ;
            }else {
                return STB_LANG_IR_OPERAND(IR_REG, ast->value);
            }
        )
        STB_LANG_IR_CASE(AST_STRING,
            long offset = STB_CONCAT(CUR_IR_PREFIX, _symbol_new)(ir, ast->value, strlen(ast->value));
            return STB_LANG_IR_OPERAND(IR_MEM, (char*)offset);
        )
        STB_LANG_IR_CASE(AST_DECL,
            if (STB_LANG_RHS(ast) != NULL) {
                STB_LANG_IR_EMIT(IR_DECL, STB_LANG_IR_OPERAND_EXTRA(IR_VAR, ast->value, 1), STB_LANG_IR_RHS(ast), NULL);
            }
        )
        STB_LANG_IR_CASE(AST_INT,
            STB_LANG_IR_RETURN_SELF(IR_INT);
        )
        STB_LANG_IR_CASE(AST_FLOAT,
            STB_LANG_IR_RETURN_SELF(IR_FLOAT);
        )
        STB_LANG_IR_CASE(AST_FUNCALL,
            Lang_Parser_AST *params[64]; int paramslen = 0;
            STB_LANG_ITERATE_LINKED_LIST(ast->left, arg, Lang_Parser_AST,
                params[paramslen++] = arg;
            )
            int idx = -1;
            int varidx = -1;
            for (int i = 0; i < paramslen; i++) {
                if (params[i]->flags == STB_LANG_TYPEINFO_VARIADIC) {
                    varidx++;
                } else if (params[i]->typeinfo.type == AST_TYPE_FLOAT && params[i]->typeinfo.ptrnum == 0){
                } else {
                    idx++;
                }
            }
            long origidx = (long)idx;


            int vidx = 0;
            for (int i = paramslen - 1; i >= 0; i--) {
                Lang_Parser_AST *param = params[i];

                STB_CONCAT(CUR_IR_NAME, _Operand) *operand = lang_ir_ast(ir, param, 0);
                char str[32];
                if (param->flags != STB_LANG_TYPEINFO_VARIADIC) {
                    if (param->typeinfo.type == AST_TYPE_FLOAT && param->typeinfo.ptrnum == 0){
                        snprintf(str, 32, "v%d", vidx++); 
                    }else {
                        snprintf(str, 32, "a%d", idx--); 
                    }

                    STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND(IR_REG, arena_strdup(&g_arena, str)), operand, NULL, .typeinfo=param->typeinfo);
                } else {
                    snprintf(str, 32, ".arg%d", varidx--);
                    STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND(IR_REG, arena_strdup(&g_arena, str)), operand, NULL, .typeinfo=param->typeinfo);
                }
            }
            STB_LANG_IR_EMIT(IR_CALL, STB_LANG_IR_OPERAND(IR_VAR, ast->value), NULL, NULL, .typeinfo=(Lang_TypeInfo_Typeinfo){.type = origidx});
            return STB_LANG_IR_AS_TEMP(IR_REG, "v0")
        )
        STB_LANG_IR_CASE(AST_ADD,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_ADD, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast), .typeinfo=ast->typeinfo);
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_SUB,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_SUB, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast), .typeinfo=ast->typeinfo);
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_MUL,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_MUL, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast), .typeinfo=ast->typeinfo);
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_DIV,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_DIV, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast), .typeinfo=ast->typeinfo);
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_MODULO,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_MOD, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast), .typeinfo=ast->typeinfo);
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_BOR,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_BOR, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast), .typeinfo=ast->typeinfo);
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_BAND,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_BAND, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast), .typeinfo=ast->typeinfo);
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_BSHL,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_BSHL, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast), .typeinfo=ast->typeinfo);
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_BSHR,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_BSHR, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast), .typeinfo=ast->typeinfo);
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_AND,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_AND, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast), .typeinfo=ast->typeinfo);
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_OR,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_OR, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast), .typeinfo=ast->typeinfo);
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_XOR,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_XOR, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast), .typeinfo=ast->typeinfo);
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_LT,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_LT, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast), .typeinfo=ast->typeinfo);
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_LTE,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_LTE, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast), .typeinfo=ast->typeinfo);
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_GT,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_GT, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast), .typeinfo=ast->typeinfo);
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_GTE,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_GTE, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast), .typeinfo=ast->typeinfo);
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_EQ,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_EQ, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast), .typeinfo=ast->typeinfo);
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_NEQ,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_NEQ, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast), .typeinfo=ast->typeinfo);
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_IF,
            STB_LANG_IR_NEW_LABEL(label)
            STB_LANG_IR_NEW_LABEL(end)

            STB_CONCAT(CUR_IR_NAME, _Operand) *operand = STB_LANG_IR_LHS(ast);
            char *temp = STB_CONCAT(CUR_IR_PREFIX, _make_temp_reg_string)(ir);
            STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND(IR_REG, temp), operand, NULL);

            STB_LANG_IR_EMIT(IR_JUMP_IF_FALSE, STB_LANG_IR_LABEL(IR_VAR, label), operand, STB_LANG_IR_OPERAND(IR_REG, temp));
            STB_LANG_IR_BLOCK()
            STB_LANG_IR_EMIT(IR_JUMP, STB_LANG_IR_LABEL(IR_VAR, end), NULL, NULL);
            STB_LANG_IR_EMIT(IR_LABEL, STB_LANG_IR_LABEL(IR_VAR, label), operand, NULL);
            STB_LANG_IR_RUN(ast->middle);
            STB_LANG_IR_EMIT(IR_LABEL, STB_LANG_IR_LABEL(IR_VAR, end), operand, NULL);
        )
        STB_LANG_IR_CASE(AST_RET,
            STB_CONCAT(CUR_IR_NAME, _Operand) *operand = STB_LANG_IR_LHS(ast);
            STB_LANG_IR_EMIT(IR_RET, NULL, operand, NULL);
        )
        STB_LANG_IR_CASE(AST_CAST,
            return STB_LANG_IR_GET_OPERAND(ast->left);
        )
        STB_LANG_IR_CASE(AST_EXPR,
            return STB_LANG_IR_GET_OPERAND(ast->left);
        )
        STB_LANG_IR_CASE(AST_REF,
            STB_LANG_IR_NEW_TEMP(dest);
            STB_CONCAT(CUR_IR_NAME, _Operand) *operand = STB_LANG_IR_LHS_EXTRA(ast, 1);
            STB_LANG_IR_EMIT(IR_ADDR, STB_LANG_IR_AS_TEMP(IR_REG, dest), operand, NULL);
            return STB_LANG_IR_AS_TEMP(IR_REG, dest);
        )
        STB_LANG_IR_CASE(AST_DEREF,

            STB_LANG_IR_NEW_TEMP(dest);


            STB_CONCAT(CUR_IR_NAME, _Operand) *operand;
            operand = STB_LANG_IR_LHS(ast);


            if (extr == 0){
                STB_LANG_IR_EMIT(IR_LOAD, STB_LANG_IR_AS_TEMP(IR_REG, dest), operand, NULL, .typeinfo=ast->typeinfo);
            }else {
                STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_AS_TEMP(IR_REG, dest), operand, NULL, .typeinfo=ast->typeinfo);
            }

            return STB_LANG_IR_AS_TEMP(IR_REG, dest);
        )
        STB_LANG_IR_CASE(AST_INDEX,
            STB_LANG_IR_NEW_TEMP(dest);
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_NEW_TEMP(temp2);
            char str[32]; 
            int typ = STB_LANG_LHS(ast)->typeinfo.type;
            if (typ == AST_TYPE_ARRAY) {
                snprintf(str, 32, "%d", STB_LANG_LOOKUP_SIZE(ir->root_scope, STB_LANG_LHS(ast)->typeinfo.data.array.elem_type));
            } else {
                snprintf(str, 32, "%d", STB_LANG_LOOKUP_SIZE(ir->root_scope, &ast->typeinfo));
            }
            STB_LANG_IR_EMIT(IR_MUL, STB_LANG_IR_OPERAND(IR_REG, temp_reg), STB_LANG_IR_RHS(ast), STB_LANG_IR_OPERAND(IR_INT, arena_strdup(&g_arena, str)));
            if (STB_LANG_LHS(ast)->typeinfo.type == AST_TYPE_ARRAY) {
                STB_LANG_IR_EMIT(IR_ADDR, STB_LANG_IR_OPERAND(IR_REG, temp2), STB_LANG_IR_LHS_EXTRA(ast, 1), NULL);
            }else {
                STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND(IR_REG, temp2), STB_LANG_IR_LHS(ast), NULL);
            }
            STB_LANG_IR_EMIT(IR_ADD, STB_LANG_IR_OPERAND(IR_REG, temp2), STB_LANG_IR_OPERAND(IR_REG, temp2), STB_LANG_IR_OPERAND(IR_REG, temp_reg));
            STB_LANG_IR_EMIT(IR_LOAD, STB_LANG_IR_AS_TEMP(IR_REG, dest), STB_LANG_IR_OPERAND(IR_REG, temp2), NULL, .typeinfo=ast->typeinfo);
            return STB_LANG_IR_AS_TEMP(IR_REG, dest);
        )
        STB_LANG_IR_CASE(AST_WHILE,
            STB_LANG_IR_NEW_LABEL(label)
            STB_LANG_IR_EMIT(IR_LABEL, STB_LANG_IR_LABEL(IR_VAR, label), NULL, NULL);

            STB_LANG_IR_NEW_LABEL(label1)

            STB_CONCAT(CUR_IR_NAME, _Operand) *operand = STB_LANG_IR_LHS(ast);
            char *temp = STB_CONCAT(CUR_IR_PREFIX, _make_temp_reg_string)(ir);
            STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND(IR_REG, temp), operand, NULL);
            STB_LANG_IR_EMIT(IR_JUMP_IF_FALSE, STB_LANG_IR_LABEL(IR_VAR, label1), operand, STB_LANG_IR_OPERAND(IR_REG, temp));


            STB_LANG_IR_BLOCK()


            STB_LANG_IR_EMIT(IR_JUMP, STB_LANG_IR_LABEL(IR_VAR, label), operand, NULL);

            STB_LANG_IR_EMIT(IR_LABEL, STB_LANG_IR_LABEL(IR_VAR, label1), NULL, NULL);
        )
    )
);

typedef struct {
    Lang_IR_Operand *operand;
    Lang_IR_Instr *instr;
    int uses;
}Lang_Optimizer_Reg;
dymarray_typenew(Lang_Optimizer_Reg, 20, 5);
// dymarray_Lang_Optimizer_Reg

#define CUR_OPTIMIZER_NAME Lang_Optimizer
#define CUR_OPTIMIZER_PREFIX lang_optimizer


#define STB_LANG_GET_OPT_REG(reg, str) \
int num = -1; \
if (str[0] == 't'){ \
    num = atoi(str+1) + 1; \
}else if (strcmp(str, "v0") == 0){ \
     num = 0; \
}

#define STB_LANG_OPT_REG(str, ...) \
STB_LANG_GET_OPT_REG(num, str) \
if (num != -1){ \
    if (optimizer->regs.datalen > num){ \
        STB_LANG_OPTIMIZER_ERROR_MINOR(optimizer->files, instr->offset, instr->file, "OptimizerError", "Too few regs"); \
    }else { \
        optimizer->regs.data[num] = (Lang_Optimizer_Reg){.operand=__VA_ARGS__, .instr=instr}; \
    }; \
}

#define STB_LANG_GET_OPERAND(val, num) \
if (num != -1){ \
    if (optimizer->regs.datalen > num){ \
        STB_LANG_OPTIMIZER_ERROR_MINOR(optimizer->files, instr->offset, instr->file, "OptimizerError", "Could not access reg"); \
    }else { \
        val = &optimizer->regs.data[num]; \
        val->uses++; \
        if (val->uses > 1){ \
        } \
    }; \
}

#define STB_LANG_OPTIMIZE_OPERATION(op) \
STB_LANG_OPT_LHS(instr); \
STB_LANG_OPT_RHS(instr); \
if (instr->left->type == IR_INT && instr->right->type == IR_INT){ \
    char *left_value = instr->left->value; \
    char *right_value = instr->right->value; \
    if (left_value && right_value){ \
        int l = atoi(left_value); \
        int r = atoi(right_value); \
        char *res = arena_alloc(&g_arena, 10); \
        snprintf(res, 10, "%d", l op r); \
\
        instr->type = IR_ASSIGN; \
        instr->left->type = IR_INT; \
        instr->left->value = arena_strdup(&g_arena, res); \
        STB_LANG_OPTIMIZE(instr); \
\
    } \
}

        // free(left_value);
        // free(right_value);


STB_LANG_NEW_OPTIMIZER(
STB_LANG_OPTIMIZER_FIELDS(
    dymarray_Lang_Optimizer_Reg regs;
    int interpreted;
),
STB_LANG_OPTIMIZER_INIT(
    optimizer->regs = dymarray_Lang_Optimizer_Reg_new();
    for (int i=0; i<optimizer->regs.datalen; i++){
        optimizer->regs.data[i].operand = NULL;
        optimizer->regs.data[i].instr = NULL;
        optimizer->regs.data[i].uses = 0;
    };
    optimizer->interpreted = ir->interpreted;
),
STB_LANG_OPTIMIZER_OPERANDS(
    STB_LANG_OPTIMIZER_OPERAND(IR_REG,
        STB_LANG_GET_OPT_REG(num, operand->value);
        Lang_Optimizer_Reg *r = NULL;
        STB_LANG_GET_OPERAND(r, num);

        if (r != NULL){
            Lang_IR_Operand *op = r->operand;
            if (op != NULL){
                r->instr->type = IR_NOP;
                // free(operand);

                return op;
                // STB_LANG_OPT_OPERAND(operand, instr);
            }
        }
    )
),
STB_LANG_OPTIMIZER_CASES(
    STB_LANG_OPTIMIZER_CASE(IR_FUNCDEF_BEGIN,
    )
    STB_LANG_OPTIMIZER_CASE(IR_FUNCDEF_END,
    )
    STB_LANG_OPTIMIZER_CASE(IR_PUSH,
    )
    STB_LANG_OPTIMIZER_CASE(IR_POP,
        // if (instr->dest->type == IR_REG){
        //     STB_LANG_OPT_REG(instr->dest->value, NULL);
        // }
    )
    STB_LANG_OPTIMIZER_CASE(IR_CALL,
        // char *str = "v0";
        // STB_LANG_OPT_REG(str, NULL);
    )
    STB_LANG_OPTIMIZER_CASE(IR_JUMP_IF_FALSE,
    )
    STB_LANG_OPTIMIZER_CASE(IR_JUMP,
    )
    STB_LANG_OPTIMIZER_CASE(IR_LABEL,
    )
    STB_LANG_OPTIMIZER_CASE(IR_RET,
    )
    STB_LANG_OPTIMIZER_CASE(IR_ADDR,
        // if (instr->dest->type == IR_REG){
        //     STB_LANG_OPT_REG(instr->dest->value, NULL);
        // }
    )
    STB_LANG_OPTIMIZER_CASE(IR_LOAD,
        // if (instr->dest->type == IR_REG){
        //     STB_LANG_OPT_REG(instr->dest->value, NULL);
        // }
    )
    STB_LANG_OPTIMIZER_CASE(IR_STORE,
        // if (instr->dest->type == IR_REG){
        //     STB_LANG_OPT_REG(instr->dest->value, NULL);
        // }
    )
    STB_LANG_OPTIMIZER_CASE(IR_ASSIGN,
        // STB_LANG_OPT_LHS(instr);
        // STB_LANG_OPT_RHS(instr);
        // if (instr->dest->type == IR_REG){
        //     STB_LANG_OPT_REG(instr->dest->value, instr->left);
        // }
    )
    STB_LANG_OPTIMIZER_CASE(IR_DECL,
    )
    STB_LANG_OPTIMIZER_CASE(IR_ADD,
        // STB_LANG_OPTIMIZE_OPERATION(+)
    )
    STB_LANG_OPTIMIZER_CASE(IR_SUB,
        // STB_LANG_OPTIMIZE_OPERATION(-)
    )
    STB_LANG_OPTIMIZER_CASE(IR_MUL,
        // STB_LANG_OPTIMIZE_OPERATION(*)
    )
    STB_LANG_OPTIMIZER_CASE(IR_DIV,
        // STB_LANG_OPTIMIZE_OPERATION(/)
    )
    STB_LANG_OPTIMIZER_CASE(IR_MOD,
        // STB_LANG_OPTIMIZE_OPERATION(%)
    )
    STB_LANG_OPTIMIZER_CASE(IR_LT,
        // STB_LANG_OPTIMIZE_OPERATION(<)
    )
    STB_LANG_OPTIMIZER_CASE(IR_LTE,
        // STB_LANG_OPTIMIZE_OPERATION(<=)
    )
    STB_LANG_OPTIMIZER_CASE(IR_GT,
        // STB_LANG_OPTIMIZE_OPERATION(>)
    )
    STB_LANG_OPTIMIZER_CASE(IR_GTE,
        // STB_LANG_OPTIMIZE_OPERATION(>=)
    )
    STB_LANG_OPTIMIZER_CASE(IR_EQ,
        // STB_LANG_OPTIMIZE_OPERATION(==)
    )
    STB_LANG_OPTIMIZER_CASE(IR_NEQ,
        // STB_LANG_OPTIMIZE_OPERATION(!=)
    )
    STB_LANG_OPTIMIZER_CASE(IR_BOR,
        // STB_LANG_OPTIMIZE_OPERATION(|)
    )
    STB_LANG_OPTIMIZER_CASE(IR_BAND,
        // STB_LANG_OPTIMIZE_OPERATION(&)
    )
    STB_LANG_OPTIMIZER_CASE(IR_AND,
        // STB_LANG_OPTIMIZE_OPERATION(&&)
    )
    STB_LANG_OPTIMIZER_CASE(IR_OR,
        // STB_LANG_OPTIMIZE_OPERATION(||)
    )
    STB_LANG_OPTIMIZER_CASE(IR_XOR,
        // STB_LANG_OPTIMIZE_OPERATION(^)
    )
    STB_LANG_OPTIMIZER_CASE(IR_BSHL,
        // STB_LANG_OPTIMIZE_OPERATION(<<)
    )
    STB_LANG_OPTIMIZER_CASE(IR_BSHR,
        // STB_LANG_OPTIMIZE_OPERATION(>>)
    )
)
);


#define CUR_REGALLOC_NAME Lang_RegAlloc
#define CUR_REGALLOC_PREFIX lang_regalloc
STB_LANG_NEW_REGALLOC(
    STB_LANG_REGALLOC_FIELDS(
        int interpreted;
    ),
    STB_LANG_REGALLOC_INIT(
        regalloc->interpreted = optimizer->interpreted;
    ),
    STB_LANG_REGALLOC_REGISTERS(
        REG_X9, REG_X10, REG_X11, REG_X12, REG_X13, REG_X14, REG_X15,
        REG_X19, REG_X20
    ),
    STB_LANG_REGALLOC_REGISTER_NAMES(
        STB_LANG_REGALLOC_REGISTER_MATCH(REG_X9, "x9", "w9", "w9", "w9")
        STB_LANG_REGALLOC_REGISTER_MATCH(REG_X10, "x10", "w10", "w10", "w10")
        STB_LANG_REGALLOC_REGISTER_MATCH(REG_X11, "x11", "w11", "w11", "w11")
        STB_LANG_REGALLOC_REGISTER_MATCH(REG_X12, "x12", "w12", "w12", "w12")
        STB_LANG_REGALLOC_REGISTER_MATCH(REG_X13, "x13", "w13", "w13", "w13")
        STB_LANG_REGALLOC_REGISTER_MATCH(REG_X14, "x14", "w14", "w14", "w14")
        STB_LANG_REGALLOC_REGISTER_MATCH(REG_X15, "x15", "w15", "w15", "w15")
        STB_LANG_REGALLOC_REGISTER_MATCH(REG_X19, "x19", "w19", "w19", "w19")
        STB_LANG_REGALLOC_REGISTER_MATCH(REG_X20, "x20", "w20", "w20", "w20")

    ),
    // STB_LANG_REGALLOC_REGISTERS(
    //     REG_R10, REG_R11, REG_R12, REG_R13, REG_R14
    //     // Yes, r8 and r9 are technically not available as scratch registers, but we're using them as it for now for registers space
    //     // TODO: distinguish callee and caller registers (in regalloc phase)
    // ),
    // STB_LANG_REGALLOC_REGISTER_NAMES(
    //     STB_LANG_REGALLOC_REGISTER_MATCH(REG_R10, "r10", "r10", "r10", "r10b")
    //     STB_LANG_REGALLOC_REGISTER_MATCH(REG_R11, "r11", "r11", "r11", "r11b")
    //     STB_LANG_REGALLOC_REGISTER_MATCH(REG_R12, "r12", "r12", "r12", "r12b")
    //     STB_LANG_REGALLOC_REGISTER_MATCH(REG_R13, "r13", "r13", "r13", "r13b")
    //     STB_LANG_REGALLOC_REGISTER_MATCH(REG_R14, "r14", "r14", "r14", "r14b")
    //
    // ),
    // ^ For legacy x86_64
    STB_LANG_REGALLOC_LIST(
        STB_LANG_REGALLOC_CASE(IR_EXTERN,
        )
        STB_LANG_REGALLOC_CASE(IR_FUNCDEF_BEGIN,
            for (int i=0; i<regalloc->regs->datalen; i++){
                regalloc->regs->data[i].available = 1;
            }
        )
        STB_LANG_REGALLOC_CASE(IR_FUNCDEF_END,
        )
        STB_LANG_REGALLOC_2CASES(IR_ASSIGN, IR_DECL,
            STB_LANG_SAVE_REG(phys[0]);
        )
        STB_LANG_REGALLOC_CASE(IR_STORE,
            STB_LANG_SAVE_REG(phys[0], {
                STB_LANG_SAVE_REG(phys[1]);
            });
        )
        STB_LANG_REGALLOC_CASE(IR_JUMP_IF_FALSE,
        )
        STB_LANG_REGALLOC_CASE(IR_LABEL,
        )

        STB_LANG_REGALLOC_CASE(IR_PUSH,
            STB_LANG_SAVE_REG(phys[0]);
        )
        STB_LANG_REGALLOC_CASE(IR_POP,
        )

        STB_LANG_REGALLOC_2CASES(IR_ADD, IR_SUB,
            STB_LANG_SAVE_REG(phys[0], {
                if (instr->right->type != IR_INT){
                    STB_LANG_SAVE_REG(phys[1]);
                }else {
                    STB_LANG_REGALLOC_NEGATE(phys[1]);
                };
            })
        )
        STB_LANG_REGALLOC_6CASES(IR_MUL, IR_DIV, IR_BOR, IR_BAND, IR_AND, IR_OR,
            STB_LANG_SAVE_REG(phys[0], {
                STB_LANG_SAVE_REG(phys[1]);
            });
        )
        STB_LANG_REGALLOC_3CASES(IR_XOR, IR_BSHL, IR_BSHR,
            STB_LANG_SAVE_REG(phys[0], {
                STB_LANG_SAVE_REG(phys[1]);
            });
        )
        STB_LANG_REGALLOC_CASE(IR_ADDR,
        )
        STB_LANG_REGALLOC_CASE(IR_LOAD,
            STB_LANG_SAVE_REG(phys[0]);
        )
        STB_LANG_REGALLOC_CASE(IR_MOD,
            STB_LANG_SAVE_REG(phys[1], {
                STB_LANG_SAVE_REG(phys[0], {
                    STB_LANG_SAVE_REG(phys[2]); // Extra register for msub temp
                });
            });
        )
        STB_LANG_REGALLOC_6CASES(IR_LT, IR_LTE, IR_GT, IR_GTE, IR_EQ, IR_NEQ,
            STB_LANG_SAVE_REG(phys[0], {
                if (instr->right->type != IR_INT){
                    STB_LANG_SAVE_REG(phys[1]);
                }else {
                    STB_LANG_REGALLOC_NEGATE(phys[1]);
                };
            });
        )

        STB_LANG_REGALLOC_CASE(IR_CALL,
        )

        STB_LANG_REGALLOC_CASE(IR_SYSCALL3,
        )

        STB_LANG_REGALLOC_CASE(IR_RET,
        )

        STB_LANG_REGALLOC_CASE(IR_JUMP,
        )

        STB_LANG_REGALLOC_CASE(IR_NOP,
        )
    ),
    IR_REG
)


#define CUR_CODEGEN_NAME Lang_CodeGen_Arm
#define CUR_CODEGEN_PREFIX lang_codegen_arm

#include "arm.c"

#undef CUR_CODEGEN_NAME
#undef CUR_CODEGEN_PREFIX
#define CUR_CODEGEN_NAME Lang_CodeGen_Intrp
#define CUR_CODEGEN_PREFIX lang_codegen_intrp

#include "interpreted.c"

#undef CUR_CODEGEN_NAME
#undef CUR_CODEGEN_PREFIX
#define CUR_CODEGEN_NAME Lang_CodeGen_Arm
#define CUR_CODEGEN_PREFIX lang_codegen_arm


// #include "x86_64.c"
// ^ legacy x86_64


#define CUR_DRIVER_PREFIX lang_driver

void STB_LANG_INVOKE_DRIVER(Lang_CodeGen_Arm *gen, char *output){
    char *asm_path = "__res/main.s";
    STB_LANG_DRIVER_WRITE_DATA(asm_path);

    char exec_path[100];
    snprintf(exec_path, 100, "__res/%s", output);
    // STB_LANG_DRIVER_RUN_SCRIPT("cat %s", asm_path);
    STB_LANG_DRIVER_RUN_SCRIPT("clang -O0 -arch arm64 -c %s -o %s", asm_path, exec_path);
    // STB_LANG_DRIVER_RUN_SCRIPT( "yasm -f macho64 %s -o %s", asm_path, exec_path);
    // STB_LANG_DRIVER_RUN_SCRIPT(exec_instr, asm_path, exec_path);
    STB_LANG_DRIVER_RUN_SCRIPT("rm %s", asm_path);
}

void STB_LANG_DRIVER_LINK(char *objs, char *exec_path){

    char exec_instr[500];
    strncpy(exec_instr, "clang -O0 -arch arm64 %s -o %s -e _main -Wl,-w -Wl,-platform_version,macos,11.0,11.0 -lc", 500);
    
    for (int i=0; i<linker_data.libpaths.datalen; i++){
        strcat(exec_instr, " -L");
        strcat(exec_instr, linker_data.libpaths.data[i]);
    };
    
    for (int i=0; i<linker_data.libraries.datalen; i++){
        strcat(exec_instr, " lib");
        strcat(exec_instr, linker_data.libraries.data[i]);
        strcat(exec_instr, ".a");
    };
    
    for (int i=0; i<linker_data.frameworks.datalen; i++){
        strcat(exec_instr, " -framework ");
        strcat(exec_instr, linker_data.frameworks.data[i]);
    };
    STB_LANG_DRIVER_RUN_SCRIPT(exec_instr, objs, exec_path);
}


// STB_LANG_DRIVER_RUN_SCRIPT(
//     "ld -arch x86_64 %s -o %s -e _main -w -lSystem -syslibroot $(xcrun --show-sdk-path) -platform_version macos 11.0 11.0", 
//     obj_path, exec_path
// );
// ^ for legacy x86_64



void *lang_comp_data(Lang_Tokenizer_File file, char far, char driver, char entry, char *output, Lang_TypeInfo *typnf, int save){
/* Far variable possibilities
 * -1: Go all the way through
 * 0: Don't do anything
 * 1: Stop after tokenizer
 * 2: Stop after preprocessor
 * 3: Stop after parser
 * 4: Stop after typechecker
 * 5: Stop after IR generation
 * 6: Stop after optimization
 * 7: Stop after register allocation
 * 8: Stop after init typeinfo
*/

    Lang_TypeInfo *checker;

    if (typnf == (Lang_TypeInfo*)NULL){
        (void)driver;
        if (far < 0) far = -1;
        if (far == 0) return NULL;
        Lang_Tokenizer *tokenizer = lang_tokenizer_init(file);
        while (lang_tokenizer_token(tokenizer) == 0){
        }
        if (far != -1 && far == 1) return (void*)tokenizer;
        Lang_Preprocessor *processor = lang_preprocessor_init(tokenizer, 0);
        while (lang_preprocessor_token(processor) == 0){
        }

        if (far != -1 && far == 2) return (void*)processor;


        Lang_Parser *parser = lang_parser_init(processor);
        while (lang_parser_parse_body(parser) == 0){
        }
        if (far != -1 && far == 3) return (void*)parser;
        checker = lang_typeinfo_init(parser);
        if (save){
            while (lang_typeinfo_check(checker) == 0){
            }
        }
        if (far != -1 && far == 8) return (void*)checker;
    }else {
        checker = typnf;
    }
    while (lang_typeinfo_check(checker) == 0){
    }
    if (entry){
        STB_LANG_FIND_FUNCTION_UNDERLYING(
            checker->root_scope, "main", 

            (void)symnew;
        )else {
            stb_lang_error_major_global_underlying("EntryError", "No entry point could be found");
            stb_lang_error_hint("add a `main` function", "int main(){\n\treturn 0;\n}");
        }
    }

    if (far != -1 && far == 4) return (void*)checker;
    Lang_IR *ir = lang_ir_init(checker);


    while (lang_ir_translate(ir) == 0){
    }
    if (far != -1 && far == 5) return (void*)ir;


    Lang_Optimizer *optimizer = lang_optimizer_init(ir);
    while (lang_optimizer_optimize(optimizer) == 0){

    }
    if (far != -1 && far == 6) return (void*)optimizer;


    Lang_RegAlloc *regalloc = lang_regalloc_init(optimizer);
    lang_regalloc_backtrace(regalloc);
    while (lang_regalloc_alloc(regalloc) == 0){
    }
    if (far != -1 && far == 7) return (void*)regalloc;


    if (regalloc->interpreted == 0){
        Lang_CodeGen_Arm *gen = lang_codegen_arm_init(regalloc);
        while (lang_codegen_arm_ir(gen) == 0){
        }
        STB_LANG_INVOKE_DRIVER(gen, output);
    }else {
        // Basic interpreter -- in testing
        fprintf(stderr, "----- INTERPRETER -----\n");
        Lang_CodeGen_Intrp *gen = lang_codegen_intrp_init(regalloc);
        while (lang_codegen_intrp_ir(gen) == 0){
        }
    }
    return NULL;
};

#define lang_comp_data_from_file(input_file, ...) ({\
    if (input_file == NULL){ \
        stb_lang_error_major_global("ArgsError", "No input file provided"); \
    } \
    lang_comp_data(lang_tokenizer_file_init(input_file), __VA_ARGS__); \
})



#define lang_comp_data_from_text(n, t, ...) ({\
Lang_Tokenizer_File file; \
file.file = NULL; \
file.name = n; \
file.contents = t; \
file.contentlen = strlen(t); \
lang_comp_data(file, __VA_ARGS__); \
})


int main(int argc, char **argv){
    arena_reset(&g_arena);


    linker_data.frameworks = dymarray_String_new();
    linker_data.libraries = dymarray_String_new();
    linker_data.libpaths = dymarray_String_new();
    linker_data.structexports = dymarray_String_new();

    char *input_file = NULL;
    char *output_file = NULL;
    (void)input_file;
    (void)output_file;
    for (int i=1; i<argc; i++){
        char *str = argv[i];
        if (str == NULL){break;};
        if (strcmp(str, "-help") == 0){
            fprintf(stderr, "%s", HELP);
            stb_lang_exit(-1);
        }else if (strcmp(str, "-o") == 0){
            output_file = (char*)1;
            continue;
        }else if (output_file == (char*)1){
            output_file = str;
        }else {
            input_file = str;
        }
        if (output_file == (char*)1){
            output_file = NULL;
        }
    }
    if (input_file == NULL){
        fprintf(stderr, "No files given\n");
        stb_lang_exit(-1);
    }
    if (output_file == (char*)1 || output_file == NULL){
        output_file = "main";
    };

    int count = 0;
    char *input_file_dir = strdup(input_file);
    for (int i=strlen(input_file_dir); i>0; i--){
        if (input_file_dir[i] == '/'){
            count++;
            input_file_dir[i+1] = '\0';
            break;
        };
    }
    if (count == 0){
        free(input_file_dir);
        input_file_dir = arena_alloc(&g_arena, 1);
        input_file_dir[0] = '\0';
    }


    STB_LANG_DRIVER_RUN_SCRIPT("mkdir __res");
    
    Lang_TypeInfo *typnf = lang_comp_data_from_file(input_file, 8, 0, 1, "main.o", NULL, 0);



    char *newone = malloc(200);
    strncpy(newone, "__res/main.o", 200);


    for (int i=0; i<linker_data.modules.datalen; i++){
        char *str = malloc(100);
        snprintf(str, 100, "%s", input_file_dir);
        char *prntstr = strdup(linker_data.modules.data[i]);
        for (int i=0; i<(int)strlen(prntstr); i++){
            char ch = prntstr[i];
            if (ch == '.'){
                ch = '/';
            };
            int ln = strlen(str);
            str[ln] = ch;
            str[ln + 1] = '\0';
        }
        strcat(str, ".bsh");

        // snprintf(str, 100, "%s%s.bsh", input_file_dir, linker_data.modules.data[i]);
        char *str2 = malloc(100);
        snprintf(str2, 100, "%s.o", linker_data.modules.data[i]);
            // Only temporary
        // lang_comp_data_from_file(str, -1, 0, 0, str2, NULL);
        Lang_TypeInfo *typn = lang_comp_data_from_file(str, 8, 0, 1, str2, NULL, 1);
        Lang_TypeInfo_Scope *scope = (Lang_TypeInfo_Scope*)typn->root_scope;
        for (int i=0; i<scope->symbols.datalen; i++){
            if (scope->symbols.data[i].kind == STB_LANG_SYMBOL_FUNCTION){
                for (int v=0; v<linker_data.exports.datalen; v++){ // To fix later, slow
                    if (strcmp(scope->symbols.data[i].name, linker_data.exports.data[v]) == 0){
                        dymarray_Lang_TypeInfo_Symbol_add(&((Lang_TypeInfo_Scope*)typnf->root_scope)->symbols, scope->symbols.data[i]);
                    }
                }
            }else if (scope->symbols.data[i].kind == STB_LANG_SYMBOL_DATA){
                for (int v=0; v<linker_data.structexports.datalen; v++){ // To fix later, slow
                    if (strcmp(scope->symbols.data[i].name, linker_data.structexports.data[v]) == 0){
                        dymarray_Lang_TypeInfo_Symbol_add(&((Lang_TypeInfo_Scope*)typnf->root_scope)->symbols, scope->symbols.data[i]);
                    }
                }
            }
        }
        lang_comp_data_from_file(str, -1, 0, 0, str2, typn, 1);


        snprintf(str2, 100, "__res/%s.o", linker_data.modules.data[i]);
        strncat(newone, " ", 1);
        strcat(newone, str2);

        free(str);
        free(str2);
    }
    lang_comp_data_from_file(input_file, -1, 0, 1, "main.o", typnf, 0);


    STB_LANG_DRIVER_LINK(newone, output_file);
    arena_free(&g_arena);
    STB_LANG_DRIVER_RUN_SCRIPT("rm -rf __res");
    free(newone);


    // printf("-------- ASSEMBLY CODE --------\n");
    // printf("%s", gen->code.data);
    // printf("-------------------------------\n");
    return 0;
}
