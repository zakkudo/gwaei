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
//! @file dictionarylist.c
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
#include <libwaei/libwaei.h>

#include <libwaei/dictionarylist-private.h>

static LwDictionaryListClass *_klass = NULL;
static LwDictionaryListClassPrivate *_klasspriv = NULL;

G_DEFINE_TYPE (LwDictionaryList, lw_dictionarylist, G_TYPE_OBJECT)


//!
//! @brief Creates a new LwDictionaryList object
//! @param MAX The maximum items you want in the dictionary before old ones are deleted
//! @return An allocated LwDictionaryList that will be needed to be freed by lw_dictionary_free.
//!
LwDictionaryList* 
lw_dictionarylist_new (LwPreferences *preferences)
{
    //Declarations
    LwDictionaryList *dictionary = NULL;

    //Initializations
    dictionary = LW_DICTIONARYLIST (g_object_new (LW_TYPE_DICTIONARYLIST, "preferences", preferences, NULL));

    return dictionary;
}


static void 
lw_dictionarylist_init (LwDictionaryList *self)
{
    self->priv = LW_DICTIONARYLIST_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LwDictionaryListPrivate));

    LwDictionaryListPrivate *priv = NULL;

    priv = self->priv;

    priv->data.index.typename = g_hash_table_new (g_str_hash, g_str_equal);
    priv->data.index.filename = g_hash_table_new (g_str_hash, g_str_equal);
    priv->data.index.id = g_hash_table_new_full (g_str_hash, g_str_equal, (GDestroyNotify) g_free, NULL);

    lw_dictionarylist_connect_signals (self);
}


static void 
lw_dictionarylist_set_property (GObject      *object,
                                guint         property_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
    //Declarations
    LwDictionaryList *self = NULL;
    LwDictionaryListPrivate *priv = NULL;

    //Initializations
    self = LW_DICTIONARYLIST (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_PREFERENCES:
        lw_dictionarylist_set_preferences (self, g_value_get_object (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_dictionarylist_get_property (GObject      *object,
                                guint         property_id,
                                GValue       *value,
                                GParamSpec   *pspec)
{
    //Declarations
    LwDictionaryList *self = NULL;
    LwDictionaryListPrivate *priv = NULL;

    //Initializations
    self = LW_DICTIONARYLIST (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_PREFERENCES:
        g_value_set_object (value, lw_dictionarylist_get_preferences (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_dictionarylist_finalize (GObject *object)
{
    //Declarations
    LwDictionaryList *self = NULL;
    LwDictionaryListPrivate *priv = NULL;

    //Initalizations
    self = LW_DICTIONARYLIST (object);
    priv = self->priv;

    lw_dictionarylist_set_menumodel (self, NULL);

    G_OBJECT_CLASS (lw_dictionarylist_parent_class)->finalize (object);
}


static void
lw_dictionarylist_dispose (GObject *object)
{
    //Declarations
    LwDictionaryList *self = NULL;

    //Initializations
    self = LW_DICTIONARYLIST (object);

    lw_dictionarylist_clear (self);
    lw_dictionarylist_disconnect_signals (self);

    G_OBJECT_CLASS (lw_dictionarylist_parent_class)->dispose (object);
}



static void
lw_dictionarylist_class_init (LwDictionaryListClass *klass)
{
    //Declarations
    GObjectClass *object_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    klass->priv = g_new0 (LwDictionaryListClassPrivate, 1);
    object_class->set_property = lw_dictionarylist_set_property;
    object_class->get_property = lw_dictionarylist_get_property;
    object_class->dispose = lw_dictionarylist_dispose;
    object_class->finalize = lw_dictionarylist_finalize;

    g_type_class_add_private (object_class, sizeof (LwDictionaryListPrivate));

    _klass = klass;
    _klasspriv = klass->priv;

    _klasspriv->signalid[CLASS_SIGNALID_ROW_CHANGED] = g_signal_new (
        "internal-row-changed",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwDictionaryListClass, row_changed),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    _klasspriv->signalid[CLASS_SIGNALID_ROW_INSERTED] = g_signal_new (
        "internal-row-inserted",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwDictionaryListClass, row_inserted),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    _klasspriv->signalid[CLASS_SIGNALID_ROW_DELETED] = g_signal_new (
        "internal-row-deleted",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwDictionaryListClass, row_deleted),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    _klasspriv->signalid[CLASS_SIGNALID_ROWS_REORDERED] = g_signal_new (
        "internal-rows-reordered",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwDictionaryListClass, rows_reordered),
        NULL, NULL,
        g_cclosure_marshal_VOID__POINTER,
        G_TYPE_NONE, 1,
        G_TYPE_POINTER
    );

    _klasspriv->pspec[PROP_PREFERENCES] = g_param_spec_object (
        "preferences",
        "Preferences construct prop",
        "Set the preferences object",
        LW_TYPE_PREFERENCES,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_PREFERENCES, _klasspriv->pspec[PROP_PREFERENCES]);
}


void
lw_dictionarylist_set_preferences (LwDictionaryList *self,
                                   LwPreferences    *preferences)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYLIST (self));

    //Declarations
    LwDictionaryListPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

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

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_PREFERENCES]);
}


LwPreferences*
lw_dictionarylist_get_preferences (LwDictionaryList *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYLIST (self));

    //Declarations
    LwDictionaryListPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (priv->config.preferences == NULL)
    {
      lw_dictionarylist_set_preferences (self, lw_preferences_get_default ());
    }

    return priv->config.preferences;
}


void
lw_dictionarylist_clear (LwDictionaryList *self)
{
    //Sanity checks
    g_return_if_fail (LW_DICTIONARYLIST (self));

    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    GList *removed = NULL;
    gint length = 0;

    //Initializations
    priv = self->priv;
    length = lw_dictionarylist_length (self);

    removed = lw_dictionarylist_remove (self, NULL);

errored:

    if (removed != NULL) g_list_free_full (removed, (GDestroyNotify) g_object_unref);
}


void
lw_dictionarylist_invalidate_length (LwDictionaryList *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYLIST (self), 0);

    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    gint length = 0;

    //Initializations
    priv = self->priv;

    priv->data.length = -1;
}


static void
_add_to_index (LwDictionaryList *self,
               LwDictionary     *dictionary)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYLIST (self));
    g_return_if_fail (LW_IS_DICTIONARY (dictionary));
    
    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    gchar *id = NULL;
    gchar *typename = NULL;
    gchar *filename = NULL;

    //Initializations
    priv = self->priv;
    id = g_ascii_strdown (lw_dictionary_get_id (dictionary), -1);
    typename = g_ascii_strdown (G_OBJECT_TYPE_NAME (dictionary), -1);
    filename = g_ascii_strdown (lw_dictionary_get_filename (dictionary), -1);
   
    if (typename != NULL && !g_hash_table_contains (priv->data.index.typename, typename))
    {
      g_hash_table_insert (priv->data.index.typename, typename, dictionary);
    }

    if (filename != NULL && !g_hash_table_contains (priv->data.index.filename, filename))
    {
      g_hash_table_insert (priv->data.index.filename, filename, dictionary);
    }

    if (id != NULL && !g_hash_table_contains (priv->data.index.id, id))
    {
      g_hash_table_insert (priv->data.index.id, id, dictionary);
    }

errored:

    if (id != NULL) g_free (id); id = NULL;
    if (typename != NULL) g_free (typename); typename = NULL;
    if (filename != NULL) g_free (filename); filename = NULL;

    return;
}


static void
_remove_from_index (LwDictionaryList *self,
                    LwDictionary     *dictionary)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYLIST (self));
    g_return_if_fail (LW_IS_DICTIONARY (dictionary));
    
    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    gchar *id = NULL;
    gchar *typename = NULL;
    gchar *filename = NULL;

    //Initializations
    priv = self->priv;
    id = g_ascii_strdown (lw_dictionary_get_id (dictionary), -1);
    typename = g_ascii_strdown (G_OBJECT_TYPE_NAME (dictionary), -1);
    filename = g_ascii_strdown (lw_dictionary_get_filename (dictionary), -1);
   
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
_rebuild_array (LwDictionaryList *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYLIST (self));

    lw_dictionarylist_invalidate_length (self);

    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    gint length = 0;

    //Initializations
    priv = self->priv;
    length = lw_dictionarylist_length (self);

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

    lw_dictionarylist_invalidate_length (self);
}


static gint
_insert (LwDictionaryList *self,
         gint             *position,
         GList            *dictionaries)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYLIST (self));
    if (dictionaries == NULL) return 0;

    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    gint number_inserted = 0;
    gint length = 0;
    gboolean append = FALSE;

    //Initializations
    priv = self->priv;
    length = lw_dictionarylist_length (self);
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

    lw_dictionarylist_invalidate_length (self);

    return number_inserted;
}


static void
_insert_propogate_changes (LwDictionaryList *self,
                           gint              position,
                           gint              number_inserted)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYLIST (self));

    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    gint length = 0;
    gint i = 0;

    //Initializations
    priv = self->priv;
    length = lw_dictionarylist_length (self);

    //Rows that were inserted
    for (i = position; i < position + number_inserted; i++)
    {
      g_signal_emit (G_OBJECT (self), _klasspriv->signalid[CLASS_SIGNALID_ROW_INSERTED], 0, i);
    }

    //Rows with modified indexes
    for (i = position + number_inserted; i < length; i++)
    {
      g_signal_emit (G_OBJECT (self), _klasspriv->signalid[CLASS_SIGNALID_ROW_CHANGED], 0, i);
    }
}


static gint*
_find_duplicates (LwDictionaryList *self, 
                  gint              position, 
                  GList            *dictionaries)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYLIST (self), NULL);
    if (dictionaries == NULL) return NULL;

    //Declarations
    gint length = -1;
    gint *indices = NULL;

    //Initializations
    length = g_list_length (dictionaries);
    if (position < 0) position = lw_dictionarylist_length (self);
    indices = g_new0 (gint, length + 1);
    if (indices == NULL) goto errored;

    {
      GList *link = NULL;
      gint i = 0;
      for (link = dictionaries; link != NULL; link = link->next)
      {
        LwDictionary *a = LW_DICTIONARY (link->data);
        if (a == NULL) continue;
        LwDictionary *b = LW_DICTIONARY (lw_dictionarylist_nth (self, a->row.current_index));
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
lw_dictionarylist_insert (LwDictionaryList *self, 
                          gint              position,
                          GList            *dictionaries)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYLIST (self));
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

    removed = lw_dictionarylist_remove (self, duplicates);

errored:

    if (duplicates != NULL) g_free (duplicates); duplicates = NULL;
    if (removed != NULL) g_list_free (removed); removed;

    return lw_dictionarylist_get_indices (self, dictionaries);
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
_sanitize_indices (LwDictionaryList *self,
                   gint             *indices)
{
    g_return_val_if_fail (LW_IS_DICTIONARYLIST (self), NULL);

    //Declarations
    gint size = 0;
    gint length = 0;

    //Initializations
    length = lw_dictionarylist_length (self);

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
_remove (LwDictionaryList *self,
         gint             *indices)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYLIST (self));
    if (indices == NULL) return NULL;

    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    gint length = 0;
    GList *list = NULL;

    //Initializations
    priv = self->priv;
    length = lw_dictionarylist_length (self);
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

    lw_dictionarylist_invalidate_length (self);

    return list;
}


static void
_remove_propogate_changes (LwDictionaryList *self,
                           gint             *indices)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYLIST (self));
    if (indices == NULL) return;

    //Declarations
    gint length = 0;
    gint i = 0;

    //Initializations
    length = lw_dictionarylist_length (self);

    //Rows that were removed
    for (i = 0; indices[i] != -1; i++)
    {
      g_signal_emit (G_OBJECT (self), _klasspriv->signalid[CLASS_SIGNALID_ROW_DELETED], 0, indices[i]);
    }
    i--;

    //Rows with modified indexes
    {
      gint index = 0;
      for (index = indices[i]; index < length; index++)
      {
        g_signal_emit (G_OBJECT (self), _klasspriv->signalid[CLASS_SIGNALID_ROW_CHANGED], 0, index);
      }
    }
}


GList*
lw_dictionarylist_remove (LwDictionaryList *self,
                          gint             *indices)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYLIST (self));

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
//! @return The position in the LwDictionaryList of the LwDictionary
//!
LwDictionary* 
lw_dictionarylist_nth (LwDictionaryList *self,
                       gint              index)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    LwDictionary *dictionary = NULL;
    GList *link = NULL;
    gint length = 0;

    //Initializiations
    priv = self->priv;
    length = lw_dictionarylist_length (self);
    if (index >= length) goto errored;

    link = priv->data.array[index];
    if (link == NULL) goto errored;
    dictionary = LW_DICTIONARY (link->data);

errored:

    return dictionary;
}




gint 
lw_dictionarylist_length (LwDictionaryList *self)
{
    //Sanity check
    g_return_val_if_fail (self != NULL, 0);

    //Declarations
    LwDictionaryListPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (priv->data.length < 0)
    {
      priv->data.length = g_list_length (priv->data.list);
    }

    return priv->data.length;
}





void 
lw_dictionarylist_load_installed (LwDictionaryList   *self, 
                                  LwMorphologyEngine *morphologyengine)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYLIST (self));

    //Declarations
    gchar** idlist = NULL;
    gchar **iditer = NULL;
    gchar** pair = NULL;
    gchar *typename = NULL;
    GType type = 0;
    LwDictionary *dictionary = NULL;
    const gchar *FILENAME = NULL;
    GList *dictionaries = NULL;

    //Initializations
    lw_dictionarylist_clear (self);

    idlist = lw_dictionary_get_installed_idlist (G_TYPE_NONE);
    if (idlist != NULL)
    {
      for (iditer = idlist; *iditer != NULL; iditer++)
      {
        pair = g_strsplit_set (*iditer, "/", 2);
        if (pair != NULL && pair[0] != NULL && pair[1] != NULL) 
        {
          typename = lw_dictionary_directoryname_to_typename (pair[0]);
          type = g_type_from_name (typename);
          FILENAME = pair[1];
          dictionary = LW_DICTIONARY (g_object_new (type, "filename", FILENAME, "morphologyengine", morphologyengine, NULL));
          if (dictionary != NULL && LW_IS_DICTIONARY (dictionary))
            dictionaries = g_list_prepend (dictionaries, dictionary);
          if (typename != NULL) g_free (typename); typename = NULL;
        }
        g_strfreev (pair); pair = NULL;
      }
      g_strfreev (idlist); idlist = NULL;
    }

    dictionaries = g_list_reverse (dictionaries);

    printf("BREAK loading %d dictionaries\n", g_list_length (dictionaries));

    lw_dictionarylist_insert (self, -1, dictionaries);

errored:

    g_list_free_full (dictionaries, (GDestroyNotify) g_object_unref);
    dictionaries = NULL;
}


LwDictionary* 
lw_dictionarylist_find (LwDictionaryList* self,
                        const GType       TYPE,
                        const gchar*      FILENAME)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL && FILENAME != NULL, NULL);

    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    gchar *id = NULL;
    LwDictionary *dictionary = NULL;

    //Initializations
    priv = self->priv;
    id = g_ascii_strdown (lw_dictionary_build_id_from_type (TYPE, FILENAME), -1);
    dictionary = lw_dictionarylist_find_by_id (self, id);

errored:

    if (id != NULL) g_free (id); id = NULL;

    return dictionary;
}


LwDictionary* 
lw_dictionarylist_fuzzy_find (LwDictionaryList *self,
                              const gchar      *FUZZY_DESCRIPTION)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYLIST (self), NULL);

    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    LwDictionary *dictionary = NULL;

    //Initializations
    priv = self->priv;
    dictionary = NULL;

    //Try getting the first dictionary if none is specified
    if (FUZZY_DESCRIPTION == NULL)
    {
      if (priv->data.list != NULL)
        dictionary = LW_DICTIONARY (priv->data.list->data);
      else
        dictionary = NULL;
    }

    //Otherwise try getting a dictionary using a few different string parsers
    else
    {
      if (dictionary == NULL)
        dictionary = lw_dictionarylist_find_by_id (self, FUZZY_DESCRIPTION);
      if (dictionary == NULL)
        dictionary = lw_dictionarylist_find_by_filename (self, FUZZY_DESCRIPTION);
    }

    return dictionary;
}


LwDictionary* 
lw_dictionarylist_find_by_filename (LwDictionaryList *self,
                                    const gchar      *FILENAME)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYLIST (self), NULL);
    g_assert (FILENAME != NULL);

    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    gchar *filename = NULL;
    LwDictionary *dictionary = NULL;

    //Initializations
    priv = self->priv;
    filename = g_ascii_strdown (g_strdup (FILENAME), -1);
    dictionary = g_hash_table_lookup (priv->data.index.filename, filename);

errored:

    if (filename != NULL) g_free (filename); filename = NULL;

    return dictionary;
}


LwDictionary* 
lw_dictionarylist_find_by_id (LwDictionaryList *self,
                              const gchar      *ID)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYLIST (self), NULL);
    g_return_val_if_fail (ID != NULL, NULL);

    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    gchar *id = NULL;
    LwDictionary *dictionary = NULL;

    //Initializations
    priv = self->priv;
    id = g_ascii_strdown (g_strdup (ID), -1);
    dictionary = g_hash_table_lookup (priv->data.index.id, id);

errored:

    if (id != NULL) g_free (id); id = NULL;

    return dictionary;
}


gboolean 
lw_dictionarylist_dictionary_exists (LwDictionaryList *self,
                                     LwDictionary     *dictionary)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, FALSE);
    g_return_val_if_fail (dictionary != NULL, FALSE);

    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    gboolean exists = NULL;
    GList *link = NULL;

    //Initializations
    priv = self->priv;
    exists = FALSE;
    link = priv->data.list;

    //Return true if the dictionary exists
    while (link != NULL && exists == FALSE)
    {
      exists = lw_dictionary_equals (dictionary, LW_DICTIONARY (link->data));
      link = link->next;
    }

    return exists;
}


//
//! @brief Saves the current load order to the preferences
//
void 
lw_dictionarylist_save_order (LwDictionaryList *self)
{
    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    LwDictionary *dictionary = NULL;
    LwPreferences *preferences = NULL;
    gchar *order = NULL;
    gchar **atoms = NULL;

    //Initializations
    priv = self->priv;
    preferences = lw_dictionarylist_get_preferences (self);

    atoms = g_new0 (gchar*, lw_dictionarylist_length (self) + 1);
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
lw_dictionarylist_build_order_map (LwDictionaryList *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYLIST (self), NULL);

    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    gchar *order = NULL;
    gchar **atoms = NULL;
    gchar i = 0;
    LwPreferences *preferences = NULL;
    GHashTable *hashtable = NULL;

    //Initializations
    priv = self->priv;
    hashtable = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
    if (hashtable == NULL) goto errored;

    preferences = lw_dictionarylist_get_preferences (self);;
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
lw_dictionarylist_convert_order_map_to_array (LwDictionaryList *self, 
                                              GHashTable       *hashtable)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYLIST (self), NULL);
    g_return_val_if_fail (hashtable != NULL, NULL);

    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    GList *link = NULL;
    gint *new_order = NULL;
    gint missing_index = 0;

    //Initializations
    priv = self->priv;
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
lw_dictionarylist_sort_compare_function (gconstpointer a,
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
lw_dictionarylist_load_order (LwDictionaryList *self)
{
  /*TODO
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYLIST (self));

    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    gint *new_order = NULL;
    GHashTable *order_map = NULL;

    //Initializations
    priv = self->priv;

    if (priv->data.list == NULL) goto errored;

    order_map = lw_dictionarylist_build_order_map (self);
    if (order_map == NULL) goto errored;

    new_order = lw_dictionarylist_convert_order_map_to_array (self, order_map);
    if (new_order == NULL) goto errored;

    priv->data.list = g_list_sort_with_data (priv->data.list, lw_dictionarylist_sort_compare_function, order_map);
    if (priv->data.list == NULL) goto errored;

    printf("BREAk lw_dictionarylist_load_order CLASS_SIGNALID_ROWS_REORDERED\n");
    g_signal_emit (G_OBJECT (self), _klasspriv->signalid[CLASS_SIGNALID_ROWS_REORDERED], 0, new_order);

errored:

    g_free (new_order); new_order = NULL;
    if (order_map != NULL) g_hash_table_destroy (order_map); order_map = NULL;
    */
}


gint*
lw_dictionarylist_get_indices (LwDictionaryList *self,
                               GList            *dictionaries)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYLIST (self));
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
}


GList*
lw_dictionarylist_dictionaries (LwDictionaryList *self)
{
    g_return_val_if_fail (LW_IS_DICTIONARYLIST (self), NULL);

    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    GList *dictionaries = NULL;

    //Initializations
    priv = self->priv;
    dictionaries = g_list_copy (priv->data.list);

    g_list_foreach (dictionaries, (GFunc) g_object_ref, NULL);

    return dictionaries;
}


void
lw_dictionarylist_sort_with_data (LwDictionaryList *self, 
                                  GCompareDataFunc  compare_func, 
                                  gpointer          user_data)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYLIST (self));

    //Declarations
    LwDictionaryListPrivate *priv;

    //Initializations
    priv = self->priv;

    priv->data.list = g_list_sort_with_data (priv->data.list, compare_func, user_data);
}


void
lw_dictionarylist_menumodel_insert (LwDictionaryList *self,
                                    LwDictionary     *dictionary,
                                    gint              index_)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYLIST (self));
    g_return_if_fail (LW_IS_DICTIONARY (dictionary));

    //Declarations
    GMenuModel *menu_model = NULL;
    GMenu *menu = NULL;
    GMenuItem *menuitem = NULL;
    gchar *detailed_action = NULL;
    const gchar *SHORTNAME = NULL;
    gchar *longname = NULL;

    //Initializations
    menu_model = lw_dictionarylist_get_menumodel (self);
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
lw_dictionarylist_menumodel_append (LwDictionaryList *self, 
                                    LwDictionary     *dictionary)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYLIST (self));
    g_return_if_fail (LW_IS_DICTIONARY (dictionary));

    //Declarations
    GMenuModel *menu_model = NULL;
    gint index = 0;

    //Initializations
    menu_model = lw_dictionarylist_get_menumodel (self);
    if (menu_model == NULL) goto errored;
    index = g_menu_model_get_n_items (menu_model);

    lw_dictionarylist_menumodel_insert (self, dictionary, index);

errored:

    return;
}


void
lw_dictionarylist_sync_menumodel (LwDictionaryList *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYLIST (self));

    //Declarations
    LwDictionaryListPrivate *priv = NULL;
    GMenuModel *menu_model = NULL;
    GMenu *menu = NULL;

    //Initializations
    priv = self->priv;
    menu_model = lw_dictionarylist_get_menumodel (self);
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
          lw_dictionarylist_menumodel_append (self, dictionary);
        }
        link = link->next;
      }
    }
}

void
lw_dictionarylist_set_menumodel (LwDictionaryList *self,
                                 GMenuModel       *menu_model)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYLIST (self));

    //Declarations
    LwDictionaryListPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

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
lw_dictionarylist_get_menumodel (LwDictionaryList *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYLIST (self), NULL);

    //Declarations
    LwDictionaryListPrivate *priv = NULL;

    //Initializations
    priv = self->priv;
    if (priv == NULL) goto errored;

    if (priv->data.menu_model == NULL)
    {
      GMenuModel *menu_model = G_MENU_MODEL (g_menu_new ());
      lw_dictionarylist_set_menumodel (self, menu_model);
      g_object_unref (G_OBJECT (menu_model));
      menu_model = NULL;
    }

errored:

    return priv->data.menu_model;
}


