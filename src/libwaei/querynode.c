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


static LwQueryNode * _parse_leaf_parenthesisnode (LwParenthesisNode * parenthesis_node, LwQueryNodeOperation * next_operation_out, GError ** error);
static LwQueryNode * _parse_parenthesisnode (LwParenthesisNode * parenthesis_node, LwQueryNodeOperation * operation, GError ** error);


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
                                            LwQueryNodeOperation  * next_operation_out,
                                            GError               ** error)
{
    //Sanity checks
    g_return_val_if_fail (parenthesis_node != NULL, NULL);
    if (error != NULL && *error != NULL) return NULL;

    //Delcarations
    GList *link = NULL;
    LwQueryNode * query_node = NULL;
    gboolean is_leaf = FALSE;

    //Initializations
    is_leaf = (parenthesis_node->children == NULL);

    if (is_leaf)
    {
      query_node = _parse_leaf_parenthesisnode (parenthesis_node, next_operation_out, error);
    }
    else
    {
      query_node = _parse_parenthesisnode (parenthesis_node, next_operation_out, error);
    }

    return query_node;
}


LwQueryNode *
lw_querynode_new_tree_from_string (gchar const *  TEXT,
                                   GError      ** error)
{
    //Sanity checks
    g_return_val_if_fail (TEXT != NULL, NULL);
    if (error != NULL && *error != NULL) return NULL;

    //Declarations
    LwParenthesisNode *parenthesis_node = NULL;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;
    LwQueryNode *query_node = NULL;

    //Initializations
    parenthesis_node = lw_parenthesisnode_new_tree_from_string (TEXT, error);
    if (parenthesis_node == NULL || (error != NULL && *error != NULL)) goto errored;
    query_node = lw_querynode_new_tree_from_parenthesisnode (parenthesis_node, &operation, error);

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


static GList *
_tokenize_leaf (LwParenthesisNode    *  parenthesis_node,
                LwQueryNodeOperation *  next_node_operation_out,
                GError               ** error)
{
    //Sanity checks
    g_return_if_fail (parenthesis_node != NULL);
    if (error != NULL && *error != NULL) return NULL;

    // Declarations
    gchar const * c = parenthesis_node->OPEN;
    gchar const * OPEN = parenthesis_node->OPEN;
    gchar const * CLOSE = parenthesis_node->CLOSE;
    GList * children = NULL;
    GList * children_out;
    LwQueryNodeOperation operation = LW_QUERYNODE_OPERATION_NONE;
    gchar const * PARENTHESIS = NULL;

    //Initializations

    c = OPEN = _read_regex_parenthesis (OPEN, &PARENTHESIS);
    if (PARENTHESIS != NULL && *CLOSE != ')')
    {
      *error = g_error_new (
        LW_QUERYNODE_ERROR,
        LW_QUERYNODE_UNCLOSED_PARENTHESIS,
        "Expected closing parenthesis at character %d", CLOSE - c
      );
      goto errored;
    }
    if (PARENTHESIS != NULL)
    {
      CLOSE--;
    }

    if (next_node_operation_out != NULL) operation = *next_node_operation_out;

    while (*c != '\0' && c <= CLOSE)
    {
      if (!lw_utf8_isescaped (OPEN, c))
      {
        if (strncmp(c, "&&", strlen("&&")) == 0)
        {
          if (OPEN < c)
          {
            LwQueryNode *query_node = lw_querynode_new (PARENTHESIS, OPEN, c, operation);
            children = g_list_prepend (children, query_node);
          }
          c += strlen("&&");
          operation = LW_QUERYNODE_OPERATION_AND;
          OPEN = c;
        }
        else if (strncmp(c, "||", strlen("||")) == 0)
        {
          if (OPEN < c)
          {
            LwQueryNode *query_node = lw_querynode_new (PARENTHESIS, OPEN, c, operation);
            children = g_list_prepend (children, query_node);
          }
          c += strlen("||");
          operation = LW_QUERYNODE_OPERATION_OR;
          OPEN = c;
        }
        else
        {
          c = g_utf8_next_char (c);
        }
      }
      else
      {
        c = g_utf8_next_char (c);
      }
    }

    if (OPEN < c)
    {
      LwQueryNode *query_node = lw_querynode_new (PARENTHESIS, OPEN, c, operation);
      children = g_list_prepend (children, query_node);
      operation = LW_QUERYNODE_OPERATION_NONE;
    }

    children_out = g_list_reverse (children);
    children = NULL;

    if (next_node_operation_out != NULL)
    {
      *next_node_operation_out = operation;
    }

errored:

    g_list_free_full (children, (GDestroyNotify) lw_querynode_unref);
    children = NULL;

    return children_out;
}


static LwQueryNode *
_parse_leaf_parenthesisnode (LwParenthesisNode    *  parenthesis_node,
                             LwQueryNodeOperation *  next_operation_out,
                             GError               ** error)
{
    //Sanity checks
    g_return_val_if_fail (parenthesis_node != NULL, NULL);
    if (error != NULL && *error != NULL) return NULL;

    //Declarations
    GList * children = NULL;
    LwQueryNode * query_node = NULL;

    //Initializations
    children = _tokenize_leaf (parenthesis_node, next_operation_out, error);

    if (children != NULL && children->next != NULL)
    {
      query_node = lw_querynode_new (NULL, NULL, NULL, *next_operation_out); //Create a parent
      if (query_node == NULL) goto errored;
      query_node->children = children;
      children = NULL;
    }
    else if (children != NULL)
    {
      query_node = children->data;
      g_list_free (children);
      children = NULL;
    }

errored:

    g_list_free_full (children, (GDestroyNotify) lw_querynode_unref);
    children = NULL;

    return query_node;
}


static LwQueryNode *
_parse_parenthesisnode (LwParenthesisNode    *  parenthesis_node,
                        LwQueryNodeOperation *  operation,
                        GError               ** error)
{
    //Sanity checks
    g_return_val_if_fail (parenthesis_node != NULL, NULL);
    if (error != NULL && *error != NULL) return NULL;

    //Declarations
    GList * children = NULL;
    GList * link = NULL;
    LwQueryNode * query_node = NULL;
    LwQueryNodeOperation next_operation;

    //Initializations
    next_operation = LW_QUERYNODE_OPERATION_NONE;

    for (link = parenthesis_node->children; link != NULL; link = link->next)
    {
      LwQueryNode * child = lw_querynode_new_tree_from_parenthesisnode(LW_PARENTHESISNODE (link->data), &next_operation, error);
      if (child != NULL)
      {
        children = g_list_prepend (children, child);
      }
    }
    if (children == NULL) goto errored;
    if (next_operation != LW_QUERYNODE_OPERATION_NONE)
    {
      *error = g_error_new (LW_QUERYNODE_ERROR, LW_QUERYNODE_HANGING_LOGICAL_CONNECTOR, "Could not patch a logical connector between two expressions %d", next_operation);
      goto errored;
    }

    query_node = lw_querynode_new (NULL, NULL, NULL, *operation);
    if (query_node == NULL) goto errored;
    operation = LW_QUERYNODE_OPERATION_NONE;

    query_node->children = g_list_reverse (children);
    children = NULL;

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

