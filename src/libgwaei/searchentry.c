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
//! @file searchentry.c
//!
//! @brief To be written
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>
#include <libgwaei/searchentry-private.h>


//Static declarations
static void lgw_searchentry_attach_signals (LgwSearchEntry*);
static void lgw_searchentry_remove_signals (LgwSearchEntry*);

G_DEFINE_TYPE (LgwSearchEntry, lgw_searchentry, GTK_TYPE_BOX)


//!
//! @brief Sets up the variables in main-interface.c and main-callbacks.c for use
//!
GtkWidget* 
lgw_searchentry_new ()
{
    //Declarations
    LgwSearchEntry *entry = NULL;

    //Initializations
    entry = LGW_SEARCHENTRY (g_object_new (LGW_TYPE_SEARCHENTRY, NULL));

    return GTK_WIDGET (entry);
}


static void 
lgw_searchentry_init (LgwSearchEntry *entry)
{
    entry->priv = LGW_SEARCHENTRY_GET_PRIVATE (entry);
    memset(entry->priv, 0, sizeof(LgwSearchEntryPrivate));

    LgwSearchEntryPrivate *priv;
    priv = entry->priv;
}


static void 
lgw_searchentry_finalize (GObject *object)
{
    LgwSearchEntry *entry;
    LgwSearchEntryPrivate *priv;

    entry = LGW_SEARCHENTRY (object);
    priv = entry->priv;

    G_OBJECT_CLASS (lgw_searchentry_parent_class)->finalize (object);
}


static void 
lgw_searchentry_constructed (GObject *object)
{
    //Declarations
    LgwSearchEntry *entry = NULL;
    LgwSearchEntryPrivate *priv = NULL;
    GtkWidget *widget = NULL;

    //Chain the parent class
    {
      G_OBJECT_CLASS (lgw_searchentry_parent_class)->constructed (object);
    }

    //Initializations
    entry = LGW_SEARCHENTRY (object);
    priv = entry->priv;
    priv->ui.box = GTK_BOX (entry);
    widget = GTK_WIDGET (object);

    {
        GtkWidget *search_entry = gtk_search_entry_new ();
        priv->ui.search_entry = GTK_SEARCH_ENTRY (search_entry);
        gtk_box_pack_start (priv->ui.box, search_entry, FALSE, FALSE, 0);
        gtk_widget_set_size_request (search_entry, 300, -1);
        gtk_widget_show (search_entry);
    }
}


static void
lgw_searchentry_class_init (LgwSearchEntryClass *klass)
{
    GObjectClass *object_class;

    object_class = G_OBJECT_CLASS (klass);

    object_class->constructed = lgw_searchentry_constructed;
    object_class->finalize = lgw_searchentry_finalize;

    g_type_class_add_private (object_class, sizeof (LgwSearchEntryPrivate));
/*
    klass->signalid[GW_ADDVOCABULARYWINDOW_CLASS_SIGNALID_WORD_ADDED] = g_signal_new (
        "word-added",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (GwAddVocabularyWindowClass, word_added),
        NULL, NULL,
        g_cclosure_marshal_VOID__VOID,
        G_TYPE_NONE, 0
    );
*/
}

