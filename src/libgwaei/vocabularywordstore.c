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
#include <stdio.h>

#include <glib-object.h>
#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>

#include <libgwaei/vocabularywordstore-private.h>

G_DEFINE_TYPE_WITH_CODE (LgwVocabularyWordStore, lgw_vocabularywordstore, LW_TYPE_VOCABULARY,
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_MODEL, lgw_vocabularywordstore_init_interface));

LgwVocabularyWordStore*
lgw_vocabularywordstore_new (const gchar *FILENAME)
{
    //Declarations
    LgwVocabularyWordStore *vocabulary_word_store = NULL;;

    //Initializations
    vocabulary_word_store = LGW_VOCABULARYWORDSTORE (g_object_new (LGW_TYPE_VOCABULARYWORDSTORE,
                                                  "filename",        FILENAME,
                                                  NULL));
    return vocabulary_word_store;
}


static void 
lgw_vocabularywordstore_init (LgwVocabularyWordStore *vocabulary_word_store)
{
    vocabulary_word_store->priv = LGW_VOCABULARYWORDSTORE_GET_PRIVATE (vocabulary_word_store);
    memset(vocabulary_word_store->priv, 0, sizeof(LgwVocabularyWordStorePrivate));

    lgw_vocabularywordstore_connect_signals (vocabulary_word_store);
}


static void
lgw_vocabularywordstore_dispose (GObject *object)
{
    //Declarations
    LgwVocabularyWordStore *vocabulary_word_store = NULL;

    //Initializations
    vocabulary_word_store = LGW_VOCABULARYWORDSTORE (object);

    lgw_vocabularywordstore_disconnect_signals (vocabulary_word_store);

    G_OBJECT_CLASS (lgw_vocabularywordstore_parent_class)->dispose (object);
}


static void 
lgw_vocabularywordstore_finalize (GObject *object)
{
    //Declarations
    LgwVocabularyWordStore *vocabulary_word_store;
    LgwVocabularyWordStorePrivate *priv;

    //Initializations
    vocabulary_word_store = LGW_VOCABULARYWORDSTORE (object);
    priv = vocabulary_word_store->priv;

    G_OBJECT_CLASS (lgw_vocabularywordstore_parent_class)->finalize (object);
}


static void 
lgw_vocabularywordstore_set_property (GObject      *object,
                                      guint         property_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
    //Declarations
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    LgwVocabularyWordStorePrivate *priv = NULL;

    //Initializations
    vocabulary_word_store = LGW_VOCABULARYWORDSTORE (object);
    priv = vocabulary_word_store->priv;

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
    LgwVocabularyWordStore *vocabulary_word_store;
    LgwVocabularyWordStorePrivate *priv;

    vocabulary_word_store = LGW_VOCABULARYWORDSTORE (object);
    priv = vocabulary_word_store->priv;

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
void
lgw_vocabularywordstore_update_timestamp_by_iter (LgwVocabularyWordStore *store, GtkTreeIter *iter)
{
    LwWord *word;
    gchar *text;

    text = lgw_vocabularywordstore_iter_to_string (store, iter);
    word = lw_word_new_from_string (text);
    if (text != NULL)
    {
      if (word != NULL)
      {
        lw_word_update_timestamp (word);

        gtk_list_store_set (GTK_LIST_STORE (store), iter, 
          LGW_VOCABULARYWORDSTORE_COLUMN_TIMESTAMP, lw_word_get_hours (word),
          LGW_VOCABULARYWORDSTORE_COLUMN_DAYS, lw_word_get_timestamp_as_string (word),
        -1);

        lw_word_free (word);
      }
      g_free (text);
    }
}


gint
lgw_vocabularywordstore_calculate_weight (LgwVocabularyWordStore *store, GtkTreeIter *iter)
{
    if (store == NULL) return -1;
    if (iter == NULL) return -1;

    //The effect of this is short term score is important.
    //long term, time is important for deciding weight
    GtkTreeModel *vocabulary_word_store;
    gint score;
    gint numerator;
    gint denominator;
    gint incorrect;
    gint correct;
    gint weight;
    guint32 hours;

    vocabulary_word_store = GTK_TREE_MODEL (store);
    gtk_tree_model_get (vocabulary_word_store, iter, 
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
