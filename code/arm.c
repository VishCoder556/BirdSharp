#define STB_LANG_ARM_MOVE(size, right, ...) {\
if (right != NULL) { \
    char *leftr = __VA_ARGS__; \
    if (leftr[0] == 't'){ \
        leftr = STB_LANG_REGISTER(instr->dest->phys, 8); \
    } \
    if (right->type == IR_INT) { \
        STB_LANG_EMIT_CODE("\tmov %s, #%s\n", leftr, right->value); \
    }else if(right->type == IR_VAR){ \
        int newoffset = STB_CONCAT(CUR_CODEGEN_PREFIX, _get_offset_from_var)(gen, right->value); \
        if (right->extr == 0){ \
            if (newoffset == 0){ \
                STB_LANG_EMIT_CODE("\tldr %s, [sp]\n", leftr); \
            }else { \
                STB_LANG_EMIT_CODE("\tldr %s, [sp, #%d]\n", leftr, newoffset); \
            } \
        }else { \
            if (newoffset == 0){ \
                STB_LANG_EMIT_CODE("\tmov %s, sp\n", leftr); \
            }else { \
                if (leftr[0] == 'w'){ \
                    STB_LANG_EMIT_CODE("\tadd %s, wsp, #%d\n", leftr, newoffset); \
                }else {; \
                    STB_LANG_EMIT_CODE("\tadd %s, sp, #%d\n", leftr, newoffset); \
                } \
            } \
        } \
    }else if (right->type == IR_REG){ \
        if (right->value[0] == 'a'){ \
            STB_LANG_EMIT_CODE("\tmov %s, x%d\n", leftr, atoi(right->value + 1)); \
        }else if (right->value[0] == '.'){ \
            int n = atoi(instr->dest->value + 4); \
            STB_LANG_EMIT_CODE("\tldr %s, [sp, #%d]\n", leftr, n * 8); \
        }else if (right->value[0] == 'v'){ \
            STB_LANG_EMIT_CODE("\tmov %s, %c0\n", leftr, size==8?'x':'w'); \
        }else { \
            STB_LANG_EMIT_CODE("\tmov %s, %s\n", leftr, STB_LANG_REGISTER(right->phys, size)); \
        } \
    }else if (right->type == IR_MEM){ \
        STB_LANG_EMIT_CODE("\tadrp %s, mem_%ld@PAGE\n", leftr, (long)right->value); \
        STB_LANG_EMIT_CODE("\tadd %s, %s, mem_%ld@PAGEOFF\n", leftr, leftr, (long)right->value); \
    } \
} \
}




#define STB_LANG_ARM_BINARY(op) \
STB_LANG_ARM_MOVE(8, instr->left, STB_LANG_REGISTER(instr->dest->phys, 8)); \
if (instr->phys[1] != -1){ \
    STB_LANG_ARM_MOVE(8, instr->right, STB_LANG_REGISTER(instr->phys[1], 8)); \
    STB_LANG_EMIT_CODE("\t%s %s, %s, %s\n", op, STB_LANG_REGISTER(instr->dest->phys, 8), STB_LANG_REGISTER(instr->dest->phys, 8), STB_LANG_REGISTER(instr->phys[1], 8)); \
}else { \
    STB_LANG_EMIT_CODE("\t%s %s, %s, #%s\n", op, STB_LANG_REGISTER(instr->dest->phys, 8), STB_LANG_REGISTER(instr->dest->phys, 8), instr->right->value); \
}


#define STB_LANG_ARM_COMPARISON(op) \
int size = STB_LANG_LOOKUP_SIZE(gen->root_scope, &instr->typeinfo); \
STB_LANG_ARM_MOVE(size, instr->left, STB_LANG_REGISTER(instr->phys[0], size)); \
if (instr->phys[1] != -1){ \
    STB_LANG_ARM_MOVE(size, instr->right, STB_LANG_REGISTER(instr->phys[1], size)); \
    STB_LANG_EMIT_CODE("\tcmp %s, %s\n", STB_LANG_REGISTER(instr->phys[0], size), STB_LANG_REGISTER(instr->phys[1], size)); \
}else { \
    STB_LANG_EMIT_CODE("\tcmp %s, #%s\n", STB_LANG_REGISTER(instr->phys[0], size), instr->right->value); \
} \
STB_LANG_EMIT_CODE("\tcset %s, %s\n", STB_LANG_REGISTER(instr->dest->phys, size), op);


STB_LANG_NEW_CODEGEN(
    STB_LANG_CODEGEN_PREFIX(
        STB_LANG_EMIT_CODE("%s\n", ".global _main");
        STB_LANG_EMIT_CODE("%s\n", ".align 2");
    ),
    STB_LANG_CODEGEN_SUFFIX(
        STB_LANG_EMIT_CODE(".data\n");
        STB_LANG_EMIT_CODE(".align 3\n");
        STB_LANG_ITERATE(gen->symbols, Lang_IR_Symbol, 
// This looks wrong, but there are variables idx and iter used in the iterate function
// This is not meant to be used that seriously, so take it with a grain of salt.
            STB_LANG_EMIT_CODE("mem_%d: .asciz \"%s\\0\"\n", idx, iter.data);
        )
    ),
    STB_LANG_CODEGEN_LIST(
        STB_LANG_CODEGEN_CASE(IR_FUNCDEF_BEGIN,
            STB_LANG_EMIT_CODE("_%s:\n", instr->dest->value);
            STB_LANG_EMIT_CODE("\tstp x29, x30, [sp, #-16]!\n");
            STB_LANG_EMIT_CODE("\tmov x29, sp\n");

            // STB_LANG_FUNCTION_ALLOCATE(gen, 8);
            int offset = 16;
            STB_LANG_GO_TO_FUNC(instr->dest->value);
            // STB_LANG_ITERATE(STB_LANG_CURRENT_SCOPE()->symbols, Lang_TypeInfo_Symbol,
            //     if (iter.kind == STB_LANG_SYMBOL_VARIABLE){
            //         offset = iter.data.variable.offset;
            //     };
            // );
            (void)offset;
            STB_LANG_EMIT_CODE("\tsub sp, sp, #%d\n", 4080);
        )
        STB_LANG_CODEGEN_CASE(IR_EXTERN,
        )
        STB_LANG_CODEGEN_CASE(IR_FUNCDEF_END,
            STB_LANG_EMIT_CODE("\tmov sp, x29\n");
            STB_LANG_EMIT_CODE("\tldp x29, x30, [sp], #16\n");
            STB_LANG_EMIT_CODE("\tret\n");
        )
        STB_LANG_CODEGEN_CASE(IR_RET,
            STB_LANG_ARM_MOVE(8, instr->left, "x0")

            STB_LANG_EMIT_CODE("\tmov sp, x29\n");
            STB_LANG_EMIT_CODE("\tldp x29, x30, [sp], #16\n");
            STB_LANG_EMIT_CODE("\tret\n");
        )
        STB_LANG_CODEGEN_CASE(IR_LABEL,
            STB_LANG_EMIT_CODE(".L_label_%s:\n", instr->dest->value)
        )
        STB_LANG_CODEGEN_CASE(IR_ADDR,
            char *dest = STB_LANG_REGISTER(instr->dest->phys, 8);
            if (instr->left->type == IR_VAR){
                int newoffset = STB_CONCAT(CUR_CODEGEN_PREFIX, _get_offset_from_var)(gen, instr->left->value);
                if (newoffset == 0) {
                    STB_LANG_EMIT_CODE("\tmov %s, sp\n", dest);
                } else {
                    STB_LANG_EMIT_CODE("\tadd %s, sp, #%d\n", dest, newoffset);
                }
            }
        )
        STB_LANG_CODEGEN_CASE(IR_LOAD,
            int size = STB_LANG_LOOKUP_SIZE(gen->root_scope, &instr->typeinfo);
            if (size == -1 || size == 0) size = 8;
            char *dest = STB_LANG_REGISTER(instr->dest->phys, size);
            char *tmp = STB_LANG_REGISTER(instr->phys[0], 8);
            STB_LANG_ARM_MOVE(8, instr->left, tmp);
            
            if (size == 1) {
                STB_LANG_EMIT_CODE("\tldrb %s, [%s]\n", dest, tmp);
            } else {
                STB_LANG_EMIT_CODE("\tldr %s, [%s]\n", dest, tmp);
            }
        )
        STB_LANG_CODEGEN_CASE(IR_STORE,
            int size = STB_LANG_LOOKUP_SIZE(gen->root_scope, &instr->typeinfo);

            char *val_reg = STB_LANG_REGISTER(instr->phys[0], size);
            STB_LANG_ARM_MOVE(size, instr->left, val_reg);

            char *addr_reg = STB_LANG_REGISTER(instr->phys[1], 8);
            STB_LANG_ARM_MOVE(8, instr->dest, addr_reg);


            if (size == 1) {
                STB_LANG_EMIT_CODE("\tstrb %s, [%s]\n", val_reg, addr_reg);
            }else {
                STB_LANG_EMIT_CODE("\tstr %s, [%s]\n", val_reg, addr_reg);
            }
        )
        STB_LANG_CODEGEN_2CASES(IR_ASSIGN, IR_DECL,
            ;if (instr->dest->type == IR_VAR){
                int size = STB_CONCAT(CUR_CODEGEN_PREFIX, _get_size_from_var)(gen, instr->dest->value);
                int offset = STB_CONCAT(CUR_CODEGEN_PREFIX, _get_offset_from_var)(gen, instr->dest->value);
                ;
                if (instr->left != NULL){
                    STB_LANG_ARM_MOVE(size, instr->left, STB_LANG_REGISTER(instr->phys[0], size))
                }
                if (offset == 0){
                    STB_LANG_EMIT_CODE("\tstr %s, [sp]\n", STB_LANG_REGISTER(instr->phys[0], size));
                }else {
                    STB_LANG_EMIT_CODE("\tstr %s, [sp, #%d]\n", STB_LANG_REGISTER(instr->phys[0], size), offset);
                }

            }else if (instr->dest->type == IR_REG){
                // printf("A, %s\n", instr->dest->value);
                // STB_LANG_EMIT_CODE("\tan, %s\n", instr->dest->value);
                char string[32];
                if (instr->dest->value[0] == 'a'){
                    snprintf(string, 32, "x%d", atoi(instr->dest->value + 1));
                    STB_LANG_ARM_MOVE(8, instr->left, string)
                }else if (instr->dest->value[0] == 't'){
                    snprintf(string, 32, "%s", instr->dest->value);
                    STB_LANG_ARM_MOVE(8, instr->left, string)
                }else if (instr->dest->value[0] == '.'){
                    int n = atoi(instr->dest->value + 4);
                    // STB_LANG_EMIT_CODE("\tstart\n");
                    STB_LANG_ARM_MOVE(8, instr->left, STB_LANG_REGISTER(instr->phys[0], 8))
                    STB_LANG_EMIT_CODE("\tstr %s, [sp, #%d]\n", STB_LANG_REGISTER(instr->phys[0], 8), n*8);
                }
                // STB_LANG_EMIT_CODE("\tana\n");
            };
            // STB_LANG_EMIT_CODE("\tb\n");
        )
        STB_LANG_CODEGEN_CASE(IR_PUSH,
            STB_LANG_ARM_MOVE(8, instr->left, STB_LANG_REGISTER(instr->phys[0], 8));
            STB_LANG_EMIT_CODE("\tstr %s, [sp, #-16]!\n", STB_LANG_REGISTER(instr->phys[0], 8));
        )
        STB_LANG_CODEGEN_CASE(IR_POP,
            STB_LANG_EMIT_CODE("\tldr %s, [sp], #16\n", instr->dest->value);
        )
        STB_LANG_CODEGEN_CASE(IR_JUMP_IF_FALSE,
            STB_LANG_EMIT_CODE("\tcbz %s, .L_label_%s\n", 
                STB_LANG_REGISTER(instr->right->phys, 8), 
                instr->dest->value
            );
        )
        STB_LANG_CODEGEN_CASE(IR_CALL,
            STB_LANG_EMIT_CODE("\tbl _%s\n", instr->dest->value);
        )
        STB_LANG_CODEGEN_CASE(IR_JUMP,
            STB_LANG_EMIT_CODE("\tb .L_label_%s\n", instr->dest->value);
        )
        STB_LANG_CODEGEN_CASE(IR_ADD,
            STB_LANG_ARM_BINARY("add")
        )
        STB_LANG_CODEGEN_CASE(IR_SUB,
            STB_LANG_ARM_BINARY("sub")
        )
        STB_LANG_CODEGEN_CASE(IR_MUL,
            STB_LANG_ARM_BINARY("mul")
        )
        STB_LANG_CODEGEN_CASE(IR_DIV,
            STB_LANG_ARM_BINARY("sdiv")
        )
        STB_LANG_CODEGEN_CASE(IR_MOD,
            char *left = STB_LANG_REGISTER(instr->phys[0], 8);
            char *right = STB_LANG_REGISTER(instr->phys[1], 8);
            char *tmp = STB_LANG_REGISTER(instr->phys[2], 8);


            STB_LANG_ARM_MOVE(8, instr->left, left);
            STB_LANG_ARM_MOVE(8, instr->right, right);

            ;
            STB_LANG_EMIT_CODE("\tsdiv %s, %s, %s\n", tmp, left, right);
            {
                STB_LANG_EMIT_CODE("\tmsub %s, %s, %s, %s\n", STB_LANG_REGISTER(instr->dest->phys, 8), tmp, right, left);
            }
        )
        STB_LANG_CODEGEN_CASE(IR_BSHL,
            STB_LANG_ARM_BINARY("lsl")
        )
        STB_LANG_CODEGEN_CASE(IR_BSHR,
            STB_LANG_ARM_BINARY("lsr")
        )
        STB_LANG_CODEGEN_CASE(IR_BOR,
            STB_LANG_ARM_BINARY("orr")
        )
        STB_LANG_CODEGEN_CASE(IR_BAND,
            STB_LANG_ARM_BINARY("and")
        )
        STB_LANG_CODEGEN_CASE(IR_XOR,
            STB_LANG_ARM_BINARY("eor")
        )
        STB_LANG_CODEGEN_CASE(IR_AND,
            char *left = STB_LANG_REGISTER(instr->phys[0], 8);
            char *right = STB_LANG_REGISTER(instr->phys[1], 8);
            STB_LANG_ARM_MOVE(8, instr->left, left);
            STB_LANG_ARM_MOVE(8, instr->right, right);

            STB_LANG_EMIT_CODE("\tcmp %s, #0\n", left);
            STB_LANG_EMIT_CODE("\tcset %s, ne\n", left);


            STB_LANG_EMIT_CODE("\tcmp %s, #0\n", right);
            STB_LANG_EMIT_CODE("\tcset %s, ne\n", right);

            STB_LANG_EMIT_CODE("\tand %s, %s, %s\n", STB_LANG_REGISTER(instr->dest->phys, 8), left, right);
        )
        STB_LANG_CODEGEN_CASE(IR_OR,
            char *left = STB_LANG_REGISTER(instr->phys[0], 8);
            char *right = STB_LANG_REGISTER(instr->phys[1], 8);
            STB_LANG_ARM_MOVE(8, instr->left, left);
            STB_LANG_ARM_MOVE(8, instr->right, right);

            STB_LANG_EMIT_CODE("\tcmp %s, #0\n", left);
            STB_LANG_EMIT_CODE("\tcset %s, ne\n", left);


            STB_LANG_EMIT_CODE("\tcmp %s, #0\n", right);
            STB_LANG_EMIT_CODE("\tcset %s, ne\n", right);

            STB_LANG_EMIT_CODE("\torr %s, %s, %s\n", STB_LANG_REGISTER(instr->dest->phys, 8), left, right);
        )
        STB_LANG_CODEGEN_CASE(IR_LT,
            STB_LANG_ARM_COMPARISON("lt")
        )
        STB_LANG_CODEGEN_CASE(IR_LTE,
            STB_LANG_ARM_COMPARISON("le")
        )
        STB_LANG_CODEGEN_CASE(IR_GT,
            STB_LANG_ARM_COMPARISON("gt")
        )
        STB_LANG_CODEGEN_CASE(IR_GTE,
            STB_LANG_ARM_COMPARISON("ge")
        )
        STB_LANG_CODEGEN_CASE(IR_EQ,
            STB_LANG_ARM_COMPARISON("eq")
        )
        STB_LANG_CODEGEN_CASE(IR_NEQ,
            STB_LANG_ARM_COMPARISON("ne")
        )
        STB_LANG_CODEGEN_CASE(IR_NOP,
        )
    )
);
