#ifndef STB_LANG_TOKENIZER_H
#define STB_LANG_TOKENIZER_H

#include "../dymarray/dymarray.h"
#include <ctype.h>


// Some hacky macro stuff
#define STB_CONCAT_RAW(a, b) a##b
#define STB_CONCAT3_RAW(a, b, c) a##b##c

#define STB_CONCAT_EVAL(a, b) STB_CONCAT_RAW(a, b)
#define STB_CONCAT3_EVAL(a, b, c) STB_CONCAT3_RAW(a, b, c)

#define STB_CONCAT(a, b) STB_CONCAT_EVAL(a, b)
#define STB_CONCAT3(a, b, c) STB_CONCAT3_EVAL(a, b, c)

int stb_lang_tokenizer_get_length(FILE *file){
    // We use fseek and ftell
    fseek(file, 0L, SEEK_END);
    int a = ftell(file);
    fseek(file, 0L, SEEK_SET);
    return a;
}

#define STB_LANG_ADD_TOKEN(...) \
STB_CONCAT(STB_CONCAT3(dymarray_, CUR_TOKENIZER_NAME, _Token), _add)(&tokenizer->tokens, (STB_CONCAT(CUR_TOKENIZER_NAME, _Token)){__VA_ARGS__, .offset=oldCursor});

#define STB_LANG_TOKEN_CHAR(ch, tkn) case ch: \
    STB_LANG_ADD_TOKEN(.type = tkn, .value=NULL); goto skip;
#define STB_LANG_SIMPLE_CASES(...) __VA_ARGS__
#define STB_LANG_ALPHA(typ, ...) \
    if (isalpha(c) || c == '_') { \
        int valcap = 100, vallen = 0; \
        char *val = malloc(valcap); \
        if (val == NULL){ \
            stb_lang_error_minor(tokenizer->file.name, tokenizer->file.contents, tokenizer->cursor, "IdentifierError", "Could not tokenize identifier"); \
        }; \
        while (isalnum(c) || c == '_'){ \
            if (vallen >= valcap){ \
                valcap += 32; \
                val = realloc(val, valcap); \
            } \
            val[vallen++] = c; \
            if (STB_CONCAT(CUR_TOKENIZER_PREFIX, _advance)(tokenizer) == -1) {goto skip;}; \
            c = tokenizer->file.contents[tokenizer->cursor]; \
        } \
        val[vallen++] = '\0'; tokenizer->cursor--; \
        STB_LANG_ADD_TOKEN(.type = typ, .value=val); goto skip; \
    }
#define STB_LANG_NUM(typ, ...) \
    if (isdigit(c)) { \
        int valcap = 100, vallen = 0; \
        char *val = malloc(valcap); \
        if (val == NULL){ \
            stb_lang_error_minor(tokenizer->file.name, tokenizer->file.contents, tokenizer->cursor, "IntegerError", "Could not tokenize number"); \
        }; \
        while (isdigit(c)){ \
            if (vallen >= valcap - 1){ \
                valcap += 32; \
                val = realloc(val, valcap); \
            } \
            val[vallen++] = c; \
            if (STB_CONCAT(CUR_TOKENIZER_PREFIX, _advance)(tokenizer) == -1){goto skip;}; \
            c = tokenizer->file.contents[tokenizer->cursor]; \
        } \
        val[vallen++] = '\0'; tokenizer->cursor--; \
        STB_LANG_ADD_TOKEN(.type = typ, .value=val); goto skip; \
    }
#define STB_LANG_STRING(ch, typ) \
if (c == ch){ \
    int valcap = 100, vallen = 0; \
    char *val = malloc(valcap); \
    if (val == NULL){ \
        stb_lang_error_minor(tokenizer->file.name, tokenizer->file.contents, tokenizer->cursor, "StringError", "Could not tokenize string"); \
    }; \
    if (STB_CONCAT(CUR_TOKENIZER_PREFIX, _advance)(tokenizer) == -1){goto skip;}; \
    c = tokenizer->file.contents[tokenizer->cursor]; \
    bool free = 0; \
    while (c != ch){ \
        if (vallen >= valcap - 1){ \
            valcap += 32; \
            val = realloc(val, valcap); \
        } \
        if (free == 1){free = 0;} \
        if (c == '\\'){ \
            free = 1;\
        } \
        val[vallen++] = c; \
        if (STB_CONCAT(CUR_TOKENIZER_PREFIX, _advance)(tokenizer) == -1){goto skip;}; \
        c = tokenizer->file.contents[tokenizer->cursor]; \
        if (free == 1 && c == ch){ \
            free = 0; \
            val[vallen++] = c; \
            if (STB_CONCAT(CUR_TOKENIZER_PREFIX, _advance)(tokenizer) == -1){goto skip;}; \
            c = tokenizer->file.contents[tokenizer->cursor]; \
            continue; \
        } \
    }; \
    val[vallen++] = '\0';\
    STB_LANG_ADD_TOKEN(.type = typ, .value=val); goto skip; \
}
#define STB_LANG_TOKEN_DOUBLE_CHAR(s, stok, chtok) \
if (c == s[0]){ \
    STB_CONCAT(CUR_TOKENIZER_PREFIX, _advance)(tokenizer); \
    c = tokenizer->file.contents[tokenizer->cursor]; \
    if (c == s[1]){ \
        STB_LANG_ADD_TOKEN(.type = stok, .value=NULL); goto skip; \
    }else { \
        STB_LANG_ADD_TOKEN(.type = chtok, .value=NULL); return 0; \
    }; \
}
#define STB_LANG_TOKEN_DOUBLE_CHAR_IF(s, stok) \
if (c == s[0]){ \
    STB_CONCAT(CUR_TOKENIZER_PREFIX, _advance)(tokenizer); \
    c = tokenizer->file.contents[tokenizer->cursor]; \
    if (c == s[1]){ \
        STB_LANG_ADD_TOKEN(.type = stok, .value=NULL); goto skip; \
    }else { \
        tokenizer->cursor--; \
        c = tokenizer->file.contents[tokenizer->cursor]; \
    }; \
}
#define STB_LANG_TOKEN_COMMENT_LINE(str) \
int _cur = tokenizer->cursor; \
if (c == str[0]){ \
    int lim = strlen(str); \
    STB_CONCAT(CUR_TOKENIZER_PREFIX, _advance)(tokenizer); \
    c = tokenizer->file.contents[tokenizer->cursor]; \
    for (int i=1; i<lim; i++){ \
        if (c == str[i]){ \
            STB_CONCAT(CUR_TOKENIZER_PREFIX, _advance)(tokenizer); \
            c = tokenizer->file.contents[tokenizer->cursor]; \
        }else { \
            tokenizer->cursor = _cur; \
            c = tokenizer->file.contents[tokenizer->cursor]; \
            goto no_comment; \
        }; \
    } \
    c = tokenizer->file.contents[tokenizer->cursor]; \
    while (c != '\n'){ \
        STB_CONCAT(CUR_TOKENIZER_PREFIX, _advance)(tokenizer); \
        c = tokenizer->file.contents[tokenizer->cursor]; \
    } \
} \
no_comment:;


#define STB_LANG_SKIP(ch) if (c==ch){goto skip;}
#define STB_LANG_TOKENS(...) __VA_ARGS__

#define STB_LANG_TOKEN_REPR(...) __VA_ARGS__

// For later on
#define STB_LANG_INVOKE_TYPENEW(tkn) dymarray_typenew(tkn, 20, 10)

#define STB_LANG_NEW_TOKENIZER(atokens, repr, _cases, ...) \
typedef enum { \
    atokens \
}STB_CONCAT(CUR_TOKENIZER_NAME, _TokenType); \
typedef struct { \
    STB_CONCAT(CUR_TOKENIZER_NAME, _TokenType) type; \
    char *value; \
    int offset; \
    int file; \
}STB_CONCAT(CUR_TOKENIZER_NAME, _Token); \
STB_LANG_INVOKE_TYPENEW(STB_CONCAT(CUR_TOKENIZER_NAME, _Token)) \
\
typedef struct { \
    FILE *file; \
    char *name; \
    char *contents; \
    int contentlen; \
}STB_CONCAT(CUR_TOKENIZER_NAME, _File); \
typedef struct { \
    STB_CONCAT(CUR_TOKENIZER_NAME, _File) file; \
    int cursor; \
    STB_CONCAT3(dymarray_, CUR_TOKENIZER_NAME, _Token) tokens; \
}CUR_TOKENIZER_NAME; \
char *STB_CONCAT(CUR_TOKENIZER_PREFIX, _get_token_value)(int tokentype){ \
    switch (tokentype){ \
        repr; \
    } \
    return "(unknown token)"; \
} \
STB_CONCAT(CUR_TOKENIZER_NAME, _File) STB_CONCAT(CUR_TOKENIZER_PREFIX, _file_init)(char *name){ \
    STB_CONCAT(CUR_TOKENIZER_NAME, _File) fl = (STB_CONCAT(CUR_TOKENIZER_NAME, _File)){0}; \
    FILE *file = fopen(name, "rb"); \
    if (file == NULL){ \
        stb_lang_error_major_global("FileError", "Could not find file '%s'", name); \
    } \
    fl.file = file; \
    fl.name = name; \
    fl.contentlen = stb_lang_tokenizer_get_length(file); \
    fl.contents = malloc(fl.contentlen + 1); \
    fread(fl.contents, 1, fl.contentlen, file); \
    if (ferror(file)){ \
        stb_lang_error_major_global("TokenizerError", "Error when reading file"); \
    } \
    fl.contents[fl.contentlen] = '\0'; \
    fclose(file); \
    return fl; \
}; \
char STB_CONCAT(CUR_TOKENIZER_PREFIX, _advance)(CUR_TOKENIZER_NAME *tokenizer){ \
    char c = tokenizer->file.contents[tokenizer->cursor]; \
    tokenizer->cursor++; \
    if (tokenizer->cursor >= tokenizer->file.contentlen) return -1; \
    return c; \
} \
CUR_TOKENIZER_NAME* STB_CONCAT(CUR_TOKENIZER_PREFIX, _init)(STB_CONCAT(CUR_TOKENIZER_NAME, _File) file){ \
    CUR_TOKENIZER_NAME *tokenizer = malloc(sizeof(*tokenizer)); \
    tokenizer->file = file; \
    tokenizer->cursor = 0; \
    tokenizer->tokens = STB_CONCAT(STB_CONCAT3(dymarray_, CUR_TOKENIZER_NAME, _Token), _new)(); \
    return tokenizer; \
}; \
char STB_CONCAT(CUR_TOKENIZER_PREFIX, _token)(CUR_TOKENIZER_NAME *tokenizer){ \
    char c = tokenizer->file.contents[tokenizer->cursor]; \
    int oldCursor = tokenizer->cursor; \
    __VA_ARGS__; \
    switch (c){ \
        _cases \
        default: { \
            goto skip; \
        }; \
    } \
skip: \
    c = STB_CONCAT(CUR_TOKENIZER_PREFIX, _advance)(tokenizer); \
    if (c == -1) return -1; \
    if (c == '\0' || c == -1) return -1; \
    return 0; \
} \


#endif
