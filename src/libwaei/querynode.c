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
 * SECTION:querynode
 * @short_description: A query tree structure for searches
 * @title: LwQueryNode
 *
 * Creates a node structure than can easily be iterated to carry out a 
 * seach against a list of results.
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

#include <libwaei/parenthesisnode.h>
#include <libwaei/querynode.h>
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
lw_querynode_new_keyed (gchar const          * KEY,
                        gchar const          * VALUE,
                        LwQueryNodeOperation   operation)
{
    //Sanity checks
    g_return_val_if_fail (KEY != NULL && VALUE != NULL, NULL);

    //Declarations
    LwQueryNode * self = NULL;
    gchar * data = NULL;
    gchar * key = NULL;

    //Initializations
    if (KEY != NULL && *KEY != '\0')
    {
      key = g_strdup (KEY);
      if (key == NULL) goto errored;
    }

    if (VALUE != NULL && *VALUE != '\0')
    {
      data = g_strdup (VALUE);
      if (data == NULL) goto errored;
    }

    self = g_new0 (LwQueryNode, 1);
    if (self == NULL) goto errored;

    self->key = key;
    key = NULL;

    self->data = data;
    data = NULL;

    self->operation = operation;

    self->refs = 1;

errored:

    g_free (data); data = NULL;
    g_free (key); key = NULL;

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


/**
 * lw_querynode_new_tree_from_string:
 * @TEXT: The text to generate the #LwQueryNode tree from
 * @operation_out: Returns any hanging operations that should be passed to the next sibling.  This should generally be set to %LW_QUERYNODE_OPERATION_NONE by outside consumers of this api.
 * @error: Pointer to a #GError or %NONE
 *
 * Generates a #LwQueryNode tree from a string.  This method is recursive, thus requiring a pointer to a #LwQueryNodeOperation for context.  This method uses #LwParenthesisNode as an intermediary compilation, so if there are any parentheiss errors, they will return through #GError with that domain.
 *
 * Returns: A new #LwQueryNode tree that can be freed with lw_querynode_unref()
 */
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


static GList *
_tokenize_explicit_columns (LeafIterator          * self,
                            GError               ** error)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    //Declarations
    GList * children = NULL;
    GList * children_out = NULL;
    gchar const * OPEN = NULL;
    gchar const * CLOSE = NULL;
    gchar const * BEFORE_LAST_WHITESPACE = NULL;
    gchar const * AFTER_LAST_WHITESPACE = NULL;
    gchar const * DELIMITER = NULL;
    gchar const * C = NULL;
    gchar * key_buffer = NULL;
    gchar * value_buffer = NULL;
    gchar const * LAST_NON_WHITESPACE = NULL;
    gchar const * LAST_WHITESPACE = NULL;
    gchar const * PREVIOUS_C = NULL;
    gunichar ch = 0;
    gint len = 0;
    LwQueryNode * query_node = NULL;

    //Initializations
    OPEN = self->OPEN;
    CLOSE = self->c;
    LAST_NON_WHITESPACE = AFTER_LAST_WHITESPACE = PREVIOUS_C = C = self->OPEN;
    len = CLOSE - OPEN + 1;
    if (len == 0) goto errored;
    key_buffer = g_new0 (gchar, len);
    if (key_buffer == NULL) goto errored;
    value_buffer = g_new0 (gchar, len);
    if (value_buffer == NULL) goto errored;

    while (*C != '\0' && C < CLOSE)
    {
      ch = g_utf8_get_char (C);

      if (g_unichar_isspace (ch))
      {
        if (LAST_WHITESPACE != PREVIOUS_C)
        {
          BEFORE_LAST_WHITESPACE = PREVIOUS_C;
        }
        LAST_WHITESPACE = C;
      }
      else
      {
        if (LAST_WHITESPACE == PREVIOUS_C)
        {
          AFTER_LAST_WHITESPACE = C;
        }
        LAST_NON_WHITESPACE = C;
      }

      if (*C == ':')
      {
        // First keyed query node
        if (DELIMITER == NULL)
        {
          DELIMITER = C;
          if (AFTER_LAST_WHITESPACE != NULL && AFTER_LAST_WHITESPACE < DELIMITER)
          {
            len = C - AFTER_LAST_WHITESPACE;
            strncpy(key_buffer, AFTER_LAST_WHITESPACE, len);
            key_buffer[len] = '\0';

            //It's okay for there to be no key (It just turns it into a normal search token)

            if (BEFORE_LAST_WHITESPACE != NULL && self->OPEN <= BEFORE_LAST_WHITESPACE)
            {
              query_node = lw_querynode_new (self->PARENTHESIS, self->OPEN, g_utf8_next_char (BEFORE_LAST_WHITESPACE), LW_QUERYNODE_OPERATION_NONE);
              if (query_node == NULL) goto errored;
              children = g_list_prepend (children, query_node);
              query_node = NULL;
            }
          }
        }
        // Any keyed query nodes after
        else if (DELIMITER != NULL && (LAST_WHITESPACE < C && LAST_WHITESPACE > DELIMITER))
        {
          //Close the previous keyed query node
          DELIMITER = g_utf8_next_char (DELIMITER);
          len = LAST_WHITESPACE - DELIMITER;
          strncpy(value_buffer, DELIMITER, len);
          value_buffer[len] = '\0';

          if (*value_buffer == '\0')
          {
            g_set_error (
              error,
              LW_QUERYNODE_ERROR,
              LW_QUERYNODE_ERROR_MISSING_VALUE_FOR_KEYED_QUERYNODE,
              "key with no value for token %s", key_buffer
            );
            goto errored;
          }
          LwQueryNodeOperation operation = (children == NULL) ? LW_QUERYNODE_OPERATION_NONE : LW_QUERYNODE_OPERATION_AND;
          query_node = lw_querynode_new_keyed (key_buffer, value_buffer, operation);
          if (query_node == NULL) goto errored;
          children = g_list_prepend (children, query_node);
          *key_buffer = '\0';
          *value_buffer = '\0';
          query_node = NULL;

          DELIMITER = C;
          if (AFTER_LAST_WHITESPACE != NULL && AFTER_LAST_WHITESPACE < DELIMITER)
          {
            len = C - AFTER_LAST_WHITESPACE;
            strncpy(key_buffer, AFTER_LAST_WHITESPACE, len);
            key_buffer[len] = '\0';
          }
        }
      }
      PREVIOUS_C = C;
      C = g_utf8_next_char (C);
    }

    //Close off the final keyed querynode if it was started
    if (DELIMITER != NULL)
    {
      DELIMITER = g_utf8_next_char (DELIMITER);
      if (DELIMITER != '\0')
      {
        gint len = self->CLOSE - DELIMITER + 1;
        strncpy(value_buffer, DELIMITER, len);
        value_buffer[len] = '\0';

        if (*key_buffer == '\0' && *value_buffer == '\0')
        {
          g_set_error (
            error,
            LW_QUERYNODE_ERROR,
            LW_QUERYNODE_ERROR_MISSING_KEY_AND_VALUE_FOR_KEYED_QUERYNODE,
            "key:value"
          );
          goto errored;
        }

        query_node = lw_querynode_new_keyed (key_buffer, value_buffer, LW_QUERYNODE_OPERATION_AND);
        if (query_node == NULL) goto errored;
        children = g_list_prepend (children, query_node);

        *key_buffer = '\0';
        *value_buffer = '\0';
        query_node = NULL;
      }
      // Null value is okay for the last keyed query as long as it can be joined later
    }
    //If there were no keyed query nodes, just use the whole substring for a query node
    else
    {
      query_node = lw_querynode_new (self->PARENTHESIS, self->OPEN, CLOSE, LW_QUERYNODE_OPERATION_NONE);
      if (query_node == NULL) goto errored;
      children = g_list_prepend (children, query_node);
      query_node = NULL;
    }

    children_out = g_list_reverse (children);
    children = NULL;

errored:

    g_list_free_full (children, (GDestroyNotify) lw_querynode_unref); children = NULL;

    if (query_node != NULL) lw_querynode_unref (query_node);
    query_node = NULL;

    g_free (key_buffer); key_buffer = NULL;
    g_free (value_buffer); value_buffer = NULL;

    return children_out;
}


static LwQueryNode *
_parent_possible_children (GList                ** children_out,
                           LwQueryNodeOperation    operation)
{
    //Sanity check
    g_return_val_if_fail (children_out != NULL, NULL);
    if (*children_out == NULL) return NULL;

    //Declarations
    LwQueryNode * query_node = NULL;
    GList * children = NULL;

    //Initializations
    children = *children_out;

    //Has multiple children, so create a parent node
    if (children->next != NULL)
    {
      query_node = lw_querynode_new (NULL, NULL, NULL, operation);
      if (query_node == NULL) goto errored;
      query_node->children = children;
      children = NULL;
    }
    //One one child, so just return it directly
    else
    {
      query_node = children->data;
      query_node->operation = operation;
      g_list_free (children);
      children = NULL;
    }

    *children_out = NULL;

errored:

    return query_node;
}


static LwQueryNode *
leafiterator_new_connector_node (LeafIterator          * self,
                                 const gchar           * CONNECTOR_STR,
                                 LwQueryNodeOperation    next_operation,
                                 GError               ** error)
{
    //Declarations
    LwQueryNode * query_node = NULL;
    LwQueryNode * query_node_out = NULL;
    GList * children = NULL;

    if (self->OPEN < self->c)
    {
      children = _tokenize_explicit_columns (self, error);
      if (error != NULL && *error != NULL) goto errored;
      query_node = _parent_possible_children (&children, self->operation);
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

    query_node_out = query_node;
    query_node = NULL;

errored:

    if (query_node != NULL) lw_querynode_unref (query_node);
    query_node = NULL;

    return query_node_out;
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
    GList * children = NULL;

    if (self->OPEN < self->c)
    {
      children = _tokenize_explicit_columns (self, error);
      if (error != NULL && *error != NULL) goto errored;
      query_node = _parent_possible_children (&children, self->operation);
      self->operation = LW_QUERYNODE_OPERATION_NONE;
    }

    self->c = NULL;

errored:

    return query_node;
}


static LwQueryNode *
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

      if (self->PARENTHESIS && self->operation != LW_QUERYNODE_OPERATION_NONE)
      {
        g_set_error (
          error,
          LW_QUERYNODE_ERROR,
          LW_QUERYNODE_HANGING_END_LOGICAL_CONNECTOR,
          "is missing left side of query before logical connector"
        );
        goto errored;
      }
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

    query_node = _parent_possible_children (&children, operation);

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


/**
 * lw_querynode_ref:
 * @self: A #LwQueryNode
 * 
 * Returns: Increases the reference count of the #LwQueryNode and returns it
 */
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
    g_free (self->key);
    if (self->regex != NULL) g_regex_unref (self->regex);
    g_list_free_full (self->children, (GDestroyNotify) lw_querynode_free);
    
    memset(self, 0, sizeof(LwQueryNode));

    g_free (self); self = NULL;
}


/**
 * lw_querynode_unref:
 * @self: A #LwQueryNode
 * 
 * Decreases the references on the #LwQueryNode and frees it if it reaches 0
 */
void
lw_querynode_unref (LwQueryNode *self)
{
    g_return_if_fail (self != NULL);

    if (g_atomic_int_dec_and_test (&self->refs))
    {
      lw_querynode_free (self);
    }
}


/**
 * lw_querynode_assert_equals:
 * @self: A #LwQueryNode
 * @other: Another #LwQueryNode
 *
 * Returns: %TRUE if the internal fields and children match
 */
void
lw_querynode_assert_equals (LwQueryNode *self,
                            LwQueryNode *other)
{
    g_assert_nonnull (self);
    g_assert_nonnull (other);
    g_assert_cmpuint (self->operation, ==, other->operation);
    g_assert_cmpstr (self->key, ==, other->key);
    g_assert_cmpstr (self->data, ==, other->data);
    g_assert_cmpint (self->refs, ==, other->refs);
    if (self->regex != NULL && other->regex != NULL)
    {
      g_assert_cmpstr (g_regex_get_pattern (self->regex), ==, g_regex_get_pattern (other->regex));
    }
    else
    {
      g_assert (self->regex == other->regex);
    }

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


/**
 * lw_querynode_walk:
 * @self: A #LwQueryNode
 * @func: A method to run on each #LwQueryNode
 * @data: A #gpointer to pass to each @func
 *
 * Returns: %TRUE if the walk was cancelled early
 */
gboolean
lw_querynode_walk (LwQueryNode         * self,
                   LwQueryNodeWalkFunc   func,
                   gpointer              data)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    //Declarations
    gboolean should_stop = FALSE;
    GList * link = NULL;

    should_stop = func(self, data);
    if (should_stop) goto errored;

    for (link = self->children; link != NULL; link = link->next)
    {
      should_stop = lw_querynode_walk (link->data, func, data);
      if (should_stop) goto errored;
    }

errored:

    return should_stop;
}


static gboolean
_querynode_apply_implied_logical_junctions (LwQueryNode * self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);

    //Declarations
    GList * link = NULL;
    gboolean has_junction_out = FALSE;
    gboolean has_junction = FALSE;
    LwQueryNode * query_node = NULL;
    LwQueryNode * next_query_node = NULL;
    LwQueryNode * previous_query_node = NULL;

    for (link = self->children; link != NULL; link = link->next)
    {
      query_node = LW_QUERYNODE (link->data);
      next_query_node = (link->next) ? LW_QUERYNODE (link->next->data) : NULL;
      has_junction = _querynode_apply_implied_logical_junctions (query_node);
      if (has_junction)
      {
        if (query_node != NULL && query_node->operation == LW_QUERYNODE_OPERATION_NONE && (previous_query_node != NULL && !LW_QUERYNODE_IS_DANGLING_KEY (previous_query_node)))
        {
          query_node->operation = LW_QUERYNODE_OPERATION_AND;
        }
        if (next_query_node != NULL && next_query_node->operation == LW_QUERYNODE_OPERATION_NONE)
        {
          next_query_node->operation = LW_QUERYNODE_OPERATION_AND;
        }
        has_junction_out = TRUE;
      }
      previous_query_node = query_node;
    }

    return has_junction_out || self->operation != LW_QUERYNODE_OPERATION_NONE;
}


static GList *
_reduce_previous_none_operations (GList *  children,
                                  GList *  link,
                                  gchar ** tokens,
                                  gint  *  num_tokens_out)
{
    //Sanity checks
    g_return_val_if_fail (tokens != NULL, NULL);
    g_return_val_if_fail (num_tokens_out != NULL, NULL);

    if (children == NULL) return children;
    if (*num_tokens_out < 1 || *tokens == NULL) return children;
    if (link == NULL) return children;
    if (link->prev == NULL) return children;

    //Declarations
    LwQueryNode * query_node = NULL;
    gchar * new_data = NULL;
    GList * previous_link = NULL;
    LwQueryNode * previous_query_node = NULL;

    //Initializations
    query_node = LW_QUERYNODE (link->data);
    if (query_node == NULL || query_node->operation != LW_QUERYNODE_OPERATION_NONE) goto errored;
    new_data = g_strjoinv (NULL, tokens);
    if (new_data == NULL) goto errored;
    previous_link = link->prev;
    previous_query_node = (previous_link != NULL) ? LW_QUERYNODE (previous_link->data) : NULL;

    while (previous_query_node != NULL && previous_query_node->operation == LW_QUERYNODE_OPERATION_NONE && previous_query_node->key == NULL && previous_query_node->children == NULL)
    {
      lw_querynode_unref (previous_query_node);
      children = g_list_delete_link (children, previous_link);
      
      previous_link = link->prev;
      previous_query_node = (previous_link != NULL) ? LW_QUERYNODE (previous_link->data) : NULL;
    }

    g_free (query_node->data);
    query_node->data = new_data;
    new_data = NULL;

    memset(tokens, 0, *num_tokens_out * sizeof(gchar*));
    *num_tokens_out = 0;

errored:

    g_free (new_data); new_data = NULL;

    return children;
}


static void
_querynode_reparent_possible_children (LwQueryNode * self)
{
    //Declarations
    LwQueryNode * query_node = NULL;

    //If there is only one child, merge it into the parent
    if (self->children != NULL && self->children->next == NULL)
    {
      // Prune the link
      query_node = LW_QUERYNODE (self->children->data);
      if ((self->key == NULL || query_node->key == NULL) && self->data == NULL)
      {
        self->children = g_list_delete_link (self->children, self->children);

        if (query_node->key != NULL && self->key == NULL)
        {
          self->key = query_node->key;
          query_node->key = NULL;
        }

        g_free (self->data);
        self->data = query_node->data;
        query_node->data = NULL;

        if (self->regex) g_regex_unref (self->regex);
        self->regex = query_node->regex;
        query_node->regex = NULL;

        g_list_free_full (self->children, (GDestroyNotify) lw_querynode_unref);
        self->children = query_node->children;
        query_node->children = NULL;

        // Remove the old node
        g_free (query_node);
        query_node = NULL;
      }
    }
}


static void
_querynode_reduce_keyed_missing_values (LwQueryNode *  self,
                                        GError      ** error)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    if (error != NULL && *error != NULL) return;

    //Declarations
    GList * link = NULL;
    GList * next = NULL;
    LwQueryNode * keyed_query_node = NULL;
    LwQueryNode * query_node = NULL;

    //Initializations
    link = self->children;

    while (link != NULL)
    {
      query_node = LW_QUERYNODE (link->data);
      _querynode_reduce_keyed_missing_values (query_node, error);
      if (error != NULL && *error != NULL) goto errored;

      if (query_node->key != NULL && query_node->data == NULL && query_node->children == NULL)
      {
        keyed_query_node = query_node;
        if (link->next != NULL)
        {
          link = link->next;
          next = (link->next != NULL) ? link->next : NULL;
          query_node = LW_QUERYNODE (link->data);
          if (query_node->operation == LW_QUERYNODE_OPERATION_NONE)
          {
            self->children = g_list_remove_link (self->children, link);
            keyed_query_node->children = link;
            link = next;
          }
          else
          {
            g_set_error (
              error,
              LW_QUERYNODE_ERROR,
              LW_QUERYNODE_ERROR_MISSING_VALUE_FOR_KEYED_QUERYNODE,
              "key with no value for token %s", keyed_query_node->key 
            );
            goto errored;
          }
        }
        else
        {
          g_set_error (
            error,
            LW_QUERYNODE_ERROR,
            LW_QUERYNODE_ERROR_MISSING_VALUE_FOR_KEYED_QUERYNODE,
            "key with no value for token %s", keyed_query_node->key
          );
          goto errored;
        }
      }
      else
      {
        link = link->next;
      }
    }

    _querynode_reparent_possible_children (self);

errored:

    return;
}


static void
_querynode_reduce (LwQueryNode * self)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    //Declaration
    GList * link = NULL;
    gint num_children = 0;
    gchar ** tokens = NULL;
    gint num_tokens = 0;
    LwQueryNode * query_node = NULL;

    //Initializations
    num_children = g_list_length (self->children);
    if (num_children == 0) goto errored;
    tokens = g_new0 (gchar *, num_children + 1);
    if (tokens == NULL) goto errored;

    for (link = self->children; link != NULL; link = link->next)
    {
      query_node = LW_QUERYNODE (link->data);
      _querynode_reduce (query_node);
      if (query_node->operation == LW_QUERYNODE_OPERATION_NONE && query_node->key == NULL && query_node->children == NULL)
      {
        if (query_node->data != NULL)
        {
          tokens[num_tokens++] = query_node->data;
        }
      }
      else if (query_node->operation != LW_QUERYNODE_OPERATION_NONE || query_node->key != NULL || query_node->children != NULL)
      {
        self->children = _reduce_previous_none_operations (self->children, link->prev, tokens, &num_tokens);
      }
    }
    self->children = _reduce_previous_none_operations (self->children, g_list_last (self->children), tokens, &num_tokens);
    
    _querynode_reparent_possible_children (self);

errored:

    g_free (tokens); tokens = NULL;
}


static void
_querynode_compile (LwQueryNode * self,
                    gchar const * KEY,
                    LwUtf8Flag    flags,
                    GError      ** error)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    if (error != NULL && *error != NULL) return;

    //Declarations
    GList *link = NULL;
    LwQueryNode *query_node = NULL;
    gchar * normalized_pattern = NULL;
    GRegex * regex = NULL;

    if (self->key != NULL)
    {
      KEY = self->key;
    }
    else if (self->data != NULL)
    {
      self->key = g_strdup (KEY);
    }

    //Initializations
    if (self->data != NULL)
    {
      normalized_pattern = lw_utf8_normalize (self->data, -1, flags);
      if (normalized_pattern == NULL) goto errored;
      regex = g_regex_new (normalized_pattern, G_REGEX_OPTIMIZE, 0, error);
      if (error != NULL && *error != NULL) goto errored;
    }

    for (link = self->children; link != NULL; link = link->next)
    {
      query_node = LW_QUERYNODE (link->data);
      _querynode_compile (query_node, KEY, flags, error);
      if (error != NULL && *error != NULL) goto errored;
    }

    if (self->regex != NULL) goto errored;
    self->regex = regex;
    regex = NULL;

errored:

    if (regex != NULL) g_regex_unref (regex);
    regex =  NULL;

    if (self->data == NULL)
    {
      g_free (self->key);
      self->key = NULL;
    }

    g_free (normalized_pattern);
    normalized_pattern = NULL;
}


/**
 * lw_querynode_nnodes:
 * @self: A #LwQueryNode
 *
 * Returns: the total number of nodes
 */
gint
lw_querynode_nnodes (LwQueryNode * self)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    //Declarations
    gint nnodes = 1;
    LwQueryNode * query_node = NULL;
    GList * link = NULL;

    for (link = self->children; link != NULL; link = link->next)
    {
      query_node = LW_QUERYNODE (link->data);
      nnodes += lw_querynode_nnodes (query_node);
    }

    return nnodes;
}


/**
 * lw_querynode_compile:
 * @self: A #LwQueryNode
 * @flags: The flags for the prefered normalization of the query data
 * @error: An #GError to track errors or %NULL to ignore them
 *
 * Compiles the #LwQueryNode into a more compact form that is more ideal
 * for comparisons.  Once compiled, the #LwQueryNode structure will not be in
 * the same form as the original query and all regexes will be filled.
 * You should run this before any uses of lw_querynode_match_parsedline().
 */
void
lw_querynode_compile (LwQueryNode *  self,
                      LwUtf8Flag     flags,
                      GError      ** error)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    _querynode_apply_implied_logical_junctions (self);
    _querynode_reduce (self);
    _querynode_reduce_keyed_missing_values (self, error);
    _querynode_reduce (self);
    _querynode_compile (self, NULL, flags, error);
}


static gboolean
_children_match_parsedline (LwQueryNode           * self,
                            LwParsedLine          * parsed_line,
                            LwQueryNodeMatchInfo  * match_info_out)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (parsed_line != NULL, FALSE);

    //Declarations
    LwQueryNode * query_node = NULL;
    LwQueryNode * previous_query_node = NULL;
    gboolean matches = FALSE;
    gboolean previous_matches = FALSE;
    GList * link = NULL;
    
    for (link = self->children; link != NULL; link = link->next)
    {
      query_node = LW_QUERYNODE (link->data);
      matches = lw_querynode_match_parsedline (query_node, parsed_line, match_info_out);

      if (previous_query_node != NULL)
      {
        switch (query_node->operation)
        {
          case LW_QUERYNODE_OPERATION_OR:
              matches = (previous_matches || matches);
            break;
          case LW_QUERYNODE_OPERATION_AND:
              matches = (previous_matches && matches);
          default:
            break;
        }
      }

      previous_matches = matches;
      previous_query_node = query_node;
    }

errored:

    return matches;
}


static gboolean
_value_matches_column (LwQueryNode          * self,
                       LwParsedLine         * parsed_line,
                       gint                   column,
                       LwQueryNodeMatchInfo * match_info_out)
{
    //Declarations
    gint j = 0;
    gchar const ** strv = NULL;
    GMatchInfo * match_info = NULL;
    gboolean matches = FALSE;

    //Initializations
    strv = lw_parsedline_get_strv (parsed_line, column);

    if (strv != NULL)
    {
      for (j = 0; strv[j] != NULL; j++)
      {
        if (match_info_out != NULL)
        {
          matches = g_regex_match (self->regex, strv[j], 0, &match_info);
          while (g_match_info_matches (match_info))
          {
            lw_querynodematchinfo_insert (match_info_out, strv[j], match_info);
            g_match_info_next (match_info, NULL);
          }
          g_match_info_free (match_info);
        }
        else
        {
          matches = g_regex_match (self->regex, strv[j], 0, NULL);
          if (matches) goto errored;
        }
      }
    }

errored:

    return matches;
}

static gboolean
_value_matches_parsedline (LwQueryNode           * self,
                           LwParsedLine          * parsed_line,
                           LwQueryNodeMatchInfo  * match_info_out)
{
    //Declarations
    gboolean matches = FALSE;
    gint i = 0;
    gint column = -1;

    for (i = 0; self->columns[i] != -1 && !matches; i++)
    {
      column = self->columns[i];
      matches = _value_matches_column (self, parsed_line, column, match_info_out);
    }

errored:

    return matches;
}


/**
 * lw_querynode_match_parsedline:
 * @self: A #LwQueryNode
 * @parsed_line: A @parsed_line to search
 * @match_info_out: A #LwQueryNodeMatchInfo to record match information or %NULL to ignore it
 *
 * Returns: %TRUE if @self matches against the @parsed_line
 */
gboolean
lw_querynode_match_parsedline (LwQueryNode           * self,
                               LwParsedLine          * parsed_line,
                               LwQueryNodeMatchInfo  * match_info_out)
{
    //Declarations
    GList * link = NULL;
    gboolean matches = FALSE;

    if (self->children != NULL && self->data == NULL)
    {
      matches = _children_match_parsedline (self, parsed_line, match_info_out);
    }
    else if (self->data != NULL && self->children == NULL)
    {
      matches = _value_matches_parsedline (self, parsed_line, match_info_out);
    }
    else
    {
      g_assert_not_reached ();
    }

    if (match_info_out != NULL)
    {
      lw_querynodematchinfo_compile (match_info_out);
    }

errored:

    return matches;
}
