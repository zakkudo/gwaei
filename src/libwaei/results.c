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
 * SECTION:results
 * @short_description: A result list pertaining to a specific dictionary
 * @title: LwResults
 * @include: libwaei/results.h
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include <libwaei/gettext.h>
#include <libwaei/results.h>

#define lw_results_get_class(self) G_TYPE_INSTANCE_GET_CLASS(self, LW_TYPE_RESULTS, LwResultsClass)
#define lw_results_get_class_private(self) G_TYPE_CLASS_GET_PRIVATE(lw_results_get_class(self), LW_TYPE_RESULTS, LwResultsClassPrivate)
#define lw_results_class_get_private(klass) G_TYPE_CLASS_GET_PRIVATE(klass, LW_TYPE_RESULTS, LwResultsClassPrivate)

typedef enum {
    PROP_0,
    PROP_DICTIONARY_CACHE,
    PROP_DICTIONARY,
    TOTAL_PROPS
} Props;

struct _LwResults {
    LwSortable parent;
};

typedef struct {
    LwResults * results;
    GSequenceIter * sequence_iter;
    gpointer user_data2;
    gpointer user_data3;
} LwResultsIter;

typedef struct {
  GSequence * sequence;
  LwDictionaryCache * dictionary_cache;
  LwDictionary * dictionary;
} LwResultsPrivate;

typedef struct {
  GParamSpec *pspec[TOTAL_PROPS];
} LwResultsClassPrivate;

static LwDictionary * lw_results_get_dictionary (LwResults * self);
static void lw_results_set_dictionary (LwResults * self, LwDictionary * dictionary);

static void lw_results_set_dictionary_cache (LwResults * self, LwDictionaryCache * dictionary_cache);
static LwDictionaryCache * lw_results_get_dictionary_cache (LwResults * self);

static void lw_results_set_sequence (LwResults * self, GSequence * sequence);
static GSequence * lw_results_get_sequence (LwResults * self);


static gboolean lw_results_iter_is_valid (LwResultsIter * self);
static void lw_results_get_begin_iter (LwResults * self, LwResultsIter * iter);
static void lw_results_get_end_iter (LwResults * self, LwResultsIter * iter);
static gint lw_results_get_n_columns (LwResults * self);
static GType lw_results_get_column_type (LwResults * self, gint column);
static gboolean lw_results_get_iter_at_position (LwResults * self, LwResultsIter * iter, gint position);
static gint lw_results_get_length (LwResults * self);
static gint lw_results_iter_get_position (LwResultsIter * self);
static void lw_results_iter_get_value (LwResultsIter * self, gint column, GValue * value);
static gboolean lw_results_iter_next (LwResultsIter * self);
static gboolean lw_results_iter_previous (LwResultsIter * self);

static void lw_results_sort (LwResults * self, gint column, LwSortDirection direction);

G_DEFINE_TYPE_WITH_CODE (LwResults, lw_results, LW_TYPE_SORTABLE, G_ADD_PRIVATE(LwResults) g_type_add_class_private(LW_TYPE_RESULTS, sizeof(LwResultsClassPrivate)) )


/**
 * lw_results_new:
 * @dictioanry_cache: (transfer full): A #LwDictionaryCache which the results will be referencing.
 * Returns: (transfer full): A new #LwResults which should be freed with g_object_unref()
 */
LwResults *
lw_results_new (LwDictionary      * dictionary,
                LwDictionaryCache * dictionary_cache)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY_CACHE (dictionary_cache), NULL);

    //Declarations
    LwResults * self = NULL;

    //Initializations
    self = LW_RESULTS (g_object_new (LW_TYPE_RESULTS, "dictionary", dictionary, "dictionary-cache", dictionary_cache, NULL));

    return self;
}


static void
lw_results_init (LwResults * self)
{
    LwResultsPrivate *priv = NULL;

    priv = lw_results_get_instance_private (self);

    lw_results_set_sequence (self, g_sequence_new ((GDestroyNotify) lw_result_free));
}


static void 
lw_results_set_property (GObject      * object,
                         guint          property_id,
                         const GValue * value,
                         GParamSpec   * pspec)
{
    //Declarations
    LwResults *self = NULL;
    LwResultsPrivate *priv = NULL;

    //Initializations
    self = LW_RESULTS (object);
    priv = lw_results_get_instance_private (self);

    switch (property_id)
    {
      case PROP_DICTIONARY:
        lw_results_set_dictionary (self, g_value_get_object (value));
        break;
      case PROP_DICTIONARY_CACHE:
        lw_results_set_dictionary_cache (self, g_value_get_object (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_results_get_property (GObject    * object,
                         guint        property_id,
                         GValue     * value,
                         GParamSpec * pspec)
{
    //Declarations
    LwResults *self = NULL;
    LwResultsPrivate *priv = NULL;

    //Initializations
    self = LW_RESULTS (object);
    priv = lw_results_get_instance_private (self);

    switch (property_id)
    {
      case PROP_DICTIONARY:
        g_value_set_object (value, lw_results_get_dictionary (self));
        break;
      case PROP_DICTIONARY_CACHE:
        g_value_set_object (value, lw_results_get_dictionary_cache (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_results_finalize (GObject * object)
{
    //Declarations
    LwResults *self = NULL;
    LwResultsPrivate *priv = NULL;

    //Initalizations
    self = LW_RESULTS (object);
    priv = lw_results_get_instance_private (self);

    lw_results_set_sequence (self, NULL);
    lw_results_set_dictionary_cache (self, NULL);
    lw_results_set_dictionary (self, NULL);

    memset(priv, 0, sizeof(LwResultsPrivate));

    G_OBJECT_CLASS (lw_results_parent_class)->finalize (object);
}


static void
lw_results_class_init (LwResultsClass * klass)
{
    //Declarations
    GObjectClass *object_class = NULL;
    LwIterableClass * iterable_class = NULL;
    LwSortableClass * sortable_class = NULL;
    LwResultsClassPrivate * klasspriv = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    iterable_class = LW_ITERABLE_CLASS (klass);
    klasspriv = lw_results_class_get_private (klass);

    object_class->set_property = lw_results_set_property;
    object_class->get_property = lw_results_get_property;
    object_class->finalize = lw_results_finalize;

    iterable_class->get_begin_iter = (LwIterableGetBeginIterFunc) lw_results_get_begin_iter;
    iterable_class->get_end_iter = (LwIterableGetEndIterFunc) lw_results_get_end_iter;
    iterable_class->get_n_columns = (LwIterableGetNColumnsFunc) lw_results_get_n_columns;
    iterable_class->get_column_type = (LwIterableGetColumnTypeFunc) lw_results_get_column_type;
    iterable_class->get_iter_at_position = (LwIterableGetIterAtPositionFunc) lw_results_get_iter_at_position;
    iterable_class->get_length = (LwIterableGetLengthFunc) lw_results_get_length;

    iterable_class->iter_get_position = (LwIterableIterGetPositionFunc) lw_results_iter_get_position;
    iterable_class->iter_get_value = (LwIterableIterGetValueFunc) lw_results_iter_get_value;
    iterable_class->iter_next = (LwIterableIterNextFunc) lw_results_iter_next;
    iterable_class->iter_previous = (LwIterableIterPreviousFunc) lw_results_iter_previous;

    sortable_class->sort = (LwSortableSortFunc) lw_results_sort;

    /**
     * LwResults:dictionary:
     * The dictionary for column information
     */
    klasspriv->pspec[PROP_DICTIONARY] = g_param_spec_object (
        "dictionary",
        gettext("Dictionary"),
        gettext("The dictionary which the results reference"),
        LW_TYPE_DICTIONARY,
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITE
    );
    g_object_class_install_property (object_class, PROP_DICTIONARY, klasspriv->pspec[PROP_DICTIONARY]);

    /**
     * LwResults:dictionary-cache:
     * The dictionary_cache storing the #LwParsedLines that the results reference
     */
    klasspriv->pspec[PROP_DICTIONARY_CACHE] = g_param_spec_object (
        "dictionary-cache",
        gettext("Dictionary Cache"),
        gettext("The dictionary which the results reference"),
        LW_TYPE_DICTIONARY_CACHE,
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITE
    );
    g_object_class_install_property (object_class, PROP_DICTIONARY_CACHE, klasspriv->pspec[PROP_DICTIONARY_CACHE]);
}


static void
lw_results_set_dictionary (LwResults    * self,
                           LwDictionary * dictionary)
{
    //Sanity checks
    g_return_if_fail (LW_IS_RESULTS (self));

    // Declarations
    LwResultsPrivate * priv = NULL;
    LwResultsClassPrivate * klasspriv = NULL;

    // Initializations
    priv = lw_results_get_instance_private (self);
    klasspriv = lw_results_get_class_private (self);
    if (priv->dictionary == dictionary) goto errored;

    if (dictionary != NULL) g_object_ref (dictionary);
    if (priv->dictionary != NULL) g_object_unref (priv->dictionary);
    priv->dictionary = dictionary;

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_DICTIONARY_CACHE]);

errored:

    return;
}

/**
 * lw_results_get_dictionary:
 * @self: A #LwResults
 * Returns: (transfer none): The dictionary cached associated witht the #LwResults.  The #LwDictionaryCache is owned by the object and should not be freed or modified.
 */
static LwDictionary *
lw_results_get_dictionary (LwResults * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_RESULTS (self), NULL);

    // Declarations
    LwResultsPrivate * priv = NULL;

    // Initializations
    priv = lw_results_get_instance_private (self);

    return priv->dictionary;
}


static void
lw_results_set_dictionary_cache (LwResults         * self,
                                LwDictionaryCache * dictionary_cache)
{
    //Sanity checks
    g_return_if_fail (LW_IS_RESULTS (self));

    // Declarations
    LwResultsPrivate * priv = NULL;
    LwResultsClassPrivate * klasspriv = NULL;

    // Initializations
    priv = lw_results_get_instance_private (self);
    klasspriv = lw_results_get_class_private (self);
    if (priv->dictionary_cache == dictionary_cache) goto errored;

    if (dictionary_cache != NULL) g_object_ref (dictionary_cache);
    if (priv->dictionary_cache != NULL) g_object_unref (priv->dictionary_cache);
    priv->dictionary_cache = dictionary_cache;

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_DICTIONARY_CACHE]);

errored:

    return;
}

/**
 * lw_results_get_dictionary_cache:
 * @self: A #LwResults
 * Returns: (transfer none): The dictionary cached associated witht the #LwResults.  The #LwDictionaryCache is owned by the object and should not be freed or modified.
 */
static LwDictionaryCache *
lw_results_get_dictionary_cache (LwResults * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_RESULTS (self), NULL);

    LwResultsPrivate * priv = NULL;

    priv = lw_results_get_instance_private (self);

    return priv->dictionary_cache;
}


static void
lw_results_set_sequence (LwResults * self,
                         GSequence * sequence)
{
    //Sanity checks
    g_return_if_fail (LW_IS_RESULTS (self));

    //Declarations
    LwResultsPrivate * priv = NULL;

    //Initializations
    priv = lw_results_get_instance_private (self);
    if (priv->sequence == sequence) goto errored;

    if (priv->sequence != NULL) g_sequence_free (priv->sequence);
    priv->sequence = sequence;

errored:

    return;
}


/**
 * lw_results_get_sequence:
 * @self: A #LwResults
 * Returns: (transfer none): The #GSequence internally used to store the results. The sequence is owned by the object and should not be freed or modified.
 */
static GSequence *
lw_results_get_sequence (LwResults * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_RESULTS (self), NULL);

    //Declarations
    LwResultsPrivate * priv = NULL;

    //Initializations
    priv = lw_results_get_instance_private (self);

    return priv->sequence;
}


/**
 * lw_results_append_result:
 * @self: A #LwResults
 * @result: A #LwResult to append to the list
 * Returns: An iterator pointing to where the result was inserted
 */
LwResultsIter *
lw_results_append (LwResults * self,
                   LwResult  * result)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_RESULTS (self), NULL);
    g_return_val_if_fail (result != NULL, NULL);

    //Declarations
    LwResultsPrivate * priv = NULL;
    LwResultsClassPrivate * klasspriv = NULL;
    LwResultsIter * iter = NULL;

    //Initializations
    priv = lw_results_get_instance_private (self);
    klasspriv = lw_results_get_class_private (self);
    iter = g_sequence_append (priv->sequence, result);
    result->index = priv->length;
    priv->length += 1;

    lw_iterable_emit_row_inserted (self, result->index);
    lw_iterable_emit_row_changed (self, result->index);

    return iter;
}

/**
 * lw_results_compare_score_func:
 * @a: The first result being compared
 * @b: The second result being compared
 * @Returns: less than 0 if a is less than b, 0 if a equals b, greater than 0 if a is greater than b
 *
 * A compare method made to be directly compatible with g_sequence_sort() or
 * wrapped in a thin wrapper for #GtkTreeIterCompareFunc to implement #GtkTreeSortable.
 */
static gint
lw_results_compare_score_func (LwResult * a,
                               LwResult * b)
{
    return a->score - b->score;
}
                        

/**
 * lw_results_sort_by_score:
 * @self: A #LwResults
 *
 * Sorts a results list by the match score, raising a reordered signal when finished.
 */
static void
lw_results_sort_by_score (LwResults * self)
{
    //Declarations
    LwResultsPrivate * priv = NULL;
    GSequence * sequence = NULL;
    LwResultsClassPrivate * klasspriv = NULL;
    gint * new_order = NULL;

    //Initializations
    priv = lw_results_get_instance_private (self);
    sequence = priv->sequence;
    klasspriv = lw_results_get_class_private (self);

    g_sequence_sort (sequence, (GCompareDataFunc) lw_results_compare_score_func, NULL);

    new_order = lw_results_normalize_indices (self);
    lw_iterable_emit_rows_reordered (self, new_order);

errored:

    g_free (new_order);
    new_order = NULL;
}


/**
 * lw_results_compare_index_func:
 * @a: The first result being compared
 * @b: The second result being compared
 * @Returns: less than 0 if a is less than b, 0 if a equals b, greater than 0 if a is greater than b
 *
 * A compare method made to be directly compatible with g_sequence_sort() or
 * wrapped in a thin wrapper for #GtkTreeIterCompareFunc to implement #GtkTreeSortable.
 */
static gint
lw_results_compare_index_func (LwResult * a,
                               LwResult * b)
{
    return a->index - b->index;
}
                        

/**
 * lw_results_sort_by_score:
 * @self: A #LwResults
 *
 * Sorts a results list by the match score, raising a reordered signal when finished.
 */
static void
lw_results_sort_by_index (LwResults * self)
{
    //Declarations
    LwResultsPrivate * priv = NULL;
    GSequence * sequence = NULL;
    LwResultsClassPrivate * klasspriv = NULL;
    gint * new_order = NULL;

    //Initializations
    priv = lw_results_get_instance_private (self);
    sequence = priv->sequence;
    klasspriv = lw_results_get_class_private (self);

    g_sequence_sort (sequence, (GCompareDataFunc) lw_results_compare_index_func, NULL);
    new_order = lw_results_normalize_indices (self);

    lw_iterable_emit_rows_reordered (self, new_order);

errored:

    g_free (new_order);
    new_order = NULL;
}


/**
 * lw_results_compare_index_func:
 * @a: The first result being compared
 * @b: The second result being compared
 * @data: Sort data
 * @Returns: less than 0 if a is less than b, 0 if a equals b, greater than 0 if a is greater than b
 *
 * A compare method made to be directly compatible with g_sequence_sort() or
 * wrapped in a thin wrapper for #GtkTreeIterCompareFunc to implement #GtkTreeSortable.
 */
static gint
lw_results_compare_column_func (LwResult                                  * a,
                                  LwResult                                  * b,
                                  struct LwResultsCompareByColumnIdFuncData * data)
{
    //Declarations
    gchar const ** strva = NULL;
    gchar const ** strvb = NULL;
    gint i = 0;
    gint comparison = 0;
    LwParsedLine * pa = NULL, * pb = NULL;

    //Initializations

    {
      pa = lw_parsed_get_line (data->parsed, a->index);
      if (pa == NULL) goto errored;
      strva = lw_parsed_line_get_strv (pa, data->column);
      if (strva == NULL) goto errored;
    }

    {
      pb = lw_parsed_get_line (data->parsed, b->index);
      if (pb == NULL) goto errored;
      strvb = lw_parsed_line_get_strv (pb, data->column);
      if (strvb == NULL) goto errored;
    }
    
    if (data->language == data->number)
    {
      for (i = 0; comparison == 0; i++)
      {
        comparison = lw_utf8_cmpnumber0 (strva[i], strvb[i]);
        if (strva[i] == NULL || strvb[i] == NULL) break;
      }
    }
    else
    {
      for (i = 0; comparison == 0; i++)
      {
        comparison = g_strcmp0 (strva[i], strvb[i]);
        if (strva[i] == NULL || strvb[i] == NULL) break;
      }
    }

errored:

    return comparison;
}


static gint *
lw_results_normalize_indices (LwResults * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_RESULTS (self), NULL);

    //Declarations
    LwResultsPrivate * priv = NULL;
    gint * new_order = NULL;
    gint * new_order_out = NULL;
    GSequence * sequence = NULL;
    GSequenceIter * iter = NULL;
    LwResult * result = NULL;
    gint i = 0;

    //Initializations
    priv = lw_results_get_instance_private (self);
    new_order = g_new0 (gint, priv->length);
    if (new_order == NULL) goto errored;
    sequence = priv->sequence;
    if (sequence == NULL) goto errored;
    iter = g_sequence_get_begin_iter (sequence);
    if (iter == NULL) goto errored;

    while (!g_sequence_iter_is_end (iter))
    {
      result = g_sequence_get (iter);
      new_order[i] = result->index;
      result->index = i;
      i += 1;
      iter = g_sequence_iter_next (iter);
    }

    new_order_out = new_order;
    new_order = NULL;

errored:

    g_free (new_order);
    new_order = NULL;

    return new_order_out;
}

/**
 * lw_results_sort_by_column:
 * @self: A #LwResults
 * @column: The dictionary column to sort by
 *
 * Sorts a results list by the match score, raising a reordered signal when finished.
 */
static void
lw_results_sort (LwResults * self, gint column, LwSortDirection direction)
{
    //Declarations
    LwResultsPrivate * priv = NULL;
    GSequence * sequence = NULL;
    struct LwResultsCompareByColumnIdFuncData data = {0};
    GQuark language = 0;
    LwParsed * parsed = NULL;
    LwDictionaryClass * dictionary_class = NULL;
    GType dictionary_type = G_TYPE_INVALID;
    LwDictionaryCache * dictionary_cache = NULL;
    LwResultsClassPrivate * klasspriv = NULL;
    gint * new_order = NULL;

    //Initializations
    priv = lw_results_get_instance_private (self);
    sequence = priv->sequence;
    dictionary_cache = priv->dictionary_cache;
    klasspriv = lw_results_get_class_private (self);

    dictionary_type = lw_dictionary_cache_get_dictionary_type (dictionary_cache);
    if (dictionary_type == G_TYPE_INVALID || dictionary_type == G_TYPE_NONE) goto errored;
    dictionary_class = g_type_class_ref (dictionary_type);
    if (dictionary_class == NULL) goto errored;
    language = lw_dictionary_get_column_language (dictionary_class, column);
    if (language == 0) goto errored;
    parsed = lw_dictionary_cache_get_parsed (dictionary_cache);
    if (parsed == NULL) goto errored;

    data.column = column;
    data.language = language;
    data.parsed = parsed;
    data.number = g_quark_from_static_string ("number");

    g_sequence_sort (sequence, (GCompareDataFunc) lw_results_compare_column_func, &data);

    new_order = lw_results_normalize_indices (self);
    lw_iterable_emit_rows_reordered (self, new_order);

errored:

    if (dictionary_class != NULL) g_type_class_unref (dictionary_class);
    dictionary_class = NULL;

    g_free (new_order);
    new_order = NULL;

    return;
}


static gboolean
lw_results_iter_is_valid (LwResultsIter * self) {
    // Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);

    // Declarations
    LwResultsPrivate * priv = NULL;
    GSequence * sequence = NULL;
    GSequenceIter * sequence_self = NULL;

    if (!LW_IS_RESULTS (self)) {
        return FALSE;
    }

    // Initializations
    priv = lw_results_get_instance_private (self->results);
    sequence = priv->sequence;

//g_sequence_self_is_end ()
//g_sequence_self_is_begin ()

    return sequence == g_sequence_iter_get_sequence (self->sequence_iter);
}


static void
lw_results_get_begin_iter (LwResults * self,
                           LwResultsIter    * iter)
{
    // Sanity checks
    g_return_if_fail (LW_IS_RESULTS (self));
    g_return_if_fail (iter != NULL);

    // Declarations
    const LwResultsPrivate * priv = NULL;
    GSequence * sequence = NULL;
    GSequenceIter * sequence_iter = NULL;

    // Initializations
    priv = lw_results_get_instance_private (self);
    sequence = priv->sequence;
    sequence_iter =  g_sequence_get_begin_iter (sequence);

    memset(iter, 0, sizeof(LwIter));
    iter->results = self;
    iter->sequence_iter = sequence_iter;
}

static void
lw_results_get_end_iter (LwResults     * self,
                         LwResultsIter * iter)
{
    // Sanity checks
    g_return_if_fail (LW_IS_RESULTS (self));
    g_return_if_fail (iter != NULL);

    // Declarations
    const LwResultsPrivate * priv = NULL;
    GSequence * sequence = NULL;
    GSequenceIter * sequence_iter = NULL;

    // Initializations
    priv = lw_results_get_instance_private (self);
    sequence = priv->sequence;
    sequence_iter = g_sequence_get_end_iter (sequence);

    memset(iter, 0, sizeof(LwResultsIter));
    iter->results = self;
    iter->sequence_iter = sequence_iter;
}

static gint
lw_results_get_n_columns (LwResults * self)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_RESULTS (self), 0);

    // Declarations
    const LwResultsPrivate * priv = NULL;
    LwDictionaryCache * dictionary_cache = NULL;
    LwDictionary * dictionary = NULL;

    // Initializations
    priv = lw_results_get_instance_private (self);
    dictionary_cache = priv->dictionary_cache;
    dictionary = priv->dictionary;

    return lw_iterable_get_n_columns (LW_ITERABLE (dictionary));
}

static GType
lw_results_get_column_type (LwResults * self,
                            gint        column)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_RESULTS (self), G_TYPE_INVALID);
    g_return_val_if_fail (column > -1, G_TYPE_INVALID);

    // Declarations
    const LwResultsPrivate * priv = NULL;
    LwDictionaryCache * dictionary_cache = NULL;
    LwDictionary * dictionary = NULL;

    // Initializations
    priv = lw_results_get_instance_private (self);
    dictionary_cache = priv->dictionary_cache;
    dictionary = priv->dictionary;

    return lw_iterable_get_column_type (LW_ITERABLE (dictionary), column);
}

static gboolean
lw_results_get_iter_at_position (LwResults     * self,
                                 LwResultsIter * iter,
                                 gint            position)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_RESULTS (self), FALSE);
    g_return_val_if_fail (iter != NULL, FALSE);
    g_return_val_if_fail (position > -1, FALSE);

    // Declarations
    const LwResultsPrivate * priv = NULL;
    GSequence * sequence = NULL;
    GSequenceIter * sequence_iter = NULL;

    // Initializations
    priv = lw_results_get_instance_private (self);
    sequence = priv->sequence;
    sequence_iter =  g_sequence_get_iter_at_pos (sequence, position);

    memset(iter, 0, sizeof(LwResultsIter));
    iter->results = self;
    iter->sequence_iter = sequence_iter;
}

static gint
lw_results_get_length (LwResults * self)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_RESULTS (self), 0);

    // Declarations
    const LwResultsPrivate * priv = NULL;
    GSequence * sequence = NULL;

    // Initializations
    priv = lw_results_get_instance_private (self);
    sequence = priv->sequence;

    return g_sequence_get_length (sequence);
}

static gint
lw_results_iter_get_position (LwResultsIter * self)
{
    // Sanity checks
    g_return_val_if_fail (self != NULL, -1);

    return g_sequence_iter_get_position (self->sequence_iter);
}

static void
lw_results_iter_get_value (LwResultsIter * self, gint column, GValue * value)
{
    // Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (column < 0);
    g_return_if_fail (value != NULL);

    g_value_init (value, G_TYPE_POINTER);
    g_value_set_pointer (value, g_sequence_get (self->sequence_iter));
}

static gboolean
lw_results_iter_next (LwResultsIter * self)
{
    // Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);

    return !g_sequence_iter_is_end (self->sequence_iter);
}

static gboolean
lw_results_iter_previous (LwResultsIter * self)
{
    // Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);

    return !g_sequence_iter_is_begin (self->sequence_iter);
}

