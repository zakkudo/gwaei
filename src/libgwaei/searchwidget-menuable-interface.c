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
//! @file searchwidget.c
//!
//! @brief To be written
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include <glib-object.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>

#include <libgwaei/searchwidget-private.h>


static GMenuModel*
lgw_searchwidget_get_button_menu_model (LgwMenuable *menuable)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_SEARCHWIDGET (menuable));

    //Declarations
    LgwSearchWidget *search_widget = LGW_SEARCHWIDGET (menuable);
    LgwSearchWidgetPrivate *priv = NULL;

    //Initializations
    priv = search_widget->priv;

    return priv->data.button_menu_model;
}


static GMenuModel*
lgw_searchwidget_get_window_menu_model (LgwMenuable *menuable)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_SEARCHWIDGET (menuable));

    //Declarations
    LgwSearchWidget *search_widget = LGW_SEARCHWIDGET (menuable);
    LgwSearchWidgetPrivate *priv = NULL;

    //Initializations
    priv = search_widget->priv;

    return priv->data.window_menu_model;
}


void
lgw_searchwidget_implement_menuable_interface (LgwMenuableInterface *iface) {
    iface->get_window_menu_model = lgw_searchwidget_get_window_menu_model;
    iface->get_button_menu_model = lgw_searchwidget_get_button_menu_model;
}

