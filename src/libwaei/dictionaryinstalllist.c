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
//! @file dictionaryinstalllist.c
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

#include <libwaei/dictionaryinstalllist-private.h>

G_DEFINE_TYPE (LwDictionaryInstallList, lw_dictionaryinstalllist, G_TYPE_OBJECT)


//!
//! @brief Creates a new LwDictionaryInstallList object
//! @param MAX The maximum items you want in the dictionary before old ones are deleted
//! @return An allocated LwDictionaryInstallList that will be needed to be freed by lw_dictionary_free.
//!
LwDictionaryInstallList* 
lw_dictionaryinstalllist_new (LwPreferences *preferences)
{
    //Declarations
    LwDictionaryInstallList *dictionary = NULL;

    //Initializations
    dictionary = LW_DICTIONARYINSTALLLIST (g_object_new (LW_TYPE_DICTIONARYINSTALLLIST, "preferences", preferences, NULL));

    return dictionary;
}


static void 
lw_dictionaryinstalllist_init (LwDictionaryInstallList *self)
{
    self->priv = LW_DICTIONARYINSTALLLIST_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LwDictionaryInstallListPrivate));

    LwDictionaryInstallListPrivate *priv = NULL;

    priv = self->priv;

    priv->data.index.typename = g_hash_table_new_full (g_str_hash, g_str_equal, (GDestroyNotify) g_free, NULL);
    priv->data.index.filename = g_hash_table_new_full (g_str_hash, g_str_equal, (GDestroyNotify) g_free, NULL);
    priv->data.index.id = g_hash_table_new_full (g_str_hash, g_str_equal, (GDestroyNotify) g_free, NULL);

    lw_dictionaryinstalllist_connect_signals (self);
}


static void 
lw_dictionaryinstalllist_set_property (GObject      *object,
                                       guint         property_id,
                                       const GValue *value,
                                       GParamSpec   *pspec)
{
    //Declarations
    LwDictionaryInstallList *self = NULL;
    LwDictionaryInstallListPrivate *priv = NULL;

    //Initializations
    self = LW_DICTIONARYINSTALLLIST (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_PREFERENCES:
        lw_dictionaryinstalllist_set_preferences (self, g_value_get_object (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_dictionaryinstalllist_get_property (GObject      *object,
                                       guint         property_id,
                                       GValue       *value,
                                       GParamSpec   *pspec)
{
    //Declarations
    LwDictionaryInstallList *self = NULL;
    LwDictionaryInstallListPrivate *priv = NULL;

    //Initializations
    self = LW_DICTIONARYINSTALLLIST (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_PREFERENCES:
        g_value_set_object (value, lw_dictionaryinstalllist_get_preferences (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_dictionaryinstalllist_finalize (GObject *object)
{
    //Declarations
    LwDictionaryInstallList *self = NULL;
    LwDictionaryInstallListPrivate *priv = NULL;

    //Initalizations
    self = LW_DICTIONARYINSTALLLIST (object);
    priv = self->priv;

    G_OBJECT_CLASS (lw_dictionaryinstalllist_parent_class)->finalize (object);
}


static void
lw_dictionaryinstalllist_dispose (GObject *object)
{
    //Declarations
    LwDictionaryInstallList *self = NULL;

    //Initializations
    self = LW_DICTIONARYINSTALLLIST (object);

    lw_dictionaryinstalllist_clear (self);
    lw_dictionaryinstalllist_disconnect_signals (self);

    G_OBJECT_CLASS (lw_dictionaryinstalllist_parent_class)->dispose (object);
}



static void
lw_dictionaryinstalllist_class_init (LwDictionaryInstallListClass *klass)
{
    //Declarations
    GObjectClass *object_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    klass->priv = g_new0 (LwDictionaryInstallListClassPrivate, 1);
    object_class->set_property = lw_dictionaryinstalllist_set_property;
    object_class->get_property = lw_dictionaryinstalllist_get_property;
    object_class->dispose = lw_dictionaryinstalllist_dispose;
    object_class->finalize = lw_dictionaryinstalllist_finalize;

    g_type_class_add_private (object_class, sizeof (LwDictionaryInstallListPrivate));

    LwDictionaryInstallListClassPrivate *klasspriv = klass->priv;

    klasspriv->signalid[CLASS_SIGNALID_ROW_CHANGED] = g_signal_new (
        "internal-row-changed",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwDictionaryInstallListClass, row_changed),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    klasspriv->signalid[CLASS_SIGNALID_ROW_INSERTED] = g_signal_new (
        "internal-row-inserted",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwDictionaryInstallListClass, row_inserted),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    klasspriv->signalid[CLASS_SIGNALID_ROW_DELETED] = g_signal_new (
        "internal-row-deleted",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwDictionaryInstallListClass, row_deleted),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    klasspriv->signalid[CLASS_SIGNALID_ROWS_REORDERED] = g_signal_new (
        "internal-rows-reordered",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwDictionaryInstallListClass, rows_reordered),
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
lw_dictionaryinstalllist_clear (LwDictionaryInstallList *self)
{
    //Sanity checks
    g_return_if_fail (LW_DICTIONARYINSTALLLIST (self));

    //Declarations
    LwDictionaryInstallListPrivate *priv = NULL;
    GList *removed = NULL;
    gint length = 0;

    //Initializations
    priv = self->priv;
    length = lw_dictionaryinstalllist_length (self);

    removed = lw_dictionaryinstalllist_remove (self, NULL);

errored:

    if (removed != NULL) g_list_free_full (removed, (GDestroyNotify) g_object_unref);
}


void
lw_dictionaryinstalllist_set_preferences (LwDictionaryInstallList *self,
                                          LwPreferences    *preferences)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALLLIST (self));

    //Declarations
    LwDictionaryInstallListPrivate *priv = NULL;
    LwDictionaryInstallListClass *klass = NULL;
    LwDictionaryInstallListClassPrivate *klasspriv = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARYINSTALLLIST_CLASS (self);
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
lw_dictionaryinstalllist_get_preferences (LwDictionaryInstallList *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYINSTALLLIST (self), NULL);

    //Declarations
    LwDictionaryInstallListPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (priv->config.preferences == NULL)
    {
      lw_dictionaryinstalllist_set_preferences (self, lw_preferences_get_default ());
    }

    return priv->config.preferences;
}


void
lw_dictionaryinstalllist_invalidate_length (LwDictionaryInstallList *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALLLIST (self));

    //Declarations
    LwDictionaryInstallListPrivate *priv = NULL;
    gint length = 0;

    //Initializations
    priv = self->priv;

    priv->data.length = -1;
}


static void
_add_to_index (LwDictionaryInstallList *self,
               LwDictionaryInstall     *dictionaryinstall)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALLLIST (self));
    g_return_if_fail (LW_IS_DICTIONARYINSTALL (dictionaryinstall));
    
    //Declarations
    LwDictionaryInstallListPrivate *priv = NULL;
    const gchar *FILENAME = NULL;
    const gchar *TYPENAME = NULL;
    GType type = G_TYPE_INVALID;
    gchar *id = NULL;
    gchar *normalized_filename = NULL;
    gchar *normalized_typename = NULL;
    gchar *normalized_id = NULL;

    //Initializations
    priv = self->priv;
    FILENAME = lw_dictionaryinstall_get_name (dictionaryinstall);
    type = lw_dictionaryinstall_get_gtype (dictionaryinstall);
    TYPENAME = g_type_name (type);
    id = g_strdup (lw_dictionaryinstall_get_id (dictionaryinstall));

    normalized_filename = lw_utf8_normalize (FILENAME, -1, LW_UTF8FLAG_ALL);
    normalized_typename = lw_utf8_normalize (TYPENAME, -1, LW_UTF8FLAG_ALL);
    normalized_id = lw_utf8_normalize (id, -1, LW_UTF8FLAG_ALL);
   
    if (normalized_filename != NULL && !g_hash_table_contains (priv->data.index.filename, normalized_filename))
    {
      g_hash_table_insert (priv->data.index.filename, normalized_filename, dictionaryinstall);
      normalized_filename = NULL;
    }

    if (normalized_typename != NULL && !g_hash_table_contains (priv->data.index.typename, normalized_typename))
    {
      g_hash_table_insert (priv->data.index.typename, normalized_typename, dictionaryinstall);
      normalized_typename = NULL;
    }

    if (normalized_id != NULL && !g_hash_table_contains (priv->data.index.id, normalized_id))
    {
      g_hash_table_insert (priv->data.index.id, normalized_id, dictionaryinstall);
      normalized_id = NULL;
    }

errored:

    g_free (normalized_filename); normalized_filename = NULL;
    g_free (normalized_typename); normalized_typename = NULL;
    g_free (id); id = NULL;
    g_free (normalized_id); normalized_id = NULL;

    return;
}


static void
_remove_from_index (LwDictionaryInstallList *self,
                    LwDictionaryInstall     *dictionaryinstall)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALLLIST (self));
    g_return_if_fail (LW_IS_DICTIONARYINSTALL (dictionaryinstall));
    
    //Declarations
    LwDictionaryInstallListPrivate *priv = NULL;
    const gchar *FILENAME = NULL;
    const gchar *TYPENAME = NULL;
    GType type = NULL;
    gchar *id = NULL;
    gchar *normalized_filename = NULL;
    gchar *normalized_typename = NULL;
    gchar *normalized_id = NULL;

    //Initializations
    priv = self->priv;
    FILENAME = lw_dictionaryinstall_get_name (dictionaryinstall);
    type = lw_dictionaryinstall_get_gtype (dictionaryinstall);
    TYPENAME = g_type_name (type);
    id = g_strdup (lw_dictionaryinstall_get_id (dictionaryinstall));

    normalized_filename = lw_utf8_normalize (FILENAME, -1, LW_UTF8FLAG_ALL);
    normalized_typename = lw_utf8_normalize (TYPENAME, -1, LW_UTF8FLAG_ALL);
    normalized_id = lw_utf8_normalize (id, -1, LW_UTF8FLAG_ALL);
   
    if (normalized_filename != NULL && g_hash_table_lookup (priv->data.index.filename, normalized_filename) != dictionaryinstall)
    {
      g_hash_table_remove (priv->data.index.filename, normalized_filename);
    }

    if (normalized_typename != NULL && g_hash_table_lookup (priv->data.index.typename, normalized_typename) != dictionaryinstall)
    {
      g_hash_table_remove (priv->data.index.typename, normalized_typename);
    }

    if (normalized_id != NULL && g_hash_table_lookup (priv->data.index.id, normalized_id) != dictionaryinstall)
    {
      g_hash_table_remove (priv->data.index.id, normalized_id);
    }

errored:

    if (normalized_filename != NULL) g_free (normalized_filename); normalized_filename = NULL;
    if (normalized_typename != NULL) g_free (normalized_typename); normalized_typename = NULL;
    if (id != NULL) g_free (id); id = NULL;
    if (normalized_id != NULL) g_free (normalized_id); normalized_id = NULL;

    return;
}


static void
_rebuild_array (LwDictionaryInstallList *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALLLIST (self));

    lw_dictionaryinstalllist_invalidate_length (self);

    //Declarations
    LwDictionaryInstallListPrivate *priv = NULL;
    gint length = 0;

    //Initializations
    priv = self->priv;
    length = lw_dictionaryinstalllist_length (self);

    g_free (priv->data.array); priv->data.array = NULL;
    priv->data.array = g_new0 (GList*, length);

    {
      GList *link = NULL;
      gint i = 0;
      for (link = priv->data.list; link != NULL; link = link->next)
      {
        LwDictionaryInstall *dictionaryinstall = LW_DICTIONARYINSTALL (link->data);
        priv->data.array[i] = link;
        dictionaryinstall->row.current_index = i;
        i++;
      }
    }

    lw_dictionaryinstalllist_invalidate_length (self);
}


static gint
_insert (LwDictionaryInstallList *self,
         gint                    *position,
         GList                   *dictionaryinstalls)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYINSTALLLIST (self), -1);
    if (dictionaryinstalls == NULL) return 0;

    //Declarations
    LwDictionaryInstallListPrivate *priv = NULL;
    gint number_inserted = 0;
    gint length = 0;
    gboolean append = FALSE;

    //Initializations
    priv = self->priv;
    length = lw_dictionaryinstalllist_length (self);
    append = (*position < 0 || *position >= length || length == 0);
    number_inserted = g_list_length (dictionaryinstalls);

    if (append)
    {
      GList *copy = g_list_copy (dictionaryinstalls);
      *position = length;
      g_list_foreach (copy, (GFunc) g_object_ref, NULL);
      priv->data.list = g_list_concat (priv->data.list, copy);

      {
        GList *link = NULL;
        for (link = copy; link != NULL; link = link->next)
        {
          LwDictionaryInstall *dictionaryinstall = LW_DICTIONARYINSTALL (link->data);
          if (dictionaryinstall != NULL)
          {
            _add_to_index (self, dictionaryinstall);
          }
        }
      }
    }
    else
    {
      GList *insert_link = g_list_nth (priv->data.list, *position);
      if (insert_link == NULL) goto errored;
      GList *link = NULL;
      for (link = g_list_last (dictionaryinstalls); link != NULL; link = link->prev)
      {
        LwDictionaryInstall *dictionaryinstall = LW_DICTIONARYINSTALL (link->data);
        if (dictionaryinstall != NULL)
        {
          g_object_ref (dictionaryinstall);
          priv->data.list = g_list_insert_before (priv->data.list, insert_link, dictionaryinstall);
          insert_link = insert_link->prev;
          _add_to_index (self, dictionaryinstall);
        }
        else
        {
          number_inserted--;
        }
      }
    }

errored:

    lw_dictionaryinstalllist_invalidate_length (self);

    return number_inserted;
}


static void
_insert_propogate_changes (LwDictionaryInstallList *self,
                               gint      position,
                               gint      number_inserted)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALLLIST (self));

    //Declarations
    LwDictionaryInstallListPrivate *priv = NULL;
    LwDictionaryInstallListClass *klass = NULL;
    LwDictionaryInstallListClassPrivate *klasspriv = NULL;
    gint length = 0;
    gint i = 0;

    //Initializations
    priv = self->priv;
    length = lw_dictionaryinstalllist_length (self);
    klass = LW_DICTIONARYINSTALLLIST_CLASS (self);
    klasspriv = klass->priv;

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
_find_duplicates (LwDictionaryInstallList *self, 
                  gint                     position, 
                  GList                   *dictionaryinstalls)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYINSTALLLIST (self), NULL);
    if (dictionaryinstalls == NULL) return NULL;

    //Declarations
    gint length = -1;
    gint *indices = NULL;

    //Initializations
    length = g_list_length (dictionaryinstalls);
    if (position < 0) position = lw_dictionaryinstalllist_length (self);
    indices = g_new0 (gint, length + 1);
    if (indices == NULL) goto errored;

    {
      GList *link = NULL;
      gint i = 0;
      for (link = dictionaryinstalls; link != NULL; link = link->next)
      {
        LwDictionaryInstall *a = LW_DICTIONARYINSTALL (link->data);
        if (a == NULL) continue;
        LwDictionaryInstall *b = LW_DICTIONARYINSTALL (lw_dictionaryinstalllist_nth (self, a->row.current_index));
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
lw_dictionaryinstalllist_insert (LwDictionaryInstallList *self, 
                                 gint                     position,
                                 GList                   *dictionaryinstalls)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYINSTALLLIST (self), NULL);
    if (dictionaryinstalls == NULL) return NULL;

    //Declarations
    gint number_inserted = 0;
    gint *duplicates = NULL;
    GList *removed = NULL;
    gboolean changed = FALSE;

    //Initializations
    duplicates = _find_duplicates (self, position, dictionaryinstalls);
    number_inserted = _insert (self, &position, dictionaryinstalls);

    _rebuild_array (self);
    _insert_propogate_changes (self, position, number_inserted);

    changed = TRUE;

    if (duplicates == NULL) goto errored;

    removed = lw_dictionaryinstalllist_remove (self, duplicates);

errored:

    if (duplicates != NULL) g_free (duplicates); duplicates = NULL;
    if (removed != NULL) g_list_free (removed); removed;

    return lw_dictionaryinstalllist_get_indices (self, dictionaryinstalls);
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
_sanitize_indices (LwDictionaryInstallList *self,
                   gint         *indices)
{
    g_return_val_if_fail (LW_IS_DICTIONARYINSTALLLIST (self), NULL);

    //Declarations
    gint size = 0;
    gint length = 0;

    //Initializations
    length = lw_dictionaryinstalllist_length (self);

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
_remove (LwDictionaryInstallList *self,
         gint         *indices)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYINSTALLLIST (self), NULL);
    if (indices == NULL) return NULL;

    //Declarations
    LwDictionaryInstallListPrivate *priv = NULL;
    gint length = 0;
    GList *list = NULL;

    //Initializations
    priv = self->priv;
    length = lw_dictionaryinstalllist_length (self);
    if (length == 0) goto errored;

    {
      gint i = 0;
      for (i = 0; indices[i] != -1 && i < length; i++)
      {
        gint index = indices[i];
        GList *link = priv->data.array[index];
        if (link != NULL)
        {
          LwDictionaryInstall *dictionaryinstall = LW_DICTIONARYINSTALL (link->data);
          priv->data.list = g_list_remove_link (priv->data.list, link);
          list = g_list_concat (link, list);
          priv->data.array[index] = NULL;

          if (dictionaryinstall != NULL && dictionaryinstall->row.current_index == index)
          {
            _remove_from_index (self, dictionaryinstall);
          }
        }
      }
      list = g_list_reverse (list);
    }

errored:

    lw_dictionaryinstalllist_invalidate_length (self);

    return list;
}


static void
_remove_propogate_changes (LwDictionaryInstallList *self,
                           gint             *indices)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALLLIST (self));
    if (indices == NULL) return;

    //Declarations
    gint length = 0;
    LwDictionaryInstallListClass *klass = NULL;
    LwDictionaryInstallListClassPrivate *klasspriv = NULL;

    //Initializations
    length = lw_dictionaryinstalllist_length (self);
    klass = LW_DICTIONARYINSTALLLIST_CLASS (self);
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
lw_dictionaryinstalllist_remove (LwDictionaryInstallList *self,
                                 gint                    *indices)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYINSTALLLIST (self), NULL);

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
//! @return The position in the LwDictionaryInstallList of the LwDictionary
//!
LwDictionaryInstall* 
lw_dictionaryinstalllist_nth (LwDictionaryInstallList* self,
                              gint                     index)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    //Declarations
    LwDictionaryInstallListPrivate *priv = NULL;
    LwDictionaryInstall *dictionaryinstall = NULL;
    GList *link = NULL;
    gint length = 0;

    //Initializiations
    priv = self->priv;
    length = lw_dictionaryinstalllist_length (self);
    if (index >= length) goto errored;

    link = priv->data.array[index];
    if (link == NULL) goto errored;
    dictionaryinstall = LW_DICTIONARYINSTALL (link->data);

errored:

    return dictionaryinstall;
}




gint 
lw_dictionaryinstalllist_length (LwDictionaryInstallList *self)
{
    //Sanity check
    g_return_val_if_fail (self != NULL, 0);

    //Declarations
    LwDictionaryInstallListPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (priv->data.length < 0)
    {
      priv->data.length = g_list_length (priv->data.list);
    }

    return priv->data.length;
}


void
lw_dictionaryinstalllist_load_default (LwDictionaryInstallList *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALLLIST (self));

    //Declarations
    LwDictionaryInstallListPrivate *priv = NULL;
    LwPreferences *preferences = NULL;
    GList *list = NULL;

/*TODO
    //Initializations
    priv = self->priv;
    preferences = lw_dictionaryinstalllist_get_preferences (self);
    if (preferences == NULL) goto errored;

    {
      LwDictionaryInstall *d = lw_dictionaryinstall_new ();
      lw_dictionaryinstall_set_name (d, "English");
      lw_dictionaryinstall_set_gtype (d, g_type_from_name ("LwEDictionary"));
      lw_dictionaryinstall_set_description (d, gettext("The venerable edict by Jim Breen."));
      lw_dictionaryinstall_set_text_encoding (d, "euc-jp");
      lw_dictionaryinstall_set_preferences (d, preferences);
      lw_dictionaryinstall_set_download_key (d, LW_KEY_ENGLISH_SOURCE);
      list = g_list_prepend (list, d);
    }

    {
      LwDictionaryInstall *d = lw_dictionaryinstall_new ();
      lw_dictionaryinstall_set_name (d, "Kanji");
      lw_dictionaryinstall_set_gtype (d, g_type_from_name ("LwKanjiDictionary"));
      lw_dictionaryinstall_set_description (d, gettext("A Kanji dictionary based off of kanjidic with radical information combined."));
      lw_dictionaryinstall_set_text_encoding (d, "euc-jp");
      lw_dictionaryinstall_set_preferences (d, preferences);
      lw_dictionaryinstall_set_download_key (d, LW_KEY_KANJI_SOURCE);
      //lw_dictionaryinstall_set_simple_boolean_dependancy (d, "radicals/Radicals", "merge-radicals-into-kanji", TRUE);
      list = g_list_prepend (list, d);
    }

    {
      LwDictionaryInstall *d = lw_dictionaryinstall_new ();
      lw_dictionaryinstall_set_name (d, "Radicals");
      lw_dictionaryinstall_set_gtype (d, g_type_from_name ("LwRadicalsDictionary"));
      lw_dictionaryinstall_set_description (d, gettext("Radicals"));
      lw_dictionaryinstall_set_text_encoding (d, "euc-jp");
      lw_dictionaryinstall_set_preferences (d, preferences);
      lw_dictionaryinstall_set_download_key (d, LW_KEY_RADICALS_SOURCE);
      list = g_list_prepend (list, d);
    }

    {
      LwDictionaryInstall *d = lw_dictionaryinstall_new ();
      lw_dictionaryinstall_set_name (d, "Names");
      lw_dictionaryinstall_set_gtype (d, g_type_from_name ("LwEDictionary"));
      lw_dictionaryinstall_set_description (d, 
        gettext("Based off of Enamdic, but with the names split from the "
                "places for 2 separate dictionaries.")
      );
      lw_dictionaryinstall_set_text_encoding (d, "euc-jp");
      lw_dictionaryinstall_set_preferences (d, preferences);
      lw_dictionaryinstall_set_download_key (d, LW_KEY_NAMES_PLACES_SOURCE);
      list = g_list_prepend (list, d);
    }

    {
      LwDictionaryInstall *d = lw_dictionaryinstall_new ();
      lw_dictionaryinstall_set_name (d, "Examples");
      lw_dictionaryinstall_set_gtype (d, g_type_from_name ("LwExampleDictionary"));
      lw_dictionaryinstall_set_description (d, 
        gettext("A collection of Japanese/English sentences initially compiled "
                "by Professor Yasuhito Tanaka at Hyogo University and his students.")
      );
      lw_dictionaryinstall_set_text_encoding (d, "euc-jp");
      lw_dictionaryinstall_set_preferences (d, preferences);
      lw_dictionaryinstall_set_download_key (d, LW_KEY_EXAMPLES_SOURCE);
      list = g_list_prepend (list, d);
    }

    list = g_list_reverse (list);

    lw_dictionaryinstalllist_insert (self, -1, list);

errored:

    g_list_free_full (list, (GDestroyNotify) g_object_unref);
    list = NULL;
*/
}


gint*
lw_dictionaryinstalllist_get_indices (LwDictionaryInstallList *self,
                                      GList                   *dictionaryinstalls)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYINSTALLLIST (self), NULL);
    if (dictionaryinstalls == NULL) return NULL;

    //Declarations
    gint* indices = NULL;
    gint length = NULL;

    //Initializations
    length = g_list_length (dictionaryinstalls);
    if (length < 1) goto errored;
    indices = g_new0 (gint, length + 1);
    if (indices == NULL) goto errored;

    {
      GList *link = NULL;
      gint i = 0;
      for (link = dictionaryinstalls; link != NULL; link = link->next)
      {
        LwDictionaryInstall *dictionaryinstall = LW_DICTIONARYINSTALL (link->data);
        if (dictionaryinstall != NULL && dictionaryinstall->row.current_index > -1)
        {
          indices[i++] = dictionaryinstall->row.current_index;
        }
      }
      indices[i++] = -1;
    }

errored:

    return indices;
}


GList*
lw_dictionaryinstalllist_dictionaryinstalls (LwDictionaryInstallList *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYINSTALLLIST (self), NULL);

    //Declarations
    LwDictionaryInstallListPrivate *priv = NULL;
    GList *dictionaryinstalls = NULL;

    //Initializations
    priv = self->priv;

    dictionaryinstalls = g_list_copy (priv->data.list);
    g_list_foreach (dictionaryinstalls, (GFunc) g_object_ref, NULL);

    return dictionaryinstalls;
}


LwDictionaryInstall*
lw_dictionaryinstalllist_fuzzy_find (LwDictionaryInstallList *self,
                                     const gchar             *DESCRIPTION)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYINSTALLLIST (self), NULL);

    //Declarations
    LwDictionaryInstallListPrivate *priv = NULL;
    GList *dictionaryinstalls = NULL;
    LwDictionaryInstall *dictionaryinstall = NULL;
    gchar *normalized = NULL;

    //Initializations
    priv = self->priv;
    normalized = lw_utf8_normalize (DESCRIPTION, -1, LW_UTF8FLAG_ALL);

    if (dictionaryinstall == NULL)
    {
      dictionaryinstall = g_hash_table_lookup (priv->data.index.id, normalized);
    }

    if (dictionaryinstall == NULL)
    {
      dictionaryinstall = g_hash_table_lookup (priv->data.index.filename, normalized);
    }

    if (dictionaryinstall == NULL)
    {
      dictionaryinstall = g_hash_table_lookup (priv->data.index.typename, normalized);
    }

errored:

    g_free (normalized); normalized = NULL;

    return dictionaryinstall;
}

