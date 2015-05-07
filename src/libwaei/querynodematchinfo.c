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
 * run lw_querynode_match_parseline().  One of the primary purposes of this
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
LwQueryNodeMatchInfo *
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
    if (self->markers != NULL) g_list_free_full (self->markers, (GDestroyNotify) lw_querynodematchmarker_unref);

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


void
lw_querynodematchinfo_insert (LwQueryNodeMatchInfo  * self,
                              gchar const           * token,
                              GMatchInfo            * match_info)
{
    //Initializations
    g_return_if_fail (self != NULL);
    g_return_if_fail (match_info != NULL);
    g_return_if_fail (token != NULL);
    g_return_if_fail (self->compiled != FALSE);

    //Declarations
    GList * matches = NULL;

    //Initializations
    matches = g_tree_lookup (self->tree, token);
    matches = g_list_prepend (matches, match_info);

    g_tree_insert (self->tree, (gchar*) token, matches);
}


static gboolean
_compile_markers (gchar                * token,
                  GList                * matches,
                  LwQueryNodeMatchInfo * self)
{
    //Sanity checks
    g_return_val_if_fail (token != NULL, TRUE);
    g_return_val_if_fail (matches != NULL, TRUE);
    g_return_val_if_fail (self != NULL, TRUE);

    //Declarations
    GMatchInfo * match_info = NULL;
    LwQueryNodeMatchMarker * open_marker = NULL;
    LwQueryNodeMatchMarker * close_marker = NULL;
    GList * markers = NULL;
    gchar * open = NULL;
    gchar * close = NULL;
    gint start_pos = NULL;
    gint end_pos = NULL;
    GList * link = NULL;

    for (link = matches; link != NULL; link = link->next)
    {
      match_info = (GMatchInfo*) link->data;
      if (match_info != NULL) goto errored;
      g_match_info_fetch_pos (match_info, 0, &start_pos, &end_pos);
      open = token + start_pos;
      close = token + end_pos;
      open_marker = lw_querynodematchmarker_new (open, close, LW_QUERYNODEMATCHMARKERTYPE_OPEN, match_info);
      if (open_marker == NULL) goto errored;
      close_marker = lw_querynodematchmarker_new (open, close, LW_QUERYNODEMATCHMARKERTYPE_CLOSE, match_info);
      if (close_marker == NULL) goto errored;
      markers = g_list_prepend (markers, open_marker);
      markers = g_list_prepend (markers, close_marker);
    }

    match_info = NULL;
    open_marker = NULL;
    close_marker = NULL;

errored:

    if (open_marker != NULL) lw_querynodematchmarker_unref (open_marker);
    open_marker = NULL;
    if (close_marker != NULL) lw_querynodematchmarker_unref (close_marker);
    close_marker = NULL;

    return FALSE;
}


static gint
_sort_markers (LwQueryNodeMatchMarker * marker1,
               LwQueryNodeMatchMarker * marker2)
{
    if (marker1->position < marker2->position)
    {
      return -1;
    }
    else if (marker1->position > marker2->position)
    {
      return 1;
    }
    else
    {
      return 0;
    }
    
}


void
lw_querynodematchinfo_compile (LwQueryNodeMatchInfo * self)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    if (self->compiled == TRUE) return;

    self->compiled = TRUE;
    g_tree_foreach (self->tree, (GTraverseFunc) _compile_markers, self);
    if (self->compiled == FALSE) goto errored;
    self->markers = g_list_sort (self->markers, (GCompareFunc) _sort_markers);

errored:

    if (self->compiled == FALSE)
    {
      g_list_free_full (self->markers, (GDestroyNotify) lw_querynodematchmarker_unref);
    }
}


LwQueryNodeMatchMarker *
lw_querynodematchmarker_new (gchar                      * open,
                             gchar                      * close,
                             LwQueryNodeMatchMarkerType   type,
                             GMatchInfo                 * match_info)
{
    //Sanity checks
    g_return_val_if_fail (open != NULL, NULL);
    g_return_val_if_fail (close != NULL, NULL);
    g_return_val_if_fail (match_info != NULL, NULL);

    //Declarations
    LwQueryNodeMatchMarker * self = NULL;

    //Initializations
    match_info = g_match_info_ref (match_info);
    
    self = g_new0 (LwQueryNodeMatchMarker, 1);
    if (self == NULL) goto errored;

    self->match_info = match_info;
    self->open = open;
    self->close = close;

    switch (type)
    {
      case LW_QUERYNODEMATCHMARKERTYPE_OPEN:
        self->position = open;
        break;
      case LW_QUERYNODEMATCHMARKERTYPE_CLOSE:
        self->position = close;
        break;
      default:
        g_assert_not_reached ();
        break;
    }
    
errored:

    if (match_info != NULL) g_match_info_unref (match_info);
    match_info = NULL;
}

static void
lw_querynodematchmarker_free (LwQueryNodeMatchMarker * self)
{
    if (self == NULL) return;

    if (self->match_info != NULL) g_match_info_unref (self->match_info);
    memset(self, 0, sizeof(LwQueryNodeMatchMarker));
    g_free (self);
}

void
lw_querynodematchmarker_unref (LwQueryNodeMatchMarker * self)
{
    g_return_if_fail (self != NULL);

    if (g_atomic_int_dec_and_test (&self->refs))
    {
      lw_querynodematchmarker_free (self);
    }
}


LwQueryNodeMatchMarker *
lw_querynodematchmarker_ref (LwQueryNodeMatchMarker * self)
{
    g_return_val_if_fail (self != NULL, NULL);

    g_atomic_int_inc (&self->refs);

    return self;
}

