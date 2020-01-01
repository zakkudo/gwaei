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
#include <glib-object.h>

#include "query.h"
#include "gettext.h"

G_DEFINE_TYPE_WITH_CODE (LwMatchInfo, lw_match_info, LW_TYPE_LIST, G_ADD_PRIVATE(LwMatchInfo))

static void
lw_match_info_init (LwMatchInfo * self)
{
    LwMatchInfoPrivate *priv = NULL;

    priv = lw_match_info_get_instance_private (self);

    priv->tree = g_tree_new_full ((GCompareDataFunc) _compare, NULL, NULL, (GDestroyNotify) _unref_column);
}

static void
lw_match_info_finalize (LwMatchInfo * self)
{
    LwMatchInfoPrivate *priv = NULL;

    priv = lw_match_info_get_instance_private (self);

    if (priv->tree != NULL) g_tree_unref (tree);
    priv->tree = NULL;

}

static void
lw_match_info_class_init (LwMatchInfoClass * klass)
{
    //Declarations
    GObjectClass *object_class = NULL;
    LwListClass * list_class = NULL;
    LwMatchInfoClassPrivate * klasspriv = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    list_class = LW_LIST_CLASS (klass);
    klasspriv = lw_match_info_class_get_private (klass);

    object_class->set_property = lw_match_info_set_property;
    object_class->get_property = lw_match_info_get_property;
    object_class->finalize = lw_match_info_finalize;

    list_class->get_begin_iter = (LwListGetBeginIterFunc) lw_match_info_get_begin_iter;
    list_class->get_end_iter = (LwListGetEndIterFunc) lw_match_info_get_end_iter;
    list_class->get_iter_at_position = (LwListGetIterAtPositionFunc) lw_match_info_get_iter_at_position;
    list_class->get_length = (LwListGetLengthFunc) lw_match_info_get_length;

    list_class->iter_get_position = (LwListIterGetPositionFunc) lw_match_info_iter_get_position;
    list_class->iter_get = (LwListIterGetFunc) lw_match_info_iter_get;
    list_class->iter_next = (LwListIterNextFunc) lw_match_info_iter_next;
    list_class->iter_previous = (LwListIterPreviousFunc) lw_match_info_iter_previous;
    list_class->allocate = (LwListAllocateFunc) lw_match_info_allocate;
}

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
lw_match_info_new (LwList * haystack)
{
    //Declarations
    LwMatchInfo * self = NULL;
    GTree * tree = NULL;

    //Initializations
    self = LW_MATCH_INFO (g_object_new (LW_TYPE_MATCH_INFO, "haystack", haystack, NULL));

errored:

    return self;
}


/**
 * lw_match_info_set_column:
 * @self: A #LwMatchInfo
 * @column_match_info: The #LwColumnMatchInfo to set or %NULL to unset it
 */
void
lw_match_info_add_match (LwMatchInfo * self, gint column, GMatchInfo * match_info)
{
    //Initializations
    g_return_if_fail (self != NULL);
    g_return_if_fail (column_match_info != NULL);

    lw_column_match_info_ref (column_match_info);

    TODO
    if (match_info_out != NULL)
    {
        column_match_info = lw_match_info_get_column (match_info_out, column);
        if (column_match_info == NULL)
        {
            column_match_info = lw_column_match_info_new (column, strv);
            lw_match_info_set_column (match_info_out, column_match_info);
            lw_column_match_info_unref (column_match_info);
        }
    }

    lw_column_match_info_add (column_match_info, match_info);
}

static void
lw_match_info_allocate (LwMatchInfo * self)
{
    priv->length = lw_list_get_n_columns ();
    priv->columns = g_new0 (LwColumnMatchInfo*, length);
}

