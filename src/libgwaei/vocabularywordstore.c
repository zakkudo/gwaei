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
);


LgwVocabularyWordStore*
lgw_vocabularywordstore_new (const gchar *FILENAME)
{
    //Declarations
    LgwVocabularyWordStore *self = NULL;;

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


LgwVocabularyWordStore*
lgw_vocabularywordstore_copy (LgwVocabularyWordStore *self)
{
    //TODO
    g_assert_not_reached ();
    return NULL;
}


