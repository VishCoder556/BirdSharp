typedef struct {
    char *name;
    int len;
    bool isArg; // Is argument
}AS_Variable;

typedef struct {
    AS_Variable *variables;
    int variablelen;
}AS_Scope;

typedef struct {
    char *name;
    AS_Scope scope;
}AS_Func;


char dataS[5000];
int dataN = 0;
int funcN = 0;
AS_Func current_func;


AS_Scope as_initialize_scope(){
    AS_Scope scope;
    scope.variablelen = 0;
    scope.variables = malloc(sizeof(AS_Variable) * 100);
    return scope;
};

void as_add_variable_in_current_scope(AS_Variable variable){
   current_func.scope.variables[current_func.scope.variablelen++] = variable;
}

AS_Variable as_find_variable_in_current_scope(Generator *generator, AST ast, char *name){
    for (int i=0; i<current_func.scope.variablelen; i++){
        if (strcmp(current_func.scope.variables[i].name, name) == 0){
            return current_func.scope.variables[i];
        };
    };
    char string[100];
    snprintf(string, 100, "Variable \"%s\" doesn't exist", ast.data.arg.value);
    error_generate_parser("VarError", string, ast.row, ast.col, generator->name);
    return (AS_Variable){0};
}



char *arg_num_to_reg(int regn){
    switch (regn) {
        case 0: return "rdi";
        case 1: return "rsi";
        case 2: return "rdx";
        case 3: return "rcx";
        case 4: return "r8";
        case 5: return "r9";
        default: {error_generate("TODO", "Add more registers"); return "";}
    };
};


char *arg_num_to_reg_syscall(int regn){
    switch (regn) {
        case 0: return "rdi";
        case 1: return "rsi";
        case 2: return "rdx";
        case 3: return "r10";
        case 4: return "r8";
        case 5: return "r9";
        default: {error_generate("TODO", "Add more registers"); return "";}
    };
};



int typeinfo_to_len(AST_TypeInfo type){
    if (type.ptrnum > 0){
        return 8;
    }
    if (strcmp(type.type, "int") == 0){
        return 4;
    }else if (strcmp(type.type, "long") == 0){
        return 8;
    }else if (strcmp(type.type, "short") == 0){
        return 2;
    }else if (strcmp(type.type, "char") == 0){
        return 1;
    }else if(strcmp(type.type, "bool") == 0){
        return 1; // We cannot access bits
    };
    return 0;
};

char *len_to_selector(int i){
    switch (i){
        case 1: return "byte";
        case 2: return "word";
        case 4: return "dword";
        case 8: return "qword";
        default: {
            error_generate("SelectorError", "Invalid length");
        }
    }
    return "";
}


char *regs64[] = {"rax", "rbx", "rcx", "rdx", "rsi", "rdi", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};
char *regs32[] = {"eax", "ebx", "ecx", "edx", "esi", "edi", "r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d"};
char *regs16[] = {"ax", "bx", "cx", "dx", "si", "di", "r8w", "r9w", "r10w", "r11w", "r12w", "r13w", "r14w", "r15w"};
char *regs8[] = {"al", "bl", "cl", "dl", "sil", "dil", "r8b", "r9b", "r10b", "r11b", "r12b", "r13b", "r14b", "r15b"};
char *regs128[] = {"xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7", "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", "xmm14", "xmm15"};


int get_reg_size(char *reg){
    for (int i = 0; i < sizeof(regs128) / sizeof(regs128[0]); i++) {
        if (strcmp(reg, regs128[i]) == 0) {
            return 16;
        }
    };
    for (int i = 0; i < sizeof(regs64) / sizeof(regs64[0]); i++) {
        if (strcmp(reg, regs64[i]) == 0) {
            return 8;
        }
    };
    for (int i = 0; i < sizeof(regs32) / sizeof(regs32[0]); i++) {
        if (strcmp(reg, regs32[i]) == 0) {
            return 4;
        }
    };
    for (int i = 0; i < sizeof(regs16) / sizeof(regs16[0]); i++) {
        if (strcmp(reg, regs16[i]) == 0) {
            return 2;
        }
    };
    for (int i = 0; i < sizeof(regs8) / sizeof(regs8[0]); i++) {
        if (strcmp(reg, regs8[i]) == 0) {
            return 1;
        }
    };
    if (reg[0] == 'd'){
        return 4;
    }
    if (reg[0] == 'q'){
        return 8;
    }
    if (reg[0] == 'b'){
        return 1;
    }
    if (reg[0] == 'w'){
        return 2;
    }
    return 0;
}

char *reg_based_on_size(char *reg, int len) {
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

    if (reg[0] == 'w'){
        reg += 5;
    }else if (reg[0] == 'b'){
        reg += 5;
    }else if (reg[0] == 'q'){
        reg += 6;
    }else if (reg[0] == 'd'){
        reg += 6;
    }else {
        return reg;
    };

    char reg2[100];
    switch (len){
        case 1: strncpy(reg2, "byte ", 100); break;
        case 2: strncpy(reg2, "word ", 100); break;
        case 4: strncpy(reg2, "dword ", 100); break;
        case 8: strncpy(reg2, "qword ", 100); break;
        default: return reg;
    }
    strncat(reg2, reg, 100);

    return strdup(reg2);
}




void assembler_init(void *generator){
    strncpy(dataS, "", 5000);
    generator_open_text(generator);
    generator_write_text(generator, "default rel\nBITS 64\nsection .text\nglobal start\n");
};


void assembler_generate_expr(void *generator, AST ast, char *reg);

void as_select_variable(Generator *generator, AST ast, int len, int offset){
    char string[100];
    snprintf(string, 100, "%s [rsp + %d]", len_to_selector(len), offset);
    if (ast.data.assign.from->type == AST_DEREF){
        assembler_generate_expr(generator, *ast.data.assign.from->data.expr.left, "r14");
        // snprintf(string, 100, "qword [rsp + %d]", offset);
        // generator_write_text(generator, "\tmov r14, ");
        // generator_write_text(generator, string);
        // generator_write_text(generator, "\n");
        snprintf(string, 100, "%s [r14]", len_to_selector(len));
    }else if (ast.data.assign.from->type == AST_INDEX){
        assembler_generate_expr(generator, *ast.data.assign.from->data.expr.left, "rdi");
        assembler_generate_expr(generator, *ast.data.assign.from->data.expr.right, "rsi");
        AST_TypeInfo typeinfo = ast.data.assign.from->data.expr.left->typeinfo;
        typeinfo.ptrnum--;
        snprintf(string, 100, "%s [rdi + rsi * %d]", len_to_selector(len), typeinfo_to_len(typeinfo));
        assembler_generate_expr(generator, *ast.data.assign.assignto, string);
        return;
    }
    assembler_generate_expr(generator, *ast.data.assign.assignto, string);
};

void assembler_generate_expr(void *generator, AST ast, char *reg){
    if (ast.type == AST_INT){
        generator_write_text(generator, "\tmov ");
        generator_write_text(generator, reg);
        generator_write_text(generator, ", ");
        generator_write_text(generator, ast.data.arg.value);
        generator_write_text(generator, "\n");
    }else if (ast.type == AST_CHAR){
        generator_write_text(generator, "\tmov ");
        generator_write_text(generator, reg);
        generator_write_text(generator, ", ");
        if (strcmp(ast.data.arg.value, "\n") == 0){
            generator_write_text(generator, "10");
        }else {
            generator_write_text(generator, "'");
            generator_write_text(generator, ast.data.arg.value);
            generator_write_text(generator, "'");
        }
        generator_write_text(generator, "\n");
    }else if (ast.type == AST_STRING) {
        strcat(dataS, "\t");
        char str[20];
        snprintf(str, 20, "_LBC%d", dataN);
        strcat(dataS, str);
        strcat(dataS, ": db '");
        for (int i=0; i<strlen(ast.data.arg.value); i++){
            if (ast.data.arg.value[i] == '\n'){
                strcat(dataS, "', 10");
                if (i+1 != strlen(ast.data.arg.value)){
                    strcat(dataS, ", '");
                }
            }else {
                char a[2];
                a[0] = ast.data.arg.value[i];
                a[1] = '\0';
                strcat(dataS, a);
                if (i+1 == strlen(ast.data.arg.value)){
                    strcat(dataS, "'");
                }
            }
        }
        strcat(dataS, ", 0\n");


        generator_write_text(generator, "\tlea r10, [");
        generator_write_text(generator, str);
        generator_write_text(generator, "]\n\tmov ");
        generator_write_text(generator, reg);
        generator_write_text(generator, ", r10\n");
        dataN++;
    }else if(ast.type == AST_SYSCALL){
        for (int i=1; i<ast.data.funcall.argslen; i++){
            generator_write_text(generator, "\tmov ");
            char string[100];
            snprintf(string, 100, "[syscall_safe + %d]", i*8);
            generator_write_text(generator, string);
            generator_write_text(generator, ", ");
            generator_write_text(generator, arg_num_to_reg_syscall(i - 1));
            generator_write_text(generator, "\n");
        };
        for (int i=1; i<ast.data.funcall.argslen; i++){
            char *reg1 = reg_based_on_size( arg_num_to_reg_syscall(i - 1), typeinfo_to_len(ast.data.funcall.args[i]->typeinfo));
            assembler_generate_expr(generator, *(ast.data.funcall.args[i]), reg1);

            generator_write_text(generator, "\tmov ");
            char string[100];
            snprintf(string, 100, "[syscall_nowrite + %d]", i*8);
            generator_write_text(generator, string);
            generator_write_text(generator, ", ");
            generator_write_text(generator, arg_num_to_reg_syscall(i - 1));
            generator_write_text(generator, "\n\tmov ");
            generator_write_text(generator, arg_num_to_reg_syscall(i - 1));
            generator_write_text(generator, ", ");
            snprintf(string, 100, "[syscall_safe + %d]", i*8);
            generator_write_text(generator, string);
            generator_write_text(generator, "\n");

        };

        for (int i=1; i<ast.data.funcall.argslen; i++){
            generator_write_text(generator, "\tmov ");
            generator_write_text(generator, arg_num_to_reg_syscall(i - 1));
            generator_write_text(generator, ", ");
            char string[100];
            snprintf(string, 100, "[syscall_nowrite + %d]", i*8);
            generator_write_text(generator, string);
            generator_write_text(generator, "\n");
        };
        assembler_generate_expr(generator, *(ast.data.funcall.args[0]), "rax");
        generator_write_text(generator, "\tsyscall\n");


        for (int i=1; i<ast.data.funcall.argslen; i++){
            generator_write_text(generator, "\tmov ");
            generator_write_text(generator, arg_num_to_reg_syscall(i - 1));
            generator_write_text(generator, ", ");
            char string[100];
            snprintf(string, 100, "[syscall_safe + %d]", i*8);
            generator_write_text(generator, string);
            generator_write_text(generator, "\n");
        };

        if (strcmp(reg_based_on_size(reg, 8), "rax")){
            generator_write_text(generator, "\tmov ");
            generator_write_text(generator, reg);
            generator_write_text(generator, ", ");
            generator_write_text(generator, reg_based_on_size("rax", get_reg_size(reg)));
            generator_write_text(generator, "\n");
        }
    }else if(ast.type == AST_FUNCALL){
        for (int i=0; i<ast.data.funcall.argslen; i++){
            assembler_generate_expr(generator, *(ast.data.funcall.args[i]), reg_based_on_size(arg_num_to_reg(i), typeinfo_to_len(ast.data.funcall.args[i]->typeinfo)));
        };
        generator_write_text(generator, "\tcall ");
        generator_write_text(generator, ast.data.funcall.name);
        generator_write_text(generator, "\n");
        if (strcmp(reg_based_on_size(reg, 8), "rax")){
            generator_write_text(generator, "\tmov ");
            generator_write_text(generator, reg_based_on_size(reg, typeinfo_to_len(ast.typeinfo)));
            generator_write_text(generator, ", ");
            generator_write_text(generator, reg_based_on_size("rax", typeinfo_to_len(ast.typeinfo)));
            generator_write_text(generator, "\n");
        }
    }else if(ast.type == AST_DEREF){
        assembler_generate_expr(generator, *(ast.data.expr.left), "r11");
        generator_write_text(generator, "\tmov r11, [r11]\n\tmov ");
        generator_write_text(generator, reg);
        generator_write_text(generator, ", ");
        generator_write_text(generator, reg_based_on_size("r11", get_reg_size(reg)));
        generator_write_text(generator, "\n");
    }else if(ast.type == AST_INDEX){
        assembler_generate_expr(generator, *ast.data.expr.left, "rdi");
        assembler_generate_expr(generator, *ast.data.expr.right, "rsi");
        AST_TypeInfo typeinfo = ast.data.expr.left->typeinfo;
        typeinfo.ptrnum--;
        char string[100];
        snprintf(string, 100, "%s [rdi + rsi * %d]", len_to_selector(typeinfo_to_len(typeinfo)), typeinfo_to_len(typeinfo));
        generator_write_text(generator, "\tmov ");
        generator_write_text(generator, reg_based_on_size("rax", typeinfo_to_len(typeinfo)));
        generator_write_text(generator, ", ");
        generator_write_text(generator, string);
        generator_write_text(generator, "\n\tmov ");
        generator_write_text(generator, reg);
        generator_write_text(generator, ", ");
        generator_write_text(generator, reg_based_on_size("rax", get_reg_size(reg)));
        generator_write_text(generator, "\n");
    }else if(ast.type == AST_CAST){
        assembler_generate_expr(generator, *(ast.data.expr.left), reg_based_on_size(reg, typeinfo_to_len(ast.data.expr.left->typeinfo)));
    }else if(ast.type == AST_VAR){
        AS_Variable variable = (AS_Variable){"", 0};
        int e = 0;
        int j = 0;
        int len = 0;
        for (int i=0; i<current_func.scope.variablelen; i++){
            if (strcmp(current_func.scope.variables[i].name, ast.data.arg.value) == 0){
                e = 1;
                variable = current_func.scope.variables[i];
                len = variable.len;
                break;
            };
            j += current_func.scope.variables[i].len;
        };
        if (e == 0){
            char string[100];
            snprintf(string, 100, "Variable \"%s\" doesn't exist", ast.data.arg.value);
            error_generate_parser("VarError", string, ast.row, ast.col, ((Generator*)generator)->name);
        }
        char string[500];
        if (variable.isArg == 0){
            snprintf(string, 500, "\tmov %s, %s [rsp + %d]\n\tmov %s, %s\n", reg_based_on_size("r15", len), len_to_selector(len), j, reg, reg_based_on_size("r15", get_reg_size(reg)));
        }else if (variable.isArg == 1){
            j = 0;
            for (int i=0; i<current_func.scope.variablelen; i++){
                if (strcmp(current_func.scope.variables[i].name, ast.data.arg.value) == 0){
                    break;
                }
                if (current_func.scope.variables[i].isArg == 1){
                    j++;
                }
            }
            char str[30];
            snprintf(string, 100, "\tmov %s, [rsp + %d]\n", reg, j * 8);
            if (j == 0){
                snprintf(str, 30, "byte [rsp]");
            }else {
                snprintf(str, 30, "byte [rsp + %d]", j * 8);
            }

            char *r9 = reg_based_on_size("r9", get_reg_size(reg));
            snprintf(string, 500, "\tmov %s, %s\n\tmov %s, %s\n", r9, reg_based_on_size(str, get_reg_size(reg)), reg, r9);
        }else {
            error_generate("TODOError", "Todo");
        }
        generator_write_text(generator, string);
    }else if(ast.type == AST_PLUS){
        char *r8 = reg_based_on_size("r8", typeinfo_to_len(ast.data.expr.left->typeinfo));
        char *r9 = reg_based_on_size("r9", typeinfo_to_len(ast.data.expr.right->typeinfo));
        assembler_generate_expr(generator, *ast.data.expr.left, r8);
        assembler_generate_expr(generator, *ast.data.expr.right, r9);
        generator_write_text(generator, "\tadd r8, r9\n\tmov ");
        generator_write_text(generator, reg);
        generator_write_text(generator, ", ");
        generator_write_text(generator, r8);
        generator_write_text(generator, "\n");
    }else if(ast.type == AST_SUB){
        char *r8 = reg_based_on_size("r8", typeinfo_to_len(ast.data.expr.left->typeinfo));
        char *r9 = reg_based_on_size("r9", typeinfo_to_len(ast.data.expr.right->typeinfo));
        assembler_generate_expr(generator, *ast.data.expr.left, r8);
        assembler_generate_expr(generator, *ast.data.expr.right, r9);
        generator_write_text(generator, "\tsub r8, r9\n");
        if (strcmp(reg_based_on_size(r8, 4), reg_based_on_size(reg, 4))){
            generator_write_text(generator, "\tmov ");
            generator_write_text(generator, reg);
            generator_write_text(generator, ", ");
            generator_write_text(generator, r8);
            generator_write_text(generator, "\n");
        }
    }else if(ast.type == AST_MUL){
        char *rcx = reg_based_on_size("rcx", typeinfo_to_len(ast.data.expr.left->typeinfo));
        char *eax = reg_based_on_size("eax", typeinfo_to_len(ast.data.expr.right->typeinfo));
        assembler_generate_expr(generator, *ast.data.expr.left, rcx);
        assembler_generate_expr(generator, *ast.data.expr.right, eax);
        generator_write_text(generator, "\timul ecx, eax\n\tmov ");
        generator_write_text(generator, reg);
        generator_write_text(generator, ", ");
        generator_write_text(generator, rcx);
        generator_write_text(generator, "\n");
    }else if(ast.type == AST_DIV){
        char *rax = reg_based_on_size("rax", typeinfo_to_len(ast.data.expr.left->typeinfo));
        char *rbx = reg_based_on_size("rbx", typeinfo_to_len(ast.data.expr.right->typeinfo));
        assembler_generate_expr(generator, *ast.data.expr.left, rax);
        assembler_generate_expr(generator, *ast.data.expr.right, rbx);
        generator_write_text(generator, "\tcdq\n\tidiv ");
        generator_write_text(generator, rbx);
        generator_write_text(generator, "\n\tmov ");
        generator_write_text(generator, reg);
        generator_write_text(generator, ", ");
        generator_write_text(generator, reg_based_on_size("eax", get_reg_size(reg)));
        generator_write_text(generator, "\n");
    }else if(ast.type == AST_MODULO){
        char *rax = reg_based_on_size("rax", typeinfo_to_len(ast.data.expr.left->typeinfo));
        char *rbx = reg_based_on_size("rbx", typeinfo_to_len(ast.data.expr.right->typeinfo));
        assembler_generate_expr(generator, *ast.data.expr.left, rax);
        assembler_generate_expr(generator, *ast.data.expr.right, rbx);
        generator_write_text(generator, "\tcdq\n\tidiv ");
        generator_write_text(generator, rbx);
        generator_write_text(generator, "\n\tmov ");
        generator_write_text(generator, reg);
        generator_write_text(generator, ", ");
        generator_write_text(generator, reg_based_on_size("edx", get_reg_size(reg)));
        generator_write_text(generator, "\n");
    }else if(ast.type == AST_EQ){
        char *r12 = reg_based_on_size("r12", typeinfo_to_len(ast.data.expr.left->typeinfo));
        char *r13 = reg_based_on_size("r13", typeinfo_to_len(ast.data.expr.right->typeinfo));
        assembler_generate_expr(generator, *ast.data.expr.left, r12);
        assembler_generate_expr(generator, *ast.data.expr.right, r13);
        generator_write_text(generator, "\tcmp ");
        generator_write_text(generator, r12);
        generator_write_text(generator, ", ");
        generator_write_text(generator, r13);
        generator_write_text(generator, "\n\tsete al\n");
        if (strcmp(reg_based_on_size("rax", typeinfo_to_len(ast.typeinfo)), "al")) {
            generator_write_text(generator, "\tmovzx ");
            generator_write_text(generator, reg);
            generator_write_text(generator, ", al\n");
        }else if (strcmp(reg, "al") == 0){
            ;
        }else {
            generator_write_text(generator, "\tmov ");
            generator_write_text(generator, reg);
            generator_write_text(generator, ", al\n");
        }
    }else if(ast.type == AST_NEQ){
        char *r12 = reg_based_on_size("r12", typeinfo_to_len(ast.data.expr.left->typeinfo));
        char *r13 = reg_based_on_size("r13", typeinfo_to_len(ast.data.expr.right->typeinfo));
        assembler_generate_expr(generator, *ast.data.expr.left, r12);
        assembler_generate_expr(generator, *ast.data.expr.right, r13);
        generator_write_text(generator, "\tcmp ");
        generator_write_text(generator, r12);
        generator_write_text(generator, ", ");
        generator_write_text(generator, r13);
        generator_write_text(generator, "\n\tsetne al\n");
        if (strcmp(reg_based_on_size("rax", typeinfo_to_len(ast.typeinfo)), "al")) {
            generator_write_text(generator, "\tmovzx ");
            generator_write_text(generator, reg);
            generator_write_text(generator, ", al\n");
        }else if (strcmp(reg, "al") == 0){
            ;
        }else {
            generator_write_text(generator, "\tmov ");
            generator_write_text(generator, reg);
            generator_write_text(generator, ", al\n");
        }
    }else if(ast.type == AST_GT){
        char *r12 = reg_based_on_size("r12", typeinfo_to_len(ast.data.expr.left->typeinfo));
        char *r13 = reg_based_on_size("r13", typeinfo_to_len(ast.data.expr.right->typeinfo));
        assembler_generate_expr(generator, *ast.data.expr.left, r12);
        assembler_generate_expr(generator, *ast.data.expr.right, r13);
        generator_write_text(generator, "\tcmp ");
        generator_write_text(generator, r12);
        generator_write_text(generator, ", ");
        generator_write_text(generator, r13);
        generator_write_text(generator, "\n\tsetl al\n");
        if (strcmp(reg_based_on_size("rax", typeinfo_to_len(ast.typeinfo)), "al")) {
            generator_write_text(generator, "\tmovzx ");
            generator_write_text(generator, reg);
            generator_write_text(generator, ", al\n");
        }else if (strcmp(reg, "al") == 0){
            ;
        }else {
            generator_write_text(generator, "\tmov ");
            generator_write_text(generator, reg);
            generator_write_text(generator, ", al\n");
        }
    }else if(ast.type == AST_LT){
        char *r12 = reg_based_on_size("r12", typeinfo_to_len(ast.data.expr.left->typeinfo));
        char *r13 = reg_based_on_size("r13", typeinfo_to_len(ast.data.expr.right->typeinfo));
        assembler_generate_expr(generator, *ast.data.expr.left, r12);
        assembler_generate_expr(generator, *ast.data.expr.right, r13);
        generator_write_text(generator, "\tcmp ");
        generator_write_text(generator, r12);
        generator_write_text(generator, ", ");
        generator_write_text(generator, r13);
        generator_write_text(generator, "\n\tsetg al\n");
        if (strcmp(reg_based_on_size("rax", typeinfo_to_len(ast.typeinfo)), "al")) {
            generator_write_text(generator, "\tmovzx ");
            generator_write_text(generator, reg);
            generator_write_text(generator, ", al\n");
        }else if (strcmp(reg, "al") == 0){
            ;
        }else {
            generator_write_text(generator, "\tmov ");
            generator_write_text(generator, reg);
            generator_write_text(generator, ", al\n");
        }
    }else if(ast.type == AST_GTE){
        char *r12 = reg_based_on_size("r12", typeinfo_to_len(ast.data.expr.left->typeinfo));
        char *r13 = reg_based_on_size("r13", typeinfo_to_len(ast.data.expr.right->typeinfo));
        assembler_generate_expr(generator, *ast.data.expr.left, r12);
        assembler_generate_expr(generator, *ast.data.expr.right, r13);
        generator_write_text(generator, "\tcmp ");
        generator_write_text(generator, r12);
        generator_write_text(generator, ", ");
        generator_write_text(generator, r13);
        generator_write_text(generator, "\n\tsetle al\n");
        if (strcmp(reg_based_on_size("rax", typeinfo_to_len(ast.typeinfo)), "al")) {
            generator_write_text(generator, "\tmovzx ");
            generator_write_text(generator, reg);
            generator_write_text(generator, ", al\n");
        }else if (strcmp(reg, "al") == 0){
            ;
        }else {
            generator_write_text(generator, "\tmov ");
            generator_write_text(generator, reg);
            generator_write_text(generator, ", al\n");
        }
    }else if(ast.type == AST_LTE){
        char *r12 = reg_based_on_size("r12", typeinfo_to_len(ast.data.expr.left->typeinfo));
        char *r13 = reg_based_on_size("r13", typeinfo_to_len(ast.data.expr.right->typeinfo));
        assembler_generate_expr(generator, *ast.data.expr.left, r12);
        assembler_generate_expr(generator, *ast.data.expr.right, r13);
        generator_write_text(generator, "\tcmp ");
        generator_write_text(generator, r12);
        generator_write_text(generator, ", ");
        generator_write_text(generator, r13);
        generator_write_text(generator, "\n\tsetge al\n");
        if (strcmp(reg_based_on_size("rax", typeinfo_to_len(ast.typeinfo)), "al")) {
            generator_write_text(generator, "\tmovzx ");
            generator_write_text(generator, reg);
            generator_write_text(generator, ", al\n");
        }else if (strcmp(reg, "al") == 0){
            ;
        }else {
            generator_write_text(generator, "\tmov ");
            generator_write_text(generator, reg);
            generator_write_text(generator, ", al\n");
        }
    }else if(ast.type == AST_EXPR){
        assembler_generate_expr(generator, *ast.data.expr.left, reg);
    }else if(ast.type == AST_NOT){
        char *r15 = reg_based_on_size("r15", typeinfo_to_len(ast.data.expr.left->typeinfo));
        assembler_generate_expr(generator, *ast.data.expr.left, r15);
        generator_write_text(generator, "\tcmp ");
        generator_write_text(generator, r15);
        generator_write_text(generator, ", 0\n\tsete al\n\t");
        if (strcmp(reg, "al") == 0){
        }else if (strcmp(reg_based_on_size("eax", typeinfo_to_len(ast.typeinfo)), "al") == 0){
            generator_write_text(generator, "mov ");
            generator_write_text(generator, reg);
            generator_write_text(generator, ", al\n");
        }else {
            generator_write_text(generator, "movzx ");
            generator_write_text(generator, reg);
            generator_write_text(generator, ", al\n");
        }
    }else if(ast.type == AST_AND){
        char *r12 = reg_based_on_size("r12", typeinfo_to_len(ast.data.expr.left->typeinfo));
        char *r13 = reg_based_on_size("r13", typeinfo_to_len(ast.data.expr.right->typeinfo));
        assembler_generate_expr(generator, *ast.data.expr.left, r12);
        assembler_generate_expr(generator, *ast.data.expr.right, r13);
        char a[100];snprintf(a, 100, "._LBF%d", funcN++);
        char b[100];snprintf(b, 100, "._LBF%d", funcN++);
        generator_write_text(generator, "\tcmp ");
        generator_write_text(generator, r12);
        generator_write_text(generator, ", 0\n\tje ");
        generator_write_text(generator, a);
        generator_write_text(generator, "\n\tcmp ");
        generator_write_text(generator, r13);
        generator_write_text(generator, ", 0\n\tje ");
        generator_write_text(generator, a);
        generator_write_text(generator, "\n\tmov ");
        generator_write_text(generator, reg);
        generator_write_text(generator, ", 1\n\tjmp ");
        generator_write_text(generator, b);
        generator_write_text(generator, "\n");
        generator_write_text(generator, a);
        generator_write_text(generator, ":\n\tmov ");
        generator_write_text(generator, reg);
        generator_write_text(generator, ", 0\n");
        generator_write_text(generator, b);
        generator_write_text(generator, ":\n");
    }else if(ast.type == AST_OR){
        char *rl = reg_based_on_size("rbx", typeinfo_to_len(ast.data.expr.left->typeinfo));
        char *rr = reg_based_on_size("rcx", typeinfo_to_len(ast.data.expr.right->typeinfo));
        assembler_generate_expr(generator, *ast.data.expr.left, rl);
        assembler_generate_expr(generator, *ast.data.expr.right, rr);
        char a[100];snprintf(a, 100, "._LBF%d", funcN++);
        char b[100];snprintf(b, 100, "._LBF%d", funcN++);
        generator_write_text(generator, "\tmov ");
        generator_write_text(generator, reg);
        generator_write_text(generator, ", 0\n\tcmp ");
        generator_write_text(generator, rl);
        generator_write_text(generator, ", 0\n\tjne ");
        generator_write_text(generator, a);
        generator_write_text(generator, "\n\tcmp ");
        generator_write_text(generator, rr);
        generator_write_text(generator, ", 0\n\tjne ");
        generator_write_text(generator, a);
        generator_write_text(generator, "\n\tjmp ");
        generator_write_text(generator, b);
        generator_write_text(generator, "\n");
        generator_write_text(generator, a);
        generator_write_text(generator, ":\n\tmov ");
        generator_write_text(generator, reg);
        generator_write_text(generator, ", 1\n");
        generator_write_text(generator, b);
        generator_write_text(generator, ":\n");
    }else if(ast.type == AST_MODE){
        if (strcmp(ast.data.mode.name, "extern") == 0){
            generator_write_text(generator, "extern _");
            generator_write_text(generator, ast.data.mode.res);
            generator_write_text(generator, "\n");
        }
    }else {
        error_generate("TodoError", "Expression not supported");
    };
};


void assembler_generate_ast(void *generator, AST ast){
    if (ast.type == AST_FUNCDEF){
        funcN = 0;
        current_func = (AS_Func){ast.data.funcdef.name, as_initialize_scope()};
        for (int i=0; i<ast.data.funcdef.argslen; i++){
            as_add_variable_in_current_scope((AS_Variable){ast.data.funcdef.args[i]->arg, typeinfo_to_len(ast.data.funcdef.args[i]->type), 1});
        };
        if (strcmp(ast.data.funcdef.name, "main")){
            generator_write_text(generator, ast.data.funcdef.name);
            generator_write_text(generator, ":\n\tpush rbp\n\tmov rbp, rsp\n\tsub rsp, 16\n");
            for (int i=0; i<ast.data.funcdef.argslen; i++){
                generator_write_text(generator, "\tpush ");
                generator_write_text(generator, arg_num_to_reg(i));
                generator_write_text(generator, "\n");
            };
        }else {
            generator_write_text(generator, "start:\n\tmov rdi, [rsp]\n\tlea rsi, [rsp + 8]\n\tpush rbp\n\tmov rbp, rsp\n\tsub rsp, 32\n\tpush rsi\n\tpush rdi\n");
        }
        for (int i=0; i<ast.data.funcdef.blocklen; i++){
            assembler_generate_ast(generator, *(ast.data.funcdef.block[i]));
        };
    }else if(ast.type == AST_RET){
        if (strcmp(current_func.name, "main") == 0){
            assembler_generate_expr(generator, *(ast.data.ret.ret), "edi");
            // generator_write_text(generator, "\tmov rsp, rbp\n");
            // generator_write_text(generator, "\tpop rbp\n");
            generator_write_text(generator, "\tmov rax, 0x2000001\n");
            generator_write_text(generator, "\tsyscall\n");
        }else {
            assembler_generate_expr(generator, *(ast.data.ret.ret), reg_based_on_size("rax", typeinfo_to_len(ast.data.ret.ret->typeinfo)));
            AS_Variable variable = (AS_Variable){"", 0};
            int j = 0;
            for (int i=0; i<current_func.scope.variablelen; i++){
                if (current_func.scope.variables[i].isArg == 1){
                    generator_write_text(generator, "\tpop ");
                    generator_write_text(generator, arg_num_to_reg(j++));
                    generator_write_text(generator, "\n");
                };
            };

            generator_write_text(generator, "\tadd rsp, 16\n");
            generator_write_text(generator, "\tpop rbp\n");
            generator_write_text(generator, "\tret\n");
        }
    }else if(ast.type == AST_ASSIGN){
        int iv = 0;
        int len = typeinfo_to_len(ast.typeinfo);
        int v = 0;

        AST *ast1 = ast.data.assign.from;
        char *name = ast1->data.arg.value;
        while (ast1->type == AST_DEREF || ast1->type == AST_INDEX){
            ast1 = ast1->data.expr.left;
            name = ast1->data.arg.value;
        }

        for (int i=0; i<current_func.scope.variablelen; i++){
            if (strcmp(current_func.scope.variables[i].name, name) == 0){
                v = 1;
                break;
            }
            iv += current_func.scope.variables[i].len;
        };

        if (v == 0){
            as_add_variable_in_current_scope((AS_Variable){ast.data.assign.from->data.arg.value, len, 0});
        }
        as_select_variable(generator, ast, len, iv);
    }else if(ast.type == AST_IF){
        assembler_generate_expr(generator, *(ast.data.if1.block.condition), reg_based_on_size("rax", typeinfo_to_len(ast.data.if1.block.condition->typeinfo)));
        generator_write_text(generator, "\tcmp ");
        generator_write_text(generator, reg_based_on_size("rax", typeinfo_to_len(ast.data.if1.block.condition->typeinfo)));
        generator_write_text(generator, ", 0\n");
        char a[100];snprintf(a, 100, "._LBF%d", funcN++);
        char b[100];snprintf(b, 100, "._LBF%d", funcN++);
        char c[100];snprintf(c, 100, "._LBF%d", funcN++);
        generator_write_text(generator, "\tje ");
        generator_write_text(generator, b);
        generator_write_text(generator, "\n");
        generator_write_text(generator, a);
        generator_write_text(generator, ":\n");
        for (int i=0; i<ast.data.if1.block.statementlen; i++){
            assembler_generate_ast(generator, *ast.data.if1.block.statements[i]);
        };
        generator_write_text(generator, "\tjmp ");
        generator_write_text(generator, c);
        generator_write_text(generator, "\n");
        generator_write_text(generator, b);
        generator_write_text(generator, ":\n");

        for (int i=0; i<ast.data.if1.elselen; i++){
            assembler_generate_ast(generator, *ast.data.if1.else1[i]);
        };

        generator_write_text(generator, "\n");
        generator_write_text(generator, c);
        generator_write_text(generator, ":\n");

    }else if(ast.type == AST_WHILE){
        char a[100];snprintf(a, 100, "._LBF%d", funcN++);
        char b[100];snprintf(b, 100, "._LBF%d", funcN++);
        assembler_generate_expr(generator, *(ast.data.while1.condition), reg_based_on_size("rax", typeinfo_to_len(ast.data.while1.condition->typeinfo)));
        generator_write_text(generator, "\tcmp ");
        generator_write_text(generator, reg_based_on_size("rax", typeinfo_to_len(ast.data.while1.condition->typeinfo)));
        generator_write_text(generator, ", 0\n");
        generator_write_text(generator, "\tje ");
        generator_write_text(generator, b);
        generator_write_text(generator, "\n");
        generator_write_text(generator, a);
        generator_write_text(generator, ":\n");
        for (int i=0; i<ast.data.while1.blocklen; i++){
            assembler_generate_ast(generator, *ast.data.while1.block[i]);
        };
        
        assembler_generate_expr(generator, *(ast.data.while1.condition), reg_based_on_size("rax", typeinfo_to_len(ast.data.while1.condition->typeinfo)));
        generator_write_text(generator, "\tcmp ");
        generator_write_text(generator, reg_based_on_size("rax", typeinfo_to_len(ast.data.while1.condition->typeinfo)));
        generator_write_text(generator, ", 0\n");
        generator_write_text(generator, "\tjne ");
        generator_write_text(generator, a);
        generator_write_text(generator, "\n");


        generator_write_text(generator, "\n");
        generator_write_text(generator, b);
        generator_write_text(generator, ":\n");
    }else {
        assembler_generate_expr(generator, ast, "rax");
    };
};

void assembler_close(void *generator){
    generator_write_text(generator, "section .data\n");
    generator_write_text(generator, dataS);
    generator_write_text(generator, "section .bss\n\tsyscall_safe resq 10\n\tsyscall_nowrite resq 10\n");

    generator_close(generator);
    Generator *gen = (Generator*)generator;


    char asm_output_name[100];
    char exec_output_name[100];
    char obj_output_name[100];
    strncpy(asm_output_name, gen->output->filename, strlen(gen->output->filename));
    strncpy(exec_output_name, asm_output_name, strlen(gen->output->filename) - 3);
    strncpy(obj_output_name, asm_output_name, strlen(gen->output->filename) - 3);
    asm_output_name[strlen(gen->output->filename)] = '\0';
    exec_output_name[strlen(gen->output->filename) - 4] = '\0';
    obj_output_name[strlen(gen->output->filename) - 4] = '\0';
    strcat(exec_output_name, ".out");
    strcat(obj_output_name, ".o");


    strcat(exec_output_name, "");

    char asm_command[100];
    snprintf(asm_command, 100, "yasm -f macho64 %s -o %s", asm_output_name, obj_output_name); // TODO: nasm is much more cross-platform, so maybe for the sake of being cross-platform we could use nasm, but yasm is way better for quickly compiling macho64
    system(asm_command);
    char ld_command[200];
    snprintf(ld_command, 200, "ld -macosx_version_min 13.00 -o %s %s %s", exec_output_name, obj_output_name, (_BirdSharpTypes == TYPE_STATIC) ? "-static" : "-L/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/lib -lSystem");
    system(ld_command);
}
