/******************************************************************************
    AUTHOR:
    File written and Copyrighted by Zachary Dovel. All Rights Reserved.

    LICENSE:
    This file is part of gWaei.

    gWaei is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    gWaei is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with gWaei.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

//!
//! @file parenthesisnode.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <ctype.h>

#include <glib.h>

#include <libwaei/libwaei.h>
#include <libwaei/gettext.h>


static void lw_parenthesisnode_parse_string (LwParenthesisNode * self, gchar const * TEXT);


LwParenthesisNode*
lw_parenthesisnode_new (gchar const * TEXT)
{
  //Sanity checks
  g_return_val_if_fail (TEXT != NULL, NULL);

  //Declarations
  LwParenthesisNode * self = NULL;

  //Inititalizations
  self = g_new0 (LwParenthesisNode, 1);
  if (self == NULL) goto errored;
  self->refs = 1;
  self->has_parenthesis = FALSE;
  self->OPEN = TEXT;
  self->CLOSE = (*TEXT == '\0') ? TEXT : TEXT + strlen(TEXT) - 1;
  lw_parenthesisnode_parse_string (self, TEXT);

errored:

  return self;
}


static LwParenthesisNode*
lw_parenthesischildnode_new (gchar const * OPEN,
                             gchar const * CLOSE,
                             gboolean      has_parenthesis)
{
  //Sanity checks
  g_return_val_if_fail (OPEN != NULL && CLOSE != NULL, NULL);

  //Declarations
  LwParenthesisNode * self = NULL;

  //Inititalizations
  self = g_new0 (LwParenthesisNode, 1);
  if (self == NULL) goto errored;
  self->refs = 1;
  self->OPEN = OPEN;
  self->CLOSE = CLOSE;
  self->has_parenthesis = has_parenthesis;

errored:

  return self;
}



GType
lw_parenthesisnode_get_type ()
{
    static GType type = 0;

    if (type == 0)
    {
      type = g_boxed_type_register_static (
        "LwParenthesisNode",
        (GBoxedCopyFunc) lw_parenthesisnode_ref,
        (GBoxedFreeFunc) lw_parenthesisnode_unref
      );
    }

    return type;
}


static void
lw_parenthesisnode_sync_children (LwParenthesisNode * self)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    //Declarations
    GList *children = NULL;
    GList *link = NULL;
    LwParenthesisNode * node = NULL;
    gchar const * C = NULL;
    gchar const * OPEN = NULL;
    gchar const * CLOSE = NULL;

    //Initializations
    OPEN = C = self->OPEN;

    //Iterate adding parenthesis and non-parenthesis groups
    for (link = self->explicit_children; link != NULL; link = link->next)
    {
      node = LW_PARENTHESISNODE (link->data);
      CLOSE = node->OPEN;
      if (*CLOSE == '(' || *CLOSE == ')') CLOSE--;  // Remove overlap
      if (*OPEN == '(' || *OPEN == ')') OPEN++;
      if (OPEN <= CLOSE)
      {
        LwParenthesisNode *new_node = lw_parenthesischildnode_new (OPEN, CLOSE, FALSE);
        if (new_node == NULL) goto errored;
        children = g_list_prepend (children, new_node);
      }
      OPEN = node->CLOSE;
      children = g_list_prepend (children, lw_parenthesisnode_ref (node));
    }

    //Get final non-parenthesis group
    CLOSE = self->CLOSE;
    if (*CLOSE == '(' || *CLOSE == ')') CLOSE--;  // Remove overlap
    if (*OPEN == '(' || *OPEN == ')') OPEN++;
    if (OPEN < CLOSE)
    {
        LwParenthesisNode *new_node = lw_parenthesischildnode_new (OPEN, CLOSE, FALSE);
        if (new_node == NULL) goto errored;
        children = g_list_prepend (children, new_node);
    }

    g_list_free_full (self->children, (GDestroyNotify) lw_parenthesisnode_unref);
    self->children = g_list_reverse (children);
    children = NULL;

errored:

    g_list_free_full (children, (GDestroyNotify) lw_parenthesisnode_unref);
    children = NULL;
}


static void
lw_parenthesisnode_set_explicit_children (LwParenthesisNode * self,
                                          GList             * explicit_children)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    //Declarations
    GList *children = NULL;

    //Initializations
    children = self->explicit_children;
    self->explicit_children = explicit_children;
    lw_parenthesisnode_sync_children (self);

    g_list_free_full (children, (GDestroyNotify) lw_parenthesisnode_unref);
    children = NULL;
}


gboolean
lw_parenthesisnode_contains (LwParenthesisNode * self,
                             LwParenthesisNode * other)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (other != NULL, FALSE);

    return (self->OPEN <= other->OPEN && self->CLOSE >= other->CLOSE);
}


static GList*
lw_parenthesisnode_take_children (LwParenthesisNode * self, GList * nodes)
{
    GList * children = NULL;
    GList * child = NULL;

    while (nodes != NULL && lw_parenthesisnode_contains (self, nodes->data)) {
      child = nodes;
      nodes = g_list_remove_link (nodes, nodes);
      children = g_list_concat (child, children);
    }

    lw_parenthesisnode_set_explicit_children (self, children);
    children = NULL;

    return nodes;
}


static void
lw_parenthesisnode_parse_string (LwParenthesisNode * self,
                                 gchar const       * TEXT)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (TEXT != NULL);

    //Declarations
    gchar const * C = NULL;
    gchar const * * OPEN = NULL;
    gsize num_open = 0;
    GList * children = NULL;
    GList * children_out = NULL;
    LwParenthesisNode * node = NULL;
    gboolean isescaped = FALSE;

    //Initializations
    C = TEXT;
    OPEN = g_new0 (gchar const *, strlen(TEXT) + 1);
    if (OPEN == NULL) goto errored;

    while (*C != '\0')
    {
      isescaped = lw_utf8_isescaped (TEXT, C);
      if (num_open == 0 || *OPEN[num_open - 1] != '[')
      {
        if ((*C == '(' || *C == '[') && !isescaped)
        {
          OPEN[num_open] = C;
          num_open++;
        }
        else if (*C == ')' && !isescaped)
        {
          if (num_open < 1) goto errored;
          num_open--;
          node = lw_parenthesischildnode_new (OPEN[num_open], C, TRUE);

          if (children != NULL && lw_parenthesisnode_contains (node, children->data))
          {
            children = lw_parenthesisnode_take_children (node, children);
          }
          //Is another child/sibling node to the pairs
          children = g_list_prepend (children, node);
          node = NULL;
        }
      }
      else if (*C == ']' && *OPEN[num_open - 1] == '[' && !isescaped)
      {
        if (num_open < 1) goto errored;
        num_open--;
      }
      C = g_utf8_next_char (C);
    }

    children_out = g_list_reverse (children);
    children = NULL;

errored:

    g_list_free_full(children, (GDestroyNotify) lw_parenthesisnode_unref);
    children = NULL;

    if (node) lw_parenthesisnode_unref (node);
    node = NULL;

    g_free (OPEN);
    OPEN = NULL;

    lw_parenthesisnode_set_explicit_children (self, children_out);
    children_out = NULL;
}


static void
lw_parenthesisnode_free (LwParenthesisNode * self)
{
    if (self == NULL) return;

    g_list_free_full (self->explicit_children, (GDestroyNotify) lw_parenthesisnode_unref);
    g_list_free_full (self->children, (GDestroyNotify) lw_parenthesisnode_unref);

    memset(self, 0, sizeof(LwParenthesisNode));
    g_free (self);
}


void
lw_parenthesisnode_unref (LwParenthesisNode * self)
{
    g_return_if_fail (self != NULL);

    if (g_atomic_int_dec_and_test (&self->refs))
    {
      lw_parenthesisnode_free (self);
    }
}


LwParenthesisNode *
lw_parenthesisnode_ref (LwParenthesisNode * self)
{
    g_return_val_if_fail (self != NULL, NULL);

    g_atomic_int_inc (&self->refs);

    return self;
}
