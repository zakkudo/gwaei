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
//! @file libgwaei.c
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


gchar*
lgw_get_symbolic_icon_name_if_exists (const gchar* ICON_NAME) {
    //Sanity checks
    g_return_val_if_fail (ICON_NAME != NULL, NULL);

    //Delcarations
    GtkIconTheme *theme = NULL;
    gchar* symbolic_name = NULL;
    gchar *icon_name = NULL;

    //Initializations
    theme = gtk_icon_theme_get_default (); if (theme == NULL) goto errored;
    symbolic_name = g_strjoin ("-", ICON_NAME, "symbolic"); if (symbolic_name == NULL) goto errored;

    if (gtk_icon_theme_has_icon (theme, symbolic_name)) {
      icon_name = symbolic_name;
      symbolic_name = NULL;
    }
    else {
      icon_name = g_strdup (ICON_NAME);
    }

errored:

    g_free (symbolic_name); symbolic_name = NULL;

    return icon_name;
}
