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
//! @file mainwindow-callbacks.c
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

#include <gwaei/gettext.h>
#include <gwaei/gwaei.h>

#include <gwaei/mainwindow-private.h>


void
gw_mainwindow_application_property_changed_cb (GwMainWindow *main_window,
                                               GParamSpec   *pspec,
                                               gpointer      data)
{
    //Sanity checks
    g_return_if_fail (GW_IS_MAINWINDOW (main_window));

    //Declarations
    GwMainWindowPrivate *priv = NULL;
    GtkApplication *application = NULL;
    LgwDictionaryListStore *dictionary_list_store = NULL;
    LgwVocabularyListStore *vocabulary_list_store = NULL;

    //Initializations
    priv = main_window->priv;
    if (priv == NULL) goto errored;
    application = gtk_window_get_application (GTK_WINDOW (main_window));
    if (application == NULL) goto errored;
    dictionary_list_store = gw_application_get_installed_dictionaryliststore (GW_APPLICATION (application));
    vocabulary_list_store = gw_application_get_vocabularyliststore (GW_APPLICATION (application));

    lgw_searchwidget_set_dictionaryliststore (priv->ui.search_widget, dictionary_list_store);
    lgw_vocabularywidget_set_vocabularyliststore (priv->ui.vocabulary_widget, vocabulary_list_store);

    if (application != NULL)
    {
      gw_mainwindow_application_visible_child_property_changed_cb (main_window, pspec, data);
    }

errored:

    return;
}


void
gw_mainwindow_application_visible_child_property_changed_cb (GwMainWindow *main_window,
                                                             GParamSpec   *pspec,
                                                             gpointer      data)
{
    //Sanity checks
    g_return_if_fail (GW_IS_MAINWINDOW (main_window));

    //Declarations
    GwMainWindowPrivate *priv = NULL;
    GtkApplication *application = NULL;
    LgwDictionaryListStore *dictionary_list_store = NULL;
    GtkWidget *widget = NULL;
    LgwActionable *actionable = NULL;

    //Initializations
    priv = main_window->priv;
    if (priv == NULL) goto errored;
    widget = gtk_stack_get_visible_child (priv->ui.stack);
    application = gtk_window_get_application (GTK_WINDOW (main_window));
    if (application == NULL) goto errored;
    actionable = LGW_ACTIONABLE (main_window);

    lgw_actionable_sync_actions (actionable);

    //Assign the menus
    if (widget != NULL && LGW_IS_MENUABLE (widget)) {
      LgwMenuable *menuable = LGW_MENUABLE (widget);
      lgw_window_set_window_menumodel (LGW_WINDOW (main_window), lgw_menuable_get_window_menu_model (menuable));
      lgw_window_set_button_menumodel (LGW_WINDOW (main_window), lgw_menuable_get_button_menu_model (menuable));
    }
    else
    {
      lgw_window_set_window_menumodel (LGW_WINDOW (main_window), NULL);
      lgw_window_set_button_menumodel (LGW_WINDOW (main_window), NULL);
    }

    g_signal_handler_block (priv->ui.search_toggle_button, priv->data.signalid[SIGNALID_SEARCH_BUTTON_TOGGLED]);
    gtk_toggle_button_set_active (priv->ui.search_toggle_button, (widget == GTK_WIDGET (priv->ui.search_widget)));
    g_signal_handler_unblock (priv->ui.search_toggle_button, priv->data.signalid[SIGNALID_SEARCH_BUTTON_TOGGLED]);

errored:

    return;
}


void
gw_mainwindow_close_cb (GSimpleAction *action,
                        GVariant      *parameter,
                        gpointer       data)
{
    //Sanity checks
    g_return_if_fail (data != NULL);

    //Declarations
    GwMainWindow *window = NULL;
    GtkWidget *widget = NULL;

    //Initializations
    window = GW_MAINWINDOW (data);
    if (window == NULL) goto errored;
    widget = GTK_WIDGET (data);
    if (widget == NULL) goto errored;

    gtk_widget_destroy (widget);

errored:

    return;
}


//!
//! @brief Sets the show menu boolean to match the widget
//! @see gw_mainwindow_set_menu_show ()
//! @param widget Unused GtkWidget pointer.
//! @param data Unused gpointer
//!
void
gw_mainwindow_menubar_show_toggled_cb (GSimpleAction *action, 
                                       GVariant      *parameter, 
                                       gpointer       data)
{
    //Sanity checks
    g_return_if_fail (data != NULL);

    //Declarations
    GwMainWindow *window = NULL;
    LwPreferences *preferences = NULL;
    gboolean show = FALSE;

    //Initializations
    window = GW_MAINWINDOW (data);
    preferences = lw_preferences_get_default ();
    show = lw_preferences_get_boolean_by_schema (preferences, LW_SCHEMA_BASE, LW_KEY_MENUBAR_SHOW);

    lw_preferences_set_boolean_by_schema (preferences, LW_SCHEMA_BASE, LW_KEY_MENUBAR_SHOW, !show);
}


//!
//! @brief Syncs the gui to the preference settinging.  It should be attached to the gsettings object
//!
void
gw_mainwindow_sync_show_menubar_cb (GSettings *settings,
                                    gchar     *key,
                                    gpointer   data)
{
    //Declarations
    GwMainWindow *window = NULL;
    GwMainWindowPrivate *priv = NULL;
    gboolean show_menubar = FALSE;
    GAction *action = NULL;

    //Initializations
    window = GW_MAINWINDOW (data);
    if (window == NULL) goto errored;
    priv = window->priv;
    show_menubar = lw_preferences_get_boolean (settings, key);
    action = g_action_map_lookup_action (G_ACTION_MAP (window), "toggle-menubar-show");
    if (action == NULL) goto errored;

    g_simple_action_set_state (G_SIMPLE_ACTION (action), g_variant_new_boolean (show_menubar));

errored:

    return;
}


void
lgw_mainwindow_child_actions_property_changed_cb (GwMainWindow *main_window,
                                                  GParamSpec      *pspec,
                                                  LgwActionable   *actionable)
{
    //Sanity checks
    g_return_if_fail (GW_IS_MAINWINDOW (main_window));
    g_return_if_fail (LGW_IS_ACTIONABLE (main_window));
    g_return_if_fail (LGW_IS_ACTIONABLE (actionable));

    printf("lgw_mainwindow_child_actions_property_changed_cb\n"); 

    lgw_actionable_sync_actions (LGW_ACTIONABLE (main_window));
}


void
lgw_mainwindow_search_button_toggled_cb (GwMainWindow    *main_window,
                                         GtkToggleButton *toggle_button)
{
    //Sanity checks
    g_return_if_fail (GW_IS_MAINWINDOW (main_window));
    g_return_if_fail (GTK_IS_TOGGLE_BUTTON (toggle_button));

    //Declarations
    GwMainWindowPrivate *priv = NULL;
    gboolean is_active = FALSE;
    GtkWidget *child = NULL;

    //Initializations
    priv = main_window->priv;
    is_active = gtk_toggle_button_get_active (toggle_button);

    if (is_active)
    {
      child = GTK_WIDGET (priv->ui.search_widget);
      gtk_stack_set_transition_type (priv->ui.stack, GTK_STACK_TRANSITION_TYPE_SLIDE_DOWN);
    }
    else 
    {
      child = GTK_WIDGET (priv->ui.vocabulary_widget);
      gtk_stack_set_transition_type (priv->ui.stack, GTK_STACK_TRANSITION_TYPE_SLIDE_UP);
    }

    gtk_stack_set_visible_child (priv->ui.stack, child);
}


void
gw_mainwindow_connect_signals (GwMainWindow *window) {
    //Sanity checks
    g_return_if_fail (GW_IS_MAINWINDOW (window));

    //Declarations
    GwMainWindowPrivate *priv = NULL;
    LwPreferences *preferences = NULL;

    //Initializations
    priv = window->priv;
    preferences = lw_preferences_get_default ();

    if (priv->data.signalid[SIGNALID_APPLICATION_PROPERTY_CHANGED] == 0)
    {
      priv->data.signalid[SIGNALID_APPLICATION_PROPERTY_CHANGED] = g_signal_connect (
          window, 
          "notify::application",
          G_CALLBACK (gw_mainwindow_application_property_changed_cb),
          NULL
      );
    }

    if (priv->data.signalid[SIGNALID_STACK_VISIBLE_CHILD_PROPERTY_CHANGED] == 0)
    {
      priv->data.signalid[SIGNALID_STACK_VISIBLE_CHILD_PROPERTY_CHANGED] = g_signal_connect_swapped (
          priv->ui.stack,
          "notify::visible-child",
          G_CALLBACK (gw_mainwindow_application_visible_child_property_changed_cb),
          window
      );
    }

    if (priv->data.signalid[SIGNALID_SHOW_MENUBAR] == 0)
    {
      priv->data.signalid[SIGNALID_SHOW_MENUBAR] = lw_preferences_add_change_listener_by_schema (
        preferences,
        LW_SCHEMA_BASE,
        LW_KEY_MENUBAR_SHOW,
        gw_mainwindow_sync_show_menubar_cb,
        window
      );
    }

    if (priv->data.signalid[SIGNALID_SEARCHWIDGET_ACTIONS] == 0)
    {
      priv->data.signalid[SIGNALID_SEARCHWIDGET_ACTIONS] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.search_widget),
          "notify::actions",
          G_CALLBACK (lgw_mainwindow_child_actions_property_changed_cb),
          window
      );
    }

    if (priv->data.signalid[SIGNALID_VOCABULARYWIDGET_ACTIONS] == 0)
    {
      priv->data.signalid[SIGNALID_VOCABULARYWIDGET_ACTIONS] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.vocabulary_widget),
          "notify::actions",
          G_CALLBACK (lgw_mainwindow_child_actions_property_changed_cb),
          window
      );
    }

    if (priv->data.signalid[SIGNALID_SEARCH_BUTTON_TOGGLED] == 0)
    {
      priv->data.signalid[SIGNALID_SEARCH_BUTTON_TOGGLED] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.search_toggle_button),
          "toggled",
          G_CALLBACK (lgw_mainwindow_search_button_toggled_cb),
          window
      );
    }

    if (priv->data.signalid[SIGNALID_SEARCHENTRY_ACTIONS] == 0)
    {
      priv->data.signalid[SIGNALID_SEARCHENTRY_ACTIONS] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.search_entry),
          "notify::actions",
          G_CALLBACK (lgw_mainwindow_child_actions_property_changed_cb),
          window
      );
    }
}


void
gw_mainwindow_disconnect_signals (GwMainWindow *window) {
    //Sanity checks
    g_return_if_fail (GW_IS_MAINWINDOW (window));

    //Declarations
    GwMainWindowPrivate *priv = NULL;
    LwPreferences *preferences = NULL;

    //Initializations
    priv = window->priv;
    preferences = lw_preferences_get_default ();

    if (priv->data.signalid[SIGNALID_APPLICATION_PROPERTY_CHANGED] != 0)
    {
      g_signal_handler_disconnect (
        G_OBJECT (window),
        priv->data.signalid[SIGNALID_APPLICATION_PROPERTY_CHANGED]
      );
      priv->data.signalid[SIGNALID_APPLICATION_PROPERTY_CHANGED] = 0;
    }

    if (priv->data.signalid[SIGNALID_STACK_VISIBLE_CHILD_PROPERTY_CHANGED] != 0)
    {
      g_signal_handler_disconnect (
        G_OBJECT (priv->ui.stack),
        priv->data.signalid[SIGNALID_STACK_VISIBLE_CHILD_PROPERTY_CHANGED]
      );
      priv->data.signalid[SIGNALID_STACK_VISIBLE_CHILD_PROPERTY_CHANGED] = 0;
    }

    if (priv->data.signalid[SIGNALID_SHOW_MENUBAR] != 0) {
      lw_preferences_remove_change_listener_by_schema (
          preferences,
          LW_SCHEMA_BASE,
          priv->data.signalid[SIGNALID_SHOW_MENUBAR]
      );
      priv->data.signalid[SIGNALID_SHOW_MENUBAR] = 0;
    }

    if (priv->data.signalid[SIGNALID_SEARCHWIDGET_ACTIONS] != 0)
    {
      g_signal_handler_disconnect (
        G_OBJECT (priv->ui.search_widget),
        priv->data.signalid[SIGNALID_SEARCHWIDGET_ACTIONS]
      );
      priv->data.signalid[SIGNALID_SEARCHWIDGET_ACTIONS] = 0;
    }

    if (priv->data.signalid[SIGNALID_VOCABULARYWIDGET_ACTIONS] != 0)
    {
      g_signal_handler_disconnect (
        G_OBJECT (priv->ui.vocabulary_widget),
        priv->data.signalid[SIGNALID_VOCABULARYWIDGET_ACTIONS]
      );
      priv->data.signalid[SIGNALID_VOCABULARYWIDGET_ACTIONS] = 0;
    }

    if (priv->data.signalid[SIGNALID_SEARCH_BUTTON_TOGGLED] != 0)
    {
      g_signal_handler_disconnect (
        G_OBJECT (priv->ui.search_toggle_button),
        priv->data.signalid[SIGNALID_SEARCH_BUTTON_TOGGLED]
      );
      priv->data.signalid[SIGNALID_SEARCH_BUTTON_TOGGLED] = 0;
    }

    if (priv->data.signalid[SIGNALID_SEARCHENTRY_ACTIONS] != 0)
    {
      g_signal_handler_disconnect (G_OBJECT (priv->ui.search_entry), priv->data.signalid[SIGNALID_SEARCHENTRY_ACTIONS]);
      priv->data.signalid[SIGNALID_SEARCHENTRY_ACTIONS] = 0;
    }
}

