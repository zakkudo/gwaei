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
lgw_vocabularyliststore_new ()
{
    //Declarations
    LgwVocabularyListStore *store = NULL;

    //Initializations
    store = LGW_VOCABULARYLISTSTORE (g_object_new (LGW_TYPE_VOCABULARYLISTSTORE, NULL));

    return store;
}


static void 
lgw_vocabularyliststore_init (LgwVocabularyListStore *store)
{
    GType types[] = { G_TYPE_STRING, G_TYPE_INT, G_TYPE_OBJECT };
    gtk_list_store_set_column_types (GTK_LIST_STORE (store), 3, types);

    store->priv = LGW_VOCABULARYLISTSTORE_GET_PRIVATE (store);
    memset(store->priv, 0, sizeof(LgwVocabularyListStorePrivate));
}


static void 
lgw_vocabularyliststore_finalize (GObject *object)
{
    LgwVocabularyListStore *store;
    LgwVocabularyListStorePrivate *priv;

    store = LGW_VOCABULARYLISTSTORE (object);
    priv = store->priv;

    G_OBJECT_CLASS (lgw_vocabularyliststore_parent_class)->finalize (object);
}


static void
lgw_vocabularyliststore_class_init (LgwVocabularyListStoreClass *klass)
{
    //Declarations
    GObjectClass *object_class;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = lgw_vocabularyliststore_finalize;

    g_type_class_add_private (object_class, sizeof (LgwVocabularyListStorePrivate));
}


