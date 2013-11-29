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


G_DEFINE_TYPE (LgwDictionaryList, lgw_dictionarylist, G_TYPE_OBJECT)

static gint lgw_dictionarylist_sort_compare_function (gconstpointer, gconstpointer, gpointer);

//!
//! @brief Creates a new LgwDictionaryList object
//! @param MAX The maximum items you want in the dictionary before old ones are deleted
//! @return An allocated LgwDictionaryList that will be needed to be freed by lw_dictionary_free.
//!
LgwDictionaryList* 
lgw_dictionarylist_new ()
{
    LgwDictionaryList *dictionary;

    //Initializations
    dictionary = LGW_DICTIONARYLIST (g_object_new (LGW_TYPE_DICTIONARYLIST, NULL));
    return dictionary;
}


static void 
lgw_dictionarylist_init (LgwDictionaryList *dictionarylist)
{
    dictionarylist->priv = LGW_DICTIONARYLIST_GET_PRIVATE (dictionarylist);
    memset(dictionarylist->priv, 0, sizeof(LgwDictionaryListPrivate));
    g_mutex_init (&dictionarylist->priv->mutex);
}


static void 
lgw_dictionarylist_finalize (GObject *object)
{
    //Declarations
    LgwDictionaryList *dictionarylist;
    LgwDictionaryListPrivate *priv;

    //Initalizations
    dictionarylist = LGW_DICTIONARYLIST (object);
    priv = dictionarylist->priv;

    lgw_dictionarylist_clear (dictionarylist);
    g_mutex_clear (&priv->mutex);

    G_OBJECT_CLASS (lgw_dictionarylist_parent_class)->finalize (object);
}


static void
lgw_dictionarylist_class_init (LgwDictionaryListClass *klass)
{
    //Declarations
    GObjectClass *object_class;
    LgwDictionaryListClassPrivate *klasspriv;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    klass->priv = g_new0 (LgwDictionaryListClassPrivate, 1);
    klasspriv = klass->priv;
    object_class->finalize = lgw_dictionarylist_finalize;

    klasspriv->signalid[CLASS_SIGNALID_ROW_CHANGED] = g_signal_new (
        "row-changed",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LgwDictionaryListClass, row_changed),
        NULL, NULL,
        g_cclosure_marshal_VOID__VOID,
        G_TYPE_NONE, 0
    );

    klasspriv->signalid[CLASS_SIGNALID_ROW_INSERTED] = g_signal_new (
        "row-inserted",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LgwDictionaryListClass, row_inserted),
        NULL, NULL,
        g_cclosure_marshal_VOID__VOID,
        G_TYPE_NONE, 0
    );

    klasspriv->signalid[CLASS_SIGNALID_ROW_HAS_CHILD_TOGGLED] = g_signal_new (
        "row-has-child-toggled",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LgwDictionaryListClass, row_has_child_toggled),
        NULL, NULL,
        g_cclosure_marshal_VOID__VOID,
        G_TYPE_NONE, 0
    );

    klasspriv->signalid[CLASS_SIGNALID_ROW_DELETED] = g_signal_new (
        "row-deleted",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LgwDictionaryListClass, row_deleted),
        NULL, NULL,
        g_cclosure_marshal_VOID__VOID,
        G_TYPE_NONE, 0
    );

    klasspriv->signalid[CLASS_SIGNALID_ROWS_REORDERED] = g_signal_new (
        "rows-reordered",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LgwDictionaryListClass, rows_reordered),
        NULL, NULL,
        g_cclosure_marshal_VOID__VOID,
        G_TYPE_NONE, 0
    );

    g_type_class_add_private (object_class, sizeof (LgwDictionaryListPrivate));
}

    klasspriv->signalid[CLASS_SIGNALID_ROW_CHANGED] = g_signal_new (
        "row-changed",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LgwDictionaryListClass, row_changed),
        NULL, NULL,
        g_cclosure_marshal_VOID__VOID,
        G_TYPE_NONE, 0
    );

    klasspriv->signalid[CLASS_SIGNALID_ROW_INSERTED] = g_signal_new (
        "row-inserted",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LgwDictionaryListClass, row_inserted),
        NULL, NULL,
        g_cclosure_marshal_VOID__VOID,
        G_TYPE_NONE, 0
    );

    klasspriv->signalid[CLASS_SIGNALID_ROW_HAS_CHILD_TOGGLED] = g_signal_new (
        "row-has-child-toggled",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LgwDictionaryListClass, row_has_child_toggled),
        NULL, NULL,
        g_cclosure_marshal_VOID__VOID,
        G_TYPE_NONE, 0
    );

    klasspriv->signalid[CLASS_SIGNALID_ROW_DELETED] = g_signal_new (
        "row-deleted",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LgwDictionaryListClass, row_deleted),
        NULL, NULL,
        g_cclosure_marshal_VOID__VOID,
        G_TYPE_NONE, 0
    );

    klasspriv->signalid[CLASS_SIGNALID_ROWS_REORDERED] = g_signal_new (
        "rows-reordered",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LgwDictionaryListClass, rows_reordered),
        NULL, NULL,
        g_cclosure_marshal_VOID__VOID,
        G_TYPE_NONE, 0
    );
