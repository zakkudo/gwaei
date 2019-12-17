#ifndef LW_PARENTHESIS_NODE_INCLUDED
#define LW_PARENTHESIS_NODE_INCLUDED

G_BEGIN_DECLS

/**
 * LwParenthesisNode:
 * @has_parenthesis: #gboolean
 * @refs: The refcount of the node
 * @OPEN: Pointer to the start of the substring represented by the node
 * @CLOSE: Pointer to the end of the substring represented by the node
 * @explicit_children: Node that are explicitly contained in parenthesis
 * @children: All nodes, including the ones adjacent to the ones contained in parenthesis
 */
struct _LwParenthesisNode {
  gboolean has_parenthesis;
  gint refs;
  gchar const * OPEN;
  gchar const * CLOSE;
  GList *explicit_children;
  GList *children;
};
typedef struct _LwParenthesisNode LwParenthesisNode;

#define LW_PARENTHESIS_NODE(obj) ((LwParenthesisNode*)(obj))

#define LW_PARENTHESIS_NODE_ERROR lw_parenthesis_node_error_quark ()
GQuark lw_parenthesis_node_error_quark (void);

/**
 * LwParenthesisNodeErrorCode:
 * @LW_PARENTHESIS_NODE_UNMATCHED_START_PARENTHESIS_ERROR: Happens parsing a string  similar to "(word"
 * @LW_PARENTHESIS_NODE_UNMATCHED_END_PARENTHESIS_ERROR: Happens parsing a string  similar to "word)"
 */
typedef enum {
  /*< public >*/
  LW_PARENTHESIS_NODE_UNMATCHED_START_PARENTHESIS_ERROR,
  LW_PARENTHESIS_NODE_UNMATCHED_END_PARENTHESIS_ERROR,
} LwParenthesisNodeErrorCode;


//Methods

LwParenthesisNode* lw_parenthesis_node_new_tree_from_string (gchar const * TEXT, GError ** error);
gboolean lw_parenthesis_node_contains (LwParenthesisNode * self, LwParenthesisNode * other);
void lw_parenthesis_node_unref (LwParenthesisNode *self);
LwParenthesisNode * lw_parenthesis_node_ref (LwParenthesisNode * self);
void lw_parenthesis_node_assert_equals (LwParenthesisNode * self, LwParenthesisNode * other);

G_END_DECLS

#endif
