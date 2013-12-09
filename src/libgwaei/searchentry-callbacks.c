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
//! @file searchentry-callbacks.c
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


void
lgw_searchentry_connect_signals (LgwSearchEntry *entry)
{
    //Sanity checks
    g_return_if_fail (entry != NULL);

    //Declarations
    LgwSearchEntryPrivate *priv = NULL;

    //Initializations
    priv = entry->priv;

    priv->data.signalid[SIGNALID_CHANGED] = g_signal_connect (
        GTK_EDITABLE (priv->ui.search_entry),
        "changed",
        G_CALLBACK (lgw_searchentry_changed_cb),
        NULL
    );

    priv->data.signalid[SIGNALID_ACTIVATED] = g_signal_connect (
        GTK_ENTRY (priv->ui.search_entry),
        "activate",
        G_CALLBACK (lgw_searchentry_activated_cb),
        NULL
    );
}


void
lgw_searchentry_disconnect_signals (LgwSearchEntry *entry)
{
    //Sanity checks
    g_return_if_fail (entry != NULL);

    //Declarations
    LgwSearchEntryPrivate *priv = NULL;
    
    //Initializations
    priv = entry->priv;

    {
      gint i = 0;
      while (i < TOTAL_SIGNALIDS)
      {
        if (priv->data.signalid[i] != 0)
        {
          g_signal_handler_disconnect (G_OBJECT (priv->ui.search_entry), priv->data.signalid[i]);
          priv->data.signalid[i] = 0;
        }
        i++;
      }
    }
}


void
lgw_searchentry_activated_cb (GtkEntry *entry,
                              gpointer  data)
{
    printf("BREAK activated\n");
}


void
lgw_searchentry_changed_cb (GtkEditable *editable,
                            gpointer     data)
{
    printf("BREAK changed\n");
}


//!
//! @brief Inserts an unknown regex character into the entry
//! @see lgw_searchentry_insert_word_edge_cb ()
//! @see lgw_searchentry_insert_not_word_edge_cb ()
//! @see lgw_searchentry_insert_and_cb ()
//! @see lgw_searchentry_insert_or_cb ()
//! @param widget Unused GtkWidget pointer
//! @param data Unused gpointer
//!
void 
lgw_searchentry_insert_unknown_character_cb (GSimpleAction *action,
                                             GVariant      *parameter,
                                             gpointer       data)
{
    //Sanity checks
    g_return_if_fail (data != NULL);

    //Declarations
    LgwSearchEntry *search_entry = NULL;

    //Initializations
    search_entry = LGW_SEARCHENTRY (data);

    lgw_searchentry_insert_text (search_entry, ".");
}


//!
//! @brief Inserts an a word-boundary regex character into the entry
//! @see lgw_searchentry_insert_unknown_character_cb ()
//! @see lgw_searchentry_insert_not_word_edge_cb ()
//! @see lgw_searchentry_insert_and_cb ()
//! @see lgw_searchentry_insert_or_cb ()
//! @param widget Unused GtkWidget pointer
//! @param data Unused gpointer
//!
void 
lgw_searchentry_insert_word_edge_cb (GSimpleAction *action, 
                                     GVariant      *parameter,
                                     gpointer       data)
{
    //Sanity checks
    g_return_if_fail (data != NULL);

    //Declarations
    LgwSearchEntry *search_entry = NULL;

    //Initializations
    search_entry = LGW_SEARCHENTRY (data);

    lgw_searchentry_insert_text (search_entry, "\\b");
}


//!
//! @brief Inserts an a not-word-boundary regex character into the entry
//! @see lgw_searchentry_insert_unknown_character_cb ()
//! @see lgw_searchentry_insert_word_edge_cb ()
//! @see lgw_searchentry_insert_and_cb ()
//! @see lgw_searchentry_insert_or_cb ()
//! @param widget Unused GtkWidget pointer
//! @param data Unused gpointer
//!
void 
lgw_searchentry_insert_not_word_edge_cb (GSimpleAction *action, 
                                         GVariant      *parameter, 
                                         gpointer       data)
{
    //Sanity checks
    g_return_if_fail (data != NULL);

    //Declarations
    LgwSearchEntry *search_entry = NULL;

    //Initializations
    search_entry = LGW_SEARCHENTRY (data);

    lgw_searchentry_insert_text (search_entry, "\\B");
}


//!
//! @brief Inserts an an and regex character into the entry
//! @see lgw_searchentry_insert_unknown_character_cb ()
//! @see lgw_searchentry_insert_word_edge_cb ()
//! @see lgw_searchentry_insert_not_word_edge_cb ()
//! @see lgw_searchentry_insert_or_cb ()
//! @param widget Unused GtkWidget pointer
//! @param data Unused gpointer
//!
void 
lgw_searchentry_insert_and_cb (GSimpleAction *action,
                               GVariant      *parameter,
                               gpointer       data)
{
    //Sanity checks
    g_return_if_fail (data != NULL);

    //Declarations
    LgwSearchEntry *search_entry = NULL;

    //Initializations
    search_entry = LGW_SEARCHENTRY (data);

    lgw_searchentry_insert_text (search_entry, "&");
}


//!
//! @brief Inserts an an or regex character into the entry
//! @see lgw_searchentry_insert_unknown_character_cb ()
//! @see lgw_searchentry_insert_word_edge_cb ()
//! @see lgw_searchentry_insert_not_word_edge_cb ()
//! @see lgw_searchentry_insert_and_cb ()
//! @param widget Unused GtkWidget pointer
//! @param data Unused gpointer
//!
void 
lgw_searchentry_insert_or_cb (GSimpleAction *action, 
                              GVariant      *parameter,
                              gpointer       data)
{
    //Sanity checks
    g_return_if_fail (data != NULL);

    //Declarations
    LgwSearchEntry *search_entry = NULL;

    //Initializations
    search_entry = LGW_SEARCHENTRY (data);

    lgw_searchentry_insert_text (search_entry, "|");
}


//!
//! @brief Clears the search entry and moves the focus to it
//! @param widget Unused GtkWidget pointer
//! @param data Unused gpointer
//!
void 
lgw_searchentry_clear_search_cb (GSimpleAction *action, 
                                 GVariant      *parameter, 
                                 gpointer       data)
{
    //Sanity checks
    g_return_if_fail (data != NULL);

    //Declarations
    LgwSearchEntry *search_entry = NULL;
    LgwSearchEntryPrivate *priv = NULL;
    GtkEntry *entry = NULL;
    
    //Initializations
    search_entry = LGW_SEARCHENTRY (data);
    priv = search_entry->priv;
    entry = GTK_ENTRY (priv->ui.search_entry);

    gtk_entry_set_text (entry, "");
    gtk_widget_grab_focus (GTK_WIDGET (entry));
}

