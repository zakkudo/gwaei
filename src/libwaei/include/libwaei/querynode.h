#ifndef LW_QUERYNODE_INCLUDED
#define LW_QUERYNODE_INCLUDED


typedef enum _LwQueryNodeOperation {
  LW_QUERYNODE_OPERATION_NONE,
  LW_QUERYNODE_OPERATION_OR,
  LW_QUERYNODE_OPERATION_AND,
  TOTAL_LW_QUERYNODE_OPERATIONS
} LwQueryNodeOperation;

struct _LwQueryNode {
  LwQueryNodeOperation operation;
  gchar const * language;
  gchar * data;
  GList * children;
  gint refs;
};
typedef struct _LwQueryNode LwQueryNode;

#define LW_QUERYNODE_ERROR lw_querynode_error_quark ()
GQuark lw_querynode_error_quark (void);

typedef enum {
  LW_QUERYNODE_UNCLOSED_PARENTHESIS,
  LW_QUERYNODE_HANGING_START_LOGICAL_CONNECTOR,
  LW_QUERYNODE_HANGING_END_LOGICAL_CONNECTOR,
} LwQueryNodeErrorCode;


LwQueryNode * lw_querynode_new_tree_from_string (gchar const * TEXT, LwQueryNodeOperation * operation_out, GError ** error);
LwQueryNode* lw_querynode_ref (LwQueryNode * self);
void lw_querynode_unref (LwQueryNode *self);
void lw_querynode_assert_equals (LwQueryNode *self, LwQueryNode *other);


#endif
