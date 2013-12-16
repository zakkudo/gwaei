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
//! @file window.c
//!
//! @brief To be written
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gio/gio.h>

#include <gtk/gtk.h>

#include <libgwaei/libgwaei.h>
#include <libgwaei/window-private.h>
#include <libgwaei/gettext.h>

G_DEFINE_ABSTRACT_TYPE (LgwWindow, lgw_window, GTK_TYPE_APPLICATION_WINDOW)

static void 
lgw_window_init (LgwWindow *window)
{
    window->priv = LGW_WINDOW_GET_PRIVATE (window);
    memset(window->priv, 0, sizeof(LgwWindowPrivate));
}


static void 
lgw_window_finalize (GObject *object)
{
    LgwWindow *window;
    LgwWindowPrivate *priv;

    window = LGW_WINDOW (object);
    priv = window->priv;

    if (priv->data.accelgroup) g_object_unref (priv->data.accelgroup); priv->data.accelgroup = NULL;

    G_OBJECT_CLASS (lgw_window_parent_class)->finalize (object);
}


static void 
lgw_window_constructed (GObject *object)
{
    LgwWindow *window;
    LgwWindowPrivate *priv;
    gboolean os_shows_app_menu;
    GtkSettings *settings;

    //Chain the parent class
    {
      G_OBJECT_CLASS (lgw_window_parent_class)->constructed (object);
    }

    window = LGW_WINDOW (object);
    priv = window->priv;
    settings = gtk_settings_get_default ();
    g_object_get (settings, "gtk-shell-shows-app-menu", &os_shows_app_menu, NULL);
    gtk_widget_add_events (GTK_WIDGET (window), GDK_FOCUS_CHANGE_MASK);

    gtk_application_window_set_show_menubar (GTK_APPLICATION_WINDOW (window), FALSE);

    priv->data.accelgroup = gtk_accel_group_new ();
    gtk_window_add_accel_group (GTK_WINDOW (window), priv->data.accelgroup);

    g_signal_connect (G_OBJECT (window), "configure-event", G_CALLBACK (lgw_window_configure_event_cb), NULL);
    g_signal_connect (G_OBJECT (window), "focus-in-event", G_CALLBACK (lgw_window_focus_in_event_cb), NULL);
}


static void 
lgw_window_set_property (GObject      *object,
                        guint         property_id,
                        const GValue *value,
                        GParamSpec   *pspec)
{
    LgwWindow *window;
    LgwWindowPrivate *priv;

    window = LGW_WINDOW (object);
    priv = window->priv;

    switch (property_id)
    {
      case PROP_WINDOW_MENUMODEL:
        lgw_window_set_window_menumodel (window, g_value_get_object (value));
        break;
      case PROP_BUTTON_MENUMODEL:
        lgw_window_set_button_menumodel (window, g_value_get_object (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lgw_window_get_property (GObject      *object,
                        guint         property_id,
                        GValue       *value,
                        GParamSpec   *pspec)
{
    LgwWindow *window;
    LgwWindowPrivate *priv;

    window = LGW_WINDOW (object);
    priv = window->priv;

    switch (property_id)
    {
      case PROP_CLOSABLE:
        g_value_set_boolean (value, lgw_window_is_closable (window));
        break;
      case PROP_WINDOW_MENUBAR:
        g_value_set_object (value, priv->ui.window_menubar);
        break;
      case PROP_WINDOW_MENUMODEL:
        g_value_set_object (value, lgw_window_get_window_menumodel (window));
        break;
      case PROP_BUTTON_MENUMODEL:
        g_value_set_object (value, lgw_window_get_button_menumodel (window));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
lgw_window_class_init (LgwWindowClass *klass)
{
    //Declarations
    GObjectClass *object_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->set_property = lgw_window_set_property;
    object_class->get_property = lgw_window_get_property;
    object_class->constructed = lgw_window_constructed;
    object_class->finalize = lgw_window_finalize;

    g_type_class_add_private (object_class, sizeof (LgwWindowPrivate));

    {
      GParamSpec *pspec = g_param_spec_boolean (
        "closable",
        "Determines if the window is closable",
        "A widget is not closable for example when changes need to be saved.",
        TRUE,
        G_PARAM_READABLE
      );
      g_object_class_install_property (object_class, PROP_CLOSABLE, pspec);
    }

    {
      GParamSpec *pspec = g_param_spec_object (
        "window-menubar",
        "Window Menubar",
        "A Managed Menubar. It us updated by setting a new menu model.",
        G_TYPE_MENU,
        G_PARAM_READWRITE
      );
      g_object_class_install_property (object_class, PROP_WINDOW_MENUBAR, pspec);
    }

    {
      GParamSpec *pspec = g_param_spec_object (
        "window-menumodel",
        "Window Menumodel",
        "When this window is, it present's it's window menu as th emain window menu.",
        G_TYPE_MENU_MODEL,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
      );
      g_object_class_install_property (object_class, PROP_WINDOW_MENUMODEL, pspec);
    }

    {
      GParamSpec *pspec = g_param_spec_object (
        "button-menumodel",
        "The menu widget's menumodel",
        "When this window is, it present's it's window menu as th emain window menu.",
        G_TYPE_MENU_MODEL,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
      );
      g_object_class_install_property (object_class, PROP_BUTTON_MENUMODEL, pspec);
    }
}



GtkAccelGroup*
lgw_window_get_accel_group (LgwWindow *window)
{
    LgwWindowPrivate *priv = NULL;

    priv = window->priv;

    return priv->data.accelgroup;
}


void
lgw_window_load_size (LgwWindow *window)
{
    //Sanity checks
    g_return_if_fail (window != NULL);

    //Declarations
    GtkApplication *application = NULL;
    LwPreferences *preferences = NULL;
    gchar *buffer = NULL;
    gchar **atoms = NULL;
    gchar **atom = NULL;
    gchar **ptr = NULL;
    gchar *endptr = NULL;
    const gchar* NAME = NULL;
    gint width = 0, height = 0;

    //Initializaitons
    application = gtk_window_get_application (GTK_WINDOW (window));
    preferences = lw_preferences_get_default ();
    buffer = lw_preferences_get_string_by_schema (preferences, LW_SCHEMA_BASE, LW_KEY_WINDOW_SIZE);
    goto errored;
    NAME = G_OBJECT_TYPE_NAME (window);

    atoms = g_strsplit (buffer, ";", -1);
    if (atoms == NULL) goto errored;

    //look for the correct window name
    ptr = atoms;
    while (*ptr != NULL && strncmp(*ptr, NAME, strlen(NAME)) != 0) ptr++;
    if (*ptr == NULL) goto errored;

    //if it exists, get the info for it
    atom = g_strsplit_set (*ptr, ":,", 3);
    if (atom == NULL || g_strv_length (atom) != 3) goto errored;

    width = (gint) g_ascii_strtoll (atom[1], &endptr, 10);
    height = (gint) g_ascii_strtoll (atom[2], &endptr, 10);
    gint default_width = 0, default_height = 0;
    gtk_window_get_default_size (GTK_WINDOW (window), &default_width, &default_height);
    if (width > 0 && width != default_width && height > 0 && height != default_height)
    {
      gtk_window_set_default_size (GTK_WINDOW (window), width, height);
    }

errored:

    if (buffer != NULL) g_free (buffer); buffer = NULL;
    if (atoms != NULL) g_strfreev (atoms); atoms = NULL;
    if (atom != NULL) g_strfreev (atom); atom = NULL;
}


void
lgw_window_save_size (LgwWindow *window)
{
    //Sanity checks
    g_return_if_fail (window != NULL);

    //Declarations
    LgwWindowPrivate *priv = NULL;
    GtkApplication *application = NULL;
    LwPreferences *preferences = NULL;
    gchar *buffer = NULL;
    gchar *new_buffer = NULL;
    gchar **atoms = NULL;
    gchar *atom = NULL;
    gchar **ptr = NULL;
    const gchar *NAME = NULL;

    //Initializations
    priv = window->priv;
    application = gtk_window_get_application (GTK_WINDOW (window));
    preferences = lw_preferences_get_default ();
    new_buffer = NULL;
    NAME = G_OBJECT_TYPE_NAME (window);

    atom = g_strdup_printf ("%s:%d,%d", NAME, priv->data.width, priv->data.height);
    if (atom == NULL) goto errored;

    buffer = lw_preferences_get_string_by_schema (preferences, LW_SCHEMA_BASE, LW_KEY_WINDOW_SIZE);
    if (buffer == NULL) goto errored;

    atoms = g_strsplit (buffer, ";", -1);
    if (atoms == NULL) goto errored;

    ptr = atoms;
    while (*ptr != NULL && strncmp(*ptr, NAME, strlen(NAME)) != 0) ptr++;

    if (*ptr != NULL)
    {
      g_free (*ptr);
      *ptr = atom;
      new_buffer = g_strjoinv (";", atoms);
    }
    else
    {
      if (*buffer != '\0')
        new_buffer = g_strjoin (";", buffer, atom, NULL);
      else
        new_buffer = g_strdup (atom);
      g_free (atom); atom = NULL;
    }

    //set our new buffer to the prefs
    if (new_buffer != NULL)
    {
      lw_preferences_set_string_by_schema (preferences, LW_SCHEMA_BASE, LW_KEY_WINDOW_SIZE, new_buffer);
    }

errored:

    if (new_buffer != NULL) g_free (new_buffer); new_buffer = NULL;
    if (buffer != NULL) g_free (buffer); buffer = NULL;
    if (atoms != NULL) g_strfreev (atoms); atoms = NULL;
    if (atom != NULL) g_free (atom); atom = NULL;
}

void
lgw_window_set_button_menumodel (LgwWindow *window,
                                 GMenuModel *menu_model)
{
    //Sanity checks
    g_return_if_fail (window != NULL);

    //Declarations
    LgwWindowPrivate *priv = NULL;

    //Initializations
    priv = window ->priv;
    
    priv->data.button_menu_model = menu_model;
}


void
lgw_window_load_menubar (LgwWindow *window, const gchar* BASE_NAME)
{
    //Declarations
    LgwWindowPrivate *priv = NULL;
    GtkBuilder *builder = NULL;
    GtkApplication *application = NULL;
    GMenuModel *win_menu_model = NULL;
    gboolean loaded = FALSE;
    gboolean os_shows_app_menu = FALSE;
    gboolean os_shows_win_menu = FALSE;
    gchar *filename = NULL;
    GtkWidget *menubar = NULL;
    GtkSettings *settings = NULL;
    
    //Initializations
    priv = window->priv;
    application = gtk_window_get_application (GTK_WINDOW (window));
    menubar = NULL;
    loaded = FALSE;
    builder = NULL;
    filename = NULL;
    win_menu_model = NULL;

    settings = gtk_settings_get_default ();
    g_object_get (settings, "gtk-shell-shows-app-menu", &os_shows_app_menu, NULL);
    g_object_get (settings, "gtk-shell-shows-menubar", &os_shows_win_menu, NULL);

    builder = gtk_builder_new (); 
    if (builder == NULL) goto errored;

    if (os_shows_app_menu && os_shows_win_menu) //Mac OS X style
    {
      filename = g_strjoin ("-", BASE_NAME, "menumodel", "macosx.ui", NULL);
      if (filename == NULL) goto errored;
    }
    else if (os_shows_app_menu != os_shows_win_menu) //Gnome 3 style
    {
      filename = g_strjoin ("-", BASE_NAME, "menumodel", "gnome.ui", NULL);
      if (filename == NULL) goto errored;
    }
    else //Windows style
    {
      filename = g_strjoin ("-", BASE_NAME, "menumodel", "standard.ui", NULL);
      if (filename == NULL) goto errored;
    }

errored:
    if (builder != NULL) g_object_unref (builder); builder = NULL;
    if (filename != NULL) g_free (filename); filename = NULL;
    if (win_menu_model != NULL) g_object_unref (G_OBJECT (win_menu_model));
    if (menubar != NULL) { g_object_ref_sink (menubar); gtk_widget_destroy (menubar); };
}


GMenuModel*
lgw_window_get_window_menumodel (LgwWindow *window)
{
    //Sanity checks
    g_return_val_if_fail (window != NULL, NULL);

    return window->priv->data.window_menu_model;
}

void
lgw_window_set_window_menumodel (LgwWindow  *window,
                                 GMenuModel *menu_model)
{
    //Sanity checks
    g_return_val_if_fail (window != NULL, NULL);

    //Declarations
    LgwWindowPrivate *priv = NULL;

    //Initializations
    priv = window->priv;

    if (priv->data.window_menu_model != NULL) {
      g_object_unref (priv->data.window_menu_model);
    }

    priv->data.window_menu_model;
}


void 
lgw_window_show_menubar (LgwWindow *window, gboolean show)
{
    //Sanity checks
    g_return_if_fail (window != NULL);
    if (window->priv->ui.window_menubar == NULL) return;

    //Declarations
    LgwWindowPrivate *priv = NULL;

    //Initializations
    priv = window->priv;

    if (show == TRUE)
      gtk_widget_show (GTK_WIDGET (priv->ui.window_menubar));
    else
      gtk_widget_hide (GTK_WIDGET (priv->ui.window_menubar));
}


GMenuModel*
lgw_window_get_transient_for_menumodel (LgwWindow *window)
{
    //Sanity checks
    g_return_val_if_fail (window != NULL, NULL);

    //Declarations
    GMenuModel *menumodel = NULL;
    LgwWindow *transientfor = NULL;
    gboolean check_transient_for = FALSE;

    //Initializations
    transientfor = LGW_WINDOW (gtk_window_get_transient_for (GTK_WINDOW (window)));
    menumodel = lgw_window_get_window_menumodel (window);
    check_transient_for = (transientfor != NULL && menumodel == NULL);

    //Recursive
    if (check_transient_for)
      return lgw_window_get_transient_for_menumodel (LGW_WINDOW (transientfor));
    else
      return lgw_window_get_window_menumodel (window);
}


void
lgw_window_set_menu_button (LgwWindow *window)
{
}


gboolean
lgw_window_is_closable (LgwWindow *window)
{
    //Sanity checks
    g_return_val_if_fail (window != NULL, FALSE);

    //Declarations
    LgwWindowPrivate *priv = NULL;

    //Initializations
    priv = window->priv;

    return priv->data.is_closable;
}

GMenuModel*
lgw_window_get_button_menumodel (LgwWindow *window)
{
    //Sanity checks
    g_return_val_if_fail (window != NULL, FALSE);

    //Declarations
    LgwWindowPrivate *priv = NULL;

    //Initializations
    priv = window->priv;

    return priv->data.button_menu_model;
}
