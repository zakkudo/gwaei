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
//! @file querynode.c
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


static LwQueryNode * _parse_leaf_parenthesisnode (LwParenthesisNode * parenthesis_node, LwQueryNodeOperation * operation_out, GError ** error);
static LwQueryNode * _parse_parenthesisnode (LwParenthesisNode * parenthesis_node, LwQueryNodeOperation * operation_out, GError ** error);


GQuark
lw_querynode_error_quark ()
{
    return g_quark_from_static_string ("lw-querynode-error");
}


static LwQueryNode *
lw_querynode_new (gchar const          * PARENTHESIS,
                  gchar const          * OPEN,
                  gchar const          * CLOSE,
                  LwQueryNodeOperation   operation)
{
    //Declarations
    LwQueryNode * self = NULL;
    gchar * data = NULL;

    //Initializations
    if (OPEN != NULL && CLOSE != NULL)
    {
      if (PARENTHESIS != NULL)
      {
        data = g_strdup_printf ("%s%.*s)", PARENTHESIS, CLOSE - OPEN, OPEN);
        if (data == NULL) goto errored;
      }
      else
      {
        data = g_strndup (OPEN, CLOSE - OPEN);
        if (data == NULL) goto errored;
      }
    }
    self = g_new0 (LwQueryNode, 1);
    if (self == NULL) goto errored;

    self->operation = operation;
    self->refs = 1;

    self->data = data;
    data = NULL;

errored:

    g_free (data); data = NULL;

    return self;
}


static LwQueryNode *
lw_querynode_new_tree_from_parenthesisnode (LwParenthesisNode     * parenthesis_node,
                                            LwQueryNodeOperation  * operation_out,
                                            GError               ** error)
{
    //Sanity checks
    g_return_val_if_fail (parenthesis_node != NULL, NULL);
    if (error != NULL && *error != NULL) return NULL;

    //Delcarations
    GList *link = NULL;
    LwQueryNode * query_node = NULL;
    LwQueryNode * query_node_out = NULL;
    gboolean is_leaf = FALSE;

    //Initializations
    is_leaf = (parenthesis_node->children == NULL);

    if (is_leaf)
    {
      query_node = _parse_leaf_parenthesisnode (parenthesis_node, operation_out, error);
      if (error != NULL && *error != NULL) goto errored;
    }
    else
    {
      query_node = _parse_parenthesisnode (parenthesis_node, operation_out, error);
      if (error != NULL && *error != NULL) goto errored;
    }

    query_node_out = query_node;
    query_node = NULL;

errored:

    if (query_node != NULL) lw_querynode_unref (query_node);
    query_node = NULL;

    return query_node_out;
}


LwQueryNode *
lw_querynode_new_tree_from_string (gchar const          *  TEXT,
                                   LwQueryNodeOperation *  operation_out,
                                   GError               ** error)
{
    //Sanity checks
    g_return_val_if_fail (TEXT != NULL, NULL);
    if (error != NULL && *error != NULL) return NULL;

    //Declarations
    LwParenthesisNode *parenthesis_node = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;
    LwQueryNode *query_node = NULL;

    if (operation_out != NULL)
    {
      operation = *operation_out;
    }

    //Initializations
    parenthesis_node = lw_parenthesisnode_new_tree_from_string (TEXT, error);
    if (parenthesis_node == NULL || (error != NULL && *error != NULL)) goto errored;
    query_node = lw_querynode_new_tree_from_parenthesisnode (parenthesis_node, &operation, error);
    if (error != NULL && *error != NULL) goto errored;

    if (operation_out != NULL)
    {
      *operation_out = operation;
    }

errored:

    if (parenthesis_node != NULL) lw_parenthesisnode_unref (parenthesis_node);
    parenthesis_node = NULL;

    return query_node;
}


static gchar const *
_read_regex_parenthesis (gchar const * c,
                         gchar const ** PARENTHESIS)
{
    //Sanity checks
    g_return_val_if_fail (c != NULL, NULL);
    g_return_val_if_fail (PARENTHESIS != NULL, NULL);

    if (strncmp(c, "(?=", strlen("(?=")) == 0)
    {
      *PARENTHESIS = "(?=";
      c += strlen("(?=");
    }
    else if (strncmp(c, "(?<=", strlen("(?<=")) == 0)
    {
      *PARENTHESIS = "(?<=";
      c += strlen("(?<=");
    }
    else if (strncmp(c, "(?<!", strlen("(?<!")) == 0)
    {
      *PARENTHESIS = "(?<!";
      c += strlen("(?<!");
    }
    else if (strncmp(c, "(", strlen("(")) == 0)
    {
      *PARENTHESIS = "(";
      c += strlen("(");
    }

    return c;
}


struct _LeafIterator {
  gchar const * PARENTHESIS;
  gchar const * CLOSE;
  gchar const * OPEN;
  gchar const * c;
  LwQueryNodeOperation operation;
};
typedef struct _LeafIterator LeafIterator;


static void
leafiterator_init (LeafIterator         * self,
                   LwParenthesisNode    * parenthesis_node,
                   LwQueryNodeOperation * operation_out,
                   GError               ** error)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);
    if (error != NULL && *error != NULL) return;

    self->c = parenthesis_node->OPEN;
    self->OPEN = parenthesis_node->OPEN;
    self->CLOSE = parenthesis_node->CLOSE;
    self->operation = LW_QUERYNODE_OPERATION_NONE;

    self->c = self->OPEN = _read_regex_parenthesis (parenthesis_node->OPEN, &self->PARENTHESIS);

    if (self->PARENTHESIS != NULL)
    {
      self->CLOSE--;
    }

    if (self->PARENTHESIS == NULL && operation_out != NULL)
    {
      self->operation = *operation_out;
    }

errored:

    return;
}


static void
leafiterator_clear (LeafIterator         *  self,
                    LwQueryNodeOperation *  operation_out,
                    GError               ** error)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    if (error != NULL && *error != NULL) return;

    if (self->PARENTHESIS == NULL && operation_out != NULL)
    {
      *operation_out = self->operation;
    }

errored:

    return;
}


static LwQueryNode *
leafiterator_new_connector_node (LeafIterator          * self,
                                 const gchar           * CONNECTOR_STR,
                                 LwQueryNodeOperation    next_operation,
                                 GError               ** error)
{
    //Declarations
    LwQueryNode * query_node = NULL;

    if (self->OPEN < self->c)
    {
      query_node = lw_querynode_new (self->PARENTHESIS, self->OPEN, self->c, self->operation);
      if (query_node == NULL) goto errored;
    }
    else
    {
      g_set_error (
        error,
        LW_QUERYNODE_ERROR,
        LW_QUERYNODE_HANGING_START_LOGICAL_CONNECTOR,
        "is missing left side of query before logical connector"
      );
      goto errored;
    }
    self->c += strlen(CONNECTOR_STR);
    self->operation = next_operation;
    self->OPEN = self->c;

errored:

    return query_node;
}


static LwQueryNode *
leafiterator_new_final_node (LeafIterator          * self,
                             GError               ** error)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);
    if (error != NULL && *error != NULL) return NULL;

    //Declarations
    LwQueryNode *query_node = NULL;

    if (self->OPEN < self->c)
    {
      query_node = lw_querynode_new (self->PARENTHESIS, self->OPEN, self->c, self->operation);
      self->operation = LW_QUERYNODE_OPERATION_NONE;
    }

    if (self->PARENTHESIS != NULL && self->operation != LW_QUERYNODE_OPERATION_NONE)
    {
      g_set_error (
        error,
        LW_QUERYNODE_ERROR,
        LW_QUERYNODE_HANGING_END_LOGICAL_CONNECTOR,
        "is missing right side of query after logical connector"
      );
      goto errored;
    }

    self->c = NULL;

errored:

    return query_node;
}


static LwQueryNode*
leafiterator_read (LeafIterator  * self,
                   GError       ** error)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);
    if (error != NULL && *error != NULL) return NULL;
    if (self->c == NULL) return NULL;

    //Declarations
    LwQueryNode * query_node = NULL;
    gboolean isescaped = FALSE;

    while (*self->c != '\0' && self->c <= self->CLOSE && query_node == NULL)
    {
      isescaped = (lw_utf8_isescaped (self->OPEN, self->c));
      if (!isescaped && strncmp(self->c, "&&", strlen("&&")) == 0)
      {
        query_node = leafiterator_new_connector_node (self, "&&", LW_QUERYNODE_OPERATION_AND, error);
        if (error != NULL && *error != NULL) goto errored;
      }
      else if (!isescaped && strncmp(self->c, "||", strlen("||")) == 0)
      {
        query_node = leafiterator_new_connector_node (self, "&&", LW_QUERYNODE_OPERATION_OR, error);
        if (error != NULL && *error != NULL) goto errored;
      }
      else
      {
        self->c = g_utf8_next_char (self->c);
      }
    }

    if (query_node == NULL)
    {
      query_node = leafiterator_new_final_node (self, error);
      if (error != NULL && *error != NULL) goto errored;
    }

errored:

    return query_node;
}


static GList *
_tokenize_leaf (LwParenthesisNode    *  parenthesis_node,
                LwQueryNodeOperation *  operation_out,
                GError               ** error)
{
    //Sanity checks
    g_return_if_fail (parenthesis_node != NULL);
    if (error != NULL && *error != NULL) return NULL;

    // Declarations
    LeafIterator iter = {0};
    GList * children = NULL;
    GList * children_out = NULL;
    LwQueryNode * query_node = NULL;

    //Initializations
    leafiterator_init (&iter, parenthesis_node, operation_out, error);
    if (error != NULL && *error != NULL) goto errored;

    do {
      query_node = leafiterator_read (&iter, error);
      if (error != NULL && *error != NULL) goto errored;
      if (query_node != NULL)
      {
        children = g_list_prepend (children, query_node);
      }
    } while (query_node != NULL);

    children_out = g_list_reverse (children);
    children = NULL;

errored:

    leafiterator_clear (&iter, operation_out, error);
    g_list_free_full (children, (GDestroyNotify) lw_querynode_unref);
    children = NULL;

    return children_out;
}


static LwQueryNode *
_parse_leaf_parenthesisnode (LwParenthesisNode    *  parenthesis_node,
                             LwQueryNodeOperation *  operation_out,
                             GError               ** error)
{
    //Sanity checks
    g_return_val_if_fail (parenthesis_node != NULL, NULL);
    if (error != NULL && *error != NULL) return NULL;

    //Declarations
    GList * children = NULL;
    LwQueryNode * query_node = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;
    LwQueryNodeOperation inner_operation = LW_QUERYNODE_OPERATION_NONE;

    if (operation_out != NULL)
    {
      operation = *operation_out;
    }

    //Initializations
    children = _tokenize_leaf (parenthesis_node, &inner_operation, error);
    if (error != NULL && *error != NULL) goto errored;
    if (children == NULL) goto errored;

    //Has multiple children, so create a parent node
    if (children != NULL && children->next != NULL)
    {
      query_node = lw_querynode_new (NULL, NULL, NULL, operation);
      if (query_node == NULL) goto errored;
      query_node->children = children;
      children = NULL;
    }
    //One one child, so just return it durectly
    else if (children != NULL)
    {
      query_node = children->data;
      query_node->operation = operation;
      g_list_free (children);
      children = NULL;
    }

    if (operation_out != NULL)
    {
      *operation_out = inner_operation;
    }

errored:

    g_list_free_full (children, (GDestroyNotify) lw_querynode_unref);
    children = NULL;

    return query_node;
}


static LwQueryNode *
_parse_parenthesisnode (LwParenthesisNode    *  parenthesis_node,
                        LwQueryNodeOperation *  operation_out,
                        GError               ** error)
{
    //Sanity checks
    g_return_val_if_fail (parenthesis_node != NULL, NULL);
    if (error != NULL && *error != NULL) return NULL;

    //Declarations
    GList * children = NULL;
    GList * link = NULL;
    LwQueryNode * query_node = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;

    //Initializations
    if (operation_out != NULL)
    {
      operation = *operation_out;
    }

    for (link = parenthesis_node->children; link != NULL; link = link->next)
    {
      LwQueryNode * child = lw_querynode_new_tree_from_parenthesisnode (LW_PARENTHESISNODE (link->data), &operation, error);
      if (child != NULL)
      {
        children = g_list_prepend (children, child);
      }
    }

    if (operation_out != NULL && *operation_out != LW_QUERYNODE_OPERATION_NONE)
    {
      g_set_error (
        error,
        LW_QUERYNODE_ERROR,
        LW_QUERYNODE_HANGING_END_LOGICAL_CONNECTOR,
        "is missing right side of query after logical connector"
      );
      goto errored;
    }

    if (children == NULL) goto errored;

    if (children != NULL)
    {
      //There is only one, so we'll use it directly
      if (children->next == NULL)
      {
        query_node = children->data;
        g_list_free (children);
        children = NULL;
      }
      //Create a parent node to hold the children
      else
      {
        query_node = lw_querynode_new (NULL, NULL, NULL, *operation_out);
        if (query_node == NULL) goto errored;
        *operation_out = LW_QUERYNODE_OPERATION_NONE;

        query_node->children = g_list_reverse (children);
        children = NULL;
      }
    }
    

    if (operation_out != NULL)
    {
      *operation_out = operation;
    }

errored:

    g_list_free_full (children, (GDestroyNotify) lw_querynode_unref);
    children = NULL;

    return query_node;
}


LwQueryNode*
lw_querynode_ref (LwQueryNode * self)
{
    g_return_val_if_fail (self != NULL, NULL);

    g_atomic_int_inc (&self->refs);

    return self;
}


static void
lw_querynode_free (LwQueryNode *self)
{
    if (self == NULL) return;

    g_free (self->data);
    g_list_free_full (self->children, (GDestroyNotify) lw_querynode_free);
    
    memset(self, 0, sizeof(LwQueryNode));

    g_free (self); self = NULL;
}


void
lw_querynode_unref (LwQueryNode *self)
{
    g_return_if_fail (self != NULL);

    if (g_atomic_int_dec_and_test (&self->refs))
    {
      lw_querynode_free (self);
    }
}


void
lw_querynode_assert_equals (LwQueryNode *self,
                            LwQueryNode *other)
{
    g_assert_nonnull (self);
    g_assert_nonnull (other);
    g_assert_cmpuint (self->operation, ==, other->operation);
    g_assert_cmpstr (self->language, ==, other->language);
    g_assert_cmpstr (self->data, ==, other->data);
    g_assert_cmpint (self->refs, ==, other->refs);

    g_assert_cmpint (g_list_length (self->children), ==, g_list_length(other->children));

    {
      GList * self_link = self->children;
      GList * other_link = other->children;

      while (self_link != NULL && other_link != NULL)
      {
        lw_querynode_assert_equals (self_link->data, other_link->data);

        self_link = self_link->next;
        other_link = other_link->next;
      }
    }
}

