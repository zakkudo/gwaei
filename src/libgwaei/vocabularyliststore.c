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


G_DEFINE_TYPE_WITH_CODE (LgwVocabularyListStore, lgw_vocabularyliststore, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_MODEL, lgw_vocabularyliststore_init_interface));



LgwVocabularyListStore*
lgw_vocabularyliststore_new (LwPreferences *preferences)
{
    //Declarations
    LgwVocabularyListStore *vocabulary_list_store = NULL;

    //Initializations
    vocabulary_list_store = LGW_VOCABULARYLISTSTORE (g_object_new (LGW_TYPE_VOCABULARYLISTSTORE, "preferences", preferences, NULL));

    return vocabulary_list_store;
}


static void 
lgw_vocabularyliststore_init (LgwVocabularyListStore *vocabulary_list_store)
{
    vocabulary_list_store->priv = LGW_VOCABULARYLISTSTORE_GET_PRIVATE (vocabulary_list_store);
    memset(vocabulary_list_store->priv, 0, sizeof(LgwVocabularyListStorePrivate));

    //Declarations
    LgwVocabularyListStorePrivate *priv = NULL;
    gchar *path = NULL;

    //Initializations
    priv = vocabulary_list_store->priv;

    //TODO
    /*
    priv->file = g_file_new_for_path (path);
    priv->file_monitor = (priv->file, G_FILE_MONITOR_NONE, NULL, NULL);
    */

    lgw_vocabularyliststore_load (vocabulary_list_store);
}


static void 
lgw_vocabularyliststore_finalize (GObject *object)
{
    LgwVocabularyListStore *vocabulary_list_store;
    LgwVocabularyListStorePrivate *priv;

    vocabulary_list_store = LGW_VOCABULARYLISTSTORE (object);
    priv = vocabulary_list_store->priv;

    G_OBJECT_CLASS (lgw_vocabularyliststore_parent_class)->finalize (object);
}


static void
lgw_vocabularyliststore_set_property (GObject      *object,
                                      guint         property_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
    //Declarations
    LgwVocabularyListStore *vocabulary_list_store = NULL;
    LgwVocabularyListStorePrivate *priv = NULL;

    //Initializations
    vocabulary_list_store = LGW_VOCABULARYLISTSTORE (object);
    priv = vocabulary_list_store->priv;

    switch (property_id)
    {
      case PROP_PREFERENCES:
        lgw_vocabularyliststore_set_preferences (vocabulary_list_store, g_value_get_object (value));
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
    LgwVocabularyListStore *vocabulary_list_store;
    LgwVocabularyListStorePrivate *priv;

    vocabulary_list_store = LGW_VOCABULARYLISTSTORE (object);
    priv = vocabulary_list_store->priv;

    switch (property_id)
    {
      case PROP_PREFERENCES:
        g_value_set_object (value, lgw_vocabularyliststore_get_preferences (vocabulary_list_store));
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
lgw_vocabularyliststore_load (LgwVocabularyListStore *vocabulary_list_store)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (vocabulary_list_store));    

    //Declarations
    LgwVocabularyListStorePrivate *priv = NULL;
    LgwVocabularyListStoreClass *klass = NULL;
    LgwVocabularyListStoreClassPrivate *klasspriv = NULL;
    gchar **filenames = NULL;
    GList *wordstorelist = NULL;

    //Initializations
    priv = vocabulary_list_store->priv;
    klass = LGW_VOCABULARYLISTSTORE_GET_CLASS (vocabulary_list_store);
    klasspriv = klass->priv;
    filenames = lw_vocabulary_get_filenames ();
    if (filenames == NULL) goto errored;

    {
      gint i = 0;
      for (i = 0; filenames[i] != NULL; i++)
      {
        LgwVocabularyWordStore *s = lgw_vocabularywordstore_new (filenames[i]);
        wordstorelist = g_list_prepend (wordstorelist, s);
      }
      wordstorelist = g_list_reverse (wordstorelist);
    }

    lgw_vocabularyliststore_clear (vocabulary_list_store);
    lgw_vocabularyliststore_insert_all (vocabulary_list_store, 0, wordstorelist);

errored:

    g_strfreev (filenames); filenames = NULL;
}


void
lgw_vocabularyliststore_insert_all (LgwVocabularyListStore *vocabulary_list_store,
                                    gint                    position,
                                    GList                  *wordstorelist)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (vocabulary_list_store));
    if (wordstorelist == NULL) return;

    //Declarations
    LgwVocabularyListStorePrivate *priv = NULL;
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    gint add_length = 0;
    gint length = lgw_vocabularyliststore_length (vocabulary_list_store);
    gboolean append = (position < 0 || position >= length || length == 0);

    //Initializations
    priv = vocabulary_list_store->priv;
    add_length = g_list_length (wordstorelist);

    if (append)
    {
      position = length;

      GList *copy = g_list_copy (wordstorelist);
      GList *link = NULL;
      for (link = copy; link != NULL; link = link->next)
      {
        if (!LGW_IS_VOCABULARYWORDSTORE (link->data))
        {
          GList *next = link->next;
          copy = g_list_delete_link (copy, link);
          link = next;
          add_length--;
        }
      }
      priv->data.list = g_list_concat (priv->data.list, copy);
    }
    else
    {
      GList *insert_position = g_list_nth (priv->data.list, position);
      if (insert_position == NULL) goto errored;
      GList *link = NULL;
      for (link = g_list_last (wordstorelist); link != NULL; link = link->prev)
      {
        if (LGW_IS_VOCABULARYWORDSTORE (link->data))
        {
          priv->data.list = g_list_insert_before (priv->data.list, insert_position, link->data);
          insert_position = insert_position->prev;
        }
        else
        {
          add_length--;
        }
      }
    }

    priv->data.length = -1;
    printf("BREAK %d\n", g_list_length (priv->data.list));

    {
      gint i = 0;
      for (i = 0; i < add_length; i++)
      {
        GtkTreeIter iter;
        lgw_vocabularyliststore_initialize_tree_iter (vocabulary_list_store, &iter, position + i);
        GtkTreePath *path = gtk_tree_path_new_from_indices (position + i, -1);

        if (path != NULL) {
          printf("BREAK adding %d %d\n", position, i);
          g_signal_emit_by_name (G_OBJECT (vocabulary_list_store),
            "row-inserted",
            path,
            &iter,
            NULL
          );
          gtk_tree_path_free (path); path = NULL;
        }
      }
    }

errored:

    return;
}


//Sorts in decending order
static gint
_remove_sort (gconstpointer a, gconstpointer b)
{
    gint ia = GPOINTER_TO_INT (a);
    gint ib = GPOINTER_TO_INT (b);

    if (ia > ib) return -1;
    if (ia < ib) return 1;

    return 0;
}


void
lgw_vocabularyliststore_remove_all (LgwVocabularyListStore *vocabulary_list_store,
                                    gint                   *positions)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (vocabulary_list_store));
    if (positions == NULL) return;

    //Declarations
    LgwVocabularyListStorePrivate *priv = NULL;
    gint length = 0;

    //Initializations
    priv = vocabulary_list_store->priv;
    length = lgw_vocabularyliststore_length (vocabulary_list_store);


    //Create sorted remove list
    GList *remove_list = NULL;
    gint *p = positions;
    for (p = positions; *p > -1 && *p < length && p - positions < length; p++)
    {
      remove_list = g_list_prepend (remove_list, GINT_TO_POINTER (*p));
    }
    remove_list = g_list_sort (remove_list, _remove_sort);
    

    //Index the list
    GHashTable *hash = g_hash_table_new (g_direct_hash, g_direct_equal);
    {
      GList *link = NULL;
      gint i = 0;
      for (link = priv->data.list; link != NULL; link = link->next)
      {
        g_hash_table_insert (hash, GINT_TO_POINTER (i), link);
        i++;
      }
    }


    //Remove from the objects main model
    {
      GList *remove_link = NULL;
      for (remove_link = remove_list; remove_link != NULL; remove_link = remove_link->next)
      {
        GList *link = g_hash_table_lookup (hash, remove_link->data);
        if (link != NULL) 
        {
          g_object_unref (G_OBJECT (link->data));
          priv->data.list = g_list_delete_link (priv->data.list, link);
          g_hash_table_remove (hash, GINT_TO_POINTER (*p));
        }
      }
    }

    //Update the GtkTreeModel
    {
      GList *remove_link = NULL;
      for (remove_link = remove_list; remove_link != NULL; remove_link = remove_link->next)
      {
        gint index = GPOINTER_TO_INT (remove_link->data);
        GtkTreePath *path = gtk_tree_path_new_from_indices (index, -1);
        if (path != NULL) {
          g_signal_emit_by_name (G_OBJECT (vocabulary_list_store),
            "row-deleted",
            path,
            NULL
          );
          gtk_tree_path_free (path); path = NULL;
        }
      }
    }

errored:

    if (hash != NULL) g_hash_table_unref (hash); hash = NULL;
    if (remove_list != NULL) g_list_free (remove_list); remove_list = NULL;
    priv->data.length = -1;
}


void
lgw_vocabularyliststore_clear (LgwVocabularyListStore *vocabulary_list_store)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (vocabulary_list_store));

    //Declarations
    LgwVocabularyListStorePrivate *priv = NULL;
    gint length = 0;

    //Initializations
    priv = vocabulary_list_store->priv;
    length = lgw_vocabularyliststore_length (vocabulary_list_store);

    //Update the internal model
    g_list_free_full (priv->data.list, (GDestroyNotify) g_object_unref);
    priv->data.list = NULL;

    //Update the GtkTreeModel
    {
      gint i = 0;
      for (i = length - 1; i > -1; i--)
      {
        GtkTreePath *path = gtk_tree_path_new_from_indices (i, -1);
        if (path != NULL) {
          g_signal_emit_by_name (G_OBJECT (vocabulary_list_store),
            "row-deleted",
            path,
            NULL
          );
          gtk_tree_path_free (path); path = NULL;
        }
      }
    }

    priv->data.length = -1;
}


void
lgw_vocabularyliststore_set_preferences (LgwVocabularyListStore *vocabulary_list_store,
                                         LwPreferences          *preferences)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (vocabulary_list_store));

    //Declarations
    LgwVocabularyListStorePrivate *priv = NULL;
    LgwVocabularyListStoreClass *klass = NULL;
    LgwVocabularyListStoreClassPrivate *klasspriv = NULL;

    //Initializations
    priv = vocabulary_list_store->priv;
    klass = LGW_VOCABULARYLISTSTORE_GET_CLASS (vocabulary_list_store);
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

    g_object_notify_by_pspec (G_OBJECT (vocabulary_list_store), klasspriv->pspec[PROP_PREFERENCES]);
}


LwPreferences*
lgw_vocabularyliststore_get_preferences (LgwVocabularyListStore *vocabulary_list_store)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (vocabulary_list_store));

    //Declarations
    LgwVocabularyListStorePrivate *priv = NULL;

    //Initializations
    priv = vocabulary_list_store->priv;

    if (priv->config.preferences == NULL)
    {
      lgw_vocabularyliststore_set_preferences (vocabulary_list_store, lw_preferences_get_default ());
    }

    return priv->config.preferences;
}


gint
lgw_vocabularyliststore_length (LgwVocabularyListStore *vocabulary_list_store)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (vocabulary_list_store));

    //Declarations
    LgwVocabularyListStorePrivate *priv = NULL;

    //Initializations
    priv = vocabulary_list_store->priv;

    if (priv->data.length < 0)
    {
      priv->data.length = g_list_length (priv->data.list);
    }

    return priv->data.length;
}
