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

    if (priv->data.signalid[SIGNALID_CHANGED] == 0)
    {
      priv->data.signalid[SIGNALID_CHANGED] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.search_entry),
          "search-changed",
          G_CALLBACK (lgw_searchentry_changed_cb),
          entry
      );
    }

    if (priv->data.signalid[SIGNALID_ACTIVATED] == 0)
    {
      priv->data.signalid[SIGNALID_ACTIVATED] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.search_entry),
          "activate",
          G_CALLBACK (lgw_searchentry_activated_cb),
          entry
      );
    }

    if (priv->data.signalid[SIGNALID_FOCUS_IN_EVENT] == 0)
    {
      priv->data.signalid[SIGNALID_FOCUS_IN_EVENT] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.search_entry),
          "focus-in-event",
          G_CALLBACK (lgw_searchentry_focus_in_event_cb),
          entry 
      );
    }

    if (priv->data.signalid[SIGNALID_FOCUS_OUT_EVENT] == 0)
    {
      priv->data.signalid[SIGNALID_FOCUS_OUT_EVENT] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.search_entry),
          "focus-out-event",
          G_CALLBACK (lgw_searchentry_focus_out_event_cb),
          entry
      );
    }
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
lgw_searchentry_activated_cb (LgwSearchEntry *search_entry,
                              GtkEntry *inner_entry)
{
    printf("BREAK activated\n");
}


void
lgw_searchentry_changed_cb (LgwSearchEntry *search_entry,
                            GtkSearchEntry *inner_search_entry)
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


gboolean
lgw_searchentry_focus_in_event_cb (LgwSearchEntry *search_entry,
                                   GdkEvent       *event,
                                   GtkSearchEntry *inner_search_entry)
{
    printf("BREAK0 searchentry focus in event\n");
    //Sanity checks
    g_return_val_if_fail (search_entry != NULL, FALSE);
    g_return_val_if_fail (inner_search_entry != NULL, FALSE);

    lgw_searchentry_sync_actions (search_entry);

    printf("BREAK1 searchentry focus in event\n");

    return FALSE;
}


gboolean
lgw_searchentry_focus_out_event_cb (LgwSearchEntry *search_entry,
                                    GdkEvent       *event,
                                    GtkSearchEntry *inner_search_entry)
{
    //Sanity checks
    g_return_val_if_fail (search_entry != NULL, FALSE);
    g_return_val_if_fail (inner_search_entry != NULL, FALSE);

    lgw_searchentry_sync_actions (search_entry);

    printf("searchentry focus out event\n");

    return FALSE;
}


void
lgw_searchentry_copy_cb (GSimpleAction *action,
                         GVariant      *parameter,
                         gpointer       data)
{
    //Sanity checks
    g_return_if_fail (data != NULL);

    //Declarations
    LgwResultsTextView *results_text_view = LGW_RESULTSTEXTVIEW (data);

    //Initializations

    printf("BREAK lgw_searchentry_copy\n");
}


void
lgw_searchentry_cut_cb (GSimpleAction *action,
                        GVariant      *parameter,
                        gpointer       data)
{
    //Sanity checks
    g_return_if_fail (data != NULL);

    //Declarations
    LgwResultsTextView *results_text_view = LGW_RESULTSTEXTVIEW (data);

    //Initializations

    printf("BREAK lgw_searchentry_cut\n");
}


void
lgw_searchentry_paste_cb (GSimpleAction *action,
                          GVariant      *parameter,
                          gpointer       data)
{
    //Sanity checks
    g_return_if_fail (data != NULL);

    //Declarations
    LgwResultsTextView *results_text_view = LGW_RESULTSTEXTVIEW (data);

    //Initializations

    printf("BREAK lgw_searchentry_paste\n");
}

