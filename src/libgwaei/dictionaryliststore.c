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

#include <glib-object.h>
#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>

#include <libgwaei/dictionaryliststore-private.h>

static gint lgw_dictionaryliststore_sort_compare_function (gconstpointer, gconstpointer, gpointer);

G_DEFINE_TYPE_WITH_CODE (LgwDictionaryListStore, lgw_dictionaryliststore, LW_TYPE_DICTIONARYLIST,
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_MODEL, lgw_dictionaryliststore_init_interface));


//!
//! @brief Creates a new LgwDictionaryListStore object
//! @param MAX The maximum items you want in the dictionary before old ones are deleted
//! @return An allocated LgwDictionaryListStore that will be needed to be freed by lw_dictionary_free.
//!
LgwDictionaryListStore* 
lgw_dictionaryliststore_new (LwPreferences *preferences)
{
    //Declarations
    LgwDictionaryListStore *dictionary = NULL;

    //Initializations
    dictionary = LGW_DICTIONARYLISTSTORE (g_object_new (LGW_TYPE_DICTIONARYLISTSTORE, "preferences", preferences, NULL));

    return dictionary;
}


static void 
lgw_dictionaryliststore_init (LgwDictionaryListStore *dictionarylist)
{
    dictionarylist->priv = LGW_DICTIONARYLISTSTORE_GET_PRIVATE (dictionarylist);
    memset(dictionarylist->priv, 0, sizeof(LgwDictionaryListStorePrivate));

    lgw_dictionaryliststore_connect_signals (dictionarylist);
}


static void 
lgw_dictionaryliststore_finalize (GObject *object)
{
    //Declarations
    LgwDictionaryListStore *dictionarylist = NULL;
    LgwDictionaryListStorePrivate *priv = NULL;

    //Initalizations
    dictionarylist = LGW_DICTIONARYLISTSTORE (object);
    priv = dictionarylist->priv;

    lw_dictionarylist_clear (LW_DICTIONARYLIST (dictionarylist));

    G_OBJECT_CLASS (lgw_dictionaryliststore_parent_class)->finalize (object);
}


static void
lgw_dictionaryliststore_dispose (GObject *object)
{
    //Declarations
    LgwDictionaryListStore *self = NULL;

    //Initializations
    self = LGW_DICTIONARYLISTSTORE (object);

    lgw_dictionaryliststore_disconnect_signals (self);

    G_OBJECT_CLASS (lgw_dictionaryliststore_parent_class)->dispose (object);
}


static void
lgw_dictionaryliststore_class_init (LgwDictionaryListStoreClass *klass)
{
    //Declarations
    GObjectClass *object_class = NULL;
    LgwDictionaryListStoreClassPrivate *klasspriv = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->dispose = lgw_dictionaryliststore_dispose;
    object_class->finalize = lgw_dictionaryliststore_finalize;

    klass->priv = g_new0 (LgwDictionaryListStoreClassPrivate, 1);
    klasspriv = klass->priv;

    g_type_class_add_private (object_class, sizeof (LgwDictionaryListStorePrivate));
}

