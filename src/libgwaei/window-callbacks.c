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


gboolean 
lgw_window_configure_event_cb (GtkWidget *widget,
                               GdkEvent  *event,
                               gpointer   data)
{
    LgwWindow *window = NULL;
    LgwWindowPrivate *priv = NULL;
    GdkEventConfigure *event_configure = NULL;

    window = LGW_WINDOW (widget);
    priv = window->priv;
    event_configure = (GdkEventConfigure*) event;

    priv->data.x = event_configure->x;
    priv->data.y = event_configure->y;
    priv->data.width = event_configure->width;
    priv->data.height = event_configure->height;

    return FALSE;
}


gboolean
lgw_window_focus_in_event_cb (GtkWidget *widget,
                              GdkEvent  *event,
                              gpointer   data)
{
    //Declarations
    LgwWindow *window = NULL;
    GtkApplication *application = NULL;
    GMenuModel *menu_model = NULL;
    gboolean os_shows_win_menu = NULL;
    GtkSettings *settings = NULL;
    
    //Initializations
    window = LGW_WINDOW (widget);
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


