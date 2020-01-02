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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <ctype.h>

#include <glib.h>
#include <glib-object.h>

#include "column-match-info.h"
#include "match-marker.h"
#include "query.h"
#include "table.h"
#include "gettext.h"

#define lw_match_info_get_class(self) G_TYPE_INSTANCE_GET_CLASS(self, LW_TYPE_MATCH_INFO, LwMatchInfoClass)
#define lw_match_info_get_class_private(self) G_TYPE_CLASS_GET_PRIVATE(lw_match_info_get_class(self), LW_TYPE_MATCH_INFO, LwMatchInfoClassPrivate)
#define lw_match_info_class_get_private(klass) G_TYPE_CLASS_GET_PRIVATE(klass, LW_TYPE_MATCH_INFO, LwMatchInfoClassPrivate)

static void lw_match_info_allocate (LwMatchInfo * self);
static void lw_match_info_set_haystack (LwMatchInfo * self, LwTable * haystack);

static void lw_match_info_get_begin_iter (LwMatchInfo * self, LwIter * iter);
static void lw_match_info_get_end_iter (LwMatchInfo * self, LwIter * iter);
static gboolean lw_match_info_get_iter_at_position (LwMatchInfo * self, LwIter * iter, gint position);
static gint lw_match_info_get_length (LwMatchInfo *self);

static gint lw_match_info_iter_get_position (LwIter * self);
static gboolean lw_match_info_iter_next (LwIter * self);
static gboolean lw_match_info_iter_previous (LwIter * self);
static gboolean lw_match_info_iter_is_end (LwIter * self);
static gboolean lw_match_info_iter_is_begin (LwIter * self);
static void lw_match_info_iter_set (LwIter * self, gpointer data);
static gpointer lw_match_info_iter_get (LwIter * self);

typedef enum
{
  PROP_0,
  PROP_HAYSTACK,
  TOTAL_PROPS
} Props;

struct _LwMatchInfo {
    LwList parent;
};

typedef struct {
  GParamSpec *pspec[TOTAL_PROPS];
} LwMatchInfoClassPrivate;

typedef struct {
    gpointer * columns;
    gint length;
    LwTable * haystack;
} LwMatchInfoPrivate;

G_DEFINE_TYPE_WITH_CODE (LwMatchInfo, lw_match_info, LW_TYPE_LIST, G_ADD_PRIVATE(LwMatchInfo))


/**
 * lw_match_info_new:
 *
 * Returns: A new #LwMatchInfo that can be freed with lw_match_info_unref()
 */
LwMatchInfo *
lw_match_info_new (LwTable * haystack)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_TABLE (haystack), NULL);

    //Declarations
    LwMatchInfo * self = NULL;

    //Initializations
    self = LW_MATCH_INFO (g_object_new (LW_TYPE_MATCH_INFO, "haystack", haystack, NULL));

    return self;
}

static void
lw_match_info_init (LwMatchInfo * self)
{
}

static void
lw_match_info_constructed (GObject * object)
{
    LwMatchInfo * self = NULL;

    self = LW_MATCH_INFO (object);

    lw_match_info_allocate (self);
}

static void
lw_match_info_finalize (GObject * object)
{
    LwMatchInfo * self = NULL;
    LwMatchInfoPrivate *priv = NULL;
    LwColumnMatchInfo * column_match_info = NULL;

    self = LW_MATCH_INFO (object);
    priv = lw_match_info_get_instance_private (self);

    gint i = 0;

    lw_list_get_begin_iter (LW_LIST (self), &iter);

    while (i < priv->length)
    {
        GType type = lw_table_get_column_value_type (priv->haystack, i);

        if (type == G_TYPE_STRV)
        {
            gchar ** strv = lw_table_get_column (priv->haystack, i);
            for (j = 0; strv[j] != NULL; j += 1)
            {
                g_list_free_full (columns[i][j], lw_match_marker_unref);
            }
            g_free(columns[i];
        }
        else
        {
            g_list_free_full (columns[i], lw_match_marker_unref);
        }


        i += 1;
    }

    g_free (columns);
    columns = NULL;

}



static void 
lw_match_info_set_property (GObject      * object,
                            guint          property_id,
                            const GValue * value,
                            GParamSpec   * pspec)
{
    //Declarations
    LwMatchInfo * self;
    LwMatchInfoPrivate * priv;

    //Initializations
    self = LW_MATCH_INFO (object);
    priv = lw_match_info_get_instance_private (self);

    switch (property_id)
    {
      case PROP_HAYSTACK:
        lw_match_info_set_haystack (self, g_value_get_object (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_match_info_get_property (GObject      *object,
                            guint         property_id,
                            GValue       *value,
                            GParamSpec   *pspec)
{
    //Declarations
    LwMatchInfo * self = NULL;
    LwMatchInfoPrivate *priv = NULL;

    //Initializations
    self = LW_MATCH_INFO (object);
    priv = lw_match_info_get_instance_private (self);

    switch (property_id)
    {
      case PROP_HAYSTACK:
        g_value_set_object (value, lw_match_info_get_haystack (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
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
    object_class->constructed = lw_match_info_constructed;

    list_class->get_begin_iter = (LwListGetBeginIterFunc) lw_match_info_get_begin_iter;
    list_class->get_end_iter = (LwListGetEndIterFunc) lw_match_info_get_end_iter;
    list_class->get_iter_at_position = (LwListGetIterAtPositionFunc) lw_match_info_get_iter_at_position;
    list_class->get_length = (LwListGetLengthFunc) lw_match_info_get_length;

    list_class->iter_get_position = (LwListIterGetPositionFunc) lw_match_info_iter_get_position;
    list_class->iter_get = (LwListIterGetFunc) lw_match_info_iter_get;
    list_class->iter_set = (LwListIterSetFunc) lw_match_info_iter_set;
    list_class->iter_next = (LwListIterNextFunc) lw_match_info_iter_next;
    list_class->iter_previous = (LwListIterPreviousFunc) lw_match_info_iter_previous;
    list_class->iter_is_begin = (LwListIterIsBeginFunc) lw_match_info_iter_is_begin;
    list_class->iter_is_end = (LwListIterIsEndFunc) lw_match_info_iter_is_end;
    list_class->allocate = (LwListAllocateFunc) lw_match_info_allocate;

    klasspriv->pspec[PROP_HAYSTACK] = g_param_spec_object (
      "haystack",
      gettext("Haystack"),
      gettext("The reference table information"),
      LW_TYPE_TABLE,
      G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_HAYSTACK, klasspriv->pspec[PROP_HAYSTACK]);
}

/**
 * lw_match_info_set_column:
 * @self: A #LwMatchInfo
 * @column_match_info: The #LwColumnMatchInfo to set or %NULL to unset it
 */
void
lw_match_info_add (LwMatchInfo     * self,
                   gint         column, 
                   gint         index, 
                   GMatchInfo * match_info)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (match_info != NULL);
    if (g_match_info_get_match_count (match_info) == 0) return;

    // Declarations
    LwMatchInfoPrivate * priv = NULL;
    GList ** markers = NULL;

    // Initializations
    priv = lw_match_info_get_instance_private (self);

    if (lw_table_get_column_value_type (priv->haystack, column) == G_TYPE_STRV && priv->columns[column] == NULL)
    {
        priv->columns[column] = g_new0 (gpointer, g_strv_length (lw_table_get_column (priv->haystack, column)));
        markers = &priv->columns[column][index];
    } else
    {
        markers = &priv->columns[column];
    }

    if (match_info != NULL)
    {
        //Declarations
        LwMatchMarker * open_marker = NULL;
        LwMatchMarker * close_marker = NULL;

        open_marker = lw_match_marker_new (LW_MATCH_MARKER_TYPE_OPEN, match_info);
        if (open_marker == NULL) goto errored;
        close_marker = lw_match_marker_new (LW_MATCH_MARKER_TYPE_CLOSE, match_info);
        if (close_marker == NULL) goto errored;

        *markers = g_list_prepend (*markers, open_marker);
        *markers = g_list_prepend (*markers, close_marker);

        open_marker = NULL;
        close_marker = NULL;
    }

errored:

    if (open_marker != NULL) lw_match_marker_unref (open_marker);
    open_marker = NULL;
    if (close_marker != NULL) lw_match_marker_unref (close_marker);
    close_marker = NULL;
}

static void
lw_match_info_set_haystack (LwMatchInfo * self,
                            LwTable     * haystack)
{
    g_return_if_fail (LW_IS_MATCH_INFO (self));
    g_return_if_fail (LW_IS_TABLE (haystack));

    g_object_ref (haystack);
}


static gboolean
lw_match_info_iter_is_valid (LwIter * iter)
{
    if (iter == NULL) return FALSE;
    if (!LW_IS_MATCH_INFO (iter->iterable)) return FALSE;

    gint length = 0;
    gint position = 0;

    length = lw_list_get_length (LW_LIST (iter->iterable));
    position = lw_iter_get_position (iter);

    if (position > length - 1) return FALSE;
    if (position < 0) return FALSE;

    return TRUE;
}

static void 
lw_match_info_get_begin_iter (LwMatchInfo * self,
                              LwIter       * iter)
{
    // Sanity checks
    g_return_if_fail (LW_IS_MATCH_INFO (self));
    g_return_if_fail (iter != NULL);

    memset(iter, 0, sizeof(LwIter));
    iter->iterable = self;
    iter->user_data1 = GINT_TO_POINTER (0);
}

static void 
lw_match_info_get_end_iter (LwMatchInfo * self, 
                                  LwIter     * iter)
{
    // Sanity checks
    g_return_if_fail (LW_IS_MATCH_INFO (self));
    g_return_if_fail (iter != NULL);

    // Declarations
    LwMatchInfoPrivate * priv = NULL;
    gint end_position = -1;

    // Initializations
    priv = lw_match_info_get_instance_private (self);
    end_position = priv->length;

    memset(iter, 0, sizeof(LwIter));
    iter->iterable = self;
    iter->user_data1 = GINT_TO_POINTER (end_position);
}

static gboolean 
lw_match_info_get_iter_at_position (LwMatchInfo * self, 
                                          LwIter       * iter, 
                                          gint           position)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_TABLE (self), FALSE);
    g_return_val_if_fail (iter != NULL, FALSE);

    // Declarations
    gint length = -1;

    // Initializations
    length = lw_match_info_get_length (self);

    if (position > length -1) {
        position = length - 1;
    }

    if (position < 0) {
        position = 0;
    }

    memset(iter, 0, sizeof(LwIter));
    iter->iterable = self;
    iter->user_data1 = GINT_TO_POINTER (position);
}


static gint 
lw_match_info_get_length (LwMatchInfo * self)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_MATCH_INFO (self), 0);

    // Declarations
    LwMatchInfoPrivate * priv = NULL;

    // Initializations
    priv = lw_match_info_get_instance_private (self);

    return priv->length;
}

static gint
lw_match_info_iter_get_position (LwIter * self)
{
    // Sanity checks
    g_return_val_if_fail (self != NULL, -1);
    g_return_val_if_fail (lw_match_info_iter_is_valid (self), -1);

    // Declarations
    gint position = -1;

    // Initializations
    position = GPOINTER_TO_INT (self->user_data1);

    return position;
}

static gboolean 
lw_match_info_iter_next (LwIter * self)
{
    // Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (lw_match_info_iter_is_valid (self), FALSE);

    // Declarations
    LwMatchInfo * match_info = NULL;
    gint position = -1;
    gint length = -1;
    
    // Initializations
    match_info = LW_MATCH_INFO (self->iterable);
    position = GPOINTER_TO_INT (self->user_data1);
    length = lw_match_info_get_length (match_info);

    position += 1;

    if (position > length - 1) {
        position = length - 1;
    } else if (position < 0) {
        position = 0;
    }

    self->user_data1 = GINT_TO_POINTER (position);
}

static gboolean 
lw_match_info_iter_previous (LwIter * self)
{
    // Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (lw_match_info_iter_is_valid (self), FALSE);

    // Declarations
    LwMatchInfo * match_info = NULL;
    gint position = -1;
    gint length = -1;
    
    // Initializations
    match_info = LW_MATCH_INFO (self->iterable);
    position = GPOINTER_TO_INT (self->user_data1);
    length = lw_match_info_get_length (match_info);

    position -= 1;

    if (position > length - 1) {
        position = length - 1;
    } else if (position < 0) {
        position = 0;
    }

    self->user_data1 = GINT_TO_POINTER (position);
}

static gboolean 
lw_match_info_iter_is_end (LwIter * self){
    gint position = 0;
    gint length = 0;

    position = lw_iter_get_position (self);
    length = lw_list_get_length (LW_LIST (self->iterable));

    return position > length - 1;
}

static gboolean 
lw_match_info_iter_is_begin (LwIter * self)
{
    gint position = 0;

    position = lw_iter_get_position (self);

    return position < 1;
}

static void 
lw_match_info_iter_set (LwIter * self, gpointer data)
{
    // Sanity checks
    g_return_if_fail (lw_match_info_iter_is_valid (self));

    // Declarations
    LwMatchInfo * match_info = NULL;
    LwMatchInfoPrivate * priv = NULL;
    gint position = 0;

    // Initializations
    priv = lw_match_info_get_instance_private (match_info);
    position = lw_iter_get_position (self);

    priv->columns[position] = data;
}

static gpointer 
lw_match_info_iter_get (LwIter * self)
{
    // Sanity checks
    g_return_val_if_fail (lw_match_info_iter_is_valid (self), NULL);

    // Declarations
    LwMatchInfo * match_info = NULL;
    LwMatchInfoPrivate * priv = NULL;
    gint position = 0;

    // Initializations
    priv = lw_match_info_get_instance_private (match_info);
    position = lw_iter_get_position (self);

    return priv->columns[position];
}

static void
lw_match_info_allocate (LwMatchInfo * self)
{
    LwMatchInfoPrivate *priv = NULL;
    gint length = 0;

    priv = lw_match_info_get_instance_private (self);
    length = lw_list_get_length (LW_LIST (self));

    if (priv->columns != NULL)
    {
        return;
    }

    priv->columns = g_new0 (gpointer*, length);
    priv->length = length;
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
                        gint                  *  i_out,
                        gchar const           ** START_OUT,
                        gchar const           ** END_OUT,
                        gboolean              *  is_match_out)
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
lw_match_info_read (LwMatchInfo * self,
                    gint column,
                    gint index,
                    LwMatchInfoReadIter * self)
{
    MAKE THIS WORK
    SHOULD RESET when self, column, index don't match what is in iter
    should valiate
    should only iterate over current string

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

    NEEDS TO CHECK IF THIS IS STRV

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
