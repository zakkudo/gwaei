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
//! @file window-callbacks.c
//!
//! @brief To be written
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gtk/gtk.h>

#include <libgwaei/libgwaei.h>
#include <libgwaei/window-private.h>
#include <libgwaei/gettext.h>


void
lgw_window_connect_signals (LgwWindow *window)
{
    //Sanity checks
    g_return_if_fail (window != NULL);

    //Declarations
    LgwWindowPrivate *priv = NULL;
    LwPreferences *preferences = NULL;

    //Initializations
    priv = window->priv;
    preferences = lw_preferences_get_default ();

    if (priv->data.signalid[SIGNALID_CONFIGURE_EVENT] == 0)
    {
      priv->data.signalid[SIGNALID_CONFIGURE_EVENT] = g_signal_connect (
        G_OBJECT (window),
        "configure-event",
        G_CALLBACK (lgw_window_configure_event_cb),
        NULL
      );
    }

    if (priv->data.signalid[SIGNALID_FOCUS_IN_EVENT] == 0)
    {
      priv->data.signalid[SIGNALID_FOCUS_IN_EVENT] = g_signal_connect (
        G_OBJECT (window),
        "focus-in-event",
        G_CALLBACK (lgw_window_focus_in_event_cb),
        NULL
      );
    }

    if (priv->data.signalid[SIGNALID_SHOW_MENUBAR] == 0)
    {
      priv->data.signalid[SIGNALID_SHOW_MENUBAR] = lw_preferences_add_change_listener_by_schema (
        preferences,
        LW_SCHEMA_BASE,
        LW_KEY_MENUBAR_SHOW,
        lgw_window_sync_show_menubar_cb,
        window
      );
    }

    if (priv->data.signalid[SIGNALID_APPLICATION_PROPERTY] == 0)
    {
      priv->data.signalid[SIGNALID_APPLICATION_PROPERTY] = g_signal_connect (
        G_OBJECT (window),
        "notify::application",
        G_CALLBACK (lgw_window_application_property_changed_cb),
        NULL
      );
    }
}


void
lgw_window_disconnect_signals (LgwWindow *window)
{
    //Sanity checks
    g_return_if_fail (window != NULL);

    //Declarations
    LgwWindowPrivate *priv = NULL;
    LwPreferences *preferences = NULL;

    //Initializations
    priv = window->priv;
    preferences = lw_preferences_get_default ();
    
    if (priv->data.signalid[SIGNALID_CONFIGURE_EVENT] != 0) {
      g_signal_handler_disconnect (G_OBJECT (window), priv->data.signalid[SIGNALID_CONFIGURE_EVENT]);
      priv->data.signalid[SIGNALID_CONFIGURE_EVENT] = 0;
    }

    if (priv->data.signalid[SIGNALID_FOCUS_IN_EVENT] != 0) {
      g_signal_handler_disconnect (G_OBJECT (window), priv->data.signalid[SIGNALID_FOCUS_IN_EVENT]);
      priv->data.signalid[SIGNALID_FOCUS_IN_EVENT] = 0;
    }

    if (priv->data.signalid[SIGNALID_SHOW_MENUBAR] != 0) {
      lw_preferences_remove_change_listener_by_schema (
          preferences,
          LW_SCHEMA_BASE,
          priv->data.signalid[SIGNALID_SHOW_MENUBAR]
      );
      priv->data.signalid[SIGNALID_SHOW_MENUBAR] = 0;
    }

    if (priv->data.signalid[SIGNALID_APPLICATION_PROPERTY] != 0) {
      g_signal_handler_disconnect (G_OBJECT (window), priv->data.signalid[SIGNALID_APPLICATION_PROPERTY]);
      priv->data.signalid[SIGNALID_APPLICATION_PROPERTY] = 0;
    }


    if (preferences != NULL) g_object_unref (preferences);
}


gboolean 
lgw_window_configure_event_cb (LgwWindow *window,
                               GdkEvent  *event,
                               gpointer   data)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_WINDOW (window), FALSE);

    //Delcarations
    LgwWindowPrivate *priv = NULL;
    GdkEventConfigure *event_configure = NULL;

    //Initializations
    priv = window->priv;
    event_configure = (GdkEventConfigure*) event;

    priv->data.x = event_configure->x;
    priv->data.y = event_configure->y;
    priv->data.width = event_configure->width;
    priv->data.height = event_configure->height;

    return FALSE;
}


gboolean
lgw_window_focus_in_event_cb (LgwWindow *window,
                              GdkEvent  *event,
                              gpointer   data)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_WINDOW (window), FALSE);

    //Declarations
    GtkApplication *application = NULL;
    GMenuModel *menu_model = NULL;
    gboolean os_shows_win_menu = NULL;
    GtkSettings *settings = NULL;
    
    //Initializations
    application = gtk_window_get_application (GTK_WINDOW (window));
    settings = gtk_settings_get_default ();
    g_object_get (settings, "gtk-shell-shows-menubar", &os_shows_win_menu, NULL);

    menu_model = lgw_window_get_transient_for_menumodel (window);
    if (menu_model == NULL)
      menu_model = G_MENU_MODEL (g_menu_new ());
    if (menu_model == NULL) 
      return FALSE;

   gtk_application_set_menubar (application, menu_model); 

   return FALSE;
}


//!
//! @brief Syncs the gui to the preference settinging.  It should be attached to the gsettings object
//!
void
lgw_window_sync_show_menubar_cb (GSettings *settings,
                                 gchar     *key,
                                 gpointer   data)
{
    //Declarations
    LgwWindow *window = NULL;
    LgwWindowPrivate *priv = NULL;

    //Initializations
    window = LGW_WINDOW (data);
    priv = window->priv;
    priv->config.show_menubar = lw_preferences_get_boolean (settings, key);

    lgw_window_sync_menubar_show (window);
}


void
lgw_window_application_property_changed_cb (LgwWindow  *window,
                                            GParamSpec *pspec,
                                            gpointer    data)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_WINDOW (window));

    //Declarations
    LgwWindowPrivate *priv = NULL;
    GtkApplication *application = NULL;

    //Initializations
    priv = window->priv;
    application = gtk_window_get_application (GTK_WINDOW (window));
    if (application == NULL) goto errored;
    if (!gtk_widget_is_visible (GTK_WIDGET (window))) goto errored;

    lgw_window_set_window_menumodel (window, lgw_window_get_window_menumodel (window)); 
    lgw_window_set_button_menumodel (window, lgw_window_get_button_menumodel (window)); 

errored:

    return;
}

