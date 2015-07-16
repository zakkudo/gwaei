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
#include <libwaei/results-private.h>


G_DEFINE_TYPE (LwResults, lw_results, G_TYPE_OBJECT)


/**
 * lw_results_new:
 * @dictioanry_cache: (transfer full): A #LwDictionaryCache which the results will be referencing.
 * Returns: (transfer full): A new #LwResults which should be freed with g_object_unref()
 */
LwResults *
lw_results_new (LwDictionaryCache * dictionary_cache)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYCACHE (dictionary_cache), NULL);

    //Declarations
    LwResults * self = NULL;

    //Initializations
    self = LW_RESULTS (g_object_new (LW_TYPE_RESULTS, "dictionary-cache", dictionary_cache, NULL));

    return self;
}


static void
lw_results_init (LwResults * self)
{
    self->priv = LW_RESULTS_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LwResultsPrivate));

    LwResultsPrivate *priv = NULL;

    priv = self->priv;

    lw_results_set_sequence (self, g_sequence_new (NULL));
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
    priv = self->priv;

    switch (property_id)
    {
      case PROP_SEQUENCE:
        lw_results_set_sequence (self, g_value_get_pointer (value));
        break;
      case PROP_DICTIONARYCACHE:
        lw_results_set_dictionarycache (self, g_value_get_object (value));
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
    priv = self->priv;

    switch (property_id)
    {
      case PROP_SEQUENCE:
        g_value_set_pointer (value, lw_results_get_sequence (self));
        break;
      case PROP_DICTIONARYCACHE:
        g_value_set_object (value, lw_results_get_dictionarycache (self));
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
    priv = self->priv;

    lw_results_set_sequence (self, NULL);
    lw_results_set_dictionarycache (self, NULL);

    memset(priv, 0, sizeof(LwResultsPrivate));

    G_OBJECT_CLASS (lw_results_parent_class)->finalize (object);
}


static void
lw_results_dispose (GObject *object)
{
    //Declarations
    LwResults *self = NULL;

    //Initializations
    self = LW_RESULTS (object);

    G_OBJECT_CLASS (lw_results_parent_class)->dispose (object);
}


static void
lw_results_class_init (LwResultsClass * klass)
{
    //Declarations
    GObjectClass *object_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    klass->priv = g_new0 (LwResultsClassPrivate, 1);
    object_class->set_property = lw_results_set_property;
    object_class->get_property = lw_results_get_property;
    object_class->dispose = lw_results_dispose;
    object_class->finalize = lw_results_finalize;

    g_type_class_add_private (object_class, sizeof (LwResultsPrivate));

    LwResultsClassPrivate *klasspriv = klass->priv;

    /**
     * LwResults::internal-row-changed:
     * @self: A #LwResults
     * @position: The index of the row that changed
     *
     * A signal handler for #GtkTreeModel compatibility
     */
    klasspriv->signalid[CLASS_SIGNALID_ROW_CHANGED] = g_signal_new (
        "internal-row-changed",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwResultsClass, row_changed),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    /**
     * LwResults::internal-row-inserted:
     * @self: A #LwResults
     * @position: The index of the row that changed
     *
     * A signal handler for #GtkTreeModel compatibility
     */
    klasspriv->signalid[CLASS_SIGNALID_ROW_INSERTED] = g_signal_new (
        "internal-row-inserted",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwResultsClass, row_inserted),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    /**
     * LwResults::internal-row-deleted:
     * @self: A #LwResults
     * @position: The index of the row that changed
     *
     * A signal handler for #GtkTreeModel compatibility
     */
    klasspriv->signalid[CLASS_SIGNALID_ROW_DELETED] = g_signal_new (
        "internal-row-deleted",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwResultsClass, row_deleted),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    /**
     * LwResults::internal-rows-reordered:
     * @self: A #LwResults
     * @position: The index of the row that changed
     *
     * A signal handler for #GtkTreeModel compatibility
     */
    klasspriv->signalid[CLASS_SIGNALID_ROWS_REORDERED] = g_signal_new (
        "internal-rows-reordered",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwResultsClass, rows_reordered),
        NULL, NULL,
        g_cclosure_marshal_VOID__POINTER,
        G_TYPE_NONE, 1,
        G_TYPE_POINTER
    );

    /**
     * LwResults:sequence:
     * The sequence of results
     */
    klasspriv->pspec[PROP_SEQUENCE] = g_param_spec_pointer (
        "sequence",
        gettext("Sequence"),
        gettext("The iterable sequence of results"),
        G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_SEQUENCE, klasspriv->pspec[PROP_SEQUENCE]);

    /**
     * LwResults:dictionary-cache:
     * The dictionarycache storing the #LwParsedLines that the results reference
     */
    klasspriv->pspec[PROP_DICTIONARYCACHE] = g_param_spec_object (
        "dictionary-cache",
        gettext("Dictionary Cache"),
        gettext("The dictionary which the results reference"),
        LW_TYPE_DICTIONARYCACHE,
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_DICTIONARYCACHE, klasspriv->pspec[PROP_DICTIONARYCACHE]);
}


static void
lw_results_set_dictionarycache (LwResults         * self,
                                LwDictionaryCache * dictionary_cache)
{
    //Sanity checks
    g_return_if_fail (LW_IS_RESULTS (self));

    LwResultsPrivate * priv = NULL;
    LwResultsClass * klass = NULL;

    priv = self->priv;
    klass = LW_RESULTS_GET_CLASS (self);
    if (priv->dictionary_cache == dictionary_cache) goto errored;

    if (dictionary_cache != NULL) g_object_ref (dictionary_cache);
    if (priv->dictionary_cache != NULL) g_object_unref (priv->dictionary_cache);
    priv->dictionary_cache = dictionary_cache;

    g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_DICTIONARYCACHE]);

errored:

    return;
}

/**
 * lw_results_get_dictionarycache:
 * @self: A #LwResults
 * Returns: (transfer none): The dictionary cached associated witht the #LwResults.  The #LwDictionaryCache is owned by the object and should not be freed or modified.
 */
LwDictionaryCache *
lw_results_get_dictionarycache (LwResults * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_RESULTS (self), NULL);

    LwResultsPrivate * priv = NULL;

    priv = self->priv;

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
    LwResultsClass * klass = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_RESULTS_GET_CLASS (self);
    if (priv->sequence == sequence) goto errored;

    if (priv->sequence != NULL) g_sequence_free (priv->sequence);
    priv->sequence = sequence;

    g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_SEQUENCE]);

errored:

    return;
}


/**
 * lw_results_get_sequence:
 * @self: A #LwResults
 * Returns: (transfer none): The #GSequence internally used to store the results. The sequence is owned by the object and should not be freed or modified.
 */
GSequence *
lw_results_get_sequence (LwResults * self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_RESULTS (self));

    //Declarations
    LwResultsPrivate * priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->sequence;
}


/**
 * lw_results_append_line:
 * @self: A #LwResults
 * @line: (transfer none): A #LwParsedLine to append
 * Returns: (transfer none): An iterator representing where the line was inserted
 */
LwResultsIter *
lw_results_append_line (LwResults    * self,
                        LwParsedLine * line)
{
  //Sanity checks
  g_return_val_if_fail (LW_IS_RESULTS (self), NULL);
  g_return_val_if_fail (line != NULL, NULL);

  //Declarations
  LwResultsPrivate * priv = NULL;
  LwResultsClass * klass = NULL;
  LwResultsIter * iter = NULL;
  gint position = 0;

  //Initializations
  priv = self->priv;
  klass = LW_RESULTS_GET_CLASS (self);
  iter = g_sequence_append (priv->sequence, line);
  position = g_sequence_iter_get_position (iter);

  g_signal_emit (G_OBJECT (self), klass->priv->signalid[CLASS_SIGNALID_ROW_INSERTED], 0, position);

  return iter;
}

