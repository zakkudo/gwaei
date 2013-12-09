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


gboolean
gw_mainwindow_focus_cb (GtkWidget        *widget,
                        GdkEvent *event,
                        gpointer          data);

void
gw_mainwindow_connect_signals (GwMainWindow *window) {
    //Sanity checks
    g_return_if_fail (window != NULL);

    //Declarations
    GwMainWindowPrivate *priv = NULL;

    //Initializations
    priv = window->priv;

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


      /*priv->data.signalid[SIGNALID_STACK_VISIBLE_CHILD_PROPERTY_CHANGED] =*/ g_signal_connect (
          window,
          "move-focus",
          G_CALLBACK (gw_mainwindow_focus_cb),
          NULL
      );
}


void
gw_mainwindow_disconnect_signals (GwMainWindow *window) {
    //Sanity checks
    g_return_if_fail (window != NULL);

    //Declarations
    GwMainWindowPrivate *priv = NULL;

    //Initializations
    priv = window->priv;

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
    g_object_get (G_OBJECT (main_window), "application", &application, NULL);
    if (application == NULL) goto errored;
    dictionarylist = gw_application_get_installed_dictionarylist (GW_APPLICATION (application));

    lgw_searchwidget_set_dictionarylist (priv->ui.search_widget, dictionarylist);

errored:

    if (application != NULL) g_object_unref (application);

    return;
}


void
gw_mainwindow_application_visible_child_property_changed_cb (GwMainWindow *main_window,
                                                             GParamSpec   *pspec,
                                                             gpointer      data)
{
    //Declarations
    GwMainWindowPrivate *priv = NULL;
    GtkApplication *application = NULL;
    LwDictionaryList *dictionarylist = NULL;
    GObject *widget = NULL;

    priv = main_window->priv;
    if (priv == NULL) goto errored;
    g_object_get (G_OBJECT (priv->ui.stack), "visible-child", &widget, NULL);

    if (widget != NULL && LGW_IS_STACKWIDGET (widget)) {
      GMenuModel *menu_model = lgw_stackwidget_get_button_menu_model (LGW_STACKWIDGET (widget));
      gtk_menu_button_set_menu_model (priv->ui.menu_button, menu_model);
      printf("visible child changed\n");
    }
    else
    {
      gtk_menu_button_set_menu_model (priv->ui.menu_button, NULL);
    }

errored:

    return;
}


gboolean
gw_mainwindow_focus_cb (GtkWidget *widget,
                        GdkEvent  *event,
                        gpointer   data)
{
    printf("focus changed\n");
    return FALSE;
}


void
gw_mainwindow_close_cb (GSimpleAction *action,
                        GVariant      *parameter,
                        gpointer       data)
{
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

