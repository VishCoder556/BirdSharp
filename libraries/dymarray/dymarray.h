#ifndef STB_DYMARRAY_H
#define STB_DYMARRAY_H

#define dymarray_typenew(a, cap, increment) \
typedef struct { \
    a *data; \
    int datacap; \
    int datalen; \
}dymarray_##a; \
dymarray_##a dymarray_##a##_new(){ \
    dymarray_##a t = (dymarray_##a){0}; \
    t.datalen = 0; \
    t.datacap = cap; \
    t.data = malloc(sizeof(a) * t.datacap); \
    return t; \
} \
void dymarray_##a##_add(dymarray_##a *array, a thing){ \
    if (array->datalen >= array->datacap){ \
        array->datacap += increment; \
        void *_data = realloc(array->data, sizeof(a) * array->datacap); \
        if (!_data) { \
            fprintf(stderr, "Ran out of memory in dymarray\n"); \
            exit(-1); \
        } \
        array->data = _data; \
    } \
    array->data[array->datalen++] = thing; \
}


#ifdef STB_DYMARRAY_IMPLEMENTATION
#endif



#endif
