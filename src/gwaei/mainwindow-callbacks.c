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
gw_mainwindow_connect_signals (GwMainWindow *window) {
    //Sanity checks
    g_return_if_fail (window != NULL);

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
}


void
gw_mainwindow_disconnect_signals (GwMainWindow *window) {
    //Sanity checks
    g_return_if_fail (window != NULL);

    //Declarations
    GwMainWindowPrivate *priv = NULL;
    LwPreferences *preferences = NULL;

    //Initializations
    priv = window->priv;
    preferences = lw_preferences_get_default ();

    if (priv->data.signalid[SIGNALID_APPLICATION_PROPERTY_CHANGED] != 0)
    {
      g_signal_handler_disconnect (window, priv->data.signalid[SIGNALID_APPLICATION_PROPERTY_CHANGED]);
      priv->data.signalid[SIGNALID_APPLICATION_PROPERTY_CHANGED] = 0;
    }

    if (priv->data.signalid[SIGNALID_STACK_VISIBLE_CHILD_PROPERTY_CHANGED] != 0)
    {
      g_signal_handler_disconnect (priv->ui.stack, priv->data.signalid[SIGNALID_STACK_VISIBLE_CHILD_PROPERTY_CHANGED]);
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
      g_signal_handler_disconnect (G_OBJECT (priv->ui.search_widget), priv->data.signalid[SIGNALID_SEARCHWIDGET_ACTIONS]);
      priv->data.signalid[SIGNALID_SEARCHWIDGET_ACTIONS] = 0;
    }

}


void
gw_mainwindow_application_property_changed_cb (GwMainWindow *main_window,
                                               GParamSpec   *pspec,
                                               gpointer      data)
{
    //Declarations
    GwMainWindowPrivate *priv = NULL;
    GtkApplication *application = NULL;
    LwDictionaryList *dictionarylist = NULL;

    priv = main_window->priv;
    if (priv == NULL) goto errored;
    application = gtk_window_get_application (GTK_WINDOW (main_window));
    if (application == NULL) goto errored;
    dictionarylist = gw_application_get_installed_dictionarylist (GW_APPLICATION (application));

    lgw_searchwidget_set_dictionarylist (priv->ui.search_widget, dictionarylist);

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
    printf("BREAK0 visible child changed\n");
    //Declarations
    GwMainWindowPrivate *priv = NULL;
    GtkApplication *application = NULL;
    LwDictionaryList *dictionarylist = NULL;
    GtkWidget *widget = NULL;
    LgwActionable *actionable = NULL;

    //Initializations
    priv = main_window->priv;
    if (priv == NULL) goto errored;
    widget = gtk_stack_get_visible_child (priv->ui.stack);
    application = gtk_window_get_application (GTK_WINDOW (main_window));
    if (application == NULL) goto errored;
    actionable = LGW_ACTIONABLE (main_window);

    gw_mainwindow_sync_actions (main_window);

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

    printf("BREAK1 visible child changed\n");

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
    g_return_if_fail (main_window != NULL);
    g_return_if_fail (actionable != NULL);

    gw_mainwindow_sync_actions (main_window);

}

