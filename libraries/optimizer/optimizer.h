#ifndef STB_LANG_OPTIMIZER_H
#define STB_LANG_OPTIMIZER_H

#define STB_LANG_OPTIMIZER_CASES(...) __VA_ARGS__
#define STB_LANG_OPTIMIZER_CASE(item, ...) if (instr->type == item){__VA_ARGS__;}
#define STB_LANG_OPTIMIZER_EXTRA(...) __VA_ARGS__

#define STB_LANG_OPTIMIZE(instr) STB_CONCAT(CUR_OPTIMIZER_PREFIX, _optimize_inner)(optimizer, instr)

#define STB_LANG_OPTIMIZER_PREFIX(...) __VA_ARGS__

#define STB_LANG_OPTIMIZER_OPERANDS(...) __VA_ARGS__
#define STB_LANG_OPTIMIZER_OPERAND(item, ...) if (operand->type == item){__VA_ARGS__;}

#define STB_LANG_OPT_OPERAND(op, instr) ;
#define STB_LANG_OPT_DEST(instr) STB_LANG_OPT_OPERAND(instr->dest, instr)
#define STB_LANG_OPT_LHS(instr) STB_LANG_OPT_OPERAND(instr->left, instr)
#define STB_LANG_OPT_RHS(instr) STB_LANG_OPT_OPERAND(instr->right, instr)


#define STB_LANG_OPTIMIZER_ERROR_MINOR(files, where, fil, type, ...) \
if (fil > files.datalen){ \
    stb_lang_error_major_global("OptimizerError", "Failure to generate error"); \
} \
stb_lang_error_minor(files.data[fil].name, files.data[fil].contents, where, type, __VA_ARGS__);

#define STB_LANG_NEW_OPTIMIZER(extr, prefix, operands, cases) \
typedef struct { \
    STB_CONCAT3(dymarray_, CUR_TOKENIZER_NAME, _File) files; \
    int cursor; \
    STB_CONCAT3(dymarray_, CUR_IR_NAME, _Instr) instrs; \
    STB_CONCAT3(dymarray_, CUR_IR_NAME, _Symbol) symbols; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _File) file; \
    STB_CONCAT(CUR_TYPEINFO_NAME, _ScopeL) root_scope; \
    extr; \
}CUR_OPTIMIZER_NAME; \
CUR_OPTIMIZER_NAME *STB_CONCAT(CUR_OPTIMIZER_PREFIX, _init)(CUR_IR_NAME *ir){ \
    CUR_OPTIMIZER_NAME *optimizer = malloc(sizeof(*optimizer));\
    optimizer->files = ir->files; \
    optimizer->file = ir->file; \
    optimizer->cursor = 0; \
    optimizer->instrs = ir->instrs; \
    optimizer->symbols = ir->symbols; \
    optimizer->root_scope = ir->root_scope; \
    prefix; \
    return optimizer; \
} \
STB_CONCAT(CUR_IR_NAME, _Operand) *STB_CONCAT(CUR_OPTIMIZER_PREFIX, _optimize_operand)(CUR_OPTIMIZER_NAME *optimizer, STB_CONCAT(CUR_IR_NAME, _Operand) *operand, STB_CONCAT(CUR_IR_NAME, _Instr) *instr){ \
    if (operand == NULL) return NULL; \
    if (instr == NULL) return NULL; \
    (void)instr; \
    (void)optimizer; \
    if (0){}operands else { \
    }; \
    return operand; \
}; \
char STB_CONCAT(CUR_OPTIMIZER_PREFIX, _optimize_inner)(CUR_OPTIMIZER_NAME *optimizer, STB_CONCAT(CUR_IR_NAME, _Instr) *instr){ \
    STB_CONCAT(CUR_IR_NAME, _Operand) *tmp = NULL; \
    (void)tmp; \
    (void)optimizer; \
    if (instr == NULL) return -1; \
    if (0){}cases else { \
    }; \
    return 0; \
} \
char STB_CONCAT(CUR_OPTIMIZER_PREFIX, _optimize)(CUR_OPTIMIZER_NAME *optimizer){ \
    if (optimizer->cursor >= optimizer->instrs.datalen){ \
        return -1; \
    } \
    STB_CONCAT(CUR_IR_NAME, _Instr) *instr = (optimizer->instrs.data + optimizer->cursor); \
    if (STB_CONCAT(CUR_OPTIMIZER_PREFIX, _optimize_inner)(optimizer, instr) == -1){return -1;}; \
    optimizer->cursor++; \
    return 0; \
};

#endif
