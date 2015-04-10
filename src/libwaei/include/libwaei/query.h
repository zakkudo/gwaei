#ifndef LW_QUERY_INCLUDED
#define LW_QUERY_INCLUDED

typedef enum _LwQueryNodeOperation {
  LW_QUERY_OPERATION_OR,
  LW_QUERY_OPERATION_AND,
  TOTAL_LW_QUERY_OPERATIONS
} LwQueryNodeOperation;

struct _LwQueryNode {
  LwQueryNodeOperation operation;
  gchar *language;
  gchar *leafdata;
  GList *children;
};
typedef struct _LwQueryNode LwQueryNode;


struct _LwQuery {
  gchar *buffer;
  LwQueryNode *root;
  LwDictionary *dictionary;
};
typedef struct _LwQuery LwQuery;

#endif
