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
 * SECTION:match_info
 * @short_description: A way to inspect deeper information on matches
 * @title: LwMatchInfo
 *
 * Provides a way to more deeply inspect matches from #LwQueryNode when you
 * run lw_querynode_match_parsed_line().  One of the primary purposes of this
 * library is to allow easy highlighting of substrings of the match.
 */

/*
              if (g_match_info_fetch_pos (match_info, 0, &start_pos, &end_post))
              {
              }
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

#include <libwaei/query-node.h>
#include <libwaei/gettext.h>



static gint
_compare (gconstpointer * a,
          gconstpointer * b,
          gpointer      user_data)
{
    return GPOINTER_TO_INT (a) - GPOINTER_TO_INT (b);
}

static void
_unref_column (LwColumnMatchInfo * self)
{
    if (self == NULL) return;
    lw_column_match_info_unref (self);
}

/**
 * lw_match_info_new:
 *
 * Returns: A new #LwMatchInfo that can be freed with lw_match_info_unref()
 */
LwMatchInfo *
lw_match_info_new ()
{
    //Declarations
    LwMatchInfo * self = NULL;
    GTree * tree = NULL;

    //Initializations
    tree = g_tree_new_full ((GCompareDataFunc) _compare, NULL, NULL, (GDestroyNotify) _unref_column);
    if (tree == NULL) goto errored;
    self = g_new0 (LwMatchInfo, 1);
    if (self == NULL) goto errored;
    self->refs = 1;

    self->tree = tree;
    tree = NULL;

errored:

    if (tree != NULL) g_tree_unref (tree);
    tree = NULL;

    return self;
}


static void
lw_match_info_free (LwMatchInfo * self)
{
    if (self == NULL) return;

    if (self->tree != NULL) g_tree_unref (self->tree);

    memset(self, 0, sizeof(LwMatchInfo));

    g_free (self);
}


/**
 * lw_match_info_ref:
 * @self: A #LwMatchInfo
 *
 * Returns: The #LwMatchInfo with its reference count incremented
 */
LwMatchInfo*
lw_match_info_ref (LwMatchInfo * self)
{
    g_return_val_if_fail (self != NULL, NULL);

    g_atomic_int_inc (&self->refs);

    return self;
}


/**
 * lw_match_info_unref:
 * @self: A #LwMatchInfo
 *
 * Decreses the reference count on the #LwMatchInfo, freeing it once it reaches 0
 */
void
lw_match_info_unref (LwMatchInfo * self)
{
    g_return_if_fail (self != NULL);

    if (g_atomic_int_dec_and_test (&self->refs))
    {
      lw_match_info_free (self);
    }
}


/**
 * lw_match_info_get_column:
 * @self: A #LwMatchInfo
 * @column: The column you want to fetch
 * Returns: A #LwColumnMatchInfo or %NULL if it wasn't previously set
 */
LwColumnMatchInfo *
lw_match_info_get_column (LwMatchInfo * self,
                                  gint                   column)
{
    g_return_val_if_fail (self != NULL, NULL);

    return g_tree_lookup (self->tree, GINT_TO_POINTER (column));
}


/**
 * lw_match_info_set_column:
 * @self: A #LwMatchInfo
 * @column_match_info: The #LwColumnMatchInfo to set or %NULL to unset it
 */
void
lw_match_info_set_column (LwMatchInfo       * self,
                                  LwColumnMatchInfo * column_match_info)
{

    //Initializations
    g_return_if_fail (self != NULL);
    g_return_if_fail (column_match_info != NULL);

    lw_column_match_info_ref (column_match_info);

    g_tree_insert (self->tree, GINT_TO_POINTER (column_match_info->column), column_match_info);
}
