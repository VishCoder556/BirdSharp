#ifndef STB_LANG_REGALLOC_H
#define STB_LANG_REGALLOC_H

#define STB_LANG_INVOKE_TYPENEW4(a) dymarray_typenew(a, 8, 1)
#define STB_LANG_INVOKE_TYPENEW3(a) dymarray_typenew(a, STB_LANG_REGALLOC_REG_MAX, 1)

#define STB_LANG_REGALLOC_ERROR_MINOR(files, where, fil, type, ...) \
if (fil > files.datalen){ \
    stb_lang_error_major_global("RegallocError", "Failure to generate error"); \
} \
stb_lang_error_minor(files.data[fil].name, files.data[fil].contents, where, type, __VA_ARGS__);

#define STB_LANG_REGALLOC_LIST(...) __VA_ARGS__
#define STB_LANG_REGALLOC_CASE(typ, ...) else if( instr->type == typ ) {__VA_ARGS__;}
#define STB_LANG_REGALLOC_2CASES(typ, typ2, ...) else if( instr->type == typ || instr->type == typ2 ) {__VA_ARGS__;}
#define STB_LANG_REGALLOC_3CASES(typ, typ2, typ3, ...) else if( instr->type == typ || instr->type == typ2 || instr->type == typ3) {__VA_ARGS__;}
#define STB_LANG_REGALLOC_4CASES(typ, typ2, typ3, typ4, ...) else if( instr->type == typ || instr->type == typ2 || instr->type == typ3 || instr->type == typ4) {__VA_ARGS__;}
#define STB_LANG_REGALLOC_5CASES(typ, typ2, typ3, typ4, typ5, ...) else if( instr->type == typ || instr->type == typ2 || instr->type == typ3 || instr->type == typ4 || instr->type == typ5) {__VA_ARGS__;}
#define STB_LANG_REGALLOC_6CASES(typ, typ2, typ3, typ4, typ5, typ6, ...) else if( instr->type == typ || instr->type == typ2 || instr->type == typ3 || instr->type == typ4 || instr->type == typ5 || instr->type == typ6) {__VA_ARGS__;}

#define STB_LANG_REGALLOC_OPERAND_INNER(reg, a) \
if (instr->a != NULL){ \
    if (instr->a->type == reg){ \
        int found = 0; \
        for (int i=0; i<regalloc->backtrack.datalen; i++){ \
            if (strcmp(regalloc->backtrack.data[i].val, instr->a->value) == 0){ \
                found = 1; \
            }; \
        }; \
        if (found == 0){ \
            instr->a->isLast = 1; \
            STB_CONCAT(STB_CONCAT3(dymarray_, CUR_REGALLOC_NAME, _Register_BackTrack), _add)(&regalloc->backtrack, (STB_CONCAT(CUR_REGALLOC_NAME, _Register_BackTrack)){instr->a->value}); \
        } \
    }; \
} \


#define STB_LANG_REGALLOC_OPERAND(reg) \
STB_LANG_REGALLOC_OPERAND_INNER(reg, dest); \
STB_LANG_REGALLOC_OPERAND_INNER(reg, left); \
STB_LANG_REGALLOC_OPERAND_INNER(reg, right);

#define STB_LANG_NEW_REGALLOC(regtypes, matches, list, reg) \
typedef struct{ \
    char *val; \
} STB_CONCAT(CUR_REGALLOC_NAME, _Register_BackTrack); \
STB_LANG_INVOKE_TYPENEW4(STB_CONCAT(CUR_REGALLOC_NAME, _Register_BackTrack)); \
typedef enum{ \
    regtypes, \
    STB_LANG_REGALLOC_REG_MAX \
} STB_CONCAT(CUR_REGALLOC_NAME, _Reg); \
typedef struct{ \
    bool available; \
} STB_CONCAT(CUR_REGALLOC_NAME, _Register); \
STB_LANG_INVOKE_TYPENEW3(STB_CONCAT(CUR_REGALLOC_NAME, _Register)); \
typedef struct { \
    char *virtual; \
    STB_CONCAT(CUR_REGALLOC_NAME, _Reg) real; \
    STB_CONCAT(CUR_IR_NAME, _Operand) *last_use; \
}STB_CONCAT(CUR_REGALLOC_NAME, _VirtualRegister); \
STB_LANG_INVOKE_TYPENEW3(STB_CONCAT(CUR_REGALLOC_NAME, _VirtualRegister)); \
typedef struct { \
    STB_CONCAT3(dymarray_, CUR_REGALLOC_NAME, _VirtualRegister) *virtual_regs; \
    STB_CONCAT3(dymarray_, CUR_REGALLOC_NAME, _Register) *regs; \
    STB_CONCAT3(dymarray_, CUR_IR_NAME, _Instr) instrs; \
    STB_CONCAT(CUR_TYPEINFO_NAME, _ScopeL) root_scope; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _File) file; \
    STB_CONCAT3(dymarray_, CUR_REGALLOC_NAME, _Register_BackTrack) backtrack; \
    int cursor; \
    STB_CONCAT3(dymarray_, CUR_IR_NAME, _Symbol) symbols; \
    STB_CONCAT3(dymarray_, CUR_TOKENIZER_NAME, _File) files; \
}CUR_REGALLOC_NAME; \
STB_CONCAT(CUR_REGALLOC_NAME, _Reg) STB_CONCAT(CUR_REGALLOC_PREFIX, _alloc_register)(CUR_REGALLOC_NAME *regalloc, int offset, int file){ \
    (void)file; \
    (void)offset; \
    STB_CONCAT3(dymarray_, CUR_REGALLOC_NAME, _Register) *dym = regalloc->regs; \
    for (int v=0; v<dym->datalen; v++){ \
        if (dym->data[v].available == 1){ \
            dym->data[v].available = 0; \
            return (STB_CONCAT(CUR_REGALLOC_NAME, _Reg))v; \
        } \
    } \
    return -1; \
}; \
char *STB_CONCAT(CUR_REGALLOC_PREFIX, _register_from_reg_inner)(STB_CONCAT(CUR_REGALLOC_NAME, _Reg) r, int size){ \
    if(0){}matches; \
    return NULL; \
} \
char *STB_CONCAT(CUR_REGALLOC_PREFIX, _register_from_reg)(STB_CONCAT3(dymarray_, CUR_TOKENIZER_NAME, _File) files, int fl, int offset, STB_CONCAT(CUR_REGALLOC_NAME, _Reg) r, int size){ \
    char *str = STB_CONCAT(CUR_REGALLOC_PREFIX, _register_from_reg_inner)(r, size); \
    if (str == NULL){ \
        return STB_CONCAT(CUR_REGALLOC_PREFIX, _register_from_reg_inner)(r, 8); \
        STB_LANG_REGALLOC_ERROR_MINOR(files, offset, fl, "RegisterError", "Invalid register or size accessed (reg=%d, size=%d)", r, size); \
        return NULL; \
    } \
    return str; \
}; \
STB_CONCAT3(dymarray_, CUR_REGALLOC_NAME, _Register) *STB_CONCAT(CUR_REGALLOC_PREFIX, _regs_init)(){ \
    STB_CONCAT3(dymarray_, CUR_REGALLOC_NAME, _Register) *dym = malloc(sizeof(*dym)); \
    *dym = STB_CONCAT(STB_CONCAT3(dymarray_, CUR_REGALLOC_NAME, _Register), _new)(); \
    for (int i=0; i<STB_LANG_REGALLOC_REG_MAX; i++){ \
        dym->data[dym->datalen++] = (STB_CONCAT(CUR_REGALLOC_NAME, _Register)){.available=1}; \
    } \
    return dym;\
}\
void STB_CONCAT(CUR_REGALLOC_PREFIX, _free_register)(STB_CONCAT(CUR_REGALLOC_NAME, _Reg) reg, STB_CONCAT3(dymarray_, CUR_REGALLOC_NAME, _Register) *regs){ \
    if ((int)reg != -1) { \
        regs->data[(int)reg].available = 1; \
    } \
}; \
STB_CONCAT(CUR_REGALLOC_NAME, _VirtualRegister) STB_CONCAT(CUR_REGALLOC_PREFIX, _virtual_reg_init)(CUR_REGALLOC_NAME *regalloc, char *virtual, STB_CONCAT(CUR_IR_NAME, _Instr) *instr){ \
    STB_CONCAT(CUR_REGALLOC_NAME, _VirtualRegister) vreg; \
    STB_LANG_ALLOC_REGISTER(reg); \
    vreg.real = reg; \
    vreg.virtual = virtual; \
    STB_CONCAT(STB_CONCAT3(dymarray_, CUR_REGALLOC_NAME, _VirtualRegister), _add)(regalloc->virtual_regs, vreg); \
    return vreg; \
}; \
STB_CONCAT(CUR_REGALLOC_NAME, _VirtualRegister) STB_CONCAT(CUR_REGALLOC_PREFIX, _virtual_reg_find)(CUR_REGALLOC_NAME * regalloc, char *virtual, STB_CONCAT(CUR_IR_NAME, _Instr) *instr){ \
    for (int i=0; i<regalloc->virtual_regs->datalen; i++){ \
        if (strcmp(regalloc->virtual_regs->data[i].virtual, virtual) == 0){ \
            return regalloc->virtual_regs->data[i]; \
        }; \
    }; \
    if (instr != NULL){ \
        stb_lang_error_major_global("RegisterError", "Could not find virtual register"); \
    } \
    stb_lang_error_major_global("RegisterError", "Could not find virtual register"); \
    return (STB_CONCAT(CUR_REGALLOC_NAME, _VirtualRegister)){0}; \
}; \
STB_CONCAT(CUR_REGALLOC_NAME, _VirtualRegister) STB_CONCAT(CUR_REGALLOC_PREFIX, _virtual_reg_init_find)(CUR_REGALLOC_NAME * regalloc, char *virtual, STB_CONCAT(CUR_IR_NAME, _Instr) *instr){ \
    for (int i=0; i<regalloc->virtual_regs->datalen; i++){ \
        if (strcmp(regalloc->virtual_regs->data[i].virtual, virtual) == 0){ \
            return regalloc->virtual_regs->data[i]; \
        }; \
    }; \
    STB_CONCAT(CUR_REGALLOC_NAME, _VirtualRegister) vreg; \
    STB_LANG_ALLOC_REGISTER(reg); \
    vreg.real = reg; \
    vreg.virtual = virtual; \
    STB_CONCAT(STB_CONCAT3(dymarray_, CUR_REGALLOC_NAME, _VirtualRegister), _add)(regalloc->virtual_regs, vreg); \
    return vreg; \
}; \
CUR_REGALLOC_NAME *STB_CONCAT(CUR_REGALLOC_PREFIX, _init)(CUR_IR_NAME *ir){ \
    CUR_REGALLOC_NAME *regalloc = malloc(sizeof(*regalloc)); \
    regalloc->virtual_regs = malloc(sizeof(*regalloc->virtual_regs)); \
    *regalloc->virtual_regs = STB_CONCAT(STB_CONCAT3(dymarray_, CUR_REGALLOC_NAME, _VirtualRegister), _new)(); \
    regalloc->root_scope = ir->root_scope; \
    regalloc->file = ir->file; \
    regalloc->files = ir->files; \
    regalloc->instrs = ir->instrs; \
    regalloc->regs = STB_CONCAT(CUR_REGALLOC_PREFIX, _regs_init)(); \
    regalloc->backtrack = STB_CONCAT(STB_CONCAT3(dymarray_, CUR_REGALLOC_NAME, _Register_BackTrack), _new)(); \
    regalloc->cursor = 0; \
    regalloc->symbols = ir->symbols; \
    return regalloc; \
} \
char STB_CONCAT(CUR_REGALLOC_PREFIX, _ir)(CUR_REGALLOC_NAME *regalloc, STB_CONCAT(CUR_IR_NAME, _Instr) *instr){ \
    STB_LANG_EXPAND_OPERAND(dest, reg); \
    STB_LANG_EXPAND_OPERAND(left, reg); \
    STB_LANG_EXPAND_OPERAND(right, reg); \
    if(0){}list else { \
        STB_LANG_REGALLOC_ERROR_MINOR(regalloc->files, instr->offset, instr->file, "RegallocError", "Could not get correct register allocations for instruction '%d'", instr->type); \
    }; \
    STB_LANG_FREE_OPERAND(dest, reg); \
    STB_LANG_FREE_OPERAND(left, reg); \
    STB_LANG_FREE_OPERAND(right, reg); \
    return 0; \
}; \
char STB_CONCAT(CUR_REGALLOC_PREFIX, _alloc)(CUR_REGALLOC_NAME *regalloc){ \
    if (regalloc->cursor >= regalloc->instrs.datalen){ \
        return -1; \
    } \
    STB_CONCAT(CUR_IR_NAME, _Instr) *instr = (regalloc->instrs.data + regalloc->cursor); \
    if (STB_CONCAT(CUR_REGALLOC_PREFIX, _ir)(regalloc, instr) == -1){return -1;}; \
    regalloc->cursor++; \
    return 0; \
}; \
char STB_CONCAT(CUR_REGALLOC_PREFIX, _back)(CUR_REGALLOC_NAME *regalloc){ \
    if (regalloc->cursor < 0){ \
        return -1; \
    } \
    STB_CONCAT(CUR_IR_NAME, _Instr) *instr = (regalloc->instrs.data + regalloc->cursor); \
    STB_LANG_REGALLOC_OPERAND(reg); \
    regalloc->cursor--; \
    return 0; \
}; \
char STB_CONCAT(CUR_REGALLOC_PREFIX, _backtrace)(CUR_REGALLOC_NAME *regalloc){ \
    regalloc->cursor = regalloc->instrs.datalen - 1; \
    while (STB_CONCAT(CUR_REGALLOC_PREFIX, _back)(regalloc) == 0){ \
        ; \
    } \
    regalloc->cursor = 0; \
    return 0; \
};




#define STB_LANG_ARM_REGISTERS() \
STB_LANG_REGALLOC_REGISTERS( \
    REG_X9, \
    REG_X10, \
    REG_X11, \
    REG_X12, \
    REG_X13, \
    REG_X14, \
    REG_X15 \
)

#define STB_LANG_ARM_REGISTER_MAPS() \
STB_LANG_REGALLOC_REGISTER_NAMES( \
    STB_LANG_ARM_REG_MAP(9) \
    STB_LANG_ARM_REG_MAP(10) \
    STB_LANG_ARM_REG_MAP(11) \
    STB_LANG_ARM_REG_MAP(12) \
    STB_LANG_ARM_REG_MAP(13) \
    STB_LANG_ARM_REG_MAP(14) \
    STB_LANG_ARM_REG_MAP(15) \
)


#define STB_LANG_REGALLOC_REGISTERS(...) __VA_ARGS__

#define STB_LANG_REGALLOC_REGISTER_NAMES(...) __VA_ARGS__
#define STB_LANG_REGALLOC_REGISTER_MATCH(reg, n8, n4, n2, n1) else if (r == reg){switch(size){case 8: return n8; case 4: return n4; case 2: return n2; case 1: return n1;};}


// STB_LANG_ITERATE(STB_LANG_CURRENT_SCOPE()->symbols, Lang_TypeInfo_Symbol,
//     if (iter.kind == STB_LANG_SYMBOL_VARIABLE){
//         iter.data.variable.offset += allocated;
//     };
// );

#define STB_LANG_ALLOC_REGISTER(reg)  \
STB_CONCAT(CUR_REGALLOC_NAME, _Reg) reg; \
if (instr == NULL){ \
    reg = STB_CONCAT(CUR_REGALLOC_PREFIX, _alloc_register)(regalloc, -1, -1); \
    if ((int)reg == -1){ \
        stb_lang_error_major_global("RegisterError", "Could not allocate a register"); \
    } \
}else { \
    reg = STB_CONCAT(CUR_REGALLOC_PREFIX, _alloc_register)(regalloc, instr->offset, instr->file); \
    if ((int)reg == -1){ \
    } \
}

// STB_LANG_REGALLOC_ERROR_MINOR(regalloc->files, instr->offset, instr->file, "RegisterError", "Could not allocate a register");


#define STB_LANG_REGISTER(r, size) STB_CONCAT(CUR_REGALLOC_PREFIX, _register_from_reg)(gen->files, instr->file, instr->offset, r, size)

#define STB_LANG_FREE_REGISTER(r) STB_CONCAT(CUR_REGALLOC_PREFIX, _free_register)(r, regalloc->regs)

#define STB_LANG_EXPAND_OPERAND(operand, reg) \
STB_CONCAT(CUR_REGALLOC_NAME, _VirtualRegister) operand; \
if (instr->operand != NULL) { \
if (instr->operand->type == reg){ \
    operand = STB_CONCAT(CUR_REGALLOC_PREFIX, _virtual_reg_init_find)(regalloc, instr->operand->value, instr); \
    instr->operand->phys = operand.real; \
} \
}

#define STB_LANG_FREE_OPERAND(operand, reg) \
if (instr->operand != NULL){ \
if (instr->operand->isLast == 1){ \
    if (instr->operand->type == reg){ \
        STB_LANG_FREE_REGISTER(operand.real); \
    } \
} \
}


#define STB_LANG_ARM_REG_MAP(num) \
    STB_LANG_REGALLOC_REGISTER_MATCH(REG_X##num, "x"#num, "w"#num, "w"#num, "w"#num)

#define STB_LANG_REGALLOC_RHS() \
STB_CONCAT(CUR_REGALLOC_PREFIX, _ir)(regalloc, instr->left);




#define STB_LANG_ALLOC_REAL_REGISTER(name, phys) \
    STB_LANG_ALLOC_REGISTER(name); \
    instr->phys = (int)name;


#define STB_LANG_REGALLOC_NEGATE(phys) instr->phys = -1;


#define STB_LANG_SAVE_REG(phys, ...) \
STB_LANG_ALLOC_REAL_REGISTER(name, phys); \
__VA_ARGS__; \
STB_LANG_FREE_REGISTER(name);


#define STB_LANG_REGALLOC_ARG(place, arg) \
if (instr->place->type == arg){ \
    snprintf(instr->dest->value, 10, "x%d", atoi(instr->place->value + 1)); \
}else 







#endif
