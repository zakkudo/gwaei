#ifndef LW_QUERYNODE_INCLUDED
#define LW_QUERYNODE_INCLUDED

#include "querynodematchinfo.h"
#include "parsedline.h"
#include "utf8.h"

G_BEGIN_DECLS


/**
 * LwQueryNodeOperation:
 * @LW_QUERYNODE_OPERATION_NONE: There is no specific logical relation with the previous node.  This can also mean that this is the first node in a set of children.
 * @LW_QUERYNODE_OPERATION_OR: The result of comparisons on this node should be logically ored with the previous node.
 * @LW_QUERYNODE_OPERATION_AND: The result of comparisons on this node should be logically anded with the previous node.
 * @TOTAL_LW_QUERYNODE_OPERATIONS: Total number of possible query node operations
 */
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
 * @regex: A #GRegex that gets filled when lw_querynode_compile() is called
 */
struct _LwQueryNode {
  LwQueryNodeOperation operation;
  gchar * key;
  gchar * data;
  GList * children;
  gint refs;

  gint * columns;
  GRegex * regex;
};
typedef struct _LwQueryNode LwQueryNode;

typedef gboolean(*LwQueryNodeWalkFunc)(LwQueryNode * self, gpointer data);

#define LW_QUERYNODE_ERROR lw_querynode_error_quark ()
GQuark lw_querynode_error_quark (void);

#define LW_QUERYNODE(obj) ((LwQueryNode*)(obj))
#define LW_TYPE_QUERYNODE (lw_querynode_get_type ())

#define LW_QUERYNODE_IS_DANGLING_KEY(obj) (obj->data == NULL && obj->key != NULL && obj->children == NULL)
#define LW_QUERYNODE_IS_EMPTY(obj) (obj->key == NULL, obj->data == NULL && obj->children == NULL)

/**
 * LwQueryNodeErrorCode:
 * @LW_QUERYNODE_HANGING_START_LOGICAL_CONNECTOR: This will usually mean the first node in a set of children has an #LwQueryNodeOperation
 * @LW_QUERYNODE_HANGING_END_LOGICAL_CONNECTOR: Compiling finished on a set of nodes with #LwQueryNodeOperation not set to %LW_QUERYNODE_OPERATION_NONE
 * @LW_QUERYNODE_ERROR_MISSING_VALUE_FOR_KEYED_QUERYNODE: There is a node with only it's key set, but on compilation it wasn't able to be paired with any sibling nodes for it to have a value
 * @LW_QUERYNODE_ERROR_MISSING_KEY_AND_VALUE_FOR_KEYED_QUERYNODE: Equivialent to an empy search of ":"
 */
typedef enum {
  LW_QUERYNODE_HANGING_START_LOGICAL_CONNECTOR,
  LW_QUERYNODE_HANGING_END_LOGICAL_CONNECTOR,
  LW_QUERYNODE_ERROR_MISSING_VALUE_FOR_KEYED_QUERYNODE,
  LW_QUERYNODE_ERROR_MISSING_KEY_AND_VALUE_FOR_KEYED_QUERYNODE,
} LwQueryNodeErrorCode;


LwQueryNode * lw_querynode_new_tree_from_string (gchar const * TEXT, LwQueryNodeOperation * operation_out, GError ** error);
LwQueryNode* lw_querynode_ref (LwQueryNode * self);
void lw_querynode_unref (LwQueryNode *self);
void lw_querynode_assert_equals (LwQueryNode * self, LwQueryNode *other);
gboolean lw_querynode_walk (LwQueryNode * self, LwQueryNodeWalkFunc func, gpointer data);
gint lw_querynode_nnodes (LwQueryNode * self);
void lw_querynode_compile (LwQueryNode * self, LwUtf8Flag flags, GError ** error);

GType lw_querynode_get_type (void);

gboolean lw_querynode_match_parsedline (LwQueryNode * self, LwParsedLine * parsed_line, LwQueryNodeMatchInfo * match_info_out);

G_END_DECLS

#endif
