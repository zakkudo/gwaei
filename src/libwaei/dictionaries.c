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

#include <libwaei/gettext.h>

typedef enum {
    PROP_0,
    PROP_PREFERENCES,
    TOTAL_PROPS
} Props;


typedef enum {
  SIGNALID_CHANGED,
  SIGNALID_INSERTED,
  SIGNALID_DELETED,
  SIGNALID_REORDERED,
  TOTAL_SIGNALIDS
} SignalId;

typedef enum {
  CLASS_SIGNALID_ROW_CHANGED,
  CLASS_SIGNALID_ROW_INSERTED,
  CLASS_SIGNALID_ROW_DELETED,
  CLASS_SIGNALID_ROWS_REORDERED,
  TOTAL_CLASS_SIGNALIDS
} ClassSignalId;

struct _Index {
  GHashTable *typename;
  GHashTable *filename;
  GHashTable *id;
};

struct _Data {
  struct _Index index;
  GList *list; //<!-- A Glist of LwWord
  GList **array;
  gint length;
  gboolean changed; //<!-- hint of there are any savable changes
  gboolean loaded;
  GMenuModel *menu_model;
  guint signalid[TOTAL_SIGNALIDS];
};

struct _Config {
  LwPreferences *preferences;
};

struct _LwDictionaryListPrivate {
  struct _Data data;
  struct _Config config;
};

struct _LwDictionaryListClassPrivate {


G_DEFINE_TYPE_WITH_PRIVATE (LwDictionaries, lw_dictionaries, LW_TYPE_ITERABLE)


//!
//! @brief Creates a new LwDictionaries object
//! @param MAX The maximum items you want in the dictionary before old ones are deleted
//! @return An allocated LwDictionaries that will be needed to be freed by lw_dictionary_free.
//!
LwDictionaries* 
lw_dictionaries_new (LwPreferences *preferences)
{
    //Declarations
    LwDictionaries *dictionary = NULL;

    //Initializations
    dictionary = LW_DICTIONARIES (g_object_new (LW_TYPE_DICTIONARIES, "preferences", preferences, NULL));

    return dictionary;
}


static void 
lw_dictionaries_init (LwDictionaries *self)
{
    self->priv = LW_DICTIONARIES_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LwDictionariesPrivate));

    LwDictionariesPrivate *priv = NULL;

    priv = lw_dictionaries_get_instance_private (self);

    priv->data.index.typename = g_hash_table_new_full (g_str_hash, g_str_equal, (GDestroyNotify) g_free, NULL);
    priv->data.index.filename = g_hash_table_new_full (g_str_hash, g_str_equal, (GDestroyNotify) g_free, NULL);
    priv->data.index.id = g_hash_table_new_full (g_str_hash, g_str_equal, (GDestroyNotify) g_free, NULL);

    lw_dictionaries_connect_signals (self);
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

    switch (property_id)
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

    lw_dictionaries_set_menumodel (self, NULL);

    G_OBJECT_CLASS (lw_dictionaries_parent_class)->finalize (object);
}


static void
lw_dictionaries_dispose (GObject *object)
{
    //Declarations
    LwDictionaries *self = NULL;

    //Initializations
    self = LW_DICTIONARIES (object);

    lw_dictionaries_clear (self);
    lw_dictionaries_disconnect_signals (self);

    G_OBJECT_CLASS (lw_dictionaries_parent_class)->dispose (object);
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
    object_class->dispose = lw_dictionaries_dispose;
    object_class->finalize = lw_dictionaries_finalize;

    LwDictionariesClassPrivate *klasspriv = klass->priv;

    klasspriv->signalid[CLASS_SIGNALID_ROW_CHANGED] = g_signal_new (
        "internal-row-changed",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwDictionariesClass, row_changed),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    klasspriv->signalid[CLASS_SIGNALID_ROW_INSERTED] = g_signal_new (
        "internal-row-inserted",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwDictionariesClass, row_inserted),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    klasspriv->signalid[CLASS_SIGNALID_ROW_DELETED] = g_signal_new (
        "internal-row-deleted",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwDictionariesClass, row_deleted),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    klasspriv->signalid[CLASS_SIGNALID_ROWS_REORDERED] = g_signal_new (
        "internal-rows-reordered",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwDictionariesClass, rows_reordered),
        NULL, NULL,
        g_cclosure_marshal_VOID__POINTER,
        G_TYPE_NONE, 1,
        G_TYPE_POINTER
    );

    klasspriv->pspec[PROP_PREFERENCES] = g_param_spec_object (
        "preferences",
        "Preferences construct prop",
        "Set the preferences object",
        LW_TYPE_PREFERENCES,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_PREFERENCES, klasspriv->pspec[PROP_PREFERENCES]);
}


void
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

    if (priv->config.preferences != NULL) {
      g_object_remove_weak_pointer (G_OBJECT (priv->config.preferences), (gpointer*) &(priv->config.preferences));
      g_object_unref (priv->config.preferences);
      priv->config.preferences = NULL;
    }

    priv->config.preferences = preferences;

    if (priv->config.preferences != NULL) {
      g_object_add_weak_pointer (G_OBJECT (priv->config.preferences), (gpointer*) &(priv->config.preferences));
    }

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_PREFERENCES]);
}


LwPreferences*
lw_dictionaries_get_preferences (LwDictionaries *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARIES (self), NULL);

    //Declarations
    LwDictionariesPrivate *priv = NULL;

    //Initializations
    priv = lw_dictionaries_get_instance_private (self);

    if (priv->config.preferences == NULL)
    {
      lw_dictionaries_set_preferences (self, lw_preferences_get_default ());
    }

    return priv->config.preferences;
}


void
lw_dictionaries_clear (LwDictionaries *self)
{
    //Sanity checks
    g_return_if_fail (LW_DICTIONARIES (self));

    //Declarations
    LwDictionariesPrivate *priv = NULL;
    GList *removed = NULL;
    gint length = 0;

    //Initializations
    priv = lw_dictionaries_get_instance_private (self);
    length = lw_dictionaries_length (self);

    removed = lw_dictionaries_remove (self, NULL);

errored:

    if (removed != NULL) g_list_free_full (removed, (GDestroyNotify) g_object_unref);
}


void
lw_dictionaries_invalidate_length (LwDictionaries *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARIES (self));

    //Declarations
    LwDictionariesPrivate *priv = NULL;
    gint length = 0;

    //Initializations
    priv = lw_dictionaries_get_instance_private (self);

    priv->data.length = -1;
}


static void
_add_to_index (LwDictionaries *self,
               LwDictionary     *dictionary)
{
printf("BREAK1 _add_to_index\n");
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARIES (self));
    g_return_if_fail (LW_IS_DICTIONARY (dictionary));
    
    //Declarations
    LwDictionariesPrivate *priv = NULL;
    gchar *id = NULL;
    gchar *typename = NULL;
    gchar *filename = NULL;

    //Initializations
    priv = lw_dictionaries_get_instance_private (self);
    id = g_utf8_casefold (lw_dictionary_get_id (dictionary), -1);
    typename = g_utf8_casefold (G_OBJECT_TYPE_NAME (dictionary), -1);
    filename = g_utf8_casefold (lw_dictionary_get_contents_filename (dictionary), -1);
printf("BREAK2 _add_to_index filename: %s\n", filename);
   
    if (typename != NULL && !g_hash_table_contains (priv->data.index.typename, typename))
    {
printf("BREAK3 _add_to_index typename: %s\n", typename);
      g_hash_table_insert (priv->data.index.typename, typename, dictionary);
      typename = NULL;
    }

    if (filename != NULL && !g_hash_table_contains (priv->data.index.filename, filename))
    {
printf("BREAK4 _add_to_index filename: %s\n", filename);
      g_hash_table_insert (priv->data.index.filename, filename, dictionary);
      filename = NULL;
    }

    if (id != NULL && !g_hash_table_contains (priv->data.index.id, id))
    {
printf("BREAK5 _add_to_index id: %s\n", id);
      g_hash_table_insert (priv->data.index.id, id, dictionary);
      id = NULL;
    }

errored:

    g_free (id); id = NULL;
    g_free (typename); typename = NULL;
    g_free (filename); filename = NULL;

    return;
}


static void
_remove_from_index (LwDictionaries *self,
                    LwDictionary     *dictionary)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARIES (self));
    g_return_if_fail (LW_IS_DICTIONARY (dictionary));
    
    //Declarations
    LwDictionariesPrivate *priv = NULL;
    gchar *id = NULL;
    gchar *typename = NULL;
    gchar *filename = NULL;

    //Initializations
    priv = lw_dictionaries_get_instance_private (self);
    id = g_utf8_casefold (lw_dictionary_get_id (dictionary), -1);
    typename = g_utf8_casefold (G_OBJECT_TYPE_NAME (dictionary), -1);
    filename = g_utf8_casefold (lw_dictionary_get_contents_filename (dictionary), -1);
   
    if (g_hash_table_lookup (priv->data.index.typename, typename) == dictionary)
    {
      g_hash_table_remove (priv->data.index.typename, typename);
    }

    if (g_hash_table_lookup (priv->data.index.filename, filename) == dictionary)
    {
      g_hash_table_remove (priv->data.index.filename, filename);
    }

    if (g_hash_table_lookup (priv->data.index.id, id) == dictionary)
    {
      g_hash_table_remove (priv->data.index.id, id);
    }

errored:

    if (id != NULL) g_free (id); id = NULL;
    if (typename != NULL) g_free (typename); typename = NULL;
    if (filename != NULL) g_free (filename); filename = NULL;

    return;
}


static void
_rebuild_array (LwDictionaries *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARIES (self));

    lw_dictionaries_invalidate_length (self);

    //Declarations
    LwDictionariesPrivate *priv = NULL;
    gint length = 0;

    //Initializations
    priv = lw_dictionaries_get_instance_private (self);
    length = lw_dictionaries_length (self);

    g_free (priv->data.array); priv->data.array = NULL;
    priv->data.array = g_new0 (GList*, length);

    {
      GList *link = NULL;
      gint i = 0;
      for (link = priv->data.list; link != NULL; link = link->next)
      {
        LwDictionary *dictionary = LW_DICTIONARY (link->data);
        priv->data.array[i] = link;
        dictionary->row.current_index = i;
        i++;
      }
    }

    lw_dictionaries_invalidate_length (self);
}


static gint
_insert (LwDictionaries *self,
         gint             *position,
         GList            *dictionaries)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARIES (self), -1);
    if (dictionaries == NULL) return 0;

    //Declarations
    LwDictionariesPrivate *priv = NULL;
    gint number_inserted = 0;
    gint length = 0;
    gboolean append = FALSE;

    //Initializations
    priv = lw_dictionaries_get_instance_private (self);
    length = lw_dictionaries_length (self);
    append = (*position < 0 || *position >= length || length == 0);
    number_inserted = g_list_length (dictionaries);

    if (append)
    {
      GList *copy = g_list_copy (dictionaries);
      *position = length;
      g_list_foreach (copy, (GFunc) g_object_ref, NULL);
      priv->data.list = g_list_concat (priv->data.list, copy);

      {
        GList *link = NULL;
        for (link = copy; link != NULL; link = link->next)
        {
          LwDictionary *dictionary = LW_DICTIONARY (link->data);
          if (dictionary != NULL)
          {
            _add_to_index (self, dictionary);
          }
        }
      }
    }
    else
    {
      GList *insert_link = g_list_nth (priv->data.list, *position);
      if (insert_link == NULL) goto errored;
      GList *link = NULL;
      for (link = g_list_last (dictionaries); link != NULL; link = link->prev)
      {
        LwDictionary *dictionary = LW_DICTIONARY (link->data);
        if (dictionary != NULL)
        {
          g_object_ref (dictionary);
          priv->data.list = g_list_insert_before (priv->data.list, insert_link, dictionary);
          insert_link = insert_link->prev;
          _add_to_index (self, dictionary);
        }
        else
        {
          number_inserted--;
        }
      }
    }

errored:

    lw_dictionaries_invalidate_length (self);

    return number_inserted;
}


static void
_insert_propogate_changes (LwDictionaries *self,
                           gint              position,
                           gint              number_inserted)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARIES (self));

    //Declarations
    LwDictionariesPrivate *priv = NULL;
    LwDictionariesClass *klass = NULL;
    LwDictionariesClassPrivate *klasspriv = NULL;
    gint length = 0;
    gint i = 0;

    //Initializations
    priv = lw_dictionaries_get_instance_private (self);
    klass = LW_DICTIONARIES_CLASS (self);
    klasspriv = klass->priv;
    length = lw_dictionaries_length (self);

    //Rows that were inserted
    for (i = position; i < position + number_inserted; i++)
    {
      g_signal_emit (G_OBJECT (self), klasspriv->signalid[CLASS_SIGNALID_ROW_INSERTED], 0, i);
    }

    //Rows with modified indexes
    for (i = position + number_inserted; i < length; i++)
    {
      g_signal_emit (G_OBJECT (self), klasspriv->signalid[CLASS_SIGNALID_ROW_CHANGED], 0, i);
    }
}


static gint*
_find_duplicates (LwDictionaries *self, 
                  gint              position, 
                  GList            *dictionaries)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARIES (self), NULL);
    if (dictionaries == NULL) return NULL;

    //Declarations
    gint length = -1;
    gint *indices = NULL;

    //Initializations
    length = g_list_length (dictionaries);
    if (position < 0) position = lw_dictionaries_length (self);
    indices = g_new0 (gint, length + 1);
    if (indices == NULL) goto errored;

    {
      GList *link = NULL;
      gint i = 0;
      for (link = dictionaries; link != NULL; link = link->next)
      {
        LwDictionary *a = LW_DICTIONARY (link->data);
        if (a == NULL) continue;
        LwDictionary *b = LW_DICTIONARY (lw_dictionaries_nth (self, a->row.current_index));
        if (b == NULL) continue;

        if (a == b)
        {
          gint index = a->row.current_index;
          if (index >= position)
          {
            index += length;
          }
          indices[i] = index;
          i++;
        }
      }
      indices[i] = -1;
    }

errored:

    return indices;
}


gint*
lw_dictionaries_insert (LwDictionaries *self, 
                          gint              position,
                          GList            *dictionaries)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARIES (self), NULL);
    if (dictionaries == NULL) return NULL;

    //Declarations
    gint number_inserted = 0;
    gint *duplicates = NULL;
    GList *removed = NULL;
    gboolean changed = FALSE;

    //Initializations
    duplicates = _find_duplicates (self, position, dictionaries);
    number_inserted = _insert (self, &position, dictionaries);

    _rebuild_array (self);
    _insert_propogate_changes (self, position, number_inserted);

    changed = TRUE;

    if (duplicates == NULL) goto errored;

    removed = lw_dictionaries_remove (self, duplicates);

errored:

    if (duplicates != NULL) g_free (duplicates); duplicates = NULL;
    if (removed != NULL) g_list_free (removed); removed;

    return lw_dictionaries_get_indices (self, dictionaries);
}


static gint
_remove_sort (gconstpointer a, gconstpointer b)
{
    gint ia = *((gint*) a);
    gint ib = *((gint*) b);

    if (ia > ib) return -1;
    if (ia < ib) return 1;

    return 0;
}


static gint* 
_sanitize_indices (LwDictionaries *self,
                   gint             *indices)
{
    g_return_val_if_fail (LW_IS_DICTIONARIES (self), NULL);

    //Declarations
    gint size = 0;
    gint length = 0;

    //Initializations
    length = lw_dictionaries_length (self);

    if (indices == NULL) //New
    {
      indices = g_new0 (gint, length + 1);
      {
        gint i = 0;
        for (i = 0; i < length; i++)
        {
          indices[i] = i;
        }
        indices[i] = -1;
      }
    }
    else { //Copy
      gint *copy = NULL;
      gint i = 0;
      while (indices[i] > -1 && indices[i] < length && i < length) i++;
      size = i;
      copy = g_new0 (gint, size + 1);
      memcpy(copy, indices, sizeof(gint) * (size));
      copy[size] = -1;
      indices = copy;
    }

    //Sort
    qsort(indices, size, sizeof(gint), _remove_sort);
  
    //Remove duplicates
    {
      gint i = 0, j = 1;
      while (indices[i] != -1 && indices[j] != -1)
      {
        if (indices[i] != indices[j])
        {
          i++;
          indices[i] = indices[j];
        }
        j++;
      }
      i++;
      indices[i] = -1;
    }

    return indices;
}


static GList*
_remove (LwDictionaries *self,
         gint             *indices)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARIES (self), NULL);
    if (indices == NULL) return NULL;

    //Declarations
    LwDictionariesPrivate *priv = NULL;
    gint length = 0;
    GList *list = NULL;

    //Initializations
    priv = lw_dictionaries_get_instance_private (self);
    length = lw_dictionaries_length (self);
    if (length == 0) goto errored;

    {
      gint i = 0;
      for (i = 0; indices[i] != -1 && i < length; i++)
      {
        gint index = indices[i];
        GList *link = priv->data.array[index];
        if (link != NULL)
        {
          LwDictionary *dictionary = LW_DICTIONARY (link->data);
          priv->data.list = g_list_remove_link (priv->data.list, link);
          list = g_list_concat (link, list);
          priv->data.array[index] = NULL;

          if (dictionary != NULL && dictionary->row.current_index == index)
          {
            _remove_from_index (self, dictionary);
          }
        }
      }
      list = g_list_reverse (list);
    }

errored:

    lw_dictionaries_invalidate_length (self);

    return list;
}


static void
_remove_propogate_changes (LwDictionaries *self,
                           gint             *indices)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARIES (self));
    if (indices == NULL) return;

    //Declarations
    gint length = 0;
    LwDictionariesClass *klass = NULL;
    LwDictionariesClassPrivate *klasspriv = NULL;

    //Initializations
    length = lw_dictionaries_length (self);
    klass = LW_DICTIONARIES_CLASS (self);
    klasspriv = klass->priv;

    //Rows that were removed
    {
      gint i = 0;
      for (i = 0; indices[i] > -1; i++) ;
      i--;
      while (i > -1) 
      {
        g_signal_emit (G_OBJECT (self), klasspriv->signalid[CLASS_SIGNALID_ROW_DELETED], 0, indices[i]);
        i--;
      }
    }

    //Rows with modified indexes
    {
      gint index = 0;
      for (index = indices[0]; index > -1 && index < length; index++)
      {
        g_signal_emit (G_OBJECT (self), klasspriv->signalid[CLASS_SIGNALID_ROW_CHANGED], 0, index);
      }
    }
}


GList*
lw_dictionaries_remove (LwDictionaries *self,
                          gint             *indices)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARIES (self), NULL);

    //Declarations
    GList *removed = NULL;
    gboolean changed = FALSE;

    //Initializations
    indices = _sanitize_indices (self, indices);
    if (indices == NULL) goto errored;

    removed = _remove (self, indices);
    _rebuild_array (self);
    _remove_propogate_changes (self, indices);

    changed = TRUE;

errored:

    if (indices != NULL) g_free (indices); indices = NULL;

    return removed;
}


//!
//! @brief Gets the dictionary by load position in the GUI
//! @param request The GUI load position of the desired dictionary
//! @return The position in the LwDictionaries of the LwDictionary
//!
LwDictionary* 
lw_dictionaries_nth (LwDictionaries *self,
                       gint              index)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    //Declarations
    LwDictionariesPrivate *priv = NULL;
    LwDictionary *dictionary = NULL;
    GList *link = NULL;
    gint length = 0;

    //Initializiations
    priv = lw_dictionaries_get_instance_private (self);
    length = lw_dictionaries_length (self);
    if (index >= length) goto errored;

    link = priv->data.array[index];
    if (link == NULL) goto errored;
    dictionary = LW_DICTIONARY (link->data);

errored:

    return dictionary;
}




gint 
lw_dictionaries_length (LwDictionaries *self)
{
    //Sanity check
    g_return_val_if_fail (self != NULL, 0);

    //Declarations
    LwDictionariesPrivate *priv = NULL;

    //Initializations
    priv = lw_dictionaries_get_instance_private (self);

    if (priv->data.length < 0)
    {
      priv->data.length = g_list_length (priv->data.list);
    }

    return priv->data.length;
}


void 
lw_dictionaries_load_installed (LwDictionaries   *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARIES (self));

    //Declarations
    GList * dictionaries = NULL;
    gchar const * INSTALL_DIRECTORY = NULL;
    GDir * installdirectory = NULL;
    gchar const * TYPENAME = NULL;

    //Initializations
    INSTALL_DIRECTORY = lw_dictionary_get_install_directory ();
    installdirectory = g_dir_open (INSTALL_DIRECTORY, 0, NULL);

    lw_dictionaries_clear (self);

    while ((TYPENAME = g_dir_read_name (installdirectory)) != NULL)
    {
      GType type = g_type_from_name (TYPENAME);
      if (type != G_TYPE_INVALID && g_type_is_a (type, LW_TYPE_DICTIONARY))
      {
        gchar * typedirectorypath = g_build_path (INSTALL_DIRECTORY, TYPENAME, NULL);
        if (typedirectorypath != NULL)
        {
          GDir * typedirectory = g_dir_open (typedirectorypath, 0, NULL);
          if (typedirectory != NULL)
          {
            gchar const * FILENAME = NULL;
            while ((FILENAME = g_dir_read_name (typedirectory)) != NULL)
            {
              LwDictionary * dictionary = lw_dictionary_new (type, FILENAME);
              if (dictionary != NULL)
              {
                dictionaries = g_list_prepend (dictionaries, dictionary);
              }
            }
            g_dir_close (typedirectory);
            typedirectory = NULL;
          }
          g_free (typedirectorypath);
          typedirectorypath = NULL;
        }
      }
    }
    
    dictionaries = g_list_reverse (dictionaries);

    lw_dictionaries_insert (self, -1, dictionaries);

errored:

    if (installdirectory != NULL)
    {
      g_dir_close (installdirectory);
      installdirectory = NULL;
    }

    g_list_free_full (dictionaries, (GDestroyNotify) g_object_unref);
    dictionaries = NULL;
}


LwDictionary* 
lw_dictionaries_find (LwDictionaries* self,
                        const GType       TYPE,
                        const gchar*      FILENAME)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL && FILENAME != NULL, NULL);

    //Declarations
    LwDictionariesPrivate *priv = NULL;
    gchar *id = NULL;
    gchar * normalized = NULL;
    LwDictionary *dictionary = NULL;

    //Initializations
    priv = lw_dictionaries_get_instance_private (self);
    id = g_strdup_printf ("%s/%s", g_type_name (TYPE), FILENAME);
    if (id == NULL) goto errored;
    normalized = g_utf8_casefold (id, -1);
    if (normalized == NULL) goto errored;

    dictionary = lw_dictionaries_find_by_id (self, normalized);

errored:

    g_free (id); id = NULL;
    g_free (normalized); normalized = NULL;

    return dictionary;
}


LwDictionary* 
lw_dictionaries_fuzzy_find (LwDictionaries *self,
                              const gchar      *FUZZY_DESCRIPTION)
{
printf("BREAK1 lw_dictionaries_fuzzy_find\n");
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARIES (self), NULL);

    //Declarations
    LwDictionariesPrivate *priv = NULL;
    LwDictionary *dictionary = NULL;

    //Initializations
    priv = lw_dictionaries_get_instance_private (self);
    dictionary = NULL;

    //Try getting the first dictionary if none is specified
    if (FUZZY_DESCRIPTION == NULL)
    {
printf("BREAK2 lw_dictionaries_fuzzy_find\n");
      if (priv->data.list != NULL)
      {
        dictionary = LW_DICTIONARY (priv->data.list->data);
      }
      else
      {
        dictionary = NULL;
      }
    }

    //Otherwise try getting a dictionary using a few different string parsers
    else
    {
      if (dictionary == NULL)
      {
printf("BREAK3 lw_dictionaries_fuzzy_find\n");
        dictionary = lw_dictionaries_find_by_id (self, FUZZY_DESCRIPTION);
      }
      if (dictionary == NULL)
      {
        dictionary = lw_dictionaries_find_by_filename (self, FUZZY_DESCRIPTION);
printf("BREAK4 lw_dictionaries_fuzzy_find %d\n", dictionary);
      }
    }

    return dictionary;
}


LwDictionary* 
lw_dictionaries_find_by_filename (LwDictionaries *self,
                                    const gchar      *FILENAME)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARIES (self), NULL);
    g_assert (FILENAME != NULL);

    //Declarations
    LwDictionariesPrivate *priv = NULL;
    gchar *filename = NULL;
    LwDictionary *dictionary = NULL;

    //Initializations
    priv = lw_dictionaries_get_instance_private (self);
    filename = g_utf8_casefold (FILENAME, -1);
    dictionary = g_hash_table_lookup (priv->data.index.filename, filename);
printf("BREAK lw_dictionaries_find_by_filename %s %d %d\n", filename, dictionary, g_hash_table_size (priv->data.index.filename));

{
  GList *keys = g_hash_table_get_keys (priv->data.index.filename);
  GList *link = NULL;
  for (link = keys; link != NULL; link = link->next)
  {
    printf("BREAK lw_dictionaries_find_by_filename key %s\n", link->data);
  }
}
    

errored:

    if (filename != NULL) g_free (filename); filename = NULL;

    return dictionary;
}


LwDictionary* 
lw_dictionaries_find_by_id (LwDictionaries *self,
                              const gchar      *ID)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARIES (self), NULL);
    g_return_val_if_fail (ID != NULL, NULL);

    //Declarations
    LwDictionariesPrivate *priv = NULL;
    gchar *id = NULL;
    LwDictionary *dictionary = NULL;

    //Initializations
    priv = lw_dictionaries_get_instance_private (self);
    id = g_utf8_casefold (g_strdup (ID), -1);
printf("BREAK lw_dictionaries_find_by_id %s\n", id);
    dictionary = g_hash_table_lookup (priv->data.index.id, id);

errored:

    if (id != NULL) g_free (id); id = NULL;

    return dictionary;
}


gboolean 
lw_dictionaries_dictionary_exists (LwDictionaries *self,
                                     LwDictionary     *dictionary)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, FALSE);
    g_return_val_if_fail (dictionary != NULL, FALSE);

    //Declarations
    LwDictionariesPrivate *priv = NULL;
    gboolean exists = NULL;
    GList *link = NULL;

    //Initializations
    priv = lw_dictionaries_get_instance_private (self);
    exists = FALSE;
    link = priv->data.list;

    //Return true if the dictionary exists
    while (link != NULL && exists == FALSE)
    {
      //exists = lw_dictionary_equals (dictionary, LW_DICTIONARY (link->data));
      //TODO
      link = link->next;
    }

    return exists;
}


//
//! @brief Saves the current load order to the preferences
//
void 
lw_dictionaries_save_order (LwDictionaries *self)
{
    //Declarations
    LwDictionariesPrivate *priv = NULL;
    LwDictionary *dictionary = NULL;
    LwPreferences *preferences = NULL;
    gchar *order = NULL;
    gchar **atoms = NULL;

    //Initializations
    priv = lw_dictionaries_get_instance_private (self);
    preferences = lw_dictionaries_get_preferences (self);

    atoms = g_new0 (gchar*, lw_dictionaries_length (self) + 1);
    if (atoms == NULL) goto errored;

    //Construct the preference string
    {
      gchar **ptr = atoms;
      GList *link = priv->data.list;
      while (link != NULL)
      {
        dictionary = LW_DICTIONARY (link->data);
        *ptr = (gchar*) lw_dictionary_get_id (dictionary);
        if (*ptr == NULL) break;
        ptr++;
        link = link->next;
      }
      *ptr = NULL;
    }

    order = g_strjoinv (";", atoms);
    if (order == NULL) goto errored;

    lw_preferences_set_string_by_schema (preferences, LW_SCHEMA_DICTIONARY, LW_KEY_ORDER, order);

errored:

    g_free (order); order = NULL;
    g_free (atoms); atoms = NULL;
}


static GHashTable*
lw_dictionaries_build_order_map (LwDictionaries *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARIES (self), NULL);

    //Declarations
    LwDictionariesPrivate *priv = NULL;
    gchar *order = NULL;
    gchar **atoms = NULL;
    gchar i = 0;
    LwPreferences *preferences = NULL;
    GHashTable *hashtable = NULL;

    //Initializations
    priv = lw_dictionaries_get_instance_private (self);
    hashtable = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
    if (hashtable == NULL) goto errored;

    preferences = lw_dictionaries_get_preferences (self);;
    if (preferences == NULL) goto errored;

    order = lw_preferences_get_string_by_schema (preferences, LW_SCHEMA_DICTIONARY, LW_KEY_ORDER);
    if (order == NULL) goto errored;

    atoms = g_strsplit_set (order, ";", -1);
    if (atoms == NULL) goto errored;

    //Generate the hashtable
    for (i = 0; atoms[i] != NULL; i++)
    {
      g_hash_table_insert (hashtable, atoms[i], GINT_TO_POINTER (i));
    }

errored:
    g_free (order); order = NULL;

    if (atoms != NULL) {
      if (hashtable != NULL) g_free (atoms);
      else g_strfreev (atoms); 
      atoms = NULL;
    }

    return hashtable;
}


static gint*
lw_dictionaries_convert_order_map_to_array (LwDictionaries *self, 
                                              GHashTable       *hashtable)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARIES (self), NULL);
    g_return_val_if_fail (hashtable != NULL, NULL);

    //Declarations
    LwDictionariesPrivate *priv = NULL;
    GList *link = NULL;
    gint *new_order = NULL;
    gint missing_index = 0;

    //Initializations
    priv = lw_dictionaries_get_instance_private (self);
    if (priv == NULL) goto errored;
    link = priv->data.list;
    if (link == NULL) goto errored;
    missing_index = g_list_length (priv->data.list);
    if (missing_index == 0) goto errored;
    new_order = g_new0 (gint, missing_index);
    if (new_order == NULL) goto errored;

    {
      gint i = 0;
      while (link != NULL)
      {
        LwDictionary *dictionary = LW_DICTIONARY (link->data);
        if (dictionary != NULL)
        {
          const gchar *KEY = lw_dictionary_get_id (dictionary);
          if (KEY != NULL)
          {
            gpointer position_ptr = NULL;
            if (g_hash_table_lookup_extended (hashtable, KEY, NULL, &position_ptr))
            {
              new_order[i] = GPOINTER_TO_INT (position_ptr);
            }
            else
            {
              new_order[i] = missing_index;
              missing_index++;
            }
          }
        }

        i++;
        link = link->next;
      }
    }

errored:

    return new_order;
}


//!
//! @brief All dictionaries with a specific status get switched to the requested one
//! @param a Pointer to LwDictionary object a
//! @param b Pointer to LwDictionary object b
//! @returns Whether the position of a is less than (-1), equal (0) or greater than b (1)
//!
static gint 
lw_dictionaries_sort_compare_function (gconstpointer a,
                                         gconstpointer b,
                                         gpointer      data)
{
    //Sanity checks
    g_return_val_if_fail (data != NULL, 0);
  
    //Declarations
    GHashTable *hashtable = NULL;
    const gchar *DESCRIPTION_A = NULL, *DESCRIPTION_B = NULL;
    LwDictionary *dictionary_a = NULL, *dictionary_b = NULL;
    gpointer position_a_ptr = NULL, position_b_ptr = NULL;
    gint position_a, position_b;
    gboolean found_a, found_b;
    gint order;

    //Initializations
    hashtable = (GHashTable*) data;
    found_a = found_b = FALSE;

    dictionary_a = LW_DICTIONARY (a);
    DESCRIPTION_A = lw_dictionary_get_id (dictionary_a);
    if (DESCRIPTION_A != NULL)
    {
      found_a = g_hash_table_lookup_extended (hashtable, DESCRIPTION_A, NULL, &position_a_ptr);
      position_a = GPOINTER_TO_INT (position_a_ptr);
    }
    
    dictionary_b = LW_DICTIONARY (b);
    DESCRIPTION_B = lw_dictionary_get_id (dictionary_b);
    if (DESCRIPTION_B != NULL)
    {
      found_b = g_hash_table_lookup_extended (hashtable, DESCRIPTION_B, NULL, &position_b_ptr);
      position_b = GPOINTER_TO_INT (position_b_ptr);
    }

    //Calculate the proper order
    if (found_a && found_b)
    {
      if (position_a > position_b) order = 1;
      else if (position_a < position_b) order = -1;
      else order = 0;
    }
    else if (found_a)
    {
      order = -1;
    }
    else if (found_b)
    {
      order = 1;
    }
    else
    {
      order = 0;
    }

    return order;
}


//
//! @brief Loads the load order from the preferences
//
void 
lw_dictionaries_load_order (LwDictionaries *self)
{
  /*TODO
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARIES (self));

    //Declarations
    LwDictionariesPrivate *priv = NULL;
    gint *new_order = NULL;
    GHashTable *order_map = NULL;

    //Initializations
    priv = lw_dictionaries_get_instance_private (self);

    if (priv->data.list == NULL) goto errored;

    order_map = lw_dictionaries_build_order_map (self);
    if (order_map == NULL) goto errored;

    new_order = lw_dictionaries_convert_order_map_to_array (self, order_map);
    if (new_order == NULL) goto errored;

    priv->data.list = g_list_sort_with_data (priv->data.list, lw_dictionaries_sort_compare_function, order_map);
    if (priv->data.list == NULL) goto errored;

    printf("BREAk lw_dictionaries_load_order CLASS_SIGNALID_ROWS_REORDERED\n");
    g_signal_emit (G_OBJECT (self), klasspriv->signalid[CLASS_SIGNALID_ROWS_REORDERED], 0, new_order);

errored:

    g_free (new_order); new_order = NULL;
    if (order_map != NULL) g_hash_table_destroy (order_map); order_map = NULL;
    */
}


gint*
lw_dictionaries_get_indices (LwDictionaries *self,
                               GList            *dictionaries)
{
    /*
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARIES (self), NULL);
    if (dictionaries == NULL) return NULL;

    //Declarations
    gint* indices = NULL;
    gint length = NULL;

    //Initializations
    length = g_list_length (dictionaries);
    if (length < 1) goto errored;
    indices = g_new0 (gint, length + 1);
    if (indices == NULL) goto errored;

    {
      GList *link = NULL;
      gint i = 0;
      for (link = dictionaries; link != NULL; link = link->next)
      {
        LwDictionary *dictionary = LW_DICTIONARY (link->data);
        if (dictionary != NULL && dictionary->row.current_index > -1)
        {
          indices[i++] = dictionary->row.current_index;
        }
      }
      indices[i++] = -1;
    }

errored:

    return indices;
    */
    return NULL;
}


GList*
lw_dictionaries_dictionaries (LwDictionaries *self)
{
    g_return_val_if_fail (LW_IS_DICTIONARIES (self), NULL);

    //Declarations
    LwDictionariesPrivate *priv = NULL;
    GList *dictionaries = NULL;

    //Initializations
    priv = lw_dictionaries_get_instance_private (self);
    dictionaries = g_list_copy (priv->data.list);

    g_list_foreach (dictionaries, (GFunc) g_object_ref, NULL);

    return dictionaries;
}


void
lw_dictionaries_sort_with_data (LwDictionaries *self, 
                                  GCompareDataFunc  compare_func, 
                                  gpointer          user_data)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARIES (self));

    //Declarations
    LwDictionariesPrivate *priv;

    //Initializations
    priv = lw_dictionaries_get_instance_private (self);

    priv->data.list = g_list_sort_with_data (priv->data.list, compare_func, user_data);
}


void
lw_dictionaries_menumodel_insert (LwDictionaries *self,
                                    LwDictionary     *dictionary,
                                    gint              index_)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARIES (self));
    g_return_if_fail (LW_IS_DICTIONARY (dictionary));

    //Declarations
    GMenuModel *menu_model = NULL;
    GMenu *menu = NULL;
    GMenuItem *menuitem = NULL;
    gchar *detailed_action = NULL;
    const gchar *SHORTNAME = NULL;
    gchar *longname = NULL;

    //Initializations
    menu_model = lw_dictionaries_get_menumodel (self);
    menu = G_MENU (menu_model);
    longname = NULL;
    detailed_action = NULL;
    menuitem = NULL;
    SHORTNAME = lw_dictionary_get_name (dictionary);

    longname = g_strdup_printf (gettext("%s Dictionary"), SHORTNAME);
    if (longname == NULL) goto errored;
    detailed_action = g_strdup_printf ("win.set-dictionary::%d", index_ + 1);
    if (detailed_action == NULL) goto errored;

    menuitem = g_menu_item_new (longname, detailed_action);
    if (menuitem == NULL) goto errored;
    g_menu_insert_item (menu, index_, menuitem);

errored:

    if (longname != NULL) g_free (longname); longname = NULL;
    if (detailed_action != NULL) g_free (detailed_action); detailed_action = NULL;
    if (menuitem != NULL) g_object_unref (menuitem); menuitem = NULL;
}


void
lw_dictionaries_menumodel_append (LwDictionaries *self, 
                                    LwDictionary     *dictionary)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARIES (self));
    g_return_if_fail (LW_IS_DICTIONARY (dictionary));

    //Declarations
    GMenuModel *menu_model = NULL;
    gint index = 0;

    //Initializations
    menu_model = lw_dictionaries_get_menumodel (self);
    if (menu_model == NULL) goto errored;
    index = g_menu_model_get_n_items (menu_model);

    lw_dictionaries_menumodel_insert (self, dictionary, index);

errored:

    return;
}


void
lw_dictionaries_sync_menumodel (LwDictionaries *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARIES (self));

    //Declarations
    LwDictionariesPrivate *priv = NULL;
    GMenuModel *menu_model = NULL;
    GMenu *menu = NULL;

    //Initializations
    priv = lw_dictionaries_get_instance_private (self);
    menu_model = lw_dictionaries_get_menumodel (self);
    menu = G_MENU (menu_model);

    //Clear the old menu items
    while (g_menu_model_get_n_items (menu_model) > 0)
    {
      g_menu_remove (menu, 0);
    }

    //Repopulate with the new menu items
    {
      GList *link = priv->data.list;
      while (link != NULL)
      {
        LwDictionary *dictionary = LW_DICTIONARY (link->data);
        if (dictionary != NULL)
        {
          lw_dictionaries_menumodel_append (self, dictionary);
        }
        link = link->next;
      }
    }
}

void
lw_dictionaries_set_menumodel (LwDictionaries *self,
                                 GMenuModel       *menu_model)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARIES (self));

    //Declarations
    LwDictionariesPrivate *priv = NULL;

    //Initializations
    priv = lw_dictionaries_get_instance_private (self);

    if (menu_model != NULL)
    {
      g_object_ref (G_OBJECT (menu_model));
    }

    if (priv->data.menu_model != NULL)
    {
      g_object_remove_weak_pointer (G_OBJECT (priv->data.menu_model), (gpointer*) &(priv->data.menu_model));
      g_object_unref (G_OBJECT (priv->data.menu_model));
      priv->data.menu_model = NULL;
    }

    priv->data.menu_model = menu_model;

    if (priv->data.menu_model != NULL)
    {
      g_object_add_weak_pointer (G_OBJECT (priv->data.menu_model), (gpointer*) &(priv->data.menu_model));
    }
}


GMenuModel*
lw_dictionaries_get_menumodel (LwDictionaries *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARIES (self), NULL);

    //Declarations
    LwDictionariesPrivate *priv = NULL;

    //Initializations
    priv = lw_dictionaries_get_instance_private (self);
    if (priv == NULL) goto errored;

    if (priv->data.menu_model == NULL)
    {
      GMenuModel *menu_model = G_MENU_MODEL (g_menu_new ());
      lw_dictionaries_set_menumodel (self, menu_model);
      g_object_unref (G_OBJECT (menu_model));
      menu_model = NULL;
    }

errored:

    return priv->data.menu_model;
}


