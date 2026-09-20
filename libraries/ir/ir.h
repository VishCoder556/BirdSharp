// Simple TAC IR

#ifndef STB_LANG_IR_H
#define STB_LANG_IR_H

#define STB_LANG_IR_INSTRS(...)  __VA_ARGS__
#define STB_LANG_IR_OPERANDS(...)  __VA_ARGS__
#define STB_LANG_IR_CASES(...) __VA_ARGS__
#define STB_LANG_IR_CASE(typ, ...) else if (ast->type == typ){__VA_ARGS__;}

#undef STB_LANG_INVOKE_TYPENEW
#define STB_LANG_INVOKE_TYPENEW(a) dymarray_typenew(a, 10, 5)

#define STB_LANG_IR_EMIT(typ, des, lef, righ, ...) \
STB_CONCAT(STB_CONCAT3(dymarray_, CUR_IR_NAME, _Instr), _add)(&ir->instrs, (STB_CONCAT(CUR_IR_NAME, _Instr)){.type=typ, .left=lef, .right=righ, .dest=des, .offset=offset, .file=file, .phys={-1}, __VA_ARGS__})


#define STB_LANG_IR_RUN(place) do {\
STB_CONCAT(CUR_PARSER_NAME, _AST) *_block = (STB_CONCAT(CUR_PARSER_NAME, _AST)*)place; \
while (_block != NULL){ \
    STB_CONCAT(CUR_IR_PREFIX, _ast)(ir, _block, 0); \
    _block = (STB_CONCAT(CUR_PARSER_NAME, _AST)*)_block->next; \
}}while(0);

#define STB_LANG_IR_BLOCK() \
STB_LANG_IR_RUN(ast->right);





#define STB_LANG_IR(ast) STB_CONCAT(CUR_IR_PREFIX, _ast)(ir, ast, 0)
#define STB_LANG_IR_RHS(ast) STB_CONCAT(CUR_IR_PREFIX, _ast)(ir, STB_LANG_RHS(ast), 0)
#define STB_LANG_IR_LHS(ast) STB_CONCAT(CUR_IR_PREFIX, _ast)(ir, STB_LANG_LHS(ast), 0)

#define STB_LANG_IR_EXTRA(ast, n) ({ \
    STB_CONCAT(CUR_IR_NAME, _Operand) *op = STB_CONCAT(CUR_IR_PREFIX, _ast)(ir, ast, n); \
    if (op != NULL) { \
        op->extr = n; \
    } \
    op; \
})
#define STB_LANG_IR_RHS_EXTRA(ast, n) ({ \
    STB_CONCAT(CUR_IR_NAME, _Operand) *op = STB_CONCAT(CUR_IR_PREFIX, _ast)(ir, STB_LANG_RHS(ast), n); \
    if (op != NULL) { \
        op->extr = n; \
    } \
    op; \
})

#define STB_LANG_IR_LHS_EXTRA(ast, n) ({ \
    STB_CONCAT(CUR_IR_NAME, _Operand) *op = STB_CONCAT(CUR_IR_PREFIX, _ast)(ir, STB_LANG_LHS(ast), n); \
    if (op != NULL) { \
        op->extr = n; \
    } \
    op; \
})


#define STB_LANG_IR_PARAMS(assign, pop, reg, var) do {\

// ^ popping from function args stack
// no function call implemented yet, we won't have to worry about pushing


#define STB_LANG_IR_OPERAND(typ, val) ({ \
    STB_CONCAT(CUR_IR_NAME, _Operand) *op = malloc(sizeof(*op)); \
    op->type = typ; \
    op->value = val; \
    op->phys = -1; \
    op->isLast = -1; \
    op->extr = 0; \
    op; \
})
#define STB_LANG_IR_OPERAND_EXTRA(typ, val, ...) ({ \
    STB_CONCAT(CUR_IR_NAME, _Operand) *op = malloc(sizeof(*op)); \
    op->type = typ; \
    op->value = val; \
    op->extr = __VA_ARGS__; \
    op->phys = -1; \
    op->isLast = -1; \
    op; \
})

#define STB_LANG_IR_RETURN_SELF(type) return STB_LANG_IR_OPERAND(type, ast->value);

#define STB_LANG_IR_NEW_TEMP(temp_reg) char *temp_reg = STB_CONCAT(CUR_IR_PREFIX, _make_temp_reg_string)(ir);
#define STB_LANG_IR_AS_TEMP(reg, temp_reg) STB_LANG_IR_OPERAND(reg, temp_reg)
#define STB_LANG_IR_NEW_LABEL(label) char *label = STB_CONCAT(CUR_IR_PREFIX, _make_temp_label_name)(ir);
#define STB_LANG_IR_LABEL(var, label) STB_LANG_IR_OPERAND(var, label)

#define STB_LANG_IR_GET_OPERAND(ast) STB_CONCAT(CUR_IR_PREFIX, _ast)(ir, (STB_CONCAT(CUR_PARSER_NAME, _AST)*)ast, 0)



#define STB_LANG_IR_ERROR_MINOR(where, fil, type, ...) \
if (fil > ir->files.datalen){ \
    stb_lang_error_minor(ir->file.name, ir->file.contents, where, "IRError", "Failure to generate error"); \
} \
stb_lang_error_minor(ir->files.data[fil].name, ir->files.data[fil].contents, where, type, __VA_ARGS__);


#define STB_LANG_NEW_IR(operands, types, cases) \
typedef enum{operands}STB_CONCAT(CUR_IR_NAME, _OperandType); \
typedef struct { \
    STB_CONCAT(CUR_IR_NAME, _OperandType) type; \
    char *value; \
    int phys; \
    int isLast; \
    int extr; \
}STB_CONCAT(CUR_IR_NAME, _Operand); \
typedef enum{types}STB_CONCAT(CUR_IR_NAME, _InstrType); \
typedef struct { \
    STB_CONCAT(CUR_IR_NAME, _InstrType) type; \
    STB_CONCAT(CUR_IR_NAME, _Operand) *left; \
    STB_CONCAT(CUR_IR_NAME, _Operand) *right; \
    STB_CONCAT(CUR_IR_NAME, _Operand) *dest; \
    int offset; \
    int file; \
    int phys[4]; \
    STB_CONCAT(CUR_TYPEINFO_NAME, _Typeinfo) typeinfo; \
}STB_CONCAT(CUR_IR_NAME, _Instr); \
typedef struct { \
    char *data; \
    int length; \
}STB_CONCAT(CUR_IR_NAME, _Symbol); \
STB_LANG_INVOKE_TYPENEW(STB_CONCAT(CUR_IR_NAME, _Instr));\
STB_LANG_INVOKE_TYPENEW(STB_CONCAT(CUR_IR_NAME, _Symbol));\
typedef struct { \
    STB_CONCAT(CUR_PARSER_NAME, _AST) *head; \
    STB_CONCAT(CUR_PARSER_NAME, _AST) *tail; \
    STB_CONCAT3(dymarray_, CUR_IR_NAME, _Instr) instrs; \
    STB_CONCAT(CUR_TYPEINFO_NAME, _ScopeL) root_scope; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _File) file; \
    STB_CONCAT3(dymarray_, CUR_IR_NAME, _Symbol) symbols; \
    int temp_number; \
    int label_count; \
    STB_CONCAT3(dymarray_, CUR_TOKENIZER_NAME, _File) files; \
}CUR_IR_NAME; \
char *STB_CONCAT(CUR_IR_PREFIX, _make_temp_reg_string)(Lang_IR *ir) { \
    char *str = malloc(16); \
    snprintf(str, 16, "t%d", ir->temp_number++); \
    return str; \
} \
char *STB_CONCAT(CUR_IR_PREFIX, _make_temp_label_name)(Lang_IR *ir) { \
    char *str = malloc(20); \
    snprintf(str, 20, "%d", ir->label_count++); \
    return str; \
} \
CUR_IR_NAME *STB_CONCAT(CUR_IR_PREFIX, _init)(CUR_TYPEINFO_NAME *checker){ \
    CUR_IR_NAME *ir = malloc(sizeof(*ir)); \
    ir->head = checker->head; \
    ir->tail = ir->head; \
    ir->instrs = STB_CONCAT(STB_CONCAT3(dymarray_, CUR_IR_NAME, _Instr), _new)(); \
    ir->symbols = STB_CONCAT(STB_CONCAT3(dymarray_, CUR_IR_NAME, _Symbol), _new)(); \
    ir->root_scope = checker->root_scope; \
    ir->file = checker->file; \
    ir->temp_number = 0; \
    ir->files = checker->files; \
    return ir; \
} \
long STB_CONCAT(CUR_IR_PREFIX, _symbol_new)(CUR_IR_NAME *ir, char *data, int length){ \
    STB_CONCAT(CUR_IR_NAME, _Symbol) symbol = (STB_CONCAT(CUR_IR_NAME, _Symbol)){.data=data, .length=length}; \
    STB_CONCAT(STB_CONCAT3(dymarray_, CUR_IR_NAME, _Symbol), _add)(&ir->symbols, symbol); \
    return ir->symbols.datalen-1; \
}; \
STB_CONCAT(CUR_IR_NAME, _Operand) *STB_CONCAT(CUR_IR_PREFIX, _ast)(CUR_IR_NAME *ir, STB_CONCAT(CUR_PARSER_NAME, _AST) *ast, int extr){ \
    (void)extr; \
    int offset = ast->offset; \
    int file = ast->file; \
    if (0){}cases else { \
        STB_LANG_IR_ERROR_MINOR(ast->offset, ast->file, "IRError", "Could not convert ast with type '%d' into IR", ast->type); \
    }; \
    return NULL; \
}; \
char STB_CONCAT(CUR_IR_PREFIX, _translate)(CUR_IR_NAME *ir){ \
    if (ir->tail == NULL) return -1; \
 \
    STB_CONCAT(CUR_IR_PREFIX, _ast)(ir, ir->tail, 0); \
 \
    ir->tail = (STB_CONCAT(CUR_PARSER_NAME, _AST)*)ir->tail->next; \
    return 0; \
}

#endif
