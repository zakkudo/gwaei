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
//! @file application-callbacks.c
//!
//! @brief To be written
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>

#include <gtk/gtk.h>

#include <gwaei/gettext.h>
#include <gwaei/gwaei.h>
#include <gwaei/application-private.h>


void 
gw_application_connect_signals (GwApplication *self)
{
    //Sanity checks
    g_return_if_fail (GW_IS_APPLICATION (self));

#ifdef HAVE_HUNSPELL
    //Sanity checks

    //Declarations
    LwPreferences *preferences = NULL;

    //Initializations

/*
    lw_preferences_add_change_listener_by_schema (
        preferences,
        LW_SCHEMA_BASE,
        LW_KEY_SPELLCHECK,
        gw_application_sync_spellcheck_cb,
        self 
    );
*/
#endif
}


void 
gw_application_disconnect_signals (GwApplication *self)
{
    //Sanity checks
    g_return_if_fail (GW_IS_APPLICATION (self));
}


/* TO BE REMOVED
void 
gw_application_open_searchwindow_cb (GSimpleAction *action, 
                                     GVariant      *parameter, 
                                     gpointer       data)
{
    //Declarations
    GwApplication *self;
    GtkWindow *window;

    //Initializations
    self = GW_APPLICATION (data);
    g_return_if_fail (self != NULL);
    window = gw_searchwindow_new (GTK_APPLICATION (self));

    gtk_widget_show (GTK_WIDGET (window));
}
*/


void 
gw_application_quit_cb (GSimpleAction *action,
                        GVariant      *parameter,
                        gpointer       data)
{
    gw_application_quit (GW_APPLICATION (data));
}


/*
void 
gw_application_open_settingswindow_cb (GSimpleAction *action,
                                       GVariant      *parameter,
                                       gpointer       data)
{
    //Declarations
    GwApplication *self;
    GwMainWindow *searchwindow;
    GtkWindow *settingswindow;
    GList *link;

    //Initializations
    searchwindow = GW_MAINWINDOW (gw_application_get_last_focused_searchwindow (GW_APPLICATION (data)));
    self = gw_window_get_application (GW_WINDOW (searchwindow));
    link = gtk_application_get_windows (GTK_APPLICATION (self));

    while (link != NULL && !GW_IS_SETTINGSWINDOW (link->data)) link = link->next;

    if (link != NULL)
    {
      settingswindow = GTK_WINDOW (link->data);
      gtk_window_set_transient_for (GTK_WINDOW (settingswindow), GTK_WINDOW (searchwindow));
      gtk_window_present (GTK_WINDOW (settingswindow));
    }
    else
    {
      settingswindow = gw_settingswindow_new (GTK_APPLICATION (self));
      gtk_window_set_transient_for (GTK_WINDOW (settingswindow), GTK_WINDOW (searchwindow));
      gtk_widget_show (GTK_WIDGET (settingswindow));
    }
}


void 
gw_application_open_vocabularywindow_cb (GSimpleAction *action, 
                                         GVariant      *parameter,
                                         gpointer       data)
{
    GwApplication *self;

    self = GW_APPLICATION (data);

    gw_application_show_vocabularywindow (self, -1);
}


void 
gw_application_open_vocabularywindow_index_cb (GSimpleAction *action, 
                                               GVariant      *parameter,
                                               gpointer       data)
{
    GwApplication *self;
    const gchar *value;
    gint index;

    self = GW_APPLICATION (data);
    value = g_variant_get_string (parameter, NULL);
    index = (gint) g_ascii_strtoll (value, NULL, 10);

    gw_application_show_vocabularywindow (self, index);
}
*/

//!
//! @brief Opens the gWaei about dialog
//! @param widget Unused GtkWidget pointer
//! @param data Unused gpointer
//!
void 
gw_application_open_aboutdialog_cb (GSimpleAction *action, 
                                    GVariant      *parameter,
                                    gpointer       data)
{
    gchar *path;

#ifndef G_OS_WIN32
    path = g_build_filename (DATADIR2, PACKAGE, "logo.png", NULL);
#else
    gchar *prefix;

    prefix = g_win32_get_package_installation_directory_of_module (NULL);
    path = g_build_filename (prefix, "share", PACKAGE, "logo.png", NULL);
    g_free (prefix);
#endif

    gchar *programmer_credits[] = 
    {
      "Zachary Dovel <pizzach@gmail.com>",
      "Fabrizio Sabatini",
      NULL
    };

    GdkPixbuf *logo;
    if ( (logo = gdk_pixbuf_new_from_file (path, NULL)) == NULL)
    {
      printf ("Was unable to load the gwaei logo.\n");
    }

    GtkWidget *about = g_object_new (GTK_TYPE_ABOUT_DIALOG,
               "program-name", "gWaei", 
               "version", VERSION,
               "copyright", "gWaei (C) 2008-2012 Zachary Dovel\n" 
                            "Kanjipad backend (C) 2002 Owen Taylor\n"
                            "JStroke backend (C) 1997 Robert Wells\n"
                            "Dedicated to Chuus (gl :-O)",
               "comments", gettext("Program for Japanese translation and reference. The\n"
                                    "dictionaries are supplied by Jim Breen's WWWJDIC.\n"
                                    "Special thanks to the maker of GJITEN who served as an inspiration."),
               "license", "This software is GPL Licensed.\n\n"
                          "gWaei is free software: you can redistribute it and/or modify\n"
                          "it under the terms of the GNU General Public License as published by\n "
                          "the Free Software Foundation, either version 3 of the License, or\n"
                          "(at your option) any later version.\n\n"
                          "gWaei is distributed in the hope that it will be useful,\n"
                          "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
                          "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
                          "GNU General Public License for more details.\n\n"
                          "You should have received a copy of the GNU General Public License\n"
                          "along with gWaei.  If not, see <http://www.gnu.org/licenses/>.",
               "logo", logo,
               // TRANSLATORS: You can add your own name to the translation of this field, it will be displayed in the "about" box when gwaei is run in your language
               "translator-credits", gettext("translator-credits"),
               "authors", programmer_credits,
               "website", "http://gwaei.sourceforge.net/",
               NULL);
    gtk_dialog_run (GTK_DIALOG (about));
    g_object_unref (logo);
    gtk_widget_destroy (about);
}


//!
//! @brief Sends the user to the gWaei irc channel for help
//! @param widget Unused GtkWidget pointer
//! @param data Unused gpointer
//!
void 
gw_application_open_irc_channel_cb (GSimpleAction *action, 
                                    GVariant      *parameter,
                                    gpointer       data)
{
/*
    //Initializations
    GError *error;
    GwMainWindow *window;
    GwApplication *self;

    //Declarations
    error = NULL;
    window = GW_MAINWINDOW (gtk_widget_get_ancestor (GTK_WIDGET (data), GW_TYPE_MAINWINDOW));
    g_return_if_fail (window != NULL);
    self = gw_window_get_application (GW_WINDOW (window));

    gtk_show_uri (NULL, "irc://irc.freenode.net/gWaei", gtk_get_current_event_time (), &error);

    //Cleanup
    //gw_application_handle_error (self, GTK_WINDOW (window), TRUE, &error);
*/
}


//!
//! @brief Sends the user to the gWaei homepage for whatever they need
//! @param widget Unused GtkWidget pointer
//! @param data Unused gpointer
//!
void 
gw_application_open_homepage_cb (GSimpleAction *action, 
                                 GVariant      *parameter,
                                 gpointer       data)
{
/*
    //Declarations
    GError *error;
    GwMainWindow *window;
    GwApplication *self;

    //Initializations
    error = NULL;
    window = GW_MAINWINDOW (gtk_widget_get_ancestor (GTK_WIDGET (data), GW_TYPE_MAINWINDOW));
    g_return_if_fail (window != NULL);
    self = gw_window_get_application (GW_WINDOW (window));

    gtk_show_uri (NULL, "http://gwaei.sourceforge.net/", gtk_get_current_event_time (), &error);

    //Cleanup
    //gw_application_handle_error (self, GTK_WINDOW (window), TRUE, &error);
*/
}


//!
//! @brief Opens the gWaei help documentation
//! @param widget Unused GtkWidget pointer
//! @param data Unused gpointer
//!
void 
gw_application_open_help_cb (GSimpleAction *action,
                             GVariant      *parameter,
                             gpointer       data)
{
    gtk_show_uri (NULL, "ghelp:gwaei", gtk_get_current_event_time (), NULL);
}


//!
//! @brief Opens the gWaei dictionary glossary help documentation
//! @param widget Unused GtkWidget pointer
//! @param data Unused gpointer
//!
void 
gw_application_open_glossary_cb (GSimpleAction *action,
                                 GVariant      *parameter,
                                 gpointer       data)
{
    //Declarations
    gchar *uri;
    GError *error;
    GwApplication *self;

    //Initializations
    self = GW_APPLICATION (data);
    uri = g_build_filename ("ghelp://", DATADIR2, "gnome", "help", "gwaei", "C", "glossary.xml", NULL);
    error = NULL;
    
    if (uri != NULL)
    {
      gtk_show_uri (NULL, uri, gtk_get_current_event_time (), &error);
      g_free (uri); uri = NULL;
    }

    //gw_application_handle_error (self, NULL, FALSE, &error);
}


#ifdef HAVE_HUNSPELL
//!
//! @brief Callback to toggle spellcheck in the search entry
//! @param widget Unused pointer to a GtkWidget
//! @param data Unused gpointer
//!
void 
gw_application_spellcheck_toggled_cb (GSimpleAction *action, 
                                      GVariant      *parameter, 
                                      gpointer       data)
{
    //Declarations
    GwApplication *self = NULL;
    LwPreferences *preferences = NULL;
    gboolean state = FALSE;

    //Initializations
    self = GW_APPLICATION (data);
    preferences = lw_preferences_get_default ();
    state = lw_preferences_get_boolean_by_schema (preferences, LW_SCHEMA_BASE, LW_KEY_SPELLCHECK);

    lw_preferences_set_boolean_by_schema (preferences, LW_SCHEMA_BASE, LW_KEY_SPELLCHECK, !state);
}

void
gw_application_sync_spellcheck_cb (GSettings *settings,
                                   gchar     *key,
                                   gpointer   data)
{
    //Declarations
    GwApplication *self;
    gboolean state;
    GAction *action;

    //Initializations
    self = GW_APPLICATION (data);
    state = lw_preferences_get_boolean (settings, key);
    action = g_action_map_lookup_action (G_ACTION_MAP (self), "toggle-spellcheck");

    if (action != NULL) g_simple_action_set_state (G_SIMPLE_ACTION (action), g_variant_new_boolean (state));
}
#endif


