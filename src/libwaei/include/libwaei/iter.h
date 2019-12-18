#ifndef LW_ITER_INCLUDED
#define LW_ITER_INCLUDED

G_BEGIN_DECLS

typedef struct {
    gpointer iterable;
    gpointer user_data1;
    gpointer user_data2;
    gpointer user_data3;
} LwIter;

G_END_DECLS

#endif
