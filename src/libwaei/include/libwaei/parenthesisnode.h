#ifndef LW_PARENTHESISNODE_INCLUDED
#define LW_PARENTHESISNODE_INCLUDED

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
  /*< public >*/
  gboolean has_parenthesis;
  gint refs;
  gchar const * OPEN;
  gchar const * CLOSE;
  GList *explicit_children;
  GList *children;
};
typedef struct _LwParenthesisNode LwParenthesisNode;

#define LW_PARENTHESISNODE(obj) ((LwParenthesisNode*)(obj))
GType lw_parenthesisnode_get_type (void);

#define LW_PARENTHESISNODE_ERROR lw_parenthesisnode_error_quark ()
GQuark lw_parenthesisnode_error_quark (void);

/**
 * LwParenthesisNodeErrorCode:
 * @LW_PARENTHESISNODE_UNMATCHED_START_PARENTHESIS_ERROR: Happens parsing a string  similar to "(word"
 * @LW_PARENTHESISNODE_UNMATCHED_END_PARENTHESIS_ERROR: Happens parsing a string  similar to "word)"
 */
typedef enum {
  /*< public >*/
  LW_PARENTHESISNODE_UNMATCHED_START_PARENTHESIS_ERROR,
  LW_PARENTHESISNODE_UNMATCHED_END_PARENTHESIS_ERROR,
} LwParenthesisNodeErrorCode;


//Methods

LwParenthesisNode* lw_parenthesisnode_new_tree_from_string (gchar const * TEXT, GError ** error);
gboolean lw_parenthesisnode_contains (LwParenthesisNode * self, LwParenthesisNode * other);
void lw_parenthesisnode_unref (LwParenthesisNode *self);
LwParenthesisNode * lw_parenthesisnode_ref (LwParenthesisNode * self);
void lw_parenthesisnode_assert_equals (LwParenthesisNode * self, LwParenthesisNode * other);

#endif
