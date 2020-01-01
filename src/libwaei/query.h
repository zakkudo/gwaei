#ifndef LW_QUERY_NODE_INCLUDED
#define LW_QUERY_NODE_INCLUDED

#include "match-info.h"
#include "list.h"
#include "utf8.h"

G_BEGIN_DECLS

/**
 * LwQueryNodeOperation:
 * @LW_QUERY_NODE_OPERATION_NONE: There is no specific logical relation with the previous node.  This can also mean that this is the first node in a set of children.
 * @LW_QUERY_NODE_OPERATION_OR: The result of comparisons on this node should be logically ored with the previous node.
 * @LW_QUERY_NODE_OPERATION_AND: The result of comparisons on this node should be logically anded with the previous node.
 * @TOTAL_LW_QUERY_NODE_OPERATIONS: Total number of possible query node operations
 */
typedef enum _LwQueryNodeOperation {
  LW_QUERY_NODE_OPERATION_NONE,
  LW_QUERY_NODE_OPERATION_OR,
  LW_QUERY_NODE_OPERATION_AND,
  TOTAL_LW_QUERY_NODE_OPERATIONS
} LwQueryNodeOperation;


/**
 * LwQueryNode:
 * @operation: a #LwQueryNodeOperation representing how the sibling nodes are linked
 * @key: a #gchar instructing the node that the value should only be applied against a specific dictionary column
 * @data: a #gchar representing the search data.  If it is null, this is not a leaf node and there should be children.
 * @children: A #GList of #LwQueryNodes which should be filled when this is not a leaf node.
 * @refs: The number of references on this node.  When the references drop to 0, the node is freed.
 * @regex: A #GRegex that gets filled when lw_query_node_compile() is called
 */
typedef struct {
  LwQueryNodeOperation operation;
  gchar * key;
  gchar * data;
  GList * children;
  gint refs;

  gint * columns;
  GRegex * regex;
  GValue number;
} LwQueryNode;
typedef LwQueryNode LwQuery;

LwQuery * lw_query_new (gchar const * TEXT, GError ** error);
void lw_query_node_compile (LwQuery * self, LwUtf8Flag flags, GError ** error);

gboolean lw_query_match_iter (LwQueryNode * self, LwIter * iter, LwMatchInfo * match_info_out);
gboolean lw_query_match_value (LwQueryNode * self, GValue * value, LwMatchInfo * match_info_out);

typedef gboolean(*LwQueryNodeWalkFunc)(LwQueryNode * self, gpointer data);

#define LW_QUERY_NODE_ERROR lw_query_node_error_quark ()
GQuark lw_query_node_error_quark (void);

#define LW_QUERY_NODE(obj) ((LwQueryNode*)(obj))
#define LW_TYPE_QUERY_NODE (lw_query_node_get_type ())

#define LW_QUERY_NODE_IS_DANGLING_KEY(obj) (obj->data == NULL && obj->key != NULL && obj->children == NULL)
#define LW_QUERY_NODE_IS_EMPTY(obj) (obj->key == NULL, obj->data == NULL && obj->children == NULL)

/**
 * LwQueryNodeErrorCode:
 * @LW_QUERY_NODE_HANGING_START_LOGICAL_CONNECTOR: This will usually mean the first node in a set of children has an #LwQueryNodeOperation
 * @LW_QUERY_NODE_HANGING_END_LOGICAL_CONNECTOR: Compiling finished on a set of nodes with #LwQueryNodeOperation not set to %LW_QUERY_NODE_OPERATION_NONE
 * @LW_QUERY_NODE_ERROR_MISSING_VALUE_FOR_KEYED_QUERY_NODE: There is a node with only it's key set, but on compilation it wasn't able to be paired with any sibling nodes for it to have a value
 * @LW_QUERY_NODE_ERROR_MISSING_KEY_AND_VALUE_FOR_KEYED_QUERY_NODE: Equivialent to an empy search of ":"
 */
typedef enum {
  LW_QUERY_NODE_HANGING_START_LOGICAL_CONNECTOR,
  LW_QUERY_NODE_HANGING_END_LOGICAL_CONNECTOR,
  LW_QUERY_NODE_ERROR_MISSING_VALUE_FOR_KEYED_QUERY_NODE,
  LW_QUERY_NODE_ERROR_MISSING_KEY_AND_VALUE_FOR_KEYED_QUERY_NODE,
} LwQueryNodeErrorCode;


GType lw_query_node_get_type (void);

G_END_DECLS

#endif
