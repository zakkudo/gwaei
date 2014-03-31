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
//! @file vocabularywordstore.c
//!
//! @brief To be written
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include <string.h>
#include <stdlib.h>

#include <glib-object.h>
#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>

#include <libgwaei/vocabularywordstore-private.h>

G_DEFINE_TYPE_WITH_CODE (
  LgwVocabularyWordStore, lgw_vocabularywordstore, LW_TYPE_VOCABULARY,
  G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_MODEL, lgw_vocabularywordstore_implement_treemodel_interface)
  G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_DRAG_SOURCE, lgw_vocabularywordstore_implement_treedragsource_interface)
  G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_DRAG_DEST, lgw_vocabularywordstore_implement_treedragdest_interface)
  G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_SORTABLE, lgw_vocabularywordstore_implement_treesortable_interface)
);


LgwVocabularyWordStore*
lgw_vocabularywordstore_new (const gchar *FILENAME)
{
    //Declarations
    LgwVocabularyWordStore *self = NULL;

    //Initializations
    self = LGW_VOCABULARYWORDSTORE (g_object_new (LGW_TYPE_VOCABULARYWORDSTORE,
                                                    "filename",        FILENAME,
                                                     NULL));
    return self;
}


static void 
lgw_vocabularywordstore_init (LgwVocabularyWordStore *self)
{
    self->priv = LGW_VOCABULARYWORDSTORE_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LgwVocabularyWordStorePrivate));

    lgw_vocabularywordstore_connect_signals (self);
}


static void
lgw_vocabularywordstore_dispose (GObject *object)
{
    //Declarations
    LgwVocabularyWordStore *self = NULL;

    //Initializations
    self = LGW_VOCABULARYWORDSTORE (object);

    lgw_vocabularywordstore_disconnect_signals (self);

    G_OBJECT_CLASS (lgw_vocabularywordstore_parent_class)->dispose (object);
}


static void 
lgw_vocabularywordstore_finalize (GObject *object)
{
    //Declarations
    LgwVocabularyWordStore *self;
    LgwVocabularyWordStorePrivate *priv;

    //Initializations
    self = LGW_VOCABULARYWORDSTORE (object);
    priv = self->priv;

    G_OBJECT_CLASS (lgw_vocabularywordstore_parent_class)->finalize (object);
}


static void 
lgw_vocabularywordstore_set_property (GObject      *object,
                                      guint         property_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
    //Declarations
    LgwVocabularyWordStore *self = NULL;
    LgwVocabularyWordStorePrivate *priv = NULL;

    //Initializations
    self = LGW_VOCABULARYWORDSTORE (object);
    priv = self->priv;

    switch (property_id)
    {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lgw_vocabularywordstore_get_property (GObject      *object,
                                      guint         property_id,
                                      GValue       *value,
                                      GParamSpec   *pspec)
{
    LgwVocabularyWordStore *self;
    LgwVocabularyWordStorePrivate *priv;

    self = LGW_VOCABULARYWORDSTORE (object);
    priv = self->priv;

    switch (property_id)
    {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
lgw_vocabularywordstore_class_init (LgwVocabularyWordStoreClass *klass)
{
    //Declarations
    GObjectClass *object_class;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->set_property = lgw_vocabularywordstore_set_property;
    object_class->get_property = lgw_vocabularywordstore_get_property;
    object_class->dispose = lgw_vocabularywordstore_dispose;
    object_class->finalize = lgw_vocabularywordstore_finalize;

    g_type_class_add_private (object_class, sizeof (LgwVocabularyWordStorePrivate));
}



/*DEPICATED
gint
lgw_vocabularywordstore_calculate_weight (LgwVocabularyWordStore *store, GtkTreeIter *iter)
{
    if (store == NULL) return -1;
    if (iter == NULL) return -1;

    //The effect of this is short term score is important.
    //long term, time is important for deciding weight
    GtkTreeModel *self;
    gint score;
    gint numerator;
    gint denominator;
    gint incorrect;
    gint correct;
    gint weight;
    guint32 hours;

    self = GTK_TREE_MODEL (store);
    gtk_tree_model_get (self, iter, 
          LGW_VOCABULARYWORDSTORE_COLUMN_TIMESTAMP, &hours,
          LGW_VOCABULARYWORDSTORE_COLUMN_CORRECT_GUESSES, &correct,
          LGW_VOCABULARYWORDSTORE_COLUMN_INCORRECT_GUESSES, &incorrect,
        -1);
    numerator = correct * 100;
    denominator = incorrect + correct;
  
    if (denominator != 0)
      score = (numerator / denominator);
    else
      score = 0;
  
    weight = ((gint) hours + score);
  
    return weight;
}
DEPRICATED*/


LwWord*
lgw_vocabularywordstore_get_word (LgwVocabularyWordStore *self,
                                  GtkTreePath            *tree_path)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDSTORE (self));

    //Declarations
    GtkTreeModel *tree_model = NULL;
    LwWord *word = NULL;
    GtkTreeIter iter;

    //Initializations
    tree_model = GTK_TREE_MODEL (self);

    if (gtk_tree_model_get_iter (tree_model, &iter, tree_path))
    {
      gpointer data = NULL;
      gtk_tree_model_get (tree_model, &iter, LGW_VOCABULARYWORDSTORE_COLUMN_WORD, &data, -1);
      word = LW_WORD (data);
    }

errored:

    return word;
}


GtkTreePath*
lgw_vocabularywordstore_find_by_word (LgwVocabularyWordStore *self,
                                      LwWord                 *word)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDSTORE (self));
    if (word == NULL) return NULL;

    //Declarations
    LwVocabulary *vocabulary = NULL;
    gint position = 0;
    gint length = 0;
    GtkTreePath *tree_path = NULL;

    //Initializations
    vocabulary = LW_VOCABULARY (self);
    if (vocabulary == NULL) goto errored;
    position = word->row.current_index;
    length = lw_vocabulary_length (vocabulary);
    if (position < 0 || position >= length) goto errored;
    if (lw_vocabulary_nth (vocabulary, word->row.current_index) != word) goto errored;

    tree_path = gtk_tree_path_new_from_indices (position, -1);

errored:

   return tree_path; 
}


GList*
lgw_vocabularywordstore_get_tree_paths (LgwVocabularyWordStore *self,
                                        GList                  *words)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDSTORE (self));
    if (words == NULL) return NULL;

    //Declarations
    GList *tree_paths = NULL;

    {
      GList *link = NULL;
      for (link = words; link != NULL; link = link->next)
      {
        LwWord *word = LW_WORD (link->data);
        if (word != NULL)
        {
          GtkTreePath *tree_path = lgw_vocabularywordstore_find_by_word (self, word);
          if (tree_path != NULL)
          {
            tree_paths = g_list_prepend (tree_paths, tree_path);
          }
        }
      }
    }

    return tree_paths;
}


void
lgw_vocabularywordstore_set (LgwVocabularyWordStore *self,
                             GtkTreePath            *tree_path,
                             ...)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDSTORE (self));
    g_return_if_fail (tree_path != NULL);

    //Declarations
    LwVocabulary *vocabulary = NULL;
    gint index = -1;
    va_list va;

    //Initializations
    vocabulary = LW_VOCABULARY (self);
    index = gtk_tree_path_get_indices (tree_path)[0];
    va_start(va, tree_path);

    lw_vocabulary_set_valist (vocabulary, index, va);
}


gint*
lgw_vocabularywordstore_tree_paths_to_indices (LgwVocabularyWordStore *self,
                                               GList                  *tree_paths)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYWORDSTORE (self), NULL);
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
          indices[i] = gtk_tree_path_get_indices (path)[0];
          i++;
        }
      }
      indices[i] = -1;
    }

errored:

    return indices;
}


GList*
lgw_vocabularywordstore_indices_to_tree_paths (LgwVocabularyWordStore *self,
                                               gint                   *indices)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYWORDSTORE (self), NULL);
    if (indices == NULL) return NULL;

    //Declarations
    GList *tree_paths = NULL;

    //Initializations
    
    {
      gint i = 0;
      for (i = 0; indices[i] > -1; i++)
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


GList*
lgw_vocabularywordstore_insert (LgwVocabularyWordStore *self,
                                GtkTreePath            *tree_path,
                                GList                  *words)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDSTORE (self));

    //Declarations
    gint position = -1;
    LwVocabulary *vocabulary = NULL;
    gint *indices = NULL;
    GList *inserted_tree_paths = NULL;

    //Initializations
    if (tree_path != NULL)
      position = gtk_tree_path_get_indices (tree_path)[0];
    else
      position = -1;
    vocabulary = LW_VOCABULARY (self);
    if (vocabulary == NULL) goto errored;
    indices = lw_vocabulary_insert (vocabulary, position, words);
    if (indices == NULL) goto errored;
    inserted_tree_paths = lgw_vocabularywordstore_indices_to_tree_paths (self, indices);
    if (inserted_tree_paths == NULL) goto errored;

errored:

    if (indices != NULL) g_free (indices); indices = NULL;

    return inserted_tree_paths;
}


GList*
lgw_vocabularywordstore_remove (LgwVocabularyWordStore *self,
                                GList                  *tree_paths)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDSTORE (self));

    //Declarations
    LwVocabulary *vocabulary = NULL;
    gint *indices = NULL;
    GList *removed = NULL;

    //Initializations
    vocabulary = LW_VOCABULARY (self);
    indices = lgw_vocabularywordstore_tree_paths_to_indices (self, tree_paths);
    if (indices == NULL) goto errored;
    removed = lw_vocabulary_remove (vocabulary, indices);

errored:

    if (indices != NULL) g_free (indices); indices = NULL;

    return removed;
}


LgwVocabularyWordStore*
lgw_vocabularywordstore_copy (LgwVocabularyWordStore *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDSTORE (self));

    //Declarations
    LgwVocabularyWordStore *copy = NULL;
    gchar *text = NULL;

    //Initialiations
    text = lw_vocabulary_to_string (LW_VOCABULARY (self), NULL);
    if (text == NULL) goto errored;
    copy = lgw_vocabularywordstore_new (NULL);
    if (copy == NULL) goto errored;
    lw_vocabulary_load_from_string (LW_VOCABULARY (copy), text, NULL);

errored:

    if (text != NULL) g_free (text); text = NULL;

    return copy;
}


gint
lgw_vocabularywordstore_kanji_compare_func (GtkTreeModel *model,
                                            GtkTreeIter *a,
                                            GtkTreeIter *b,
                                            gpointer user_data)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDSTORE (model));

    printf("BREAK lgw_vocabularywordstore_name_compare_func\n");

    //Declarations
    LwVocabulary *va = NULL;
    LwVocabulary *vb = NULL;
    const gchar *fa = NULL;
    const gchar *fb = NULL;
    gint result = 0;

    //Initializations
    gtk_tree_model_get (model, a, LGW_VOCABULARYWORDSTORE_COLUMN_WORD, &va, -1);
    if (va == NULL) goto errored;
    gtk_tree_model_get (model, b, LGW_VOCABULARYWORDSTORE_COLUMN_WORD, &vb, -1);
    if (vb == NULL) goto errored;
    fa = lw_vocabulary_get_filename (va);
    fb = lw_vocabulary_get_filename (vb);

errored:

    result = g_strcmp0 (fa, fb);

    if (va != NULL) g_object_unref (va); va = NULL;
    if (vb != NULL) g_object_unref (vb); va = NULL;

    return result;
}


gint
lgw_vocabularywordstore_saved_position_compare_func (GtkTreeModel *model,
                                                     GtkTreeIter  *a,
                                                     GtkTreeIter  *b,
                                                     gpointer      user_data)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDSTORE (model));

    printf("BREAK lgw_vocabularywordstore_saved_position_compare_func\n");

    //Declarations
    LwWord *wa = NULL;
    LwWord *wb = NULL;
    gint pa = -1;
    gint pb = -1;
    gint result = 0;

    //Initializations
    gtk_tree_model_get (model, a, LGW_VOCABULARYWORDSTORE_COLUMN_WORD, &wa, -1);
    if (wa == NULL) goto errored;
    gtk_tree_model_get (model, b, LGW_VOCABULARYWORDSTORE_COLUMN_WORD, &wb, -1);
    if (wb == NULL) goto errored;
    pa = wa->row.saved_index;
    pb = wb->row.saved_index;

errored:

    result = pa - pb;

    return result;
}


static gint
_compare_func (LwWord                 *wa,
               LwWord                 *wb,
               LgwVocabularyWordStore *self)
{
    //Declarations
    LgwVocabularyWordStorePrivate *priv = NULL;
    GtkTreeSortable *tree_sortable = NULL;
    GtkTreeModel *tree_model = GTK_TREE_MODEL (self);
    GtkTreeIterCompareFunc sort_func = NULL;
    gint sort_column_id = -1;
    GtkSortType order = 0;
    gboolean is_normal_column = FALSE;
    GtkTreeIter a_iter;
    GtkTreeIter b_iter;

    //Initializations
    priv = self->priv;
    tree_model = GTK_TREE_MODEL (self);
    tree_sortable = GTK_TREE_SORTABLE (self);
    is_normal_column = gtk_tree_sortable_get_sort_column_id (tree_sortable, &sort_column_id, &order);
printf("BREAK _compare_func %d %d %d\n", is_normal_column, sort_column_id, order);

    if (sort_column_id == GTK_TREE_SORTABLE_UNSORTED_SORT_COLUMN_ID) 
    {
      goto errored;
    }
    else if (sort_column_id == GTK_TREE_SORTABLE_DEFAULT_SORT_COLUMN_ID) 
    {
      printf("BREAK _compare_func GTK_TREE_SORTABLE_DEFAULT_SORT_COLUMN_ID\n");
      sort_func = priv->config.default_sort_func;
    }
    else 
    {
      printf("BREAK _compare_func %d \n", sort_column_id);
      sort_func = priv->config.sort_func[sort_column_id];
    }

    if (sort_func == NULL) goto errored;

    lgw_vocabularywordstore_initialize_tree_iter (self, &a_iter, wa->row.current_index);
    lgw_vocabularywordstore_initialize_tree_iter (self, &b_iter, wb->row.current_index);

    gint direction = (order == GTK_SORT_DESCENDING) ? -1 : 1;

    return (sort_func (tree_model, &a_iter, &b_iter, NULL) * direction);

errored:

    g_warning ("There is no sort function set for column %d", sort_column_id);

    return 0;
}


void
lgw_vocabularywordstore_sort (LgwVocabularyWordStore *vocabulary_word_store)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDSTORE (vocabulary_word_store));

    //Declarations
    LwVocabulary *vocabulary = NULL;

    //Initializations
    vocabulary = LW_VOCABULARY (vocabulary_word_store);

    lw_vocabulary_sort (vocabulary, (GCompareDataFunc) _compare_func);
}

