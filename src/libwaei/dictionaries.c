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

//!
//! @file dictionaries.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <glib.h>
#include <glib/gstdio.h>

#include "gettext.h"
#include "dictionaries-column.h"

typedef enum {
    PROP_0,
    PROP_PREFERENCES,
    TOTAL_PROPS
} Props;

typedef struct {
  GParamSpec pspec[TOTAL_PROPS];
  LwPreferences *preferences;
} LwDictionaryListPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (LwDictionaries, lw_dictionaries, LW_TYPE_EDITABLE_LIST, g_type_add_class_private(LW_TYPE_DICTIONARIES, sizeof(LwDictionariesClassPrivate)))

LW_SEQUENCE_LIST_DEFINE_METHODS (lw_dictionaries, LwDictionaries, LW_IS_DICTIONARIES)

static LwPreferences * lw_dictionaries_get_preferences (LwDictionaries * self);
static void lw_dictionaries_set_preferences (LwDictionaries *self, LwPreferences * preferences);

static gint lw_dictionaries_get_n_columns (LwDictionaries * self);
static GType lw_dictionaries_get_column_type (LwDictionaries * self, gint column);
static void lw_dictionaries_get_value (LwIter * iter, gint column, GValue * value);

//!
//! @brief Creates a new LwDictionaries object
//! @param MAX The maximum items you want in the dictionary before old ones are deleted
//! @return An allocated LwDictionaries that will be needed to be freed by lw_dictionary_free.
//!
LwDictionaries* 
lw_dictionaries_new (LwPreferences * preferences)
{
    //Declarations
    LwDictionaries *dictionary = NULL;

    //Initializations
    dictionary = LW_DICTIONARIES (g_object_new (LW_TYPE_DICTIONARIES, "preferences", preferences, NULL));

    return dictionary;
}


static void 
lw_dictionaries_init (LwDictionaries * self)
{
}


static void 
lw_dictionaries_set_property (GObject      *object,
                                guint         property_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
    //Declarations
    LwDictionaries *self = NULL;
    LwDictionariesPrivate *priv = NULL;

    //Initializations
    self = LW_DICTIONARIES (object);
    priv = lw_dictionaries_get_instance_private (self);

    switch (property_id)
    {
      case PROP_PREFERENCES:
        lw_dictionaries_set_preferences (self, g_value_get_object (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_dictionaries_get_property (GObject      *object,
                                guint         property_id,
                                GValue       *value,
                                GParamSpec   *pspec)
{
    //Declarations
    LwDictionaries *self = NULL;
    LwDictionariesPrivate *priv = NULL;

    //Initializations
    self = LW_DICTIONARIES (object);
    priv = lw_dictionaries_get_instance_private (self);
    {
      case PROP_PREFERENCES:
        g_value_set_object (value, lw_dictionaries_get_preferences (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_dictionaries_finalize (GObject *object)
{
    //Declarations
    LwDictionaries *self = NULL;
    LwDictionariesPrivate *priv = NULL;

    //Initalizations
    self = LW_DICTIONARIES (object);
    priv = lw_dictionaries_get_instance_private (self);

    G_OBJECT_CLASS (lw_dictionaries_parent_class)->finalize (object);
}

static void
lw_dictionaries_class_init (LwDictionariesClass *klass)
{
    //Declarations
    GObjectClass *object_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    klass->priv = g_new0 (LwDictionariesClassPrivate, 1);
    object_class->set_property = lw_dictionaries_set_property;
    object_class->get_property = lw_dictionaries_get_property;
    object_class->finalize = lw_dictionaries_finalize;

    LwDictionariesClassPrivate *klasspriv = klass->priv;

    klasspriv->pspec[PROP_PREFERENCES] = g_param_spec_object (
        "preferences",
        "Preferences construct prop",
        "Set the preferences object",
        LW_TYPE_PREFERENCES,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_PREFERENCES, klasspriv->pspec[PROP_PREFERENCES]);
}


static void
lw_dictionaries_set_preferences (LwDictionaries *self,
                                 LwPreferences    *preferences)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARIES (self));

    //Declarations
    LwDictionariesPrivate *priv = NULL;
    LwDictionariesClass *klass = NULL;
    LwDictionariesClassPrivate *klasspriv = NULL;

    //Initializations
    priv = lw_dictionaries_get_instance_private (self);
    klass = LW_DICTIONARIES_CLASS (self);
    klasspriv = klass->priv;

    if (preferences != NULL)
    {
      g_object_ref (preferences);
    }

    if (priv->preferences != NULL) {
      g_object_remove_weak_pointer (G_OBJECT (priv->preferences), (gpointer*) &(priv->preferences));
      g_object_unref (priv->preferences);
      priv->preferences = NULL;
    }

    priv->preferences = preferences;

    if (priv->preferences != NULL) {
      g_object_add_weak_pointer (G_OBJECT (priv->preferences), (gpointer*) &(priv->preferences));
    }

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_PREFERENCES]);
}


static LwPreferences *
lw_dictionaries_get_preferences (LwDictionaries *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARIES (self), NULL);

    //Declarations
    LwDictionariesPrivate *priv = NULL;

    //Initializations
    priv = lw_dictionaries_get_instance_private (self);

    if (priv->preferences == NULL)
    {
      lw_dictionaries_set_preferences (self, lw_preferences_get_default ());
    }

    return priv->preferences;
}

/* TODO
void
lw_dictionaries_append (LwDictionaries * self,
                        LwDictionary   * dictionary)
{
    // Sanity checks
    g_return_if_fail (LW_IS_DICTIONARIES (self));
    g_return_if_fail (LW_IS_DICTIONARY (dictionary));

    // Declarations
    LwDictionaryPrivate * priv = NULL;
    GSequence * sequence = NULL;
    gint position = -1;

    // Initializations
    priv = lw_dictionaries_get_instance_private ();
    sequence = priv->sequence;
    position = g_sequence_get_length (sequence) + 1;

    g_sequence_append (sequence, dictionary);

    lw_editable_list_emit_inserted (LW_EDITABLE_LIST (self), position);
    lw_editable_list_emit_changed (LW_EDITABLE_LIST (self), position);
}
*/

static gint
lw_dictionaries_get_n_columns (LwDictionaries * self)
{
    return TOTAL_DICTIONARIES_COLUMNS;
}

static GType
lw_dictionaries_get_column_type (LwDictionaries * self, gint column)
{
    switch (column) {
        case LW_DICTIONARIES_COLUMN_NAME:
            return G_TYPE_STRING;
        case LW_DICTIONARIES_COLUMN_TYPE_NAME:
            return G_TYPE_STRING;
        case LW_DICTIONARIES_COLUMN_ID:
            return G_TYPE_STRING;
        case LW_DICTIONARIES_COLUMN_POINTER:
            return G_TYPE_OBJECT;
        default:
            g_assert_not_reached ();
    }
}

static void
lw_dictionaries_get_value (LwIter * iter, gint column, GValue * value)
{
    // Sanity checks
    g_return_if_fail (iter != NULL);
    g_return_if_fail (column > -1);
    g_return_if_fail (value != NULL);

    // Declarations
    LwDictionaries * dictionaries = NULL;
    GSequenceIter * iter = NULL;
    LwDictionary * dictionary = NULL;
    GType type = G_TYPE_INVALID;

    // Initializations
    dictionaries = LW_DICTIONARIES (iter->iterable);
    iter = iter->user_data1;
    dictionary = LW_DICTIONARY (g_sequence_iter_get_value (iter));
    type = lw_ditionaries_get_column_type (dictionaries, column);

    g_value_init (value, type);

    switch (column) {
        case LW_DICTIONARIES_COLUMN_NAME:
            g_value_set_static_string (value, lw_dictionary_get_name (dictionary));
        case LW_DICTIONARIES_COLUMN_TYPE_NAME:
            g_value_set_static_string (value, lw_dictionary_get_type_name (dictionary));
        case LW_DICTIONARIES_COLUMN_ID:
            g_value_set_static_string (value, lw_dictionary_get_id (dictionary));
        case LW_DICTIONARIES_COLUMN_POINTER:
            g_value_set_object (value, dictionary);
        default:
            g_assert_not_reached ();
    }
}
        
        


