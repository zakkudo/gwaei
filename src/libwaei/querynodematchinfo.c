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


GList *
lw_querynodematchinfo_lookup_matches (LwQueryNodeMatchInfo * self,
                                      gchar const          * token)
{
   //Sanity checks
   g_return_val_if_fail (self != NULL, NULL);
   g_return_val_if_fail (token != NULL, NULL);

    return g_tree_lookup (self->tree, token);
}

GList *
lw_querynodematchinfo_get_markers (LwQueryNodeMatchInfo * self)
{
   //Sanity checks
   g_return_val_if_fail (self != NULL, NULL);

  return self->markers;
}


/**
 * lw_querynodematchinf_read:
 * @self: a #LwQueryNodeMatchInfo
 * @start_out: A #gchar pointer to write the token start
 * @end_out: A #gchar pointer to write the token end
 * @is_match_out: A pointer to a #gboolean to write if the token denotes a highlighted match section
 *
 *
 * Returns:  %TRUE until the iterator reaches the end of the tokens
 */
gboolean
lw_querynodematchinfo_read (LwQueryNodeMatchInfo      * self,
                            LwQueryNodeMatchInfoIter  * iter,
                            gchar                    ** start_out,
                            gchar                    ** end_out,
                            gboolean                  * is_match_out)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (iter != NULL, FALSE);
    if (self->markers == NULL) return FALSE;
    if (iter->marker == NULL) return FALSE;

    //Declarations
    gint match_level = 0;
    gchar * start = NULL;
    gchar * end = NULL;
    LwQueryNodeMatchMarker * marker = NULL;

    //Initializations
    match_level = iter->match_level;

    if (iter->marker == NULL)
    {
      iter->marker = self->markers;
      marker = LW_QUERYNODEMATCHMARKER (iter->marker->data);
      iter->end = NULL;
    }
    else if (*iter->end == '\0')
    {
      iter->marker = iter->marker->next;
      marker = LW_QUERYNODEMATCHMARKER (iter->marker->data);
      iter->end = NULL;
    }
    else
    {
      marker = LW_QUERYNODEMATCHMARKER (iter->marker->data);
    }

    if (iter->end == NULL)
    {
      start = (gchar*) lw_querynodematchmarker_get_string (marker); 
    }
    else
    {
      start = g_utf8_next_char (iter->end);
    }

    gboolean match_changed = FALSE;
    while (iter->marker != NULL && !match_changed)
    {
      marker = LW_QUERYNODEMATCHMARKER (iter->marker->data);
      switch (marker->type)
      {
        case LW_QUERYNODEMATCHMARKERTYPE_OPEN:
          match_level++;
          break;
        case LW_QUERYNODEMATCHMARKERTYPE_CLOSE:
          match_level--;
          break;
        default:
          g_assert_not_reached ();
          break;
      }
      match_changed = ((iter->match_level > 0 && match_level == 0) || (iter->match_level == 0 && match_level > 0));
      iter->marker = iter->marker->next;
    }

    if (iter->marker == NULL)
    {
      marker = NULL;
      end = start + strlen(start);
    }
    else
    {
      end = (gchar*) marker->position;
    }

    iter->marker = iter->marker->next;
    iter->match_level = match_level;
    iter->end = end;

    if (start_out != NULL) *start_out = start;
    if (end_out != NULL) *end_out = end;
    if (is_match_out != NULL) *is_match_out =  match_level;

    return (iter->marker->next != NULL || *iter->end != '\0');
}
