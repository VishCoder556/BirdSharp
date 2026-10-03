#ifndef STB_LANG_PREPROCESSOR_H
#define STB_LANG_PREPROCESSOR_H

#define STB_LANG_PROCESSOR_ADVANCE() if (STB_CONCAT(CUR_PREPROCESSOR_PREFIX, _advance)(processor) == -1){stb_lang_error_minor(processor->file.name, processor->file.contents, token.offset, "OutOfBoundsError", "Went out of bounds to peek");}else {token = processor->tokens.data[processor->cursor];;};

#define STB_LANG_PROCESSOR_UPDATE() token = processor->tokens.data[processor->cursor];

#define STB_LANG_PREPROCESSOR_PROCESS(...) __VA_ARGS__


#undef STB_LANG_INVOKE_TYPENEW
#define STB_LANG_INVOKE_TYPENEW(a) dymarray_typenew(a, 2, 2)

#define STB_LANG_NEW_PREPROCESSOR(_process) \
STB_LANG_INVOKE_TYPENEW(STB_CONCAT(CUR_TOKENIZER_NAME, _File)); \
typedef struct { \
    STB_CONCAT(CUR_TOKENIZER_NAME, _File) file; \
    int fl; \
    int cursor; \
    STB_CONCAT3(dymarray_, CUR_TOKENIZER_NAME, _Token) tokens; \
    STB_CONCAT3(dymarray_, CUR_TOKENIZER_NAME, _File) files; \
}CUR_PREPROCESSOR_NAME; \
CUR_PREPROCESSOR_NAME* STB_CONCAT(CUR_PREPROCESSOR_PREFIX, _init)(CUR_TOKENIZER_NAME *tokenizer, int fl){ \
    CUR_PREPROCESSOR_NAME *processor = malloc(sizeof(*processor)); \
    processor->tokens = tokenizer->tokens; \
    processor->cursor = 0; \
    processor->file = tokenizer->file; \
    processor->fl = fl; \
    processor->files = STB_CONCAT(STB_CONCAT3(dymarray_, CUR_TOKENIZER_NAME, _File), _new)(); \
    STB_CONCAT(STB_CONCAT3(dymarray_, CUR_TOKENIZER_NAME, _File), _add)(&processor->files, processor->file); \
    return processor; \
}; \
char STB_CONCAT(CUR_PREPROCESSOR_PREFIX, _advance)(CUR_PREPROCESSOR_NAME *processor) { \
    if (processor->cursor + 1 >= processor->tokens.datalen){ \
        return -1; \
    } \
    processor->cursor++; \
    return 0;\
} \
char STB_CONCAT(CUR_PREPROCESSOR_PREFIX, _token)(CUR_PREPROCESSOR_NAME *processor) { \
    if (processor->cursor >= processor->tokens.datalen) {return -1;} \
    if (processor->tokens.data[processor->cursor].file == 0){ \
        processor->tokens.data[processor->cursor].file = processor->fl; \
    } \
    STB_CONCAT(CUR_TOKENIZER_NAME, _Token) token = processor->tokens.data[processor->cursor]; \
    _process; \
    if (STB_CONCAT(CUR_PREPROCESSOR_PREFIX, _advance)(processor) == -1){ \
        return -1; \
    }; \
    return 0; \
};

#define STB_LANG_PROCESSOR_TRIM(start, end) \
if ((end) < processor->tokens.datalen) { \
    memmove(&processor->tokens.data[start], &processor->tokens.data[end], (processor->tokens.datalen - (end)) * sizeof(STB_CONCAT(CUR_TOKENIZER_NAME, _Token))); \
} \
processor->tokens.datalen -= ((end) - (start));

#define STB_LANG_PROCESSOR_INSERT(cursor, prpnew) \
if (processor->tokens.datalen + prpnew->tokens.datalen > processor->tokens.datacap){ \
    int _newcap = processor->tokens.datalen + prpnew->tokens.datalen; \
    processor->tokens.data = realloc(processor->tokens.data, _newcap * sizeof(STB_CONCAT(CUR_TOKENIZER_NAME, _Token))); \
    processor->tokens.datacap = _newcap; \
} \
STB_CONCAT(STB_CONCAT3(dymarray_, CUR_TOKENIZER_NAME, _File), _add)(&processor->files, prpnew->file); \
memmove(&processor->tokens.data[(cursor) + prpnew->tokens.datalen], &processor->tokens.data[cursor], (processor->tokens.datalen - (cursor)) * sizeof(STB_CONCAT(CUR_TOKENIZER_NAME, _Token))); \
memcpy(&processor->tokens.data[cursor], prpnew->tokens.data, (prpnew->tokens.datalen) * sizeof(STB_CONCAT(CUR_TOKENIZER_NAME, _Token))); \
processor->tokens.datalen += prpnew->tokens.datalen;


#endif
