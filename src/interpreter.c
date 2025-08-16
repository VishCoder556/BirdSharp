typedef struct {
    AST_TypeInfo type;
    char *name;
}I_Arg;

#define I_ARGS_MAX 30

typedef struct {
    I_Arg *args;
    int arglen;
}I_Args;

typedef struct {
    char *name;
    I_Args args;
} I_Func;

#define I_FUNCS_MAX 30

typedef struct {
    I_Func *functions;
    int functionlen;
}I_Funcs;

typedef struct {
    I_Funcs functions;
}Interpreter;



I_Funcs I_Funcs_init(){
    I_Funcs funcs;
    funcs.functionlen = 0;
    funcs.functions = malloc(sizeof(I_Func) * I_FUNCS_MAX);
    return funcs;
};

I_Args I_Args_init(){
    I_Args args;
    args.arglen = 0;
    args.args = malloc(sizeof(I_Arg) * I_ARGS_MAX);
    return args;
}

void I_Func_init(I_Func *func, char *name){
    func->name = name;
    func->args = I_Args_init();
};

void I_Arg_init(I_Arg *func, AST_TypeInfo type, char *name){
    func->name = name;
    func->type = type;
};

Interpreter *interpreter;
Generator *generator;

void interpreter_init(void *_generator){
    generator = (Generator*)_generator;
    interpreter = malloc(sizeof(Interpreter));
    interpreter->functions = I_Funcs_init();
};

void interpreter_close(void *_generator){
};

char *interpreter_parse_expr_string(Interpreter *interpreter, AST ast){
    if (ast.type == AST_STRING){
        return ast.data.arg.value;
    };
    return "";
};

int interpreter_parse_expr_int(Interpreter *interpreter, AST ast){
    if (ast.type == AST_INT){
        int res = atoi(ast.data.arg.value);
        if (res == 0){
            if (strcmp(ast.data.arg.value, "0")){
                error_generate_parser("NumError", "Invalid number", ast.row, ast.col, generator->name);
            };
        };
        return res;
    };
    return 0;
};

AST function;


void interpreter_generate_ast(void *_generator, AST ast){
    Generator *generator = (Generator*)_generator;
    if (ast.type == AST_FUNCDEF){
        function = ast;
        if (strcmp(ast.data.funcdef.name, "main") == 0){
            for (int i=0; i<ast.data.funcdef.blocklen; i++){
                interpreter_generate_ast(interpreter, *(ast.data.funcdef.block[i]));
            };
        };
        I_Func_init(interpreter->functions.functions + interpreter->functions.functionlen, ast.data.funcdef.name);
    }else if(ast.type == AST_FUNCALL){
        if (strcmp(ast.data.funcall.name, "print") == 0){
            if (ast.data.funcall.argslen == 0){
                error_generate_parser("PrintError", "Print function needs one or more argument(s).", ast.row, ast.col, generator->name);
            };
            for (int i=0; i<ast.data.funcall.argslen; i++){
                printf("%s", interpreter_parse_expr_string(interpreter, *(ast.data.funcall.args[i])));
            };
        }else if(strcmp(ast.data.funcall.name, "exit") == 0){
            ;
        };
    }else if(ast.type == AST_RET){
        if (function.typeinfo.type){
            if (strcmp(function.data.funcall.name, "main") == 0){
                exit(interpreter_parse_expr_int(interpreter, *(ast.data.ret.ret)));
            }
        }
    }
}
