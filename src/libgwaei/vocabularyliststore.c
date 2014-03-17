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
//! @file vocabularyliststore.c
//!
//! @brief To be written
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib/gstdio.h>
#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>

#include <libgwaei/vocabularyliststore-private.h>


G_DEFINE_TYPE_WITH_CODE (
  LgwVocabularyListStore, lgw_vocabularyliststore, G_TYPE_OBJECT,
  G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_MODEL, lgw_vocabularyliststore_implement_treemodel_interface)
  G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_DRAG_SOURCE, lgw_vocabularyliststore_implement_treedragsource_interface)
  G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_DRAG_DEST, lgw_vocabularyliststore_implement_treedragdest_interface)
);



LgwVocabularyListStore*
lgw_vocabularyliststore_new (LwPreferences *preferences)
{
    //Declarations
    LgwVocabularyListStore *self = NULL;

    //Initializations
    self = LGW_VOCABULARYLISTSTORE (g_object_new (LGW_TYPE_VOCABULARYLISTSTORE, "preferences", preferences, NULL));

    return self;
}


static void 
lgw_vocabularyliststore_init (LgwVocabularyListStore *self)
{
    self->priv = LGW_VOCABULARYLISTSTORE_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LgwVocabularyListStorePrivate));

    //Declarations
    LgwVocabularyListStorePrivate *priv = NULL;
    gchar *path = NULL;

    //Initializations
    priv = self->priv;

    //TODO
    /*
    priv->file = g_file_new_for_path (path);
    priv->file_monitor = (priv->file, G_FILE_MONITOR_NONE, NULL, NULL);
    */

    priv->data.index.filename = g_hash_table_new_full (g_str_hash, g_str_equal, (GDestroyNotify) g_free, NULL);
    priv->data.index.wordstore = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, NULL);
    priv->data.index.update_filename_callback = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, NULL);
    priv->data.index.changed_callback = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, NULL);

    lgw_vocabularyliststore_load (self);
}


static void 
lgw_vocabularyliststore_finalize (GObject *object)
{
    LgwVocabularyListStore *self;
    LgwVocabularyListStorePrivate *priv;

    self = LGW_VOCABULARYLISTSTORE (object);
    priv = self->priv;

    if (priv->data.index.filename != NULL) g_hash_table_unref (priv->data.index.filename); priv->data.index.filename = NULL;
    if (priv->data.index.wordstore != NULL) g_hash_table_unref (priv->data.index.wordstore); priv->data.index.wordstore = NULL;
    if (priv->data.index.update_filename_callback != NULL) g_hash_table_unref (priv->data.index.update_filename_callback); priv->data.index.update_filename_callback = NULL;
    if (priv->data.index.changed_callback != NULL) g_hash_table_unref (priv->data.index.changed_callback); priv->data.index.changed_callback = NULL;

    G_OBJECT_CLASS (lgw_vocabularyliststore_parent_class)->finalize (object);
}


static void
lgw_vocabularyliststore_set_property (GObject      *object,
                                      guint         property_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
    //Declarations
    LgwVocabularyListStore *self = NULL;
    LgwVocabularyListStorePrivate *priv = NULL;

    //Initializations
    self = LGW_VOCABULARYLISTSTORE (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_PREFERENCES:
        lgw_vocabularyliststore_set_preferences (self, g_value_get_object (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
lgw_vocabularyliststore_get_property (GObject      *object,
                                 guint         property_id,
                                 GValue       *value,
                                 GParamSpec   *pspec)
{
    LgwVocabularyListStore *self;
    LgwVocabularyListStorePrivate *priv;

    self = LGW_VOCABULARYLISTSTORE (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_PREFERENCES:
        g_value_set_object (value, lgw_vocabularyliststore_get_preferences (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}



static void
lgw_vocabularyliststore_class_init (LgwVocabularyListStoreClass *klass)
{
    //Declarations
    GObjectClass *object_class = NULL;
    LgwVocabularyListStoreClassPrivate *klasspriv = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->set_property = lgw_vocabularyliststore_set_property;
    object_class->get_property = lgw_vocabularyliststore_get_property;
    klass->priv = g_new0(LgwVocabularyListStoreClassPrivate, 1);
    klasspriv = klass->priv;

    object_class->finalize = lgw_vocabularyliststore_finalize;

    g_type_class_add_private (object_class, sizeof (LgwVocabularyListStorePrivate));

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
lgw_vocabularyliststore_load (LgwVocabularyListStore *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (self));    

    //Declarations
    LgwVocabularyListStorePrivate *priv = NULL;
    LgwVocabularyListStoreClass *klass = NULL;
    LgwVocabularyListStoreClassPrivate *klasspriv = NULL;
    gchar **filenames = NULL;
    GList *wordstorelist = NULL;

    //Initializations
    priv = self->priv;
    klass = LGW_VOCABULARYLISTSTORE_GET_CLASS (self);
    klasspriv = klass->priv;
    filenames = lw_vocabulary_get_filenames ();
    if (filenames == NULL) goto errored;

    {
      gint i = 0;
      for (i = 0; filenames[i] != NULL; i++)
      {
        LgwVocabularyWordStore *s = lgw_vocabularywordstore_new (filenames[i]);
        LwVocabulary *v = LW_VOCABULARY (s);
        v->row.saved_index = i;
        wordstorelist = g_list_prepend (wordstorelist, s);
      }
      wordstorelist = g_list_reverse (wordstorelist);
    }

    lgw_vocabularyliststore_clear (self);
    lgw_vocabularyliststore_insert (self, 0, wordstorelist);

errored:

    g_strfreev (filenames); filenames = NULL;
}


static void
_wordstore_filename_changed (LgwVocabularyListStore *self,
                             const gchar            *OLD_FILENAME,
                             LgwVocabularyWordStore *vocabulary_word_store)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (self));
    g_return_if_fail (LGW_IS_VOCABULARYWORDSTORE (vocabulary_word_store));

    //Declarations
    LgwVocabularyListStorePrivate *priv = NULL;
    LwVocabulary *vocabulary = NULL;
    gchar *filename = NULL;

    //Initializations
    priv = self->priv;
    vocabulary = LW_VOCABULARY (vocabulary_word_store);
    if (vocabulary == NULL) goto errored;
    filename = g_strdup (lw_vocabulary_get_filename (vocabulary));

    if (OLD_FILENAME != NULL)
    {
      g_hash_table_remove (priv->data.index.filename, OLD_FILENAME);
    }
    if (filename != NULL)
    {
      g_hash_table_insert (priv->data.index.filename, filename, vocabulary_word_store);
    }

errored:

    if (filename != NULL) g_free (filename); filename = NULL;
}


static void
_wordstore_changed (LgwVocabularyListStore *self,
                    GParamSpec             *pspec,
                    LgwVocabularyWordStore *vocabulary_word_store)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (self));
    g_return_if_fail (LGW_IS_VOCABULARYWORDSTORE (vocabulary_word_store));

    //Declarations
    LgwVocabularyListStorePrivate *priv = NULL;
    LwVocabulary *vocabulary = NULL;
    GtkTreeModel *tree_model = NULL;
    GtkTreePath *tree_path = NULL;
    GtkTreeIter iter;

    //Initializations
    priv = self->priv;
    vocabulary = LW_VOCABULARY (vocabulary_word_store);
    if (vocabulary == NULL) goto errored;
    tree_model = GTK_TREE_MODEL (self);
    if (tree_model == NULL) goto errored;
    tree_path = lgw_vocabularyliststore_find_by_wordstore (self, vocabulary_word_store);
    if (tree_path == NULL) goto errored;
    gtk_tree_model_get_iter (tree_model, &iter, tree_path);

    g_signal_emit_by_name (
      G_OBJECT (self),
      "row-changed",
      tree_path,
      &iter,
      NULL
    );

errored:

    if (tree_path != NULL) gtk_tree_path_free (tree_path); tree_path = NULL;
}


static void
_add_to_index (LgwVocabularyListStore *self,
               LgwVocabularyWordStore *vocabulary_word_store)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (self));
    g_return_if_fail (LGW_IS_VOCABULARYWORDSTORE (vocabulary_word_store));
    
    //Declarations
    LgwVocabularyListStorePrivate *priv = NULL;
    LwVocabulary *vocabulary = NULL;

    //Initializations
    priv = self->priv;
    vocabulary = LW_VOCABULARY (vocabulary_word_store);
    if (vocabulary == NULL) goto errored;
   
    {
      gchar *filename = g_strdup (lw_vocabulary_get_filename (vocabulary));
      if (filename != NULL)
      {
        g_hash_table_insert (priv->data.index.filename, filename, vocabulary_word_store);
      }
    }

    if (vocabulary_word_store != NULL)
    {
      g_hash_table_insert (priv->data.index.wordstore, vocabulary, vocabulary_word_store);
    }

    if (!g_hash_table_contains (priv->data.index.update_filename_callback, vocabulary_word_store))
    {
      guint signalid = g_signal_connect_swapped (G_OBJECT (vocabulary_word_store), "filename-changed", G_CALLBACK (_wordstore_filename_changed), self);
      g_hash_table_insert (priv->data.index.update_filename_callback, vocabulary_word_store, GUINT_TO_POINTER (signalid));
    }

    if (!g_hash_table_contains (priv->data.index.changed_callback, vocabulary_word_store))
    {
      guint signalid = g_signal_connect_swapped (G_OBJECT (vocabulary_word_store), "notify::changed", G_CALLBACK (_wordstore_changed), self);
      g_hash_table_insert (priv->data.index.changed_callback, vocabulary_word_store, GUINT_TO_POINTER (signalid));
    }

errored:

    return;
}


static void
_remove_from_index (LgwVocabularyListStore *self,
                    LgwVocabularyWordStore *vocabulary_word_store)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (self));
    g_return_if_fail (LGW_IS_VOCABULARYWORDSTORE (vocabulary_word_store));
    
    //Declarations
    LgwVocabularyListStorePrivate *priv = NULL;
    LwVocabulary *vocabulary = NULL;
    const gchar *FILENAME = NULL;

    //Initializations
    priv = self->priv;
    vocabulary = LW_VOCABULARY (vocabulary_word_store);
    if (vocabulary == NULL) goto errored;
    FILENAME = lw_vocabulary_get_filename (vocabulary);

    if (g_hash_table_contains (priv->data.index.update_filename_callback, vocabulary_word_store))
    {
      guint signalid = GPOINTER_TO_UINT (g_hash_table_lookup (priv->data.index.update_filename_callback, vocabulary_word_store));
      if (signalid > 0)
      {
        g_signal_handler_disconnect (G_OBJECT (vocabulary_word_store), signalid);
        g_hash_table_remove (priv->data.index.update_filename_callback, vocabulary_word_store);
      }
    }

    if (g_hash_table_contains (priv->data.index.changed_callback, vocabulary_word_store))
    {
      guint signalid = GPOINTER_TO_UINT (g_hash_table_lookup (priv->data.index.changed_callback, vocabulary_word_store));
      if (signalid > 0)
      {
        g_signal_handler_disconnect (G_OBJECT (vocabulary_word_store), signalid);
        g_hash_table_remove (priv->data.index.changed_callback, vocabulary_word_store);
      }
    }

    if (FILENAME != NULL)
    {
      g_hash_table_remove (priv->data.index.filename, FILENAME);
    }
    if (vocabulary_word_store != NULL)
    {
      g_hash_table_remove (priv->data.index.wordstore, vocabulary_word_store);
    }

errored:

    return;
}


static void
_rebuild_array (LgwVocabularyListStore *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (self));

    lgw_vocabularyliststore_invalidate_length (self);

    //Declarations
    LgwVocabularyListStorePrivate *priv = NULL;
    gint length = 0;

    //Initializations
    priv = self->priv;
    length = lgw_vocabularyliststore_length (self);

    g_free (priv->data.array); priv->data.array = NULL;
    priv->data.array = g_new0 (GList*, length);

    {
      GList *link = NULL;
      gint i = 0;
      LgwVocabularyWordStore *vocabulary_word_store = NULL;
      LwVocabulary *vocabulary = NULL;
      for (link = priv->data.list; link != NULL; link = link->next)
      {
        vocabulary_word_store = LGW_VOCABULARYWORDSTORE (link->data);
        vocabulary = LW_VOCABULARY (vocabulary_word_store);
        priv->data.array[i] = link;
        vocabulary->row.current_index = i;
        i++;
      }
    }

    lgw_vocabularyliststore_invalidate_length (self);
}


static gint
_insert (LgwVocabularyListStore *self,
             gint                   *position,
             GList                  *wordstorelist)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (self));
    if (wordstorelist == NULL) return 0;

    //Declarations
    LgwVocabularyListStorePrivate *priv = NULL;
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    gint number_inserted = 0;
    gint length = 0;
    gboolean append = FALSE;

    //Initializations
    priv = self->priv;
    number_inserted = g_list_length (wordstorelist);
    length = lgw_vocabularyliststore_length (self);
    append = (*position < 0 || *position >= length || length == 0);

    if (append)
    {
      *position = length;

      GList *copy = g_list_copy (wordstorelist);
      GList *link = NULL;
      for (link = copy; link != NULL; link = link->next)
      {
        LgwVocabularyWordStore *vocabulary_word_store = LGW_VOCABULARYWORDSTORE (link->data);
        if (vocabulary_word_store != NULL)
        {
          _add_to_index (self, vocabulary_word_store);
        }
        else
        {
          GList *next = link->next;
          copy = g_list_delete_link (copy, link);
          link = next;
          number_inserted--;
        }
      }
      g_list_foreach (copy, (GFunc) g_object_ref, NULL);
      priv->data.list = g_list_concat (priv->data.list, copy);
    }

    else
    {
      GList *insert_position = priv->data.array[*position];
      if (insert_position == NULL) goto errored;
      GList *link = NULL;
      for (link = g_list_last (wordstorelist); link != NULL; link = link->prev)
      {
        LgwVocabularyWordStore *vocabulary_word_store = LGW_VOCABULARYWORDSTORE (link->data);
        if (vocabulary_word_store != NULL)
        {
          g_object_ref (vocabulary_word_store);
          priv->data.list = g_list_insert_before (priv->data.list, insert_position, vocabulary_word_store);
          insert_position = insert_position->prev;
          _add_to_index (self, vocabulary_word_store);
        }
        else
        {
          number_inserted--;
        }
      }
    }

errored:

    lgw_vocabularyliststore_invalidate_length (self);

    return number_inserted;
}


static void
_insert_propogate_changes (LgwVocabularyListStore *self,
                               gint                    position,
                               gint                    number_inserted)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (self));
    if (number_inserted == 0) return;

    //Declarations
    GtkTreeModel *tree_model = NULL;
    gint length = 0;
    gint i = 0;
    GtkTreeIter iter;

    //Initializations
    tree_model = GTK_TREE_MODEL (self);
    length = lgw_vocabularyliststore_length (self);

    //Rows that were inserted
    for (i = position; i < position + number_inserted; i++)
    {
      GtkTreePath *path = gtk_tree_path_new_from_indices (i, -1);
      if (path != NULL)
      {
        gtk_tree_model_get_iter (tree_model, &iter, path);
        g_signal_emit_by_name (G_OBJECT (self),
          "row-inserted",
          path,
          &iter,
          NULL
        );
        gtk_tree_path_free (path); path = NULL;
      }
    }

    //Rows with modified indexes
    for (i = position + number_inserted; i < length; i++)
    {
      GtkTreePath *path = gtk_tree_path_new_from_indices (i, -1);
      if (path != NULL) {
        gtk_tree_model_get_iter (tree_model, &iter, path);
        g_signal_emit_by_name (G_OBJECT (self),
          "row-changed",
          path,
          &iter,
          NULL
        );
        gtk_tree_path_free (path); path = NULL;
      }
    }

errored:

    return;
}


static gint*
_find_duplicates (LgwVocabularyListStore *self, 
                  gint                    position, 
                  GList                  *wordstorelist)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYLISTSTORE (self), NULL);
    if (wordstorelist == NULL) return NULL;

    //Declarations
    gint length = -1;
    gint *indices = NULL;

    //Initializations
    length = g_list_length (wordstorelist);
    if (position < 0) position = lgw_vocabularyliststore_length (self);
    indices = g_new0 (gint, length + 1);
    if (indices == NULL) goto errored;

    {
      GList *link = NULL;
      gint i = 0;
      for (link = wordstorelist; link != NULL; link = link->next)
      {
        LwVocabulary *a = LW_VOCABULARY (link->data);
        if (a == NULL) continue;
        LwVocabulary *b = LW_VOCABULARY (lgw_vocabularyliststore_nth (self, a->row.current_index));
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


GList*
lgw_vocabularyliststore_get_tree_paths (LgwVocabularyListStore *self,
                                        GList                  *wordstores)
{
    //Sanity checks
    if (wordstores == NULL) return NULL;

    //Declarations
    GList *tree_paths = NULL;

    {
      GList *link = NULL;
      for (link = wordstores; link != NULL; link = link->next)
      {
        LgwVocabularyWordStore *w = LGW_VOCABULARYWORDSTORE (link->data);
        if (w != NULL)
        {
          GtkTreePath *p = lgw_vocabularyliststore_find_by_wordstore (self, w);
          if (p != NULL)
          {
            tree_paths = g_list_prepend (tree_paths, p);
            p = NULL;
          }
        }
      }
      tree_paths = g_list_reverse (tree_paths);
    }

    return tree_paths;
}


GList*
lgw_vocabularyliststore_insert (LgwVocabularyListStore *self,
                                GtkTreePath            *tree_path,
                                GList                  *wordstores)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (self));
    if (wordstores == NULL) return NULL;

    //Declarations
    gint position = -1;
    gint *duplicates = NULL;
    gint number_inserted = 0;
    GList *removed = NULL;

    //Initializations
    if (tree_path != NULL)
      position = gtk_tree_path_get_indices (tree_path)[0];
    duplicates = _find_duplicates (self, position, wordstores);
    number_inserted = _insert (self, &position, wordstores);

    _rebuild_array (self);
    _insert_propogate_changes (self, position, number_inserted);

    if (duplicates == NULL) goto errored;

    removed = lgw_vocabularyliststore_remove_by_indices (self, duplicates);

errored:

    if (duplicates != NULL) g_free (duplicates); duplicates = NULL;
    if (removed != NULL) g_list_free (removed); removed = NULL;

    return lgw_vocabularyliststore_get_tree_paths (self, wordstores);
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
_sanitize_indices (LgwVocabularyListStore *self,
                   gint                   *indices)
{
    g_return_val_if_fail (LGW_IS_VOCABULARYLISTSTORE (self), NULL);
    if (indices == NULL) return NULL;

    //Declarations
    gint size = 0;
    gint length = 0;

    //Initializations
    length = lgw_vocabularyliststore_length (self);

    //Copy
    {
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
_remove (LgwVocabularyListStore *self,
         gint                   *indices)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (self));

    //Declarations
    LgwVocabularyListStorePrivate *priv = NULL;
    gint length = 0;
    GList *removed = NULL;

    //Initializations
    priv = self->priv;
    length = lgw_vocabularyliststore_length (self);
    if (length == 0) goto errored;

    //Remove from the objects main model
    {
      gint i = 0;
      for (i = 0; indices[i] != -1 && i < length; i++)
      {
        gint index = indices[i];
        GList *link = priv->data.array[index];
        if (link != NULL) 
        {
          LgwVocabularyWordStore *vocabulary_word_store = LGW_VOCABULARYWORDSTORE (link->data);
          LwVocabulary *vocabulary = (link->data);

          priv->data.list = g_list_remove_link (priv->data.list, link);
          removed = g_list_concat (link, removed);
          priv->data.array[index] = NULL;

          if (vocabulary != NULL && vocabulary->row.current_index == index)
          {
            _remove_from_index (self, vocabulary_word_store);
            g_object_unref (vocabulary_word_store);
          }
        }
      }
      removed = g_list_reverse (removed);
    }

errored:

    lgw_vocabularyliststore_invalidate_length (self);

    return removed;
}


static void
_remove_propogate_changes (LgwVocabularyListStore *self,
                           gint                   *indices)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (self));
    if (indices == NULL) return;

    //Declarations
    GtkTreeModel *tree_model = NULL;
    gint length = 0;
    gint i = 0;
    GtkTreeIter iter;

    //Initializations
    tree_model = GTK_TREE_MODEL (self);
    length = lgw_vocabularyliststore_length (self);

    //Rows that were removed
    for (i = 0; indices[i] != -1 && i < length; i++)
    {
      GtkTreePath *path = gtk_tree_path_new_from_indices (indices[i], -1);
      if (path != NULL) {
        g_signal_emit_by_name (G_OBJECT (self),
          "row-deleted",
          path,
          NULL
        );
        gtk_tree_path_free (path); path = NULL;
      }
    }
    i--;

    //Rows with modified indexes
    {
      gint index = 0;
      for (index = indices[i]; index < length; index++)
      {
        GtkTreePath *path = gtk_tree_path_new_from_indices (index, -1);
        if (path != NULL) {
          gtk_tree_model_get_iter (tree_model, &iter, path);
          g_signal_emit_by_name (G_OBJECT (self),
            "row-changed",
            path,
            &iter,
            NULL
          );
          gtk_tree_path_free (path); path = NULL;
        }
      }
    }
}


GList*
lgw_vocabularyliststore_remove (LgwVocabularyListStore *self,
                                GList                  *tree_paths)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (self));
    if (tree_paths == NULL) return NULL;

    //Declarations
    gint *indices = NULL;
    GList *removed = NULL;

    //Initializations
    indices = lgw_vocabularyliststore_tree_paths_to_indices (self, tree_paths);
    if (indices == NULL) goto errored;
    removed = lgw_vocabularyliststore_remove_by_indices (self, indices);
    if (removed == NULL) goto errored;

errored:

    return removed;
}


GList*
lgw_vocabularyliststore_remove_by_indices (LgwVocabularyListStore *self,
                                           gint                   *indices)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (self));
    if (indices == NULL) return NULL;

    //Declarations
    GList *removed = NULL;

    //Initializations
    indices = _sanitize_indices (self, indices);
    if (indices == NULL) goto errored;

    removed = _remove (self, indices);
    _rebuild_array (self);
    _remove_propogate_changes (self, indices);

errored:

    if (indices != NULL) g_free (indices); indices = NULL;

    return removed;
}


GList*
lgw_vocabularyliststore_delete (LgwVocabularyListStore *self,
                                GList                  *tree_paths)
{
printf("BREAK0 lgw_vocabularyliststore_delete\n");
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (self));
    if (tree_paths == NULL) return NULL;
printf("BREAK1 lgw_vocabularyliststore_delete\n");


    //Declarations
    gint *indices = NULL;
    GList *removed = NULL;

    //Initializations
    indices = lgw_vocabularyliststore_tree_paths_to_indices (self, tree_paths);
    if (indices == NULL) goto errored;
printf("BREAK2 lgw_vocabularyliststore_delete\n");
    removed = lgw_vocabularyliststore_delete_by_indices (self, indices);
    if (removed == NULL) goto errored;
printf("BREAK3 lgw_vocabularyliststore_delete\n");

errored:

    return removed;
}


GList*
lgw_vocabularyliststore_delete_by_indices (LgwVocabularyListStore *self,
                                           gint                   *indices)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (indices != NULL, NULL);

    //Declarations
    GList* removed = NULL;
    
    //Initializations
    removed = lgw_vocabularyliststore_remove_by_indices (self, indices);
    
    {
      GList *link = NULL;
      for (link = removed; link != NULL; link = link->next)
      {
        LwVocabulary *vocabulary = LW_VOCABULARY (link->data);
        lw_vocabulary_set_filename (vocabulary, NULL);
      }
    }

    return removed;
}


void
lgw_vocabularyliststore_clear (LgwVocabularyListStore *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (self));

    //Declarations
    gint length = 0;
    gint* indices = NULL;
    GList *removed = NULL;

    //Initializations
    length = lgw_vocabularyliststore_length (self);
    indices = g_new0 (gint, length + 1);

    {
      gint i = 0;
      for (i = 0; i < length; i++)
      {
        indices[i] = i;
      }
      indices[i] = -1;
    }

    removed = lgw_vocabularyliststore_remove_by_indices (self, indices);

errored:

    if (indices != NULL) g_free (indices); indices = NULL;
    if (removed != NULL) g_list_free_full (removed, (GDestroyNotify) g_object_unref);
}


void
lgw_vocabularyliststore_set_preferences (LgwVocabularyListStore *self,
                                         LwPreferences          *preferences)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (self));

    //Declarations
    LgwVocabularyListStorePrivate *priv = NULL;
    LgwVocabularyListStoreClass *klass = NULL;
    LgwVocabularyListStoreClassPrivate *klasspriv = NULL;

    //Initializations
    priv = self->priv;
    klass = LGW_VOCABULARYLISTSTORE_GET_CLASS (self);
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
lgw_vocabularyliststore_get_preferences (LgwVocabularyListStore *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (self));

    //Declarations
    LgwVocabularyListStorePrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (priv->config.preferences == NULL)
    {
      lgw_vocabularyliststore_set_preferences (self, lw_preferences_get_default ());
    }

    return priv->config.preferences;
}


gint
lgw_vocabularyliststore_length (LgwVocabularyListStore *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (self));

    //Declarations
    LgwVocabularyListStorePrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (priv->data.length < 0)
    {
      priv->data.length = g_list_length (priv->data.list);
    }

    return priv->data.length;
}


void
lgw_vocabularyliststore_invalidate_length (LgwVocabularyListStore *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (self));

    //Declarations
    LgwVocabularyListStorePrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    priv->data.length = -1;
}


GtkTreePath*
lgw_vocabularyliststore_find_by_wordstore (LgwVocabularyListStore *self,
                                           LgwVocabularyWordStore *vocabulary_word_store)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (self));
    if (vocabulary_word_store == NULL) return NULL;

    //Declarations
    LgwVocabularyListStorePrivate *priv = NULL;
    LwVocabulary *vocabulary = NULL;
    gint position = 0;
    GtkTreePath *tree_path = NULL;

    //Initializations
    priv = self->priv;
    vocabulary = LW_VOCABULARY (vocabulary_word_store);
    if (vocabulary == NULL) goto errored;
    position = vocabulary->row.current_index;
    if (position < 0) goto errored;

    tree_path = gtk_tree_path_new_from_indices (position, -1);

errored:

   return tree_path; 
}


GtkTreePath*
lgw_vocabularyliststore_find_by_filename (LgwVocabularyListStore *self,
                                          const gchar            *FILENAME)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (self));
    if (FILENAME == NULL) return NULL;

    //Declarations
    LgwVocabularyListStorePrivate *priv = NULL;
    LwVocabulary *vocabulary = NULL;
    gint position = 0;
    GtkTreePath *tree_path = NULL;

    //Initializations
    priv = self->priv;
    vocabulary = LW_VOCABULARY (g_hash_table_lookup (priv->data.index.filename, FILENAME));
    if (vocabulary == NULL) goto errored;
    position = vocabulary->row.current_index;
    if (position < 0) goto errored;

    tree_path = gtk_tree_path_new_from_indices (position, -1);

errored:

   return tree_path; 
}


LgwVocabularyWordStore*
lgw_vocabularyliststore_get_wordstore (LgwVocabularyListStore *self,
                                       GtkTreePath            *tree_path)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (self));

    //Declarations
    GtkTreeModel *tree_model = NULL;
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    GtkTreePath *internal_tree_path = NULL;
    GtkTreeIter iter;

    //Initializations
    tree_model = GTK_TREE_MODEL (self);
    internal_tree_path = gtk_tree_path_new_from_indices (lgw_vocabularyliststore_length (self), -1);
    if (tree_path == NULL) tree_path = internal_tree_path;

    if (gtk_tree_model_get_iter (tree_model, &iter, tree_path))
    {
      GObject *object = NULL;
      gtk_tree_model_get (tree_model, &iter, LGW_VOCABULARYLISTSTORE_COLUMN_OBJECT, &object, -1);
      vocabulary_word_store = LGW_VOCABULARYWORDSTORE (object);
      g_object_unref (object);
    }

errored:

    if (internal_tree_path != NULL) gtk_tree_path_free (internal_tree_path); internal_tree_path = NULL;
    tree_path = NULL;

    return vocabulary_word_store;
}


LgwVocabularyWordStore*
lgw_vocabularyliststore_nth (LgwVocabularyListStore *self,
                             gint                    index)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYLISTSTORE (self), NULL);
    if (index < 0) return NULL;

    //Declarations
    LgwVocabularyListStorePrivate *priv = NULL;
    gint length = 0;
    GList *link = NULL;
    LgwVocabularyWordStore *vocabulary_word_store = NULL;

    //Initializations
    priv = self->priv;
    length = lgw_vocabularyliststore_length (self);
    if (index >= length) goto errored;

    link = priv->data.array[index];
    if (link == NULL) goto errored;
    vocabulary_word_store = LGW_VOCABULARYWORDSTORE (link->data);

errored:

    return vocabulary_word_store;
}


void
lgw_vocabularyliststore_set (LgwVocabularyListStore *self,
                             GtkTreePath            *tree_path,
                             ...)
{
    //Declarations
    va_list va;

    //Initializations
    va_start(va, tree_path);

    lgw_vocabularyliststore_set_valist (self, tree_path, va); 
}


void
lgw_vocabularyliststore_set_valist (LgwVocabularyListStore *self,
                                    GtkTreePath            *tree_path,
                                    va_list va)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (self));
    g_return_if_fail (tree_path != NULL);

    //Declarations
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    LwVocabulary *vocabulary = NULL;
    gint index = -1;

    //Initializations
    index = gtk_tree_path_get_indices (tree_path)[0];
    vocabulary_word_store = lgw_vocabularyliststore_get_wordstore (self, tree_path);
    if (vocabulary_word_store == NULL) goto errored;
    vocabulary = LW_VOCABULARY (vocabulary_word_store);

    while (TRUE)
    {
      LgwVocabularyListStoreColumn field = va_arg(va, LgwVocabularyListStoreColumn);
      if (field < 0 || field >= TOTAL_LW_WORD_FIELDS) goto errored;
      gchar *value = va_arg(va, gchar*);
      if (value == NULL) goto errored;

      switch (field)
      {
        case LGW_VOCABULARYLISTSTORE_COLUMN_FILENAME:
          lw_vocabulary_set_filename (vocabulary, value);
          break;
        default:
          g_assert_not_reached ();
          break;
      }
    }

errored:

    return;
}


gint*
lgw_vocabularyliststore_tree_paths_to_indices (LgwVocabularyListStore *self,
                                               GList                  *tree_paths)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYLISTSTORE (self), NULL);
    if (tree_paths == NULL) return NULL;

    //Declarations
    LwVocabulary *vocabulary = NULL;
    gint length = -1;
    gint *indices = NULL;


    //Initializations
    length = g_list_length (tree_paths);
    if (length < 1) goto errored;
    indices = g_new0 (gint, length + 1);
    if (indices == NULL) goto errored;

    {
      GList *link = NULL;
      gint i = 0;
      for (link = tree_paths; link != NULL; link = link->next)
      {
        GtkTreePath *path = link->data;
        if (path != NULL)
        {
          indices[i++] = gtk_tree_path_get_indices (path)[0];
        }
      }
      indices[i++] = -1;
    }

errored:

    return indices;
}


GList*
lgw_vocabularyliststore_indices_to_tree_paths (LgwVocabularyListStore *self,
                                               gint                   *indices)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYLISTSTORE (self), NULL);
    if (indices == NULL) return NULL;

    //Declarations
    GList *tree_paths = NULL;

    //Initializations
    
    {
      gint i = 0;
      for (i = 0; indices[i] > 0; i++)
      {
        GtkTreePath *tree_path = gtk_tree_path_new_from_indices (indices[i], -1);
        if (tree_path != NULL)
        {
          tree_paths = g_list_prepend (tree_paths, tree_path);
        }
      }
      tree_paths = g_list_reverse (tree_paths);
    }

    return tree_paths;
}

