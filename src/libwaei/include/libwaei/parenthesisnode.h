#ifndef LW_PARENTHESISNODE_INCLUDED
#define LW_PARENTHESISNODE_INCLUDED

struct _LwParenthesisNode {
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

typedef enum {
  LW_PARENTHESISNODE_UNMATCHED_PARENTHESIS_ERROR,
} LwParenthesisNodeErrorCode;


//Methods

LwParenthesisNode* lw_parenthesisnode_new_tree_from_string (gchar const * TEXT, GError ** error);
gboolean lw_parenthesisnode_contains (LwParenthesisNode * self, LwParenthesisNode * other);
void lw_parenthesisnode_unref (LwParenthesisNode *self);
LwParenthesisNode * lw_parenthesisnode_ref (LwParenthesisNode * self);

#endif
