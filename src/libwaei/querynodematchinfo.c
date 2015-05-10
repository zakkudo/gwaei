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
 * SECTION:querynodematchinfo
 * @short_description: A way to inspect deeper information on matches
 * @title: LwQueryNodeMatchInfo
 *
 * Provides a way to more deeply inspect matches from #LwQueryNode when you
 * run lw_querynode_match_parsedline().  One of the primary purposes of this
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

#include <libwaei/querynode.h>
#include <libwaei/gettext.h>



static gint
_compare (gconstpointer a,
          gconstpointer b,
          gpointer      user_data)
{
    return GPOINTER_TO_INT (a) - GPOINTER_TO_INT (b);
}

static void
_destroy_match_info_list (GList * matches)
{
    g_list_free_full (matches, (GDestroyNotify) g_match_info_unref);
}



/**
 * lw_querynodematchinfo_new:
 *
 * Returns: A new #LwQueryNodeMatchInfo that can be freed with lw_querynodematchinfo_unref()
 */
static LwQueryNodeMatchInfo *
lw_querynodematchinfo_new ()
{
    //Declarations
    LwQueryNodeMatchInfo * self = NULL;
    GTree * tree = NULL;

    //Initializations
    tree = g_tree_new_full (_compare, NULL, NULL, (GDestroyNotify) _destroy_match_info_list);
    if (tree == NULL) goto errored;
    self = g_new0 (LwQueryNodeMatchInfo, 1);

    self->tree = tree;
    tree = NULL;

errored:

    if (tree != NULL) g_tree_unref (tree);
    tree = NULL;

    return self;
}


static void
lw_querynodematchinfo_free (LwQueryNodeMatchInfo * self)
{
    if (self == NULL) return;

    if (self->tree != NULL) g_tree_unref (self->tree);

    memset(self, 0, sizeof(LwQueryNodeMatchInfo));

    g_free (self);
}


/**
 * lw_querynodematchinfo_ref:
 * @self: A #LwQueryNodeMatchInfo
 *
 * Returns: The #LwQueryNodeMatchInfo with its reference count incremented
 */
LwQueryNodeMatchInfo*
lw_querynodematchinfo_ref (LwQueryNodeMatchInfo * self)
{
    g_return_val_if_fail (self != NULL, NULL);

    g_atomic_int_inc (&self->refs);

    return self;
}


/**
 * lw_querynodematchinfo_unref:
 * @self: A #LwQueryNodeMatchInfo
 *
 * Decreses the reference count on the #LwQueryNodeMatchInfo, freeing it once it reaches 0
 */
void
lw_querynodematchinfo_unref (LwQueryNodeMatchInfo * self)
{
    g_return_if_fail (self != NULL);

    if (g_atomic_int_dec_and_test (&self->refs))
    {
      lw_querynodematchinfo_free (self);
    }
}


/**
 * lw_querynodematchinfo_get_column:
 * @self: A #LwQueryNodeMatchInfo
 * @column: The column you want to fetch
 * Returns: A #LwQueryNodeColumnMatchInfo or %NULL if it wasn't previously set
 */
LwQueryNodeColumnMatchInfo *
lw_querynodematchinfo_get_column (LwQueryNodeMatchInfo * self,
                                  gint                   column)
{
    g_return_val_if_fail (self != NULL, NULL);

    return g_tree_lookup (self->tree, GINT_TO_POINTER (column));
}


/**
 * lw_querynodematchinfo_set_column:
 * @self: A #LwQueryNodeMatchInfo
 * @column: The column to set the 
 * @column_match_info: The #LwQueryNodeColumnMatchInfo to set or %NULL to unset it
 */
void
lw_querynodematchinfo_set_column (LwQueryNodeMatchInfo       * self,
                                  gint                         column,
                                  LwQueryNodeColumnMatchInfo * column_match_info)
{
    //Initializations
    g_return_if_fail (self != NULL);

    if (column_match_info != NULL)
    {
      lw_querynodecolumnmatchinfo_ref (column_match_info);
    }

    g_tree_insert (self->tree, GINT_TO_POINTER (column), column_match_info);
}
