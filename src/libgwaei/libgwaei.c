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


gboolean
lgw_load_xml (GtkBuilder *builder, const gchar *FILENAME)
{
    //Declarations
    gchar *path = NULL;
    gchar *prefix = NULL;
    GError *error = NULL;

    //Initializations
    error = NULL;
#ifndef G_OS_WIN32
    path = g_build_filename (DATADIR2, PACKAGE, FILENAME, NULL);
#else
    prefix = g_win32_get_package_installation_directory_of_module (NULL);
    path = g_build_filename (prefix, "share", PACKAGE, FILENAME, NULL);
    g_free (prefix);
    prefix = NULL;
#endif

    //Search for the files
    if (g_file_test (path, G_FILE_TEST_IS_REGULAR))
    {
      gtk_builder_add_from_file (builder, path,  &error);
      if (error)
      {
        g_warning ("Problems loading xml from %s. %s\n", path, error->message);
        g_error_free (error); error = NULL;
      }
      else
      {
        gtk_builder_connect_signals (builder, NULL);
        return TRUE;
      }
    }

    return FALSE;
}



GMenuModel*
lgw_load_menu_model (const gchar* FILENAME)
{
    //Sanity checks
    g_return_val_if_fail (FILENAME != NULL, NULL);

    //Declarations
    GtkBuilder *builder = NULL;
    gboolean loaded = FALSE;
    GMenuModel *menu_model = NULL;
    
    //Initializations
    loaded = FALSE;
    builder = gtk_builder_new (); 
    if (builder == NULL) goto errored;
    loaded = lgw_load_xml (builder, FILENAME); 
    if (loaded == FALSE) goto errored;
    menu_model = G_MENU_MODEL (gtk_builder_get_object (builder, "menu")); 
    if (menu_model == NULL) goto errored;

errored:

    if (builder != NULL) g_object_unref (builder); builder = NULL;
    if (menu_model == NULL) {
      g_warning ("Failed loading the file, '%s'\n", FILENAME);
    }

    return menu_model;
}


void
lgw_menumodel_set_contents (GMenuModel *menu_model,
                            GMenuModel *other_menu_model)
{
    //Sanity checks
    g_return_if_fail (menu_model != NULL);

    //Declarations
    GMenu *menu = NULL;

    //Initializations
    menu = G_MENU (menu_model);
    if (menu == NULL) goto errored;

    //Clear the menubar
    {
      gint length = g_menu_model_get_n_items (menu_model);
      while (length-- > 0) g_menu_remove (menu, 0);
    }

    //Add the menuitem linking the menus 
    if (other_menu_model != NULL) {
      GMenuItem *menuitem = g_menu_item_new_section (NULL, other_menu_model);
      if (menuitem != NULL)
      {
        g_menu_append_item (menu, menuitem);
        g_object_unref (menuitem);
        menuitem = NULL;
      }
    }

errored:

    return;
}


void
lgw_application_add_accelerators (GtkApplication *application,
                                  GMenuModel     *menumodel)
{
    //Sanity checks
    g_return_if_fail (application != NULL);
    g_return_if_fail (menumodel != NULL);

    //Declarations
    gint total_items = 0;
    gint index = 0;
    gchar *accel = NULL;
    gchar *action = NULL;
    gchar *detail = NULL;
    GMenuModel *sublink = NULL;

    //Initializations
    total_items = g_menu_model_get_n_items (menumodel);

    for (index = 0; index < total_items; index++)
    {
      g_menu_model_get_item_attribute (menumodel, index, "accel", "s", &accel, NULL);
      g_menu_model_get_item_attribute (menumodel, index, G_MENU_ATTRIBUTE_ACTION, "s", &action, NULL);
      g_menu_model_get_item_attribute (menumodel, index, G_MENU_ATTRIBUTE_TARGET, "s", &detail, NULL);
      if (accel != NULL && action != NULL)
      {
        if (detail != NULL)
        {
          gtk_application_add_accelerator (GTK_APPLICATION (application), accel, action, g_variant_new_string (detail));
        }
        else
        {
          gtk_application_add_accelerator (GTK_APPLICATION (application), accel, action, NULL);
        }
      }

      if (accel != NULL) g_free (accel); accel = NULL;
      if (action != NULL) g_free (action); action = NULL;
      if (detail != NULL) g_free (detail); detail = NULL;

      //Recursive work
      sublink = g_menu_model_get_item_link (menumodel, index, G_MENU_LINK_SUBMENU);
      if (sublink != NULL) lgw_application_add_accelerators (application, sublink);
      sublink = g_menu_model_get_item_link (menumodel, index, G_MENU_LINK_SECTION);
      if (sublink != NULL) lgw_application_add_accelerators (application, sublink);
    }
}


void
lgw_application_remove_accelerators (GtkApplication *application,
                                     GMenuModel     *menumodel)
{
    //Sanity checks
    g_return_if_fail (application != NULL);
    g_return_if_fail (menumodel != NULL);

    //Declarations
    gint total_items = 0;
    gint index = 0;
    gchar *accel = NULL;
    gchar *action = NULL;
    gchar *detail = NULL;
    GMenuModel *sublink = NULL;

    //Initializations
    total_items = g_menu_model_get_n_items (menumodel);

    for (index = 0; index < total_items; index++)
    {
      g_menu_model_get_item_attribute (menumodel, index, "accel", "s", &accel, NULL);
      g_menu_model_get_item_attribute (menumodel, index, G_MENU_ATTRIBUTE_ACTION, "s", &action, NULL);
      g_menu_model_get_item_attribute (menumodel, index, G_MENU_ATTRIBUTE_TARGET, "s", &detail, NULL);
      if (accel != NULL && action != NULL)
      {
        if (detail != NULL)
        {
          gtk_application_remove_accelerator (GTK_APPLICATION (application), action, g_variant_new_string (detail));
        }
        else
        {
          gtk_application_remove_accelerator (GTK_APPLICATION (application), action, NULL);
        }
      }

      if (accel != NULL) g_free (accel); accel = NULL;
      if (action != NULL) g_free (action); action = NULL;
      if (detail != NULL) g_free (detail); detail = NULL;

      //Recursive work
      sublink = g_menu_model_get_item_link (menumodel, index, G_MENU_LINK_SUBMENU);
      if (sublink != NULL) lgw_application_remove_accelerators (application, sublink);
      sublink = g_menu_model_get_item_link (menumodel, index, G_MENU_LINK_SECTION);
      if (sublink != NULL) lgw_application_remove_accelerators (application, sublink);
    }
}
