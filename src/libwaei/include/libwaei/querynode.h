#ifndef LW_QUERYNODE_INCLUDED
#define LW_QUERYNODE_INCLUDED


typedef enum _LwQueryNodeOperation {
  LW_QUERYNODE_OPERATION_NONE,
  LW_QUERYNODE_OPERATION_OR,
  LW_QUERYNODE_OPERATION_AND,
  TOTAL_LW_QUERYNODE_OPERATIONS
} LwQueryNodeOperation;


/**
 * LwQueryNode:
 * @operation: a #LwQueryNodeOperation representing how the sibling nodes are linked
 * @key: a #gchar instructing the node that the value should only be applied against a specific dictionary column
 * @data: a #gchar representing the search data.  If it is null, this is not a leaf node and there should be children.
 * @children: A #GList of #LwQueryNodes which should be filled when this is not a leaf node.
 * @refs: The number of references on this node.  When the references drop to 0, the node is freed.
 * @regex: A #GRegex that gets filled when lw_querynode_comile() is called
 */
struct _LwQueryNode {
  LwQueryNodeOperation operation;
  gchar * key;
  gchar * data;
  GList * children;
  gint refs;

  GRegex * regex;
};
typedef struct _LwQueryNode LwQueryNode;

typedef gboolean(*LwQueryNodeWalkFunc)(LwQueryNode * self, gpointer data);

#define LW_QUERYNODE_ERROR lw_querynode_error_quark ()
GQuark lw_querynode_error_quark (void);

#define LW_QUERYNODE(obj) ((LwQueryNode*)(obj))

typedef enum {
  LW_QUERYNODE_UNCLOSED_PARENTHESIS,
  LW_QUERYNODE_HANGING_START_LOGICAL_CONNECTOR,
  LW_QUERYNODE_HANGING_END_LOGICAL_CONNECTOR,
  LW_QUERYNODE_ERROR_MISSING_VALUE_FOR_KEYED_QUERYNODE,
  LW_QUERYNODE_ERROR_MISSING_KEY_AND_VALUE_FOR_KEYED_QUERYNODE,
} LwQueryNodeErrorCode;


LwQueryNode * lw_querynode_new_tree_from_string (gchar const * TEXT, LwQueryNodeOperation * operation_out, GError ** error);
LwQueryNode* lw_querynode_ref (LwQueryNode * self);
void lw_querynode_unref (LwQueryNode *self);
void lw_querynode_assert_equals (LwQueryNode * self, LwQueryNode *other);
void lw_querynode_walk (LwQueryNode * self, LwQueryNodeWalkFunc func, gpointer data);
gint lw_querynode_nnodes (LwQueryNode * self);
void lw_querynode_compile (LwQueryNode * self, LwUtf8Flag flags, GError ** error);


#endif
