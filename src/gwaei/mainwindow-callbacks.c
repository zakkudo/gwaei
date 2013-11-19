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
                                               GParamSpec *pspec,
                                               gpointer data)
{
    //Declarations
    GwMainWindowPrivate *priv = NULL;
    GtkApplication *application = NULL;
    LgwDictionaryList *dictionarylist = NULL;

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


