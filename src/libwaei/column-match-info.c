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
 * SECTION:column_match_info
 * @short_description: Match information for a specific dictionary column
 * @title: LwColumnMatchInfo
 *
 * Used to store matches for a specific dictionary column, making it easy to iterate 
 * it and print highlighted results. See lw_column_match_info_read().
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

#include <libwaei/parenthesis-node.h>
#include <libwaei/query-node.h>
#include <libwaei/gettext.h>


/**
 * lw_column_match_info_new:
 * @column: Dictionary column that this match info tracks
 * @strv: (transfer none): The strings that this match info will reference
 * Returns: (transfer full): A new #LwColumnMatchInfo that can be freed with lw_column_match_info_unref()
 */
LwColumnMatchInfo *
lw_column_match_info_new (gint column, gchar const ** strv)
{
    //Declarations
    LwColumnMatchInfo * self = NULL;

    //Initializations
    self = g_new0 (LwColumnMatchInfo, 1);
    if (self == NULL) goto errored;

    self->column = column;
    self->strv = strv;
    self->refs = 1;

errored:

    return self;
}


/**
 * lw_column_match_info_add:
 * @self: A #LwColumnMatchInfo
 * @match_info: (transfer none): A #GMatchInfo used for creating the internal #LwMatchMarkers
 */
void
lw_column_match_info_add (LwColumnMatchInfo * self,
                                 GMatchInfo                 * match_info)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (match_info != NULL);
    if (g_match_info_get_match_count (match_info) == 0) return;

    //Declarations
    LwMatchMarker * open_marker = NULL;
    LwMatchMarker * close_marker = NULL;

    open_marker = NULL;
    close_marker = NULL;

    open_marker = lw_match_marker_new (LW_MATCH_MARKER_TYPE_OPEN, match_info);
    if (open_marker == NULL) goto errored;
    close_marker = lw_match_marker_new (LW_MATCH_MARKER_TYPE_CLOSE, match_info);
    if (close_marker == NULL) goto errored;

    self->markers = g_list_prepend (self->markers, open_marker);
    self->markers = g_list_prepend (self->markers, close_marker);

    open_marker = NULL;
    close_marker = NULL;

errored:

    if (open_marker != NULL) lw_match_marker_unref (open_marker);
    open_marker = NULL;
    if (close_marker != NULL) lw_match_marker_unref (close_marker);
    close_marker = NULL;
}


/**
 * lw_column_match_info_ref:
 * @self: A #LwColumnMatchInfo
 * 
 * Returns: Increases the reference count of the #LwColumnMatchInfo and returns it
 */
LwColumnMatchInfo*
lw_column_match_info_ref (LwColumnMatchInfo * self)
{
    g_return_val_if_fail (self != NULL, NULL);

    g_atomic_int_inc (&self->refs);

    return self;
}


static void
lw_column_match_info_free (LwColumnMatchInfo *self)
{
    if (self == NULL) return;

    g_list_free_full (self->markers, (GDestroyNotify) lw_match_marker_unref);
    
    memset(self, 0, sizeof(LwColumnMatchInfo));

    g_free (self); self = NULL;
}


/**
 * lw_column_match_info_unref:
 * @self: A #LwColumnMatchInfo
 * 
 * Decreases the references on the #LwColumnMatchInfo and frees it if it reaches 0
 */
void
lw_column_match_info_unref (LwColumnMatchInfo *self)
{
    g_return_if_fail (self != NULL);

    if (g_atomic_int_dec_and_test (&self->refs))
    {
      lw_column_match_info_free (self);
    }
}


static gint
_sort_markers (LwMatchMarker * marker1,
               LwMatchMarker * marker2)
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
_read_unmarked_section (LwColumnMatchInfo     *  self,
                        LwColumnMatchInfoIter *  iter,
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
_read_marked_section (LwColumnMatchInfo      * self,
                      LwColumnMatchInfoIter  * iter,
                      gint                            * i_out,
                      gchar const                    ** START_OUT,
                      gchar const                    ** END_OUT,
                      gboolean                        * is_match_out)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (iter != NULL, FALSE);

    //Declarations
    LwMatchMarker * marker = NULL;
    gchar const * START = NULL;
    gchar const * END = NULL;
    gboolean match_changed = FALSE;
    gint match_level = 0;
    gchar const * POSITION = NULL;
    LwMatchMarkerType type = 0;
    gint i = 0;

    //Initializations
    marker = LW_MATCH_MARKER (iter->marker->data);
    match_level = iter->match_level;
    i = iter->i;

    if (is_match_out != NULL) *is_match_out = FALSE;

    if (iter->END == NULL)
    {
      START = lw_match_marker_get_string (marker); 
    }
    else
    {
      START = iter->END;
    }

    while (iter->marker != NULL && lw_match_marker_get_string (marker) == self->strv[i] && !match_changed)
    {
      POSITION = lw_match_marker_get_position (marker, &type);
      switch (type)
      {
        case LW_MATCH_MARKER_TYPE_OPEN:
          if (POSITION > START || POSITION == self->strv[i])  match_level++;
          if (POSITION == START) iter->match_level = match_level;
          match_changed = (iter->match_level == 0 && match_level > 0 && POSITION > START);
          break;
        case LW_MATCH_MARKER_TYPE_CLOSE:
          if (POSITION > START || POSITION == self->strv[i])  match_level--;
          if (POSITION == START) iter->match_level = match_level;
          g_assert (match_level >= 0);
          match_changed = (iter->match_level > 0 && match_level == 0 && POSITION > START);
          break;
        default:
          g_assert_not_reached ();
          break;
      }

      if (!match_changed)
      {
        iter->marker = iter->marker->next;
        marker = (iter->marker != NULL) ? LW_MATCH_MARKER (iter->marker->data): NULL;
      }
      else
      {
        if (is_match_out != NULL) *is_match_out = !match_level;
      }
    }

    if (iter->marker == NULL || POSITION == NULL || lw_match_marker_get_string (marker) != self->strv[i] || *POSITION == '\0')
    {
      END = START + strlen(START);
      iter->END = NULL;
      iter->match_level = 0;
      iter->i = i + 1;
    }
    else
    {
      iter->END = END = POSITION;
      iter->match_level = match_level;
    }

    if (START_OUT != NULL) *START_OUT = START;
    if (END_OUT != NULL) *END_OUT = END;
    if (i_out != NULL) *i_out = i;

    return (self->strv[iter->i] != NULL);
}


/**
 * lw_column_match_info_read:
 * @self: a #LwQueryNodeMatchInfo
 * @iter: A #LwColumnMatchInfoIter initialized to {0}
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
 * LwColumnMatchInfoIter iter = {0};
 * gchar const * start = NULL;
 * gchar const * end = NULL;
 * gboolean is_match = FALSE;
 * gint previous_i = -1, i = 0;
 * 
 * while (lw_column_match_info_read (column_match_info, &iter, &i, &start, &end, &is_match))
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
lw_column_match_info_read (LwColumnMatchInfo      * self,
                                  LwColumnMatchInfoIter  * iter,
                                  gint                            * i_out,
                                  gchar const                    ** START_OUT,
                                  gchar const                    ** END_OUT,
                                  gboolean                        * is_match_out)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (iter != NULL, FALSE);
    if (self->strv[iter->i] == NULL) return FALSE;

    //Declarations
    gint match_level = 0;
    gchar const * START = NULL;
    gchar const * END = NULL;
    LwMatchMarker * marker = NULL;
    gboolean is_not_end = FALSE;

    //Initializations
    match_level = iter->match_level;
    if (iter->marker == NULL)
    {
      self->markers = g_list_sort (self->markers, (GCompareFunc) _sort_markers);
      iter->marker = self->markers;
    }
    marker = (iter->marker != NULL) ? LW_MATCH_MARKER (iter->marker->data) : NULL;
    
    if (marker == NULL || self->strv[iter->i] != marker->TOKEN)
    {
      is_not_end = _read_unmarked_section (self, iter, i_out, START_OUT, END_OUT, is_match_out);
    }
    else
    {
      is_not_end = _read_marked_section (self, iter, i_out, START_OUT, END_OUT, is_match_out);
    }

    return is_not_end;
}

gint
lw_column_match_info_get_column (LwColumnMatchInfo * self)
{
    g_return_val_if_fail (self != NULL, -1);
    
    return self->column;
}
