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
 * SECTION:querynodecolumnmatchinfo
 * @short_description: Match information for a specific dictionary column
 * @title: LwQueryNodeColumnMatchInfo
 *
 * Used to store matches for a specific dictionary column, making it easy to iterate 
 * it and print highlighted results. See lw_querynodecolumnmatchinfo_read().
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


/**
 * lw_querynodecolumnmatchinfo_new:
 * @column: Dictionary column that this match info tracks
 * @strv: (transfer none): The strings that this match info will reference
 * Returns: (transfer full): A new #LwQueryNodeColumnMatchInfo that can be freed with lw_querynodecolumnmatchinfo_unref()
 */
LwQueryNodeColumnMatchInfo *
lw_querynodecolumnmatchinfo_new (gint column, gchar const ** strv)
{
    //Declarations
    LwQueryNodeColumnMatchInfo * self = NULL;

    //Initializations
    self = g_new0 (LwQueryNodeColumnMatchInfo, 1);
    if (self == NULL) goto errored;

    self->column = column;
    self->strv = strv;
    self->refs = 1;

errored:

    return self;
}


/**
 * lw_querynodecolumnmatchinfo_add:
 * @self: A #LwQueryNodeColumnMatchInfo
 * @match_info: (transfer none): A #GMatchInfo used for creating the internal #LwQueryNodeMatchMarkers
 */
void
lw_querynodecolumnmatchinfo_add (LwQueryNodeColumnMatchInfo * self,
                                 GMatchInfo                 * match_info)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (match_info != NULL);

    //Declarations
    LwQueryNodeMatchMarker * open_marker = NULL;
    LwQueryNodeMatchMarker * close_marker = NULL;
    GList * markers = NULL;

    open_marker = NULL;
    close_marker = NULL;

    open_marker = lw_querynodematchmarker_new (LW_QUERYNODEMATCHMARKERTYPE_OPEN, match_info);
    if (open_marker == NULL) goto errored;
    close_marker = lw_querynodematchmarker_new (LW_QUERYNODEMATCHMARKERTYPE_CLOSE, match_info);
    if (close_marker == NULL) goto errored;

    markers = g_list_prepend (markers, open_marker);
    markers = g_list_prepend (markers, close_marker);

    open_marker = NULL;
    close_marker = NULL;

errored:

    if (open_marker != NULL) lw_querynodematchmarker_unref (open_marker);
    open_marker = NULL;
    if (close_marker != NULL) lw_querynodematchmarker_unref (close_marker);
    close_marker = NULL;
}


/**
 * lw_querynodecolumnmatchinfo_ref:
 * @self: A #LwQueryNodeColumnMatchInfo
 * 
 * Returns: Increases the reference count of the #LwQueryNodeColumnMatchInfo and returns it
 */
LwQueryNodeColumnMatchInfo*
lw_querynodecolumnmatchinfo_ref (LwQueryNodeColumnMatchInfo * self)
{
    g_return_val_if_fail (self != NULL, NULL);

    g_atomic_int_inc (&self->refs);

    return self;
}


static void
lw_querynodecolumnmatchinfo_free (LwQueryNodeColumnMatchInfo *self)
{
    if (self == NULL) return;

    g_list_free_full (self->markers, (GDestroyNotify) lw_querynodematchmarker_unref);
    
    memset(self, 0, sizeof(LwQueryNodeColumnMatchInfo));

    g_free (self); self = NULL;
}


/**
 * lw_querynodecolumnmatchinfo_unref:
 * @self: A #LwQueryNodeColumnMatchInfo
 * 
 * Decreases the references on the #LwQueryNodeColumnMatchInfo and frees it if it reaches 0
 */
void
lw_querynodecolumnmatchinfo_unref (LwQueryNodeColumnMatchInfo *self)
{
    g_return_if_fail (self != NULL);

    if (g_atomic_int_dec_and_test (&self->refs))
    {
      lw_querynodecolumnmatchinfo_free (self);
    }
}


static gint
_sort_markers (LwQueryNodeMatchMarker * marker1,
               LwQueryNodeMatchMarker * marker2)
{
    if (marker1->POSITION < marker2->POSITION)
    {
      return -1;
    }
    else if (marker1->POSITION > marker2->POSITION)
    {
      return 1;
    }
    else
    {
      return 0;
    }
    
}


static gboolean
_read_unmarked_section (LwQueryNodeColumnMatchInfo     *  self,
                        LwQueryNodeColumnMatchInfoIter *  iter,
                        gint                           *  i_out,
                        gchar const                    ** START_OUT,
                        gchar const                    ** END_OUT,
                        gboolean                       *  is_match_out)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (iter != NULL, FALSE);

    if (i_out != NULL) *i_out = iter->i;
    if (START_OUT != NULL) *START_OUT = self->strv[iter->i];
    if (END_OUT != NULL) *END_OUT = *START_OUT + strlen(*START_OUT);
    if (is_match_out != NULL) *is_match_out = FALSE;

    iter->i++;
    iter->END = NULL;
    iter->match_level = 0;
  

    return (self->strv[iter->i] != NULL);
}


static gboolean
_read_marked_section (LwQueryNodeColumnMatchInfo      * self,
                      LwQueryNodeColumnMatchInfoIter  * iter,
                      gint                            * i_out,
                      gchar const                    ** START_OUT,
                      gchar const                    ** END_OUT,
                      gboolean                        * is_match_out)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (iter != NULL, FALSE);

    //Declarations
    LwQueryNodeMatchMarker * marker = NULL;
    gchar const * START = NULL;
    gchar const * END = NULL;
    gboolean match_changed = FALSE;
    gint match_level = 0;
    gchar const * POSITION = NULL;
    LwQueryNodeMatchMarkerType type = 0;

    //Initializations
    marker = LW_QUERYNODEMATCHMARKER (iter->marker->data);
    match_level = iter->match_level;

    if (is_match_out != NULL)
    {
      *is_match_out = (iter->match_level > 0);
    }

    if (iter->END == NULL)
    {
      START = lw_querynodematchmarker_get_string (marker); 
    }
    else
    {
      START = g_utf8_next_char (iter->END);
    }

    while (iter->marker != NULL && lw_querynodematchmarker_get_string (marker) == self->strv[iter->i] && !match_changed)
    {
      POSITION = lw_querynodematchmarker_get_position (marker, &type);
      switch (type)
      {
        case LW_QUERYNODEMATCHMARKERTYPE_OPEN:
          match_level++;
          match_changed = (iter->match_level == 0 && match_level > 0 && POSITION > self->strv[iter->i]);
          break;
        case LW_QUERYNODEMATCHMARKERTYPE_CLOSE:
          match_level--;
          g_assert (match_level >= 0);
          match_changed = (iter->match_level > 0 && match_level == 0 && POSITION > self->strv[iter->i]);
          break;
        default:
          g_assert_not_reached ();
          break;
      }

      if (!match_changed)
      {
        iter->marker = iter->marker->next;
        marker = (iter->marker != NULL) ? LW_QUERYNODEMATCHMARKER (iter->marker->data): NULL;
      }
    }

    if (iter->marker == NULL || POSITION == NULL || lw_querynodematchmarker_get_string (marker) != self->strv[iter->i])
    {
      END = START + strlen(START);
      iter->END = NULL;
      iter->match_level = 0;
      iter->i++;
    }
    else
    {
      iter->END = END = POSITION;
      iter->match_level = match_level;
    }

    if (START_OUT != NULL) *START_OUT = START;
    if (END_OUT != NULL) *END_OUT = END;
    if (i_out != NULL) *i_out = iter->i;

    return (self->strv[iter->i] != NULL || iter->END == NULL || *iter->END != '\0');
}


/**
 * lw_querynodecolumnmatchinfo_read:
 * @self: a #LwQueryNodeMatchInfo
 * @iter: A #LwQueryNodeColumnMatchInfoIter initialized to {0}
 * @i_out: (out): An index to set or %NULL
 * @START_OUT: (out) (transfer none): A #gchar pointer to write the token start or %NULL.  This string should not be freed or modified.
 * @END_OUT: (out) (transfer none): A #gchar pointer to write the token END or %NULL. This string should not be freed or modified.
 * @is_match_out: (out): A pointer to a #gboolean to write if the token denotes a highlighted match section or %NULL
 *
 * Reads substrings iteratively from the match information such that you can easily print
 * them with highlight information.
 * 
 * Example:
 * |[<!-- language="C" -->
 * LwQueryNodeColumnMatchInfoIter iter = {0};
 * gchar const * start = NULL;
 * gchar const * end = NULL;
 * gboolean is_match = FALSE;
 * gint previous_i = -1, i = 0;
 * 
 * while (lw_querynodecolumnmatchinfo_read (column_match_info, &iter, &i, &start, &end, &is_match))
 * {
 *     if (previous_i != i)
 *     {
 *         printf("%d) ", i);
 *         previous_i = i;
 *     }
 *     if (is_match)
 *     {
 *         printf("[%.s]", end - start, start)
 *     }
 *     else
 *     {
 *         printf("%.s", end - start, start)
 *     }
 * }
 * ]|
 *
 * Returns:  %TRUE until there are no more tokens to iterate
 */
gboolean
lw_querynodecolumnmatchinfo_read (LwQueryNodeColumnMatchInfo      * self,
                                  LwQueryNodeColumnMatchInfoIter  * iter,
                                  gint                            * i_out,
                                  gchar const                    ** START_OUT,
                                  gchar const                    ** END_OUT,
                                  gboolean                        * is_match_out)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (iter != NULL, FALSE);
    if (self->markers == NULL) return FALSE;
    if (self->strv[iter->i] == NULL) return FALSE;

    //Declarations
    gint match_level = 0;
    gchar const * START = NULL;
    gchar const * END = NULL;
    LwQueryNodeMatchMarker * marker = NULL;
    gboolean is_not_end = FALSE;

    //Initializations
    match_level = iter->match_level;
    if (iter->marker == NULL)
    {
      self->markers = g_list_sort (self->markers, (GCompareFunc) _sort_markers);
      iter->marker = self->markers;
    }
    marker = LW_QUERYNODEMATCHMARKER (iter->marker->data);
    
    if (self->strv[iter->i] != marker->TOKEN)
    {
      is_not_end = _read_unmarked_section (self, iter, i_out, START_OUT, END_OUT, is_match_out);
    }
    else
    {
      is_not_end = _read_marked_section (self, iter, i_out, START_OUT, END_OUT, is_match_out);
    }

    return is_not_end;
}
