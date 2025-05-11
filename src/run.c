#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct RuntimeString{
    char *string;
}RuntimeString;
typedef struct StringTable {
    RuntimeString runtimestrings[100];
    int stringtablelen;
}StringTable;
typedef struct Type {
    char *type;
    int ptrnum;
}Type;
typedef struct Register {
    Type type;
    union {
        int intval;
        char *strval;
    } value;
}Register;

typedef enum InstructionType{
    TYPE_LOAD,
    TYPE_MOV,
    TYPE_MOV_IMM,
    TYPE_SYSCALL,
    TYPE_RET,
    TYPE_PLUS,
    TYPE_SUB,
    TYPE_MUL,
    TYPE_DIV,
    TYPE_MODULO,
    TYPE_JMP,
    TYPE_CALL
}InstructionType;

typedef enum ExprType {
    TYPE_STRING,
    TYPE_INT
}ExprType;

typedef struct Expr {
    Type typeinf;
    ExprType type;
    union {
        int intval;
        char *strval;
    } value;
}Expr;

typedef struct CallBack {
    int addr;
}CallBack;

typedef struct CallStack {
    CallBack callbacks[100];
    int callbacklen;
}CallStack;

typedef struct Instruction {
    Type typeinf;
    InstructionType type;
    int regn;
    int regn2;
    Expr *expr;
}Instruction;

typedef struct RuntimeEnv {
    StringTable stringtable;
    int instr_len;
    Register registers[16];
    Instruction instructions[100];
    int instructionsLen;
    CallStack *callstack;
    bool a;
}RuntimeEnv;

Type parse_type(RuntimeEnv *env, FILE *f){
    Type type = (Type){};
    char initial_type;
    fread(&initial_type, 1, 1, f);
    if (initial_type >= 0x60 && initial_type <= 0x63){
        fprintf(stderr, "Not defined yet");
        exit(-1);
    }else if(initial_type >= 0x55 && initial_type <= 0x5d){ // Defined type
        if (initial_type == 0x55){
            type.type = "int";
        };
    }else {
        fprintf(stderr, "Not defined yet");
        exit(-1);
    };
    return type;
}

int parse_immediate(RuntimeEnv *env, FILE *f){
    int c;
    fread(&c, sizeof(int), 1, f);
    return c;
};


void expect_type(RuntimeEnv *env, int regnum, char *type, int ptrnum){
    Register reg = env->registers[regnum];
    if (!reg.type.type){
        fprintf(stderr, "Register has not been set yet; reading into invalid memory");
        exit(-1);
    };
    if (strcmp(reg.type.type, type)){
        fprintf(stderr, "Expected type %s (%d), got %s (%d)", type, ptrnum, reg.type.type, reg.type.ptrnum);
        exit(-1);
    };
};

Type get_type(RuntimeEnv *env, FILE *f){
    Type type;
    char c;
    fread(&c, 1, 1, f);
    type.ptrnum = 0;
    if (c == 0x6d){
        type.type = "int";
    }else if (c == 0x70){
        type.type = "unknown";
        type.ptrnum = 1;
    }else if (c == 0x6e){
        type.type = "char";
    }else if (c == 0x6f){
        type.type = "string";
    };

    return type;
};

Expr *parse_bytecode_expr(RuntimeEnv *env, FILE *f){
    Expr *expr = malloc(sizeof(Expr));
    // expr->typeinf = get_type(env, f);
    expr->typeinf = (Type){"int", 0};
    expr->type = TYPE_INT;
    if (strcmp(expr->typeinf.type, "int") == 0){
        expr->value.intval = parse_immediate(env, f);
    }
    return expr;
};


char parse_bytecode_body(RuntimeEnv *env, FILE *f){
    if (feof(f)){
        return -1;
    };
    char instr;
    fread(&instr, sizeof(char), 1, f);
    if ((instr >= 0x3e && instr <= 0x4d) || (instr >= 0x00 && instr <= 0x0f) || instr == 0x6a){
        if (instr >= 0x3e && instr <= 0x4d){
            env->instructions[env->instructionsLen].type = TYPE_MOV_IMM;
            env->instructions[env->instructionsLen].regn = instr - 0x3e;
            env->instructions[env->instructionsLen++].expr = parse_bytecode_expr(env,f);
        }else if (instr >= 0x00 && instr <= 0x0f){
            env->instructions[env->instructionsLen].type = TYPE_MOV;
            env->instructions[env->instructionsLen].regn = instr;
            env->instructions[env->instructionsLen++].expr = parse_bytecode_expr(env,f);
        }else if(instr == 0x6a){
            env->instructions[env->instructionsLen].type = TYPE_LOAD;
            char addr;
            fread(&addr, 1, 1, f);
            env->instructions[env->instructionsLen].regn = addr - 1;
            env->instructions[env->instructionsLen++].expr = parse_bytecode_expr(env,f);
        };
    }else if(instr == 0x20){
        char first_reg;
        char second_reg;
        fread(&first_reg, sizeof(char), 1, f);
        fread(&second_reg, sizeof(char), 1, f);
        env->instructions[env->instructionsLen].type = TYPE_PLUS;
        env->instructions[env->instructionsLen].regn = first_reg - 1;
        env->instructions[env->instructionsLen].regn2 = second_reg - 1;
        env->instructions[env->instructionsLen].typeinf.type = "int";
        env->instructions[env->instructionsLen++].typeinf.ptrnum = 0;
    }else if(instr == 0x21){
        char first_reg;
        char second_reg;
        fread(&first_reg, sizeof(char), 1, f);
        fread(&second_reg, sizeof(char), 1, f);
        env->instructions[env->instructionsLen].type = TYPE_SUB;
        env->instructions[env->instructionsLen].regn = first_reg - 1;
        env->instructions[env->instructionsLen].regn2 = second_reg - 1;
        env->instructions[env->instructionsLen].typeinf.type = "int";
        env->instructions[env->instructionsLen++].typeinf.ptrnum = 0;
    }else if(instr == 0x22){
        char first_reg;
        char second_reg;
        fread(&first_reg, sizeof(char), 1, f);
        fread(&second_reg, sizeof(char), 1, f);
        env->instructions[env->instructionsLen].type = TYPE_MUL;
        env->instructions[env->instructionsLen].regn = first_reg - 1;
        env->instructions[env->instructionsLen].regn2 = second_reg - 1;
        env->instructions[env->instructionsLen].typeinf.type = "int";
        env->instructions[env->instructionsLen++].typeinf.ptrnum = 0;
    }else if(instr == 0x23){
        char first_reg;
        char second_reg;
        fread(&first_reg, sizeof(char), 1, f);
        fread(&second_reg, sizeof(char), 1, f);
        env->instructions[env->instructionsLen].type = TYPE_DIV;
        env->instructions[env->instructionsLen].regn = first_reg - 1;
        env->instructions[env->instructionsLen].regn2 = second_reg - 1;
        env->instructions[env->instructionsLen].typeinf.type = "int";
        env->instructions[env->instructionsLen++].typeinf.ptrnum = 0;
    }else if(instr == 0x24){
        char first_reg;
        char second_reg;
        fread(&first_reg, sizeof(char), 1, f);
        fread(&second_reg, sizeof(char), 1, f);
        env->instructions[env->instructionsLen].type = TYPE_MODULO;
        env->instructions[env->instructionsLen].regn = first_reg - 1;
        env->instructions[env->instructionsLen].regn2 = second_reg - 1;
        env->instructions[env->instructionsLen].typeinf.type = "int";
        env->instructions[env->instructionsLen++].typeinf.ptrnum = 0;
    }else if(instr == 0x6b){
        env->instructions[env->instructionsLen++].type = TYPE_SYSCALL;
    }else if (instr == 0x53){
        env->instructions[env->instructionsLen++].type = TYPE_RET;
    }else if(instr == 0x52){
        char addr;
        fread(&addr, 1, 1, f);
        env->instructions[env->instructionsLen].type = TYPE_CALL;
        env->instructions[env->instructionsLen++].regn = addr;
    };
    return 0;
};

int parse_bytecode_run_expr(RuntimeEnv *env, Expr *expr){
    return expr->value.intval;
};

char parse_bytecode_instr(RuntimeEnv *env, int idx){
    if (idx > env->instructionsLen){
        return -1;
    };
    Instruction instr = env->instructions[idx];
    if (instr.type == TYPE_LOAD){
        env->registers[instr.regn].type = (Type){"string", 0};
        env->registers[instr.regn].value.strval = env->stringtable.runtimestrings[parse_bytecode_run_expr(env, instr.expr)].string;
    }else if (instr.type == TYPE_MOV){
        env->registers[instr.regn].type = (Type){"int", 0};
        // env->registers[instr.regn].value.intval = parse_bytecode_run_expr(env, instr.expr);
        env->registers[instr.regn].value.intval = env->registers[instr.regn2].value.intval;
    }else if (instr.type == TYPE_MOV_IMM){
        env->registers[instr.regn].type = (Type){"int", 0};
        env->registers[instr.regn].value.intval = parse_bytecode_run_expr(env, instr.expr);
    }else if (instr.type == TYPE_SYSCALL){
        expect_type(env, 1, "int", 0);
        expect_type(env, 2, "int", 0);
        expect_type(env, 3, "string", 0);
        expect_type(env, 4, "int", 0);
        if (env->registers[1].value.intval == 33554436){
            int result = write(env->registers[2].value.intval, env->registers[3].value.strval, env->registers[4].value.intval);
            env->registers[0].value.intval = result;
            env->registers[0].type = (Type){"int", 0};
        }else {
            fprintf(stderr, "Unsupported syscall: %d\n", env->registers[1].value.intval);
            exit(-1);
        };
    }else if (instr.type == TYPE_RET){
        expect_type(env, 0, "int", 0);
        if (env->callstack->callbacklen != 0){
            env->callstack->callbacklen--;
            parse_bytecode_instr(env, env->callstack->callbacks[env->callstack->callbacklen].addr + 1);
        }else {
            exit(env->registers[0].value.intval);
        }
    }else if(instr.type == TYPE_PLUS){
        expect_type(env, instr.regn, "int", 0);
        expect_type(env, instr.regn2, "int", 0);
        env->registers[instr.regn].value.intval += env->registers[instr.regn2].value.intval;
    }else if(instr.type == TYPE_SUB){
        expect_type(env, instr.regn, "int", 0);
        expect_type(env, instr.regn2, "int", 0);
        env->registers[instr.regn].value.intval -= env->registers[instr.regn2].value.intval;
    }else if(instr.type == TYPE_MUL){
        expect_type(env, instr.regn, "int", 0);
        expect_type(env, instr.regn2, "int", 0);
        env->registers[instr.regn].value.intval *= env->registers[instr.regn2].value.intval;
    }else if(instr.type == TYPE_DIV){
        expect_type(env, instr.regn, "int", 0);
        expect_type(env, instr.regn2, "int", 0);
        env->registers[instr.regn].value.intval /= env->registers[instr.regn2].value.intval;
    }else if(instr.type == TYPE_MODULO){
        expect_type(env, instr.regn, "int", 0);
        expect_type(env, instr.regn2, "int", 0);
        env->registers[instr.regn].value.intval %= env->registers[instr.regn2].value.intval;
    }else if(instr.type == TYPE_CALL){
        if (env->a == true) {
            env->callstack->callbacks[env->callstack->callbacklen++].addr = idx;
        }else {
            env->a = true;
        }
        parse_bytecode_instr(env, instr.regn);
    };
    parse_bytecode_instr(env, idx + 1);
    return 0;
};

/* char parse_bytecode(RuntimeEnv *env, FILE *f){
    char instr;
    fread(&instr, sizeof(char), 1, f);
    if (instr == 0x65){ // Funcdef
        parse_type(env, f);
        int namelen;
        fread(&namelen, sizeof(char), 1, f);
        char name[100];
        fgets(name, namelen+1, f);
        fprintf(stderr, "{%s}", name);
    };
    // fprintf(stderr, "%02x\n", instr);
    return 0;
}; */

int main(int argc, char **argv){
    char *input_file = argv[1];
    FILE *f = fopen(input_file, "rb");
    if (f == NULL){
        fprintf(stderr, "Error: File doesn't exist");
        return -1;
    };
    RuntimeEnv *runtimenv = malloc(sizeof(RuntimeEnv));
    runtimenv->callstack = malloc(sizeof(CallStack));
    runtimenv->callstack->callbacklen = 0;
    long res;
    fread(&res, sizeof(long), 1, f);
    if (res != 0xf34acdba){
        fprintf(stderr, "Error: Not BirdByteCode");
        return -1;
    };
    fread(&runtimenv->stringtable.stringtablelen, sizeof(int), 1, f);
    for (int i=0; i<runtimenv->stringtable.stringtablelen; i++){
        int sizeofstring = 0;
        fread(&sizeofstring, sizeof(int), 1, f);
        char *string = malloc(sizeofstring + 1);
        fread(string, 1, sizeofstring, f);
        string[sizeofstring] = '\0';
        runtimenv->stringtable.runtimestrings[i].string = string;
    };
    while((parse_bytecode_body(runtimenv, f)) != -1){
        ;
    };
    printf("Disassembler\n");
    for (int i=0; i<runtimenv->instructionsLen; i++){
        Instruction instr = runtimenv->instructions[i];
        printf("%*c%.2d: %*c", 3, ' ', i, 5, ' ');
        if (instr.type == TYPE_LOAD){
            printf("lea r%d, 0x%.2d ; load nth value of string table\n", instr.regn, instr.expr->value.intval);
        }else if(instr.type == TYPE_MOV_IMM){
            printf("mov_imm r%d, %d\n", instr.regn, instr.expr->value.intval);
        }else if(instr.type == TYPE_MOV){
            printf("mov r%d, r%d\n", instr.regn, instr.regn2);
        }else if(instr.type == TYPE_PLUS){
            printf("add r%d, r%d\n", instr.regn, instr.regn2);
        }else if(instr.type == TYPE_SUB){
            printf("sub r%d, r%d\n", instr.regn, instr.regn2);
        }else if(instr.type == TYPE_MUL){
            printf("mul r%d, r%d\n", instr.regn, instr.regn2);
        }else if(instr.type == TYPE_DIV){
            printf("idiv r%d, r%d\n", instr.regn, instr.regn2);
        }else if(instr.type == TYPE_MODULO){
            printf("imod r%d, r%d\n", instr.regn, instr.regn2);
        }else if(instr.type == TYPE_SYSCALL){
            printf("syscall\n");
        }else if(instr.type == TYPE_RET){
            printf("ret\n");
        }else if(instr.type == TYPE_CALL){
            printf("call %.2d\n", instr.regn);
        };
    };
    printf("\n");
    parse_bytecode_instr(runtimenv, 0);
    fclose(f);
    return 0;
}
