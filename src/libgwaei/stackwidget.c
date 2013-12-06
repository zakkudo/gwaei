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
//! @file stackwidget.c
//!
//! @brief To be written
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>

G_DEFINE_INTERFACE (LgwStackWidget, lgw_stackwidget, 0);


static void
lgw_stackwidget_default_init (LgwStackWidgetInterface *class)
{
        /* add properties and signals to the interface here */
}


GMenuModel*
lgw_stackwidget_get_button_menu_model (LgwStackWidget *self)
{
    g_return_val_if_fail (self != NULL, NULL);

    return LGW_STACKWIDGET_GET_INTERFACE (self)->get_button_menu_model (self);
}


GMenuModel*
lgw_stackwidget_get_window_menu_model (LgwStackWidget *self)
{
    g_return_val_if_fail (self != NULL, NULL);

    return LGW_STACKWIDGET_GET_INTERFACE (self)->get_window_menu_model (self);
}

