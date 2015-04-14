#ifndef LW_QUERY_INCLUDED
#define LW_QUERY_INCLUDED

struct _LwQuery {
  gchar *buffer;
  LwQueryNode *root;
  LwDictionary *dictionary;
};
typedef struct _LwQuery LwQuery;

#endif
