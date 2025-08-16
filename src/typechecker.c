typedef struct {
    char *name;
    AST_TypeInfo type;
}TC_Variable;

typedef struct {
    TC_Variable *variables;
    int variablelen;
}TC_Scope;

typedef struct {
    char *name;
    AST_TypeInfo ret;
    AST_TypeInfo *args;
    int arglen;
    TC_Scope scope;
}TC_Func;

typedef struct {
    char *name;
    AST *asts;
    int astlen;
    int cur;
    TC_Func *functions;
    int functionlen;
}Typechecker;

TC_Scope *current_scope;

int is_expression(AST ast){
    switch (ast.type){
        case AST_PLUS: return 1;
        case AST_SUB: return 1;
        case AST_MUL: return 1;
        case AST_DIV: return 1;
        case AST_MODULO: return 1;
        case AST_EQ: return 1;
        case AST_NEQ: return 1;
        case AST_LT: return 1;
        case AST_LTE: return 1;
        case AST_GT: return 1;
        case AST_GTE: return 1;
        default: return 0;
    };
    return 0;
}

int get_precedence(AST ast){
    switch (ast.type){
        case AST_EXPR: return 0;
        case AST_CAST: return 0;
        case AST_INT: return 0;
        case AST_VAR: return 0;
        case AST_STRING: return 0;
        case AST_DEREF: return 0;
        case AST_PLUS: return 1;
        case AST_SUB: return 1;
        case AST_MUL: return 2;
        case AST_DIV: return 2;
        case AST_MODULO: return 2;
        case AST_EQ: return 4;
        case AST_NEQ: return 4;
        case AST_LT: return 3;
        case AST_LTE: return 3;
        case AST_GT: return 3;
        case AST_GTE: return 3;
        default: return -1;
    };
}

TC_Scope initialize_scope(){
    TC_Scope scope;
    scope.variablelen = 0;
    scope.variables = malloc(sizeof(TC_Variable) * 100);
    return scope;
};

void add_variable_in_current_scope(TC_Variable variable){
   current_scope->variables[current_scope->variablelen++] = variable;
}


AST_TypeInfo fetch_type(Typechecker *typechecker, AST ast);
TC_Variable find_variable_in_current_scope(Typechecker *typechecker, AST ast){
    if (ast.type == AST_DEREF){
        TC_Variable variable;
        variable.name = "";
        variable.type = fetch_type(typechecker, ast);
        return variable;
    };
    if (ast.type == AST_INDEX){
        TC_Variable variable;
        variable.name = "";
        variable.type = fetch_type(typechecker, ast);
        return variable;
    };
    for (int i=0; i<current_scope->variablelen; i++){
        if (strcmp(current_scope->variables[i].name, ast.data.arg.value) == 0){
            return current_scope->variables[i];
        };
    };
    char string[100];
    snprintf(string, 100, "Variable \"%s\" doesn't exist", ast.data.arg.value);
    error_generate_parser("VarError", string, ast.row, ast.col, typechecker->name);
    return (TC_Variable){0};
}

Typechecker *typechecker_init(Parser *parser){
    Typechecker *typechecker = malloc(sizeof(Typechecker));
    typechecker->asts = parser->asts;
    typechecker->astlen = parser->astlen;
    typechecker->name = parser->name;
    typechecker->cur = 0;
    typechecker->functions = malloc(sizeof(TC_Func) * 100);
    typechecker->functions[0].name = "print";
    typechecker->functions[0].ret = (AST_TypeInfo){.ptrnum=0, .type="int"};
    typechecker->functions[0].args = malloc(sizeof(AST_TypeInfo) * 100);
    typechecker->functions[0].args[0] = (AST_TypeInfo){.ptrnum=1, .type="char"};
    typechecker->functions[0].arglen = 1;
    typechecker->functions[0].scope = initialize_scope();
    typechecker->functionlen = 1;
    return typechecker;
};

char *externs[100];
int externlen = 0;


void typechecker_eat(Typechecker *typechecker, AST *ast);
AST_TypeInfo fetch_type(Typechecker *typechecker, AST ast){
    if (ast.type == AST_RET){
        return fetch_type(typechecker, *(ast.data.ret.ret));
    }else if(ast.type == AST_VAR){
        return find_variable_in_current_scope(typechecker, ast).type;
    }else if(ast.type == AST_FUNCALL){
        TC_Func func = (TC_Func){};
        _Bool a = 0;
        for (int i=0; i<typechecker->functionlen; i++){
            if (strcmp(ast.data.funcall.name, typechecker->functions[i].name) == 0){
                func = typechecker->functions[i];
                a = 1;
            };
        };
        for (int i=0; i<externlen; i++){
            if (strcmp(externs[i], ast.data.funcall.name) == 0){
                return (AST_TypeInfo){"int", 0};
            };
        }

        if (a == 0){
            char string[100];
            snprintf(string, 100, "Function \"%s\" doesn't exist", ast.data.funcall.name);
            error_generate_parser("FuncError", string, ast.row, ast.col, typechecker->name);
        };
        return func.ret;
    }else if(ast.type == AST_PLUS || ast.type == AST_SUB || ast.type == AST_MUL || ast.type == AST_DIV || ast.type == AST_GT || ast.type == AST_GTE || ast.type == AST_LT || ast.type == AST_LTE || ast.type == AST_EQ || ast.type == AST_NEQ || ast.type == AST_MODULO){

        char *a = "";
        switch (ast.type){
            case AST_PLUS: a = "addition operation"; break;
            case AST_SUB: a = "subtraction operation"; break;
            case AST_MUL: a = "multiplication operation"; break;
            case AST_DIV: a = "division operation"; break;
            case AST_LT: a = "less than comparison"; break;
            case AST_GT: a = "greater than comparison"; break;
            case AST_LTE: a = "less than or equal to comparison"; break;
            case AST_GTE: a = "greater than or equal to comparison"; break;
            case AST_EQ: a = "equal to comparison"; break;
            case AST_NEQ: a = "is not equal to comparison"; break;
            case AST_MODULO: a = "modulus operation"; break;


            default: a = "unknown expression";
        }


        AST_TypeInfo left = fetch_type(typechecker, *(ast.data.expr.left));
        AST_TypeInfo right = fetch_type(typechecker, *(ast.data.expr.right));
        if (strcmp(typeinfo_to_string(left), typeinfo_to_string(right)) != 0){
            if (!((left.ptrnum > 0 && right.ptrnum == 0) || (right.ptrnum > 0 && left.ptrnum == 0) && (ast.type == AST_PLUS || ast.type == AST_SUB))){ // Allow ptr+num and ptr-num
                char string[100];
                snprintf(string, 100, "Inequal types of \"%s\" and \"%s\" inside of %s", typeinfo_to_string(left), typeinfo_to_string(right), a);
                error_generate_parser("VarError", string, ast.data.expr.left->row, ast.data.expr.left->col, typechecker->name);
            }
        };
        if (ast.type == AST_GT || ast.type == AST_GTE || ast.type == AST_LT || ast.type == AST_LTE || ast.type == AST_EQ || ast.type == AST_NEQ){
            ast.typeinfo = (AST_TypeInfo){"bool", 0};
            return ast.typeinfo;
        };
        ast.typeinfo = left;
        return left;
    }else if(ast.type == AST_EXPR){
        ast.typeinfo = fetch_type(typechecker, *ast.data.expr.left);
        return ast.typeinfo;
    }else if(ast.type == AST_DEREF){
        AST_TypeInfo type = fetch_type(typechecker, *ast.data.expr.left);
        if (type.ptrnum == 0){
            error_generate_parser("DerefError", "Cannot deref something that's not a pointer", ast.row, ast.col, typechecker->name);
        }else {
            type.ptrnum--;
        };
        return type;
    }else if(ast.type == AST_IF || ast.type == AST_WHILE){
        return (AST_TypeInfo){"bool", 0};
    }else if(ast.type == AST_NOT){
        return (AST_TypeInfo){"bool", 0};
    }else if(ast.type == AST_AND){
        return (AST_TypeInfo){"bool", 0};
    }else if(ast.type == AST_OR){
        return (AST_TypeInfo){"bool", 0};
    }else if(ast.type == AST_SYSCALL){
        return (AST_TypeInfo){"int", 0};
    }else if(ast.type == AST_INDEX){
        typechecker_eat(typechecker, ast.data.expr.left);
        typechecker_eat(typechecker, ast.data.expr.right);
        AST_TypeInfo typeinfo = fetch_type(typechecker, *ast.data.expr.left);
        if (typeinfo.ptrnum == 0){
            error_generate_parser("ArraySubscriptError", "Cannot get array susbscript of something that's not an array / pointer", ast.row, ast.col, typechecker->name);
        };
        typeinfo.ptrnum--;
        return typeinfo;
    };

    if (ast.typeinfo.type){
        if (strcmp(ast.typeinfo.type, "")){
            return ast.typeinfo;
        }
    };
    return (AST_TypeInfo){"", 0};
};



void typechecker_eat(Typechecker *typechecker, AST *ast){
    ast->typeinfo = fetch_type(typechecker, *ast);
    // if (is_expression(*ast)){
    //     fprintf(stderr, "{%d, %d}\n", get_precedence(*ast), get_precedence(*ast->data.expr.right));
    // }
    if (ast->type == AST_FUNCDEF){
        AST_TypeInfo expected = ast->typeinfo;
        typechecker->functions[typechecker->functionlen].name = ast->data.funcdef.name;
        typechecker->functions[typechecker->functionlen].args = malloc(sizeof(AST_TypeInfo) * 100);
        typechecker->functions[typechecker->functionlen].arglen = ast->data.funcdef.argslen;
        typechecker->functions[typechecker->functionlen].ret = ast->typeinfo;
        typechecker->functions[typechecker->functionlen].scope = initialize_scope();
        current_scope = &(typechecker->functions[typechecker->functionlen].scope);
        for (int i=0; i<ast->data.funcdef.argslen; i++){
            add_variable_in_current_scope((TC_Variable){ast->data.funcdef.args[i]->arg, ast->data.funcdef.args[i]->type});
            typechecker->functions[typechecker->functionlen].args[i] = ast->data.funcdef.args[i]->type;
        };
        typechecker->functionlen++;

        // Check if expected type is valid
        if (!(expected.type)){
            goto error;
        }else if(strcmp(expected.type, "") == 0){
            goto error;
        }


        for (int i=0; i<ast->data.funcdef.blocklen; i++){
            typechecker_eat(typechecker, ast->data.funcdef.block[i]);
        };


        AST_TypeInfo found = (AST_TypeInfo){"", 0};
        int a = 0;
        for (int i=0; i<ast->data.funcdef.blocklen; i++){
            if (ast->data.funcdef.block[i]->type == AST_RET){
                found = fetch_type(typechecker, *(ast->data.funcdef.block[i]));
                a = 1;
                if(!(found.type)){
                    goto error;
                }else if(strcmp(found.type, "") == 0){
                    goto error;
                } else if(strcmp(typeinfo_to_string(expected), typeinfo_to_string(found))){
                    char string[100];
                    snprintf(string, 100, "Function expects a return type of %s, but return is found returning %s", typeinfo_to_string(expected), typeinfo_to_string(found));
                    error_generate_parser("ReturnError", string, ast->row, ast->col, typechecker->name);
                };
            };
        };
        if (a == 0){
            char string[100];
            snprintf(string, 100, "No return found in function \"%s\" that doesn't return void", ast->data.funcdef.name);
            error_generate_parser("ReturnError", string, ast->row, ast->col, typechecker->name);
        };
    }else if(ast->type == AST_INDEX){
        if (strcmp(typeinfo_to_string(ast->data.expr.left->typeinfo), "char*") == 0){
            ast->type = AST_DEREF;
            AST *astleft = ast->data.expr.left;
            AST *astright = ast->data.expr.right;
            ast->data.expr.left = malloc(sizeof(AST));
            ast->data.expr.left->type = AST_PLUS;
            ast->data.expr.left->data.expr.left = astleft;
            ast->data.expr.left->data.expr.right = astright;
            typechecker_eat(typechecker, ast);
        }
        return;
    }else if(ast->type == AST_ASSIGN){
        AST_TypeInfo expected = ast->typeinfo;
        if (strcmp(typeinfo_to_string(expected), "unknown") == 0){
            char *name = ast->data.assign.from->data.arg.value;
            expected = find_variable_in_current_scope(typechecker, *ast->data.assign.from).type;
            ast->typeinfo = expected;
        }else {
            add_variable_in_current_scope((TC_Variable){ast->data.assign.from->data.arg.value, expected});
        }
        typechecker_eat(typechecker, ast->data.assign.from);
        _Bool a = 1;

        if (!(expected.type)){
            a = 0;
        }else if(strcmp(expected.type, "") == 0){
            a = 0;
        }

        AST_TypeInfo found = fetch_type(typechecker, *(ast->data.assign.assignto));
        typechecker_eat(typechecker, ast->data.assign.assignto);
        if (strcmp(typeinfo_to_string(expected), typeinfo_to_string(found)) != 0){
            char string[100];
            snprintf(string, 100, "Variable is supposed to be of type %s but is assigned to object of type %s", typeinfo_to_string(expected), typeinfo_to_string(found));
            error_generate_parser("VarError", string, ast->row, ast->col, typechecker->name);
        };
    }else if(ast->type == AST_FUNCALL){
        TC_Func func = (TC_Func){};
        int a = 0;
        for (int i=0; i<typechecker->functionlen; i++){
            if (strcmp(ast->data.funcall.name, typechecker->functions[i].name) == 0){
                func = typechecker->functions[i];
                a = 1;
            };
        };
        for (int i=0; i<externlen; i++){
            if (strcmp(externs[i], ast->data.funcall.name) == 0){
                ast->data.funcall.name -= 1;
                ast->data.funcall.name[0] = '_';
                a = 2;
            };
        }
        if (a == 0){
            char string[100];
            snprintf(string, 100, "Function %s doesn't exist", ast->data.funcall.name);
            error_generate_parser("FuncError", string, ast->row, ast->col, typechecker->name);
        };
        if (a != 2){
            if (ast->data.funcall.argslen != func.arglen){
                char string[100];
                snprintf(string, 100, "Too many or too little arguments sent to function %s", ast->data.funcall.name);
                error_generate_parser("FuncError", string, ast->row, ast->col, typechecker->name);
            };
            for (int i=0; i<ast->data.funcall.argslen; i++){
                AST *arg = ast->data.funcall.args[i];
                typechecker_eat(typechecker, arg);
                if (strcmp(typeinfo_to_string(fetch_type(typechecker, *arg)), typeinfo_to_string(func.args[i]))){
                    char string[100];
                    snprintf(string, 100, "In function \"%s\", argument type \"%s\" is expected, but argument of type \"%s\" is given", ast->data.funcall.name, typeinfo_to_string(func.args[i]), typeinfo_to_string(fetch_type(typechecker, *arg)));
                    error_generate_parser("ArgError", string, ast->row, ast->col, typechecker->name);
                };
            };
        }
    }else if(ast->type == AST_SYSCALL){
        for (int i=0; i<ast->data.funcall.argslen; i++){
            AST *arg = ast->data.funcall.args[i];
            typechecker_eat(typechecker, arg);
        };
    }else if(ast->type == AST_RET){
        typechecker_eat(typechecker, ast->data.ret.ret);
    }else if(ast->type == AST_PLUS || ast->type == AST_SUB || ast->type == AST_MUL || ast->type == AST_DIV || ast->type == AST_MODULO){
        typechecker_eat(typechecker, ast->data.expr.left);
        typechecker_eat(typechecker, ast->data.expr.right);
        AST_TypeInfo type1 = fetch_type(typechecker, *(ast->data.expr.left));
        AST_TypeInfo type2 = fetch_type(typechecker, *(ast->data.expr.right));

        // if (type1.ptrnum > 0 && type2.ptrnum == 0 && (ast->type == AST_PLUS || ast->type == AST_SUB)){
        //     // ast->data.expr.right->typeinfo.type = "long";
        //     // type2.type = "long";
        // }else if (type2.ptrnum > 0 && type1.ptrnum == 0 && (ast->type == AST_PLUS || ast->type == AST_SUB)){
        //     // ast->data.expr.left->typeinfo.type = "long";
        //     // type1.type = "long";
        // };


        ast->typeinfo = fetch_type(typechecker, *ast);
        if (strcmp(typeinfo_to_string(type1), typeinfo_to_string(type2)) == 0){
            ;
        }
    }else if (ast->type == AST_GT || ast->type == AST_GTE || ast->type == AST_LT || ast->type == AST_LTE || ast->type == AST_EQ || ast->type == AST_NEQ){
        typechecker_eat(typechecker, ast->data.expr.left);
        typechecker_eat(typechecker, ast->data.expr.right);
        ast->typeinfo = (AST_TypeInfo){"bool", 0};
        return;
    }else if(ast->type == AST_CAST){
        typechecker_eat(typechecker, ast->data.expr.left);
    }else if(ast->type == AST_DEREF){
        typechecker_eat(typechecker, ast->data.expr.left);
    }else if(ast->type == AST_IF){
        AST_TypeInfo type = fetch_type(typechecker, *ast->data.if1.block.condition);
        typechecker_eat(typechecker, ast->data.if1.block.condition);
        if (strcmp(type.type, "bool") || type.ptrnum != 0){
            error_generate_parser("IfError", "Condition is expected to be boolean in if condition", ast->data.if1.block.condition->row, ast->data.if1.block.condition->col, typechecker->name);
        };
        for (int i=0; i<ast->data.if1.block.statementlen; i++){
            typechecker_eat(typechecker, ast->data.if1.block.statements[i]);
        };
    }else if(ast->type == AST_WHILE){
        AST_TypeInfo type = fetch_type(typechecker, *ast->data.while1.condition);
        typechecker_eat(typechecker, ast->data.while1.condition);
        if (strcmp(type.type, "bool") || type.ptrnum != 0){
            error_generate_parser("IfError", "Condition is expected to be boolean in if condition", ast->data.if1.block.condition->row, ast->data.if1.block.condition->col, typechecker->name);
        };
        for (int i=0; i<ast->data.while1.blocklen; i++){
            typechecker_eat(typechecker, ast->data.while1.block[i]);
        };
    }else if(ast->type == AST_EXPR){
        typechecker_eat(typechecker, ast->data.expr.left);
        ast->typeinfo = ast->data.expr.left->typeinfo;
    }else if(ast->type == AST_NOT){
        ast->typeinfo = (AST_TypeInfo){"bool", 0};
    }else if(ast->type == AST_VAR){
        return;
    }else if(ast->type == AST_AND){
        ast->typeinfo = (AST_TypeInfo){"bool", 0};
        typechecker_eat(typechecker, ast->data.expr.left);
        typechecker_eat(typechecker, ast->data.expr.right);
        char *type1 = typeinfo_to_string(ast->data.expr.left->typeinfo);
        char *type2 = typeinfo_to_string(ast->data.expr.right->typeinfo);
        if (strcmp(type1, "bool") || strcmp(type2, "bool")) {
            char a[1000];
            snprintf(a, 1000, "Expected boolean types, found types \"%s\" and \"%s\"", type1, type2);
            error_generate_parser("AndError", a, ast->row, ast->col, typechecker->name);
        }
    }else if(ast->type == AST_OR){
        ast->typeinfo = (AST_TypeInfo){"bool", 0};
        typechecker_eat(typechecker, ast->data.expr.left);
        typechecker_eat(typechecker, ast->data.expr.right);
        char *type1 = typeinfo_to_string(ast->data.expr.left->typeinfo);
        char *type2 = typeinfo_to_string(ast->data.expr.right->typeinfo);
        if (strcmp(type1, "bool") || strcmp(type2, "bool")) {
            char a[1000];
            snprintf(a, 1000, "Expected boolean types, found types \"%s\" and \"%s\"", type1, type2);
            error_generate_parser("AndError", a, ast->row, ast->col, typechecker->name);
        }
    }else if(ast->type == AST_MODE){
        if (strcmp(ast->data.mode.name, "extern") == 0){
            externs[externlen++] = ast->data.mode.res;
        };
    }
    return;
error:
    error_generate_parser("TypeError", "Type doesn't exist", ast->row, ast->col, typechecker->name);
    return;
};





int typechecker_eat_ast(Typechecker *typechecker){
    if (typechecker->astlen == typechecker->cur){
        return -1;
    };
    AST ast = typechecker->asts[typechecker->cur];
    typechecker_eat(typechecker, &ast);
    typechecker->cur++;
    return 0;
};
