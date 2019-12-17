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

/**
 * SECTION:parenthesis_node
 * @short_description: A node structure describing text with parenthesis
 * @title: LwParenthesisNode
 *
 * Parses a string into a set of nodes that can be used for math-like
 * logical comparisons.  It starts by creating a list of subnodes 
 * pointing ot subsections of the string that are surrouned by
 * parenthesis.  I then calculates a full list of nodes by checking
 * the nodes that are between the explicit nodes.  LwParenthesisNode
 * is used as the bases for building a #LwQueryNodes.  The difference
 * with #LwQueryNodes is that they are specialized for searches and 
 * may not match one-to-one to the original string.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <ctype.h>

#include <glib.h>

#include <libwaei/gettext.h>
#include <libwaei/parenthesis-node.h>
#include <libwaei/query-node.h>
#include <libwaei/utf8.h>


static GList * _parse_string (gchar const * TEXT, GError ** error);
static GList * _calculate_children (gchar const * OPEN, gchar const * CLOSE, GList * explicit_children);

GQuark
lw_parenthesis_node_error_quark ()
{
    return g_quark_from_static_string ("lw-parenthesis-node-error");
}


static LwParenthesisNode*
lw_parenthesis_node_new (gchar const * OPEN,
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


/**
 * lw_parenthesis_node_new_tree_from_string:
 * @TEXT: Some text to use to generate the parenthesis node tree.  This text is referenced directly and not copied. It must not be freed or modified.
 * @error: Syntax errors that causes creation of the tree to fail such as unclosed parenthesis
 *
 * This method is used to generate a tree representing the text
 * passed to it.  This tree is used for generating the LwQueryNode
 * tree which is used for searches.  The difference with LwQueryNode
 * is that it has some extra information relating to the search and
 * it doesn't necessarily map one-to-one to the source text as it is
 * optimized for fast comparisons.
 *
 * Returns: A new LwParenthesisNode tree that can be freed with lw_parenthesis_node_unref()
 */
LwParenthesisNode*
lw_parenthesis_node_new_tree_from_string (gchar const *  TEXT,
                                         GError      ** error)
{
  //Sanity checks
  g_return_val_if_fail (TEXT != NULL, NULL);
  if (error != NULL && *error != NULL) return NULL;

  //Declarations
  LwParenthesisNode * self = NULL;
  gchar const * OPEN = NULL;
  gchar const * CLOSE = NULL;
  GList * children = NULL;
  GList * explicit_children = NULL;

  //Inititalizations
  OPEN = TEXT;
  CLOSE = (*TEXT == '\0') ? TEXT : TEXT + strlen(TEXT) - 1;
  explicit_children = _parse_string (TEXT, error);
  if (error != NULL && *error != NULL) goto errored;
  children = _calculate_children (OPEN, CLOSE, explicit_children);

  //One one child returned, so return it directly 
  if (children != NULL && children->next == NULL)
  {
    self = LW_PARENTHESIS_NODE (children->data);
    g_list_free (children);
    children = NULL;
    g_list_free_full (explicit_children, (GDestroyNotify) lw_parenthesis_node_unref);
    explicit_children = NULL;
  }
  //Multiple children were found, so wrap in a parent node
  else if (children != NULL && children->next != NULL)
  {
    self = lw_parenthesis_node_new (OPEN, CLOSE, FALSE);
    if (self == NULL) goto errored;
    self->children = children;
    self->explicit_children = explicit_children;
    children = NULL;
    explicit_children = NULL;
  }

errored:

  g_list_free_full (children, (GDestroyNotify) lw_query_node_unref);
  children = NULL;

  g_list_free_full (explicit_children, (GDestroyNotify) lw_query_node_unref);
  explicit_children = NULL;

  return self;
}


static GList *
_calculate_children (gchar const * OPEN,
                     gchar const * CLOSE,
                     GList * explicit_children)
{
    //Sanity checks
    g_return_val_if_fail (OPEN != NULL, NULL);
    g_return_val_if_fail (CLOSE != NULL, NULL);

    //Declarations
    GList * children = NULL;
    GList * children_out = NULL;
    GList * link = NULL;
    LwParenthesisNode * node = NULL;
    gchar const * O = NULL;
    gchar const * C = NULL;

    //Initializations
    O = OPEN;
    C = CLOSE;

    //Iterate adding parenthesis and non-parenthesis groups
    for (link = explicit_children; link != NULL; link = link->next)
    {
      node = LW_PARENTHESIS_NODE (link->data);
      C = node->OPEN;
      if (*C == '(' || *C == ')') C--;  // Remove overlap
      if (*O == '(' || *O == ')') O++;
      if (O <= C)
      {
        LwParenthesisNode *new_node = lw_parenthesis_node_new (O, C, FALSE);
        if (new_node == NULL) goto errored;
        children = g_list_prepend (children, new_node);
      }
      children = g_list_prepend (children, lw_parenthesis_node_ref (node));
      O = node->CLOSE;
    }

    //Get final non-parenthesis group
    C = CLOSE;
    if (*C == '(' || *C == ')') C--;  // Remove overlap
    if (*O == '(' || *O == ')') O++;
    if (O <= C)
    {
        LwParenthesisNode *new_node = lw_parenthesis_node_new (O, C, FALSE);
        if (new_node == NULL) goto errored;
        children = g_list_prepend (children, new_node);
    }

    children_out = g_list_reverse (children);
    children = NULL;

errored:

    g_list_free_full (children, (GDestroyNotify) lw_parenthesis_node_unref);
    children = NULL;

    return children_out;
}


gboolean
lw_parenthesis_node_contains (LwParenthesisNode * self,
                             LwParenthesisNode * other)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (other != NULL, FALSE);

    return (self->OPEN <= other->OPEN && self->CLOSE >= other->CLOSE);
}


static GList*
_take_explicit_children (LwParenthesisNode * self,
                         GList             * explicit_children_out)
{
    GList * explicit_children = NULL;
    GList * children = NULL;

    while (explicit_children_out != NULL && lw_parenthesis_node_contains (self, explicit_children_out->data)) {
      GList * link = explicit_children_out;
      explicit_children_out = g_list_remove_link (explicit_children_out, link);
      explicit_children = g_list_concat (link, explicit_children);
    }

    //We take advantage that the input to this method is alread reversed
    // explicit_children = g_list_reverse (explicit_children);
    children = _calculate_children (self->OPEN, self->CLOSE, explicit_children);

    g_list_free_full (self->explicit_children, (GDestroyNotify) lw_parenthesis_node_unref);
    self->explicit_children = explicit_children;
    explicit_children = NULL;

    g_list_free_full (self->children, (GDestroyNotify) lw_parenthesis_node_unref);
    self->children = children;
    children = NULL;

errored:

    g_list_free_full (explicit_children, (GDestroyNotify) lw_parenthesis_node_unref);
    explicit_children = NULL;
    g_list_free_full (children, (GDestroyNotify) lw_parenthesis_node_unref);
    children = NULL;

    return explicit_children_out;
}


static GList *
_parse_string (gchar const *  TEXT,
               GError      ** error)
{
    //Sanity checks
    g_return_val_if_fail (TEXT != NULL, NULL);
    if (error != NULL && *error != NULL) return NULL;

    //Declarations
    gchar const * C = NULL;
    gchar const * * OPEN = NULL;
    gsize num_open = 0;
    GList * explicit_children = NULL;
    GList * explicit_children_out = NULL;
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
          if (num_open < 1)
          {
            g_set_error (
              error,
              LW_PARENTHESIS_NODE_ERROR,
              LW_PARENTHESIS_NODE_UNMATCHED_END_PARENTHESIS_ERROR,
              "There are more closing parenthesis than open parenthesis."
            );
            goto errored;
          }
          num_open--;
          node = lw_parenthesis_node_new (OPEN[num_open], C, TRUE);

          if (explicit_children != NULL && lw_parenthesis_node_contains (node, explicit_children->data))
          {
            explicit_children = _take_explicit_children (node, explicit_children);
          }
          //Is another child/sibling node to the pairs
          explicit_children = g_list_prepend (explicit_children, node);
          node = NULL;
        }
      }
      else if (*C == ']' && *OPEN[num_open - 1] == '[' && !isescaped)
      {
        if (num_open < 1)
        {
          g_set_error (
            error,
            LW_PARENTHESIS_NODE_ERROR,
            LW_PARENTHESIS_NODE_UNMATCHED_END_PARENTHESIS_ERROR,
            "There are mroe closing parenthesis than open parenthesis"
          );
          goto errored;
        }
        num_open--;
      }
      C = g_utf8_next_char (C);
    }

    if (num_open > 0)
    {
      g_set_error (
        error,
        LW_PARENTHESIS_NODE_ERROR,
        LW_PARENTHESIS_NODE_UNMATCHED_START_PARENTHESIS_ERROR,
        "There are more opening parenthesis than closing parenthesis."
      );
      goto errored;
    }

    explicit_children_out = g_list_reverse (explicit_children);
    explicit_children = NULL;

errored:

    g_list_free_full (explicit_children, (GDestroyNotify) lw_parenthesis_node_unref);
    explicit_children = NULL;

    if (node) lw_parenthesis_node_unref (node);
    node = NULL;

    g_free (OPEN);
    OPEN = NULL;

    return explicit_children_out;
}


static void
lw_parenthesis_node_free (LwParenthesisNode * self)
{
    if (self == NULL) return;

    g_list_free_full (self->explicit_children, (GDestroyNotify) lw_parenthesis_node_unref);
    g_list_free_full (self->children, (GDestroyNotify) lw_parenthesis_node_unref);

    memset(self, 0, sizeof(LwParenthesisNode));
    g_free (self);
}


void
lw_parenthesis_node_unref (LwParenthesisNode * self)
{
    g_return_if_fail (self != NULL);

    if (g_atomic_int_dec_and_test (&self->refs))
    {
      lw_parenthesis_node_free (self);
    }
}


LwParenthesisNode *
lw_parenthesis_node_ref (LwParenthesisNode * self)
{
    g_return_val_if_fail (self != NULL, NULL);

    g_atomic_int_inc (&self->refs);

    return self;
}


void
lw_parenthesis_node_assert_equals (LwParenthesisNode * self,
                                  LwParenthesisNode * other)
{
    g_assert_nonnull (self);
    g_assert_nonnull (other);

    g_assert_cmpint (self->has_parenthesis, ==, other->has_parenthesis);
    g_assert_cmpint (self->refs, ==, other->refs);
    g_assert_cmpstr (self->OPEN, ==, other->OPEN);
    g_assert_cmpstr (self->CLOSE, ==, other->CLOSE);
    
    g_assert_cmpint (g_list_length (self->explicit_children), ==, g_list_length (other->explicit_children));
    g_assert_cmpint (g_list_length (self->children), ==, g_list_length (other->children));

    {
      GList *self_link = NULL;
      GList *other_link = NULL;

      self_link = self->children;
      other_link = other->children;
      while (self_link != NULL && other_link != NULL)
      {
        lw_parenthesis_node_assert_equals (self_link->data, other_link->data);

        self_link = self_link->next;
        other_link = other_link->next;
      }
    }

    {
      GList *self_link = NULL;
      GList *other_link = NULL;

      self_link = self->explicit_children;
      other_link = other->explicit_children;
      while (self_link != NULL && other_link != NULL)
      {
        lw_parenthesis_node_assert_equals (self_link->data, other_link->data);
        g_assert_cmpint (g_list_index (self->children, self_link->data), !=, -1);

        self_link = self_link->next;
        other_link = other_link->next;
      }
    }
}
