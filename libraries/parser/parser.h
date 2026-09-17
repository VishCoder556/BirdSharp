/* PARSER.H library
 * Yes, there's documentation for this library and not the tokenizer library.
 * This library is going to be a bit more complicated so it needs some planning
*/

#ifndef STB_LANG_PARSER_H
#define STB_LANG_PARSER_H
#include <stdio.h>
#include <stdlib.h>
#include "../linkedlist/linkedlist.h"

// Some hacky macro stuff
#define STB_CONCAT_RAW(a, b) a##b
#define STB_CONCAT3_RAW(a, b, c) a##b##c

#define STB_CONCAT_EVAL(a, b) STB_CONCAT_RAW(a, b)
#define STB_CONCAT3_EVAL(a, b, c) STB_CONCAT3_RAW(a, b, c)

#define STB_CONCAT(a, b) STB_CONCAT_EVAL(a, b)
#define STB_CONCAT3(a, b, c) STB_CONCAT3_EVAL(a, b, c)


#define STB_LANG_PARSER_ERROR_MINOR(where, fil, type, ...) \
if (fil > parser->files.datalen){ \
    stb_lang_error_minor(parser->file.name, parser->file.contents, where, "ParserError", "Failure to generate error"); \
} \
stb_lang_error_minor(parser->files.data[fil].name, parser->files.data[fil].contents, where, type, __VA_ARGS__);

#define STB_LANG_PARSER_ERROR_FALLOUT(where) \
STB_LANG_PARSER_ERROR_MINOR(old_token.offset, old_token.file, "ParserError", "Could not parse %s", where);

#define STB_LANG_ASTS(...) __VA_ARGS__
#define STB_LANG_PARSE_BODY(...) __VA_ARGS__; STB_LANG_PARSER_ERROR_FALLOUT("body statement");
#define STB_LANG_PARSE_AST(...) __VA_ARGS__; STB_LANG_PARSER_ERROR_FALLOUT("statement");
#define STB_LANG_PARSE_EXPR(...) __VA_ARGS__;STB_LANG_PARSER_ERROR_FALLOUT("expression"); 
#define STB_LANG_PARSE_TYPEINFO(...) __VA_ARGS__;
#define STB_LANG_BINDING_POWER(...) __VA_ARGS__
#define STB_LANG_PARSER_FIELDS(...) __VA_ARGS__
#define STB_LANG_PARSER_INIT(...) __VA_ARGS__
#define STB_LANG_PARSER_SUFFIX(...) __VA_ARGS__
#define STB_LANG_PARSER_FUNCS(...) __VA_ARGS__
#define STB_LANG_MATCH_BINDING_POWER(t, v) case t: return v;

#define STB_LANG_PARSER_IN_BOUNDS() (parser->cursor + 1 < parser->tokens.datalen)

#define STB_LANG_PARSER_UPDATE() token = parser->tokens.data[parser->cursor];
#define STB_LANG_PARSER_ADVANCE(...) if (STB_CONCAT(CUR_PARSER_PREFIX, _advance)(parser) == -1){;STB_LANG_PARSER_ERROR_MINOR(token.offset, token.file, "OutOfBoundsError", "Went out of bounds to peek");}else {__VA_ARGS__; STB_LANG_PARSER_UPDATE()};

#define STB_LANG_PARSER_BACK(...) if (STB_CONCAT(CUR_PARSER_PREFIX, _back)(parser) == -1){STB_LANG_PARSER_ERROR_MINOR(token.offset, token.file, "OutOfBoundsError", "Went out of bounds to back up");}else {__VA_ARGS__; STB_LANG_PARSER_UPDATE()};


#define STB_LANG_MATCH_TOKEN(typ, ...) else if (token.type == typ) {match_token = token; __VA_ARGS__;}
#define STB_LANG_MATCH_VALUE(typ, val, ...) else if(token.type == typ) \
{ \
    if (strcmp(token.value, val) == 0){\
        STB_LANG_PARSER_ADVANCE();__VA_ARGS__; \
        break; \
    }; \
break; \
}

#define STB_LANG_SAVE(name, ...) typeof(__VA_ARGS__) name = __VA_ARGS__;

#define STB_LANG_IF_TOKEN(typ, ...) if (token.type == typ) {match_token = token; __VA_ARGS__;}
#define STB_LANG_ELSE_IF_TOKEN(...) else STB_LANG_IF_TOKEN(__VA_ARGS__)
#define STB_LANG_IF_VALUE(typ, val, ...) if (token.type == typ) {if (strcmp(token.value, val) == 0){match_token = token;__VA_ARGS__;};}

#define STB_LANG_ELSE(...) else{__VA_ARGS__;}


// STB_LANG_PARSER_EXPECT: yes, the error messages are bland and weird, this is a TODO for later
#define STB_LANG_PARSER_EXPECT(typ) \
if (token.type != typ){ \
    STB_LANG_PARSER_ERROR_MINOR(token.offset, token.file, "ExpectError", "Expected token '%d', got '%d'", typ, token.type); \
}else {STB_LANG_PARSER_ADVANCE();}


#define STB_LANG_PARSER_EXPECT_IN_PLACE(typ) \
if (token.type != typ){ \
    STB_LANG_PARSER_ERROR_MINOR(token.offset, token.file, "ExpectError", "Expected token '%d', got '%d'", typ, token.type); \
}

#define STB_LANG_TOKEN_MATCH_AST(tok, ast) else if (op_token.type == tok){ \
    parent->type = ast; \
    STB_LANG_PARSER_ADVANCE(); \
    STB_CONCAT(CUR_PARSER_NAME, _AST) *right = STB_CONCAT(CUR_PARSER_PREFIX, _parse_expr)(parser, next_bp); \
 \
    parent->typeinfo = (STB_CONCAT(CUR_TYPEINFO_NAME, _Typeinfo)){.type = -1}; \
    parent->value = op_token.value; \
    parent->left = (struct STB_CONCAT(CUR_PARSER_NAME, _AST)*)left; \
    parent->right = (struct STB_CONCAT(CUR_PARSER_NAME, _AST)*)right; \
    parent->next = NULL; \
    parent->offset = offset; \
 \
    left = parent; \
}


#define STB_LANG_TOKEN_MATCH_AST_CUSTOM(tok, ast, ...) else if (op_token.type == tok){ \
    parent->type = ast; \
    parent->typeinfo = (STB_CONCAT(CUR_TYPEINFO_NAME, _Typeinfo)){.type = -1}; \
    parent->value = op_token.value; \
    parent->left = (struct STB_CONCAT(CUR_PARSER_NAME, _AST)*)left; \
    parent->next = NULL; \
    parent->offset = offset; \
    __VA_ARGS__; \
 \
    left = parent; \
}


#define STB_LANG_PRATT_PARSER(...) \
if (left == NULL) { STB_LANG_PARSER_ERROR_FALLOUT("expression"); } \
while (parser->cursor < parser->tokens.datalen) { \
    STB_CONCAT(CUR_TOKENIZER_NAME, _Token) next_tok = parser->tokens.data[parser->cursor]; \
    int next_bp = STB_CONCAT(CUR_PARSER_PREFIX, _binding_power)(next_tok.type); \
    if (next_bp <= binding_power) { \
        break; \
    } \
    STB_CONCAT(CUR_PARSER_NAME, _AST) *parent = malloc(sizeof(*parent)); \
    STB_CONCAT(CUR_TOKENIZER_NAME, _Token) op_token = next_tok; \
    if (0){}__VA_ARGS__ else {break;}; \
\
 \
} \
 \
return left;



// The typeinfo of an AST can be left as -1 if it's unknown

#define STB_LANG_NEW_PARSER(bp_code, types, _newfields, code_init, code_suffix, _funcs, _body, _ast, _expr, _typeinfo) \
typedef enum { \
    STB_LANG_AST_NONE = -2, \
    STB_LANG_AST_TYPEINFO = -1, \
    types \
}STB_CONCAT(CUR_PARSER_NAME, _ASTType); \
typedef struct { \
    STB_CONCAT(CUR_PARSER_NAME, _ASTType) type;\
    char *value; \
    char flags; \
    struct STB_CONCAT(CUR_PARSER_NAME, _AST) *left;\
    struct STB_CONCAT(CUR_PARSER_NAME, _AST) *middle;\
    struct STB_CONCAT(CUR_PARSER_NAME, _AST) *right;\
    struct STB_CONCAT(CUR_PARSER_NAME, _AST) *next; \
    int offset; \
    int file; \
    STB_CONCAT(CUR_TYPEINFO_NAME, _Typeinfo) typeinfo; \
}STB_CONCAT(CUR_PARSER_NAME, _AST); \
typedef struct { \
    LinkedList(STB_CONCAT(CUR_PARSER_NAME, _AST)); \
}STB_CONCAT(CUR_PARSER_NAME, _ASTList); \
typedef struct { \
    STB_CONCAT3(dymarray_, CUR_TOKENIZER_NAME, _Token) tokens; \
    LinkedList(STB_CONCAT(CUR_PARSER_NAME, _AST)); \
    int cursor; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _File) file; \
    STB_CONCAT3(dymarray_, CUR_TOKENIZER_NAME, _File) files; \
    _newfields; \
}CUR_PARSER_NAME; \
int STB_CONCAT(CUR_PARSER_PREFIX, _binding_power)(STB_CONCAT(CUR_TOKENIZER_NAME, _TokenType) type){ \
    switch(type){ \
        bp_code; \
        default: return 0; \
    }; \
    return 0; \
}; \
CUR_PARSER_NAME *STB_CONCAT(CUR_PARSER_PREFIX, _init)(CUR_PREPROCESSOR_NAME *processor) { \
    CUR_PARSER_NAME *parser = malloc(sizeof(*parser)); \
    parser->cursor = 0; \
    parser->tokens = processor->tokens; \
    InitLinkedList((*parser), STB_CONCAT(CUR_PARSER_NAME, _AST)); \
    parser->file = processor->file; \
    parser->files = processor->files; \
    code_init; \
    return parser; \
} \
char STB_CONCAT(CUR_PARSER_PREFIX, _advance)(CUR_PARSER_NAME *parser) { \
    if (parser->cursor >= parser->tokens.datalen){ \
        return -1; \
    } \
    parser->cursor++; \
    return 0;\
} \
char STB_CONCAT(CUR_PARSER_PREFIX, _back)(CUR_PARSER_NAME *parser) { \
    if (parser->cursor <= 0){ \
        return -1; \
    } \
    parser->cursor--; \
    return 0;\
} \
STB_CONCAT(CUR_TYPEINFO_NAME, _Typeinfo) STB_CONCAT(CUR_PARSER_PREFIX, _parse_typeinfo)(CUR_PARSER_NAME *parser) { \
    STB_CONCAT(CUR_TOKENIZER_NAME, _Token) token = parser->tokens.data[parser->cursor]; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _Token) match_token = parser->tokens.data[parser->cursor]; \
    (void)match_token; \
    ; \
    STB_CONCAT(CUR_TYPEINFO_NAME, _Typeinfo) TypeInfo; \
    ; \
    _typeinfo; \
    return (STB_CONCAT(CUR_TYPEINFO_NAME, _Typeinfo)){.type = -1}; \
}; \
STB_CONCAT(CUR_PARSER_NAME, _AST) *STB_CONCAT(CUR_PARSER_PREFIX, _parse_expr)(CUR_PARSER_NAME *parser, int binding_power); \
STB_CONCAT(CUR_PARSER_NAME, _AST) *STB_CONCAT(CUR_PARSER_PREFIX, _parse_ast)(CUR_PARSER_NAME *parser); \
STB_CONCAT(CUR_PARSER_NAME, _AST) *STB_CONCAT(CUR_PARSER_PREFIX, _parse_body_ast)(CUR_PARSER_NAME *parser); \
char STB_CONCAT(CUR_PARSER_PREFIX, _parse_body)(CUR_PARSER_NAME *parser); \
_funcs; \
STB_CONCAT(CUR_PARSER_NAME, _AST) *STB_CONCAT(CUR_PARSER_PREFIX, _parse_expr)(CUR_PARSER_NAME *parser, int binding_power) { \
    if (parser->cursor >= parser->tokens.datalen) {goto exit;} \
    int Generic; \
    STB_CONCAT(CUR_TYPEINFO_NAME, _Typeinfo) TypeInfo; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _Token) token = parser->tokens.data[parser->cursor]; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _Token) old_token = token; \
    int offset = token.offset; \
    int file = token.file; \
    int initial_cursor = parser->cursor; \
    (void)initial_cursor; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _Token) match_token = parser->tokens.data[parser->cursor]; \
    STB_CONCAT(CUR_PARSER_NAME, _AST) *left = NULL; \
    if (0){}_expr; \
    if (left == NULL) {return NULL;}; \
exit: \
    return NULL; \
} \
STB_CONCAT(CUR_PARSER_NAME, _AST) *STB_CONCAT(CUR_PARSER_PREFIX, _parse_ast)(CUR_PARSER_NAME *parser) { \
    if (parser->cursor >= parser->tokens.datalen) {goto exit;} \
    int Generic; \
    STB_CONCAT(CUR_TYPEINFO_NAME, _Typeinfo) TypeInfo; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _Token) token = parser->tokens.data[parser->cursor]; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _Token) old_token = token; \
    int offset = token.offset; \
    int file = token.file; \
    int initial_cursor = parser->cursor; \
    (void)initial_cursor; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _Token) match_token = parser->tokens.data[parser->cursor]; \
    if (0){}_ast; \
exit: \
    return NULL; \
} \
STB_CONCAT(CUR_PARSER_NAME, _AST) *STB_CONCAT(CUR_PARSER_PREFIX, _parse_body_ast)(CUR_PARSER_NAME *parser) { \
    if (parser->cursor + 1 >= parser->tokens.datalen) {return NULL;} \
    int Generic; \
    STB_CONCAT(CUR_TYPEINFO_NAME, _Typeinfo) TypeInfo; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _Token) token = parser->tokens.data[parser->cursor]; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _Token) old_token = token; \
    int offset = token.offset; \
    int file = token.file; \
    int initial_cursor = parser->cursor; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _Token) match_token = parser->tokens.data[parser->cursor]; \
    if (0){}_body; \
    return NULL; \
} \
char STB_CONCAT(CUR_PARSER_PREFIX, _parse_body)(CUR_PARSER_NAME *parser) { \
    STB_CONCAT(CUR_TOKENIZER_NAME, _Token) token = parser->tokens.data[parser->cursor]; \
    int offset = token.offset; \
    int file = token.file; \
    int initial_cursor = parser->cursor; \
    STB_CONCAT(CUR_PARSER_NAME, _AST) *ast = (STB_CONCAT(CUR_PARSER_PREFIX, _parse_body_ast)(parser)); \
    if (ast == NULL){ \
        code_suffix; \
        return -1; \
    } \
    AppendToLinkedList((*parser), STB_CONCAT(CUR_PARSER_NAME, _AST), *ast); \
    if (parser->cursor == initial_cursor){ \
        return STB_CONCAT(CUR_PARSER_PREFIX, _advance)(parser);\
    }else if (parser->cursor + 1 >= parser->tokens.datalen){ \
        code_suffix; \
        return -1; \
    } \
    return 0; \
}

#define STB_LANG_APPEND_AST(ast) \
if (ast != NULL){ \
    AppendToLinkedList((*parser), STB_CONCAT(CUR_PARSER_NAME, _AST), *ast); \
} \

// ^ You'll notice an `int Generic` in some of these functions, it's a generic integer that can be used in macros 



#define STB_LANG_MATCH_TYPEINFO(val, typeinfo) if(strcmp(match_token.value, val) == 0) {return typeinfo;}

#define STB_LANG_TYPEINFO_SIZE(...) \
int STB_CONCAT3(CUR_TYPEINFO_PREFIX, _typeinfo, _lookup_size)(STB_CONCAT(CUR_TYPEINFO_NAME, _Typeinfo) typeinfo, STB_CONCAT(CUR_TYPEINFO_NAME, _ScopeL) root_scope){ \
    if (typeinfo.type == -1) return -1; \
    __VA_ARGS__; \
    return -1; \
}

#define STB_LANG_LOOKUP_SIZE(root_scope, typeinf) STB_CONCAT3(CUR_TYPEINFO_PREFIX, _typeinfo, _lookup_size)(*(STB_CONCAT(CUR_TYPEINFO_NAME, _Typeinfo)*)typeinf, root_scope)


#define STB_LANG_DEFINE_TYPEINFO(...) \
typedef enum { \
    STB_LANG_TYPEINFO_VARIADIC = -2, \
    STB_LANG_TYPEINFO_NONE = -1, \
    __VA_ARGS__ \
}STB_CONCAT(CUR_TYPEINFO_NAME, _TypeinfoType); \
struct STB_CONCAT(CUR_TYPEINFO_NAME, _Symbol); \
struct STB_CONCAT(CUR_PARSER_NAME, _AST); \
typedef struct { \
    STB_CONCAT(CUR_TYPEINFO_NAME, _TypeinfoType) type; \
    int ptrnum; \
    union { \
        struct { \
            int size; \
            struct AST_TypeInfo *elem_type; \
        } array; \
        struct { \
            int size; \
            struct STB_CONCAT(CUR_TYPEINFO_NAME, _Symbol) *symbol; \
            char *name; \
        }struct1; \
    } data; \
}STB_CONCAT(CUR_TYPEINFO_NAME, _Typeinfo); \
typedef enum { \
    STB_LANG_SYMBOL_VARIABLE, \
    STB_LANG_SYMBOL_FUNCTION, \
    STB_LANG_SYMBOL_DATA \
}STB_CONCAT(CUR_TYPEINFO_NAME, _SymbolKind); \
STB_LANG_INVOKE_TYPENEW(STB_CONCAT(CUR_TYPEINFO_NAME, _Typeinfo)); \
typedef struct { \
    char *name; \
    STB_CONCAT(CUR_TYPEINFO_NAME, _Typeinfo) typeinfo; \
    STB_CONCAT(CUR_TYPEINFO_NAME, _SymbolKind) kind; \
    union {; \
        struct { \
            int offset; \
        }variable; \
        struct { \
            STB_CONCAT3(dymarray_, CUR_TYPEINFO_NAME, _Typeinfo) *args; \
        }function; \
        struct { \
            struct STB_CONCAT(CUR_PARSER_NAME, _AST) *structdef; \
        }struct1; \
    }data; \
}STB_CONCAT(CUR_TYPEINFO_NAME, _Symbol);\
STB_LANG_INVOKE_TYPENEW(STB_CONCAT(CUR_TYPEINFO_NAME, _Symbol)); \
typedef struct STB_CONCAT(CUR_TYPEINFO_NAME, _Scope)* STB_CONCAT(CUR_TYPEINFO_NAME, _ScopeL); \
STB_LANG_INVOKE_TYPENEW(STB_CONCAT(CUR_TYPEINFO_NAME, _ScopeL)); \
typedef struct { \
    STB_CONCAT(CUR_TYPEINFO_NAME, _ScopeL) parent; \
    STB_CONCAT3(dymarray_, CUR_TYPEINFO_NAME, _ScopeL) children; \
    STB_CONCAT3(dymarray_, CUR_TYPEINFO_NAME, _Symbol) symbols; \
    char *name; \
}STB_CONCAT(CUR_TYPEINFO_NAME, _Scope);
// ^ some typeinfo stuff for the user

#define STB_LANG_TYPEINFO(...) (STB_CONCAT(CUR_TYPEINFO_NAME, _Typeinfo)){__VA_ARGS__}

#define STB_LANG_AST_LITERAL(typ, token) \
({ \
    STB_CONCAT(CUR_PARSER_NAME, _AST) *_n = malloc(sizeof(*_n)); \
    _n->typeinfo = (STB_CONCAT(CUR_TYPEINFO_NAME, _Typeinfo)){.type = STB_LANG_TYPEINFO_NONE}; \
    _n->type = typ; \
    _n->value = token.value; \
    _n->left = NULL; \
    _n->middle = NULL; \
    _n->right = NULL; \
    _n->next = NULL; \
    _n->offset = offset; \
    _n->file = file; \
    _n; \
})

#define STB_LANG_AS_AST(...) (struct STB_CONCAT(CUR_PARSER_NAME, _AST)*)__VA_ARGS__
#define STB_LANG_OF_AST(from, what) ((STB_CONCAT(CUR_PARSER_NAME, _AST)*)from)->what
#define STB_LANG_LINKED_LIST(what) STB_LANG_AS_AST(GetLinkedListHead(what, STB_CONCAT(CUR_PARSER_NAME, _AST)))


#define STB_LANG_AST(...) \
({ \
    STB_CONCAT(CUR_PARSER_NAME, _AST) *_n = malloc(sizeof(*_n)); \
    *_n = (STB_CONCAT(CUR_PARSER_NAME, _AST)){__VA_ARGS__}; \
    _n->offset = offset; \
    _n->file = file; \
    _n->flags = 0; \
    _n; \
})

#define STB_LANG_AST_FUNCALL(typ, name, params) \
({ \
    STB_CONCAT(CUR_PARSER_NAME, _AST) *_n = malloc(sizeof(*_n)); \
    _n->typeinfo = (STB_CONCAT(CUR_TYPEINFO_NAME, _Typeinfo)){.type = STB_LANG_TYPEINFO_NONE}; \
    _n->type = typ; \
    _n->value = name.value; \
    _n->left = (struct STB_CONCAT(CUR_PARSER_NAME, _AST)*)GetLinkedListHead(params, STB_CONCAT(CUR_PARSER_NAME, _AST)); \
    _n->right = NULL; \
    _n->next = NULL; \
    _n->offset = offset; \
    _n->file = file; \
    _n->flags = 0; \
    _n; \
})

#define STB_LANG_OPERAND(var, val) \
    typeof(val) var = val; \
    STB_LANG_PARSER_UPDATE();

#define STB_LANG_PARSE_CUSTOM_LIST(starttok, splitA, endtok, ...) \
STB_LANG_PARSER_EXPECT(starttok) \
Generic = 0; \
while (token.type != endtok){ \
    STB_LANG_SAVE(old_tok, token); \
    (void)old_tok; \
    STB_LANG_PARSER_UPDATE(); \
    __VA_ARGS__; \
    STB_LANG_PARSER_UPDATE(); \
    if ((int)token.type == splitA && splitA != -1) { \
        Generic = 1; \
        STB_LANG_PARSER_ADVANCE(); \
    }else {Generic = 0;} \
} \
if (Generic == 1){ \
    STB_LANG_PARSER_ERROR_MINOR(token.offset, token.file, "ExprListError", "Split token found before end"); \
} \
STB_LANG_PARSER_EXPECT(endtok)


#define STB_LANG_GET_AST_EXPR(where, binding_power) STB_CONCAT(CUR_PARSER_NAME, _AST) *where = STB_CONCAT(CUR_PARSER_PREFIX, _parse_expr)(parser, binding_power); STB_LANG_PARSER_UPDATE();

#define STB_LANG_PARSE_EXPR_LIST(into, starttok, splitA, endtok) \
STB_CONCAT(CUR_PARSER_NAME, _ASTList) into = (STB_CONCAT(CUR_PARSER_NAME, _ASTList)){0}; \
InitLinkedList(into, STB_CONCAT(CUR_PARSER_NAME, _AST)); \
STB_LANG_PARSE_CUSTOM_LIST(starttok, splitA, endtok,  \
    STB_LANG_GET_AST_EXPR(ast, 0); \
    AppendToLinkedList(into, STB_CONCAT(CUR_PARSER_NAME, _AST), *ast); \
)


#define STB_LANG_GET_TYPEINFO(name) \
TypeInfo = STB_CONCAT(CUR_PARSER_PREFIX, _parse_typeinfo)(parser); \
STB_LANG_SAVE(name, TypeInfo); \
STB_LANG_PARSER_UPDATE(); \
if (name.type != -1)

#define STB_LANG_PARSE_STATEMENT() (STB_CONCAT(CUR_PARSER_PREFIX, _parse_ast)(parser))
#define STB_LANG_PARSE_TOP() (STB_CONCAT(CUR_PARSER_PREFIX, _parse_body_ast)(parser))

#define STB_LANG_PARSE_STATEMENT_LIST(into, starttok, splitA, endtok) \
if (starttok != -1){ \
STB_LANG_PARSER_EXPECT(starttok) \
}\
Generic = 0; \
STB_CONCAT(CUR_PARSER_NAME, _ASTList) into = (STB_CONCAT(CUR_PARSER_NAME, _ASTList)){0}; \
InitLinkedList(into, STB_CONCAT(CUR_PARSER_NAME, _AST)); \
while (token.type != endtok){ \
    STB_LANG_SAVE(old_tok, token); \
    (void)old_tok; \
    STB_CONCAT(CUR_PARSER_NAME, _AST) *ast = (STB_CONCAT(CUR_PARSER_PREFIX, _parse_ast)(parser)); \
    if (ast == NULL) break; \
    if (ast->type == STB_LANG_AST_NONE) {STB_LANG_PARSER_ADVANCE();} \
    AppendToLinkedList(into, STB_CONCAT(CUR_PARSER_NAME, _AST), *ast); \
    STB_LANG_PARSER_UPDATE(); \
    if ((int)token.type == splitA && splitA != -1) { \
        Generic = 1; \
        STB_LANG_PARSER_ADVANCE(); \
    }else {Generic = 0;} \
}; \
if (Generic == 1){ \
    STB_LANG_PARSER_ERROR_MINOR(token.offset, token.file, "ExprListError", "Generic found before end"); \
} \
if (endtok != -1){ \
    STB_LANG_PARSER_EXPECT(endtok) \
}


// For C-like arguments

#endif
