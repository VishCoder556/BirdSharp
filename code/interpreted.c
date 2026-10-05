dymarray_typenew(Lang_IR_Operand, 40, 1);

void print_escaped(char *str){
    for (int i=0; i<(int)strlen(str); i++){
        if (str[i] == '\\' && str[i+1] == 'n'){
            putchar('\n');
            i++;
        }else {
            putchar(str[i]);
        };
    }
};

int get_reg_num(char *str){
    int num = 0;
    if (str[0] == 'a'){
        num = atoi(str + 1) + 1;
    }else if (strcmp(str, "v0") == 0){
        num = 0;
    }else if (str[0] == 't'){
        num = atoi(str + 1) + 10;
    }else {
        stb_lang_error_major_global("RegError", "Could not find register number for \"%s\" in interpreted mode", str);
    }
    if (num > 40){
        stb_lang_error_major_global("RegError", "Not enough registers in interpreted mode");
    }
    return num;
};

#define STB_LANG_CODEGEN_INTRP_OP(op) \
Lang_IR_Operand left = operand_simplify(gen, *instr->left); \
Lang_IR_Operand right = operand_simplify(gen, *instr->right); \
Lang_IR_Operand total; \
if (left.type == IR_INT && right.type == IR_INT){ \
    total.type = IR_INT; \
    char str[16]; \
    snprintf(str, 16, "%d", atoi(left.value) op atoi(right.value)); \
    total.value = strdup(str); \
} \
if (instr->dest->type == IR_REG){ \
    gen->regs->data[get_reg_num(instr->dest->value)] = total; \
}

STB_LANG_NEW_CODEGEN(
    STB_LANG_CODEGEN_FIELDS(
        dymarray_Lang_IR_Operand *regs;
        // v0:0, a0.... = 1...
    ),
    STB_LANG_CODEGEN_INIT(
        gen->regs = malloc(sizeof(*gen->regs));
        *gen->regs = dymarray_Lang_IR_Operand_new();
    ),
    STB_LANG_CODEGEN_PREFIX(
    ),
    STB_LANG_CODEGEN_SUFFIX(
    ),
    STB_LANG_CODEGEN_FUNCS(
        Lang_IR_Operand operand_simplify(Lang_CodeGen_Intrp *gen, Lang_IR_Operand operand){
            if (operand.type == IR_REG){
                operand = gen->regs->data[get_reg_num(operand.value)];
                operand_simplify(gen, operand);
            }
            return operand;
        };
    ),
    STB_LANG_CODEGEN_LIST(
        STB_LANG_CODEGEN_CASE(IR_EXTERN,
        )
        STB_LANG_CODEGEN_CASE(IR_FUNCDEF_BEGIN,
        )
        STB_LANG_CODEGEN_CASE(IR_RET,
            if (instr->left->type == IR_INT){
                exit(atoi(instr->left->value));
            };
        )
        STB_LANG_CODEGEN_CASE(IR_FUNCDEF_END,
        )
        STB_LANG_CODEGEN_CASE(IR_ASSIGN,
            if (instr->dest->type == IR_REG){
                gen->regs->data[get_reg_num(instr->dest->value)] = *instr->left;
            }
        )
        STB_LANG_CODEGEN_CASE(IR_ADD,
            STB_LANG_CODEGEN_INTRP_OP(+);
        )
        STB_LANG_CODEGEN_CASE(IR_SUB,
            STB_LANG_CODEGEN_INTRP_OP(-);
        )
        STB_LANG_CODEGEN_CASE(IR_MUL,
            STB_LANG_CODEGEN_INTRP_OP(*);
        )
        STB_LANG_CODEGEN_CASE(IR_DIV,
            STB_LANG_CODEGEN_INTRP_OP(/);
        )

        STB_LANG_CODEGEN_CASE(IR_CALL,
            if (instr->dest->value != NULL){
            if (strcmp(instr->dest->value, "print") == 0){
                Lang_IR_Operand op = operand_simplify(gen, gen->regs->data[1]);
                if (op.type == IR_MEM){
                    STB_LANG_ITERATE(gen->symbols, Lang_IR_Symbol, 
                        if (idx == (long)op.value){
                            print_escaped(iter.data);
                        }
                    )
                }
            }else if (strcmp(instr->dest->value, "int_to_string") == 0){
                Lang_IR_Operand op = operand_simplify(gen, gen->regs->data[1]);
                if (op.type == IR_INT){
                    STB_CONCAT(CUR_IR_NAME, _Symbol) symbol = (STB_CONCAT(CUR_IR_NAME, _Symbol)){.data=op.value, .length=strlen(op.value)}; \
                    STB_CONCAT(STB_CONCAT3(dymarray_, CUR_IR_NAME, _Symbol), _add)(&gen->symbols, symbol); \
                    op.type = IR_MEM;
                    op.value = (char*)(long)(gen->symbols.datalen - 1);
                    gen->regs->data[0] = op;
                }
            }
            }
        )
    )
)
