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
lgw_window_init (LgwWindow *self)
{
    self->priv = LGW_WINDOW_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LgwWindowPrivate));

    {
      LgwWindowPrivate *priv = self->priv;

      priv->data.window_menu_model = G_MENU_MODEL (g_menu_new ());
      priv->data.button_menu_model = G_MENU_MODEL (g_menu_new ());
      priv->data.accelgroup = gtk_accel_group_new ();
    }
}


static void 
lgw_window_finalize (GObject *object)
{
    //Declarations
    LgwWindow *self;
    LgwWindowPrivate *priv;

    //Initializations
    self = LGW_WINDOW (object);
    priv = self->priv;

    if (priv->data.accelgroup) g_object_unref (priv->data.accelgroup); priv->data.accelgroup = NULL;

    G_OBJECT_CLASS (lgw_window_parent_class)->finalize (object);
}


static void 
lgw_window_constructed (GObject *object)
{
    //Declarations
    LgwWindow *self = NULL;
    LgwWindowPrivate *priv = NULL;

    //Chain the parent class
    {
      G_OBJECT_CLASS (lgw_window_parent_class)->constructed (object);
    }

    //Initializations
    self = LGW_WINDOW (object);
    priv = self->priv;

    gtk_widget_add_events (GTK_WIDGET (self), GDK_FOCUS_CHANGE_MASK);
    gtk_application_window_set_show_menubar (GTK_APPLICATION_WINDOW (self), FALSE);
    gtk_window_add_accel_group (GTK_WINDOW (self), priv->data.accelgroup);

    {
      GtkWidget *box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
      priv->ui.box = GTK_BOX (box);
      gtk_container_add (GTK_CONTAINER (self), box);
      gtk_widget_show (box);

      {
        GtkWidget *menubar = gtk_menu_bar_new_from_model (priv->data.window_menu_model);
        priv->ui.window_menubar = GTK_MENU_BAR (menubar);
        gtk_box_pack_start (priv->ui.box, menubar, FALSE, FALSE, 0);
        gtk_widget_show (GTK_WIDGET (priv->ui.window_menubar));
      }
    }

    priv->ui.menu_button = GTK_MENU_BUTTON (gtk_menu_button_new ());
    gtk_menu_button_set_menu_model (priv->ui.menu_button, priv->data.button_menu_model);

    lgw_window_connect_signals (self);
}


static void 
lgw_window_set_property (GObject      *object,
                         guint         property_id,
                         const GValue *value,
                         GParamSpec   *pspec)
{
    LgwWindow *self;
    LgwWindowPrivate *priv;

    self = LGW_WINDOW (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_WINDOW_MENUMODEL:
        lgw_window_set_window_menumodel (self, g_value_get_object (value));
        break;
      case PROP_BUTTON_MENUMODEL:
        lgw_window_set_button_menumodel (self, g_value_get_object (value));
        break;
      case PROP_SHOWS_MENUBAR:
        lgw_window_set_shows_menubar (self, g_value_get_boolean (value));
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
    LgwWindow *self;
    LgwWindowPrivate *priv;

    self = LGW_WINDOW (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_CLOSABLE:
        g_value_set_boolean (value, lgw_window_is_closable (self));
        break;
      case PROP_WINDOW_MENUBAR:
        g_value_set_object (value, priv->ui.window_menubar);
        break;
      case PROP_WINDOW_MENUMODEL:
        g_value_set_object (value, lgw_window_get_window_menumodel (self));
        break;
      case PROP_BUTTON_MENUMODEL:
        g_value_set_object (value, lgw_window_get_button_menumodel (self));
        break;
      case PROP_SHOWS_MENUBAR:
        g_value_set_boolean (value, lgw_window_get_shows_menubar (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
lgw_window_dispose (GObject *object)
{
    //Declarations
    LgwWindow *self = NULL;

    //Initializations
    self = LGW_WINDOW (object);

    lgw_window_disconnect_signals (self);

    G_OBJECT_CLASS (lgw_window_parent_class)->dispose (object);
}


static void
lgw_window_class_init (LgwWindowClass *klass)
{
    //Declarations
    LgwWindowClassPrivate *klasspriv = NULL;
    GObjectClass *object_class = NULL;

    //Initializations
    klass->priv = g_new0 (LgwWindowClassPrivate, 1);
    klasspriv = klass->priv;
    object_class = G_OBJECT_CLASS (klass);
    object_class->set_property = lgw_window_set_property;
    object_class->get_property = lgw_window_get_property;
    object_class->constructed = lgw_window_constructed;
    object_class->dispose = lgw_window_dispose;
    object_class->finalize = lgw_window_finalize;

    g_type_class_add_private (object_class, sizeof (LgwWindowPrivate));

    {
      klasspriv->pspec[PROP_CLOSABLE] = g_param_spec_boolean (
        "closable",
        "Determines if the window is closable",
        "A widget is not closable for example when changes need to be saved.",
        TRUE,
        G_PARAM_READABLE
      );
      g_object_class_install_property (object_class, PROP_CLOSABLE, klasspriv->pspec[PROP_CLOSABLE]);
    }

    {
      klasspriv->pspec[PROP_SHOWS_MENUBAR] = g_param_spec_boolean (
        "shows-menubar",
        "Determines if the window can show a menubar",
        "Set this to true if you want the window to show a menubar when you set show menubar.",
        FALSE,
        G_PARAM_READWRITE
      );
      g_object_class_install_property (object_class, PROP_SHOWS_MENUBAR, klasspriv->pspec[PROP_SHOWS_MENUBAR]);
    }

    {
      klasspriv->pspec[PROP_WINDOW_MENUBAR] = g_param_spec_object (
        "window-menubar",
        "Window Menubar",
        "A Managed Menubar. It us updated by setting a new menu model.",
        G_TYPE_MENU,
        G_PARAM_READWRITE
      );
      g_object_class_install_property (object_class, PROP_WINDOW_MENUBAR, klasspriv->pspec[PROP_WINDOW_MENUBAR]);
    }

    {
      klasspriv->pspec[PROP_WINDOW_MENUMODEL] = g_param_spec_object (
        "window-menumodel",
        "Window Menumodel",
        "When this window is, it present's it's window menu as th emain window menu.",
        G_TYPE_MENU_MODEL,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
      );
      g_object_class_install_property (object_class, PROP_WINDOW_MENUMODEL, klasspriv->pspec[PROP_WINDOW_MENUMODEL]);
    }

    {
      klasspriv->pspec[PROP_BUTTON_MENUMODEL] = g_param_spec_object (
        "button-menumodel",
        "The menu widget's menumodel",
        "When this window is, it present's it's window menu as th emain window menu.",
        G_TYPE_MENU_MODEL,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
      );
      g_object_class_install_property (object_class, PROP_BUTTON_MENUMODEL, klasspriv->pspec[PROP_BUTTON_MENUMODEL]);
    }
}



GtkAccelGroup*
lgw_window_get_accel_group (LgwWindow *self)
{
    LgwWindowPrivate *priv = NULL;

    priv = self->priv;

    return priv->data.accelgroup;
}


void
lgw_window_load_size (LgwWindow *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_WINDOW (self));

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
    application = gtk_window_get_application (GTK_WINDOW (self));
    preferences = lw_preferences_get_default ();
    buffer = lw_preferences_get_string_by_schema (preferences, LW_SCHEMA_BASE, LW_KEY_WINDOW_SIZE);
    goto errored;
    NAME = G_OBJECT_TYPE_NAME (self);

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
    gtk_window_get_default_size (GTK_WINDOW (self), &default_width, &default_height);
    if (width > 0 && width != default_width && height > 0 && height != default_height)
    {
      gtk_window_set_default_size (GTK_WINDOW (self), width, height);
    }

errored:

    if (buffer != NULL) g_free (buffer); buffer = NULL;
    if (atoms != NULL) g_strfreev (atoms); atoms = NULL;
    if (atom != NULL) g_strfreev (atom); atom = NULL;
}


void
lgw_window_save_size (LgwWindow *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_WINDOW (self));

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
    priv = self->priv;
    application = gtk_window_get_application (GTK_WINDOW (self));
    preferences = lw_preferences_get_default ();
    new_buffer = NULL;
    NAME = G_OBJECT_TYPE_NAME (self);

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


GMenuModel*
lgw_window_get_window_menumodel (LgwWindow *self)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_WINDOW (self), NULL);

    return self->priv->data.window_menu_model;
}


void
lgw_window_set_window_menumodel (LgwWindow  *self,
                                 GMenuModel *menu_model)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_WINDOW (self));

    //Declarations
    GtkApplication *application = NULL;
    LgwWindowPrivate *priv = NULL;
    LgwWindowClass *klass = NULL;
    LgwWindowClassPrivate *klasspriv = NULL;

    //Initializations
    application = gtk_window_get_application (GTK_WINDOW (self));
    priv = self->priv;
    klass = LGW_WINDOW_GET_CLASS (self);
    klasspriv = klass->priv;

    if (application == NULL) goto errored;
    if (priv->data.window_menu_model == NULL) goto errored;
    if (menu_model == priv->data.window_menu_model) goto errored;

    lgw_menumodel_set_contents (priv->data.window_menu_model, menu_model);

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_WINDOW_MENUMODEL]);

errored:


    return;
}


void 
lgw_window_show_menubar (LgwWindow *self,
                         gboolean   show_menubar)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_WINDOW (self));
    if (self->priv->ui.window_menubar == NULL) return;

    //Declarations
    LgwWindowPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    priv->config.show_menubar = show_menubar;

    lgw_window_sync_menubar_show (self);
}


GMenuModel*
lgw_window_get_transient_for_menumodel (LgwWindow *self)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_WINDOW (self), NULL);

    //Declarations
    GMenuModel *menumodel = NULL;
    LgwWindow *transientfor = NULL;
    gboolean check_transient_for = FALSE;

    //Initializations
    transientfor = LGW_WINDOW (gtk_window_get_transient_for (GTK_WINDOW (self)));
    menumodel = lgw_window_get_window_menumodel (self);
    check_transient_for = (transientfor != NULL && menumodel == NULL);

    //Recursive
    if (check_transient_for)
      return lgw_window_get_transient_for_menumodel (LGW_WINDOW (transientfor));
    else
      return lgw_window_get_window_menumodel (self);
}


gboolean
lgw_window_is_closable (LgwWindow *self)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_WINDOW (self), FALSE);

    //Declarations
    LgwWindowPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->config.is_closable;
}


void
lgw_window_set_button_menumodel (LgwWindow *self,
                                 GMenuModel *menu_model)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_WINDOW (self));

    //Declarations
    LgwWindowPrivate *priv = NULL;
    LgwWindowClass *klass = NULL;
    LgwWindowClassPrivate *klasspriv = NULL;

    //Initializations
    priv = self ->priv;
    klass = LGW_WINDOW_GET_CLASS (self);
    klasspriv = klass->priv;

    if (priv->data.button_menu_model == NULL) goto errored;

    lgw_menumodel_set_contents (priv->data.button_menu_model, menu_model);

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_BUTTON_MENUMODEL]);

errored:

    return;
}


GMenuModel*
lgw_window_get_button_menumodel (LgwWindow *self)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_WINDOW (self), FALSE);

    //Declarations
    LgwWindowPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.button_menu_model;
}


GtkWidget*
lgw_window_get_menubar (LgwWindow *self)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_WINDOW (self), NULL);

    //Declarations
    LgwWindowPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return GTK_WIDGET (priv->ui.window_menubar);
}


GtkWidget*
lgw_window_get_menu_button (LgwWindow *self)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_WINDOW (self), NULL);

    //Declarations
    LgwWindowPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return GTK_WIDGET (priv->ui.menu_button);
}


void
lgw_window_set_shows_menubar (LgwWindow *self,
                              gboolean shows_menubar)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_WINDOW (self));

    //Declarations
    LgwWindowPrivate *priv = NULL;
    LgwWindowClass *klass = NULL;
    LgwWindowClassPrivate *klasspriv = NULL;

    //Initializations
    priv = self->priv;
    klass = LGW_WINDOW_GET_CLASS (self);
    klasspriv = klass->priv;

    priv->config.shows_menubar = shows_menubar;

    lgw_window_sync_menubar_show (self);

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_SHOWS_MENUBAR]);
}


gboolean
lgw_window_get_shows_menubar (LgwWindow *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_WINDOW (self));

    //Declarations
    LgwWindowPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->config.shows_menubar;
}


void
lgw_window_sync_menubar_show (LgwWindow *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_WINDOW (self));

    //Declarations
    LgwWindowPrivate *priv = NULL;
    GtkSettings *settings = NULL;
    gboolean shell_shows_menubar = FALSE;
    gboolean should_show_menubar = FALSE;

    //Initializations
    priv = self->priv;

    settings = gtk_settings_get_default ();
    g_object_get (G_OBJECT (settings), "gtk-shell-shows-menubar", &shell_shows_menubar, NULL);
    should_show_menubar = (
      priv->ui.window_menubar != NULL &&
      priv->config.shows_menubar &&
      priv->config.show_menubar &&
      !shell_shows_menubar
    );

    if (priv->ui.window_menubar != NULL)
    {
      if (should_show_menubar)
      {
        gtk_widget_show (GTK_WIDGET (priv->ui.window_menubar));
      }
      else
      {
        gtk_widget_hide (GTK_WIDGET (priv->ui.window_menubar));
      }
    }

    gtk_window_set_hide_titlebar_when_maximized (GTK_WINDOW (self), FALSE);

    if (priv->ui.menu_button != NULL)
    {
      if (should_show_menubar)
      {
        gtk_widget_hide (GTK_WIDGET (priv->ui.menu_button));
      }
      else
      {
        gtk_widget_show (GTK_WIDGET (priv->ui.menu_button));
      }
    }
}


void
lgw_window_pack_start (LgwWindow *self,
                       GtkWidget *widget,
                       gboolean   expand,
                       gboolean   fill,
                       guint      padding)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_WINDOW (self));

    //Declarations
    LgwWindowPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    gtk_box_pack_start (priv->ui.box, widget, expand, fill, padding);
}


void
lgw_window_clear_actions (LgwWindow *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_WINDOW (self));

    //Declarations
    gchar **actions = NULL;

    //Initializations
    actions = g_action_group_list_actions (G_ACTION_GROUP (self));
    if (actions == NULL) goto errored;

    {
      gint i = 0;
      for (i = 0; actions[i] != NULL; i++)
      {
          g_action_map_remove_action (G_ACTION_MAP (self), actions[i]);
      }
    }

errored:

    if (actions != NULL) g_strfreev (actions); actions = NULL;
   
    return; 
}


void
lgw_window_add_actions (LgwWindow *self,
                        GList     *action_group_list)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_WINDOW (self));

    //Declarations
    LgwWindowPrivate *priv = NULL;
    GList *link = NULL;
    GActionMap *action_map = NULL;
    GtkApplication *application = NULL;

    //Initializations
    priv = self->priv;
    action_map = G_ACTION_MAP (self);
    application = gtk_window_get_application (GTK_WINDOW (self));

    for (link = action_group_list; link != NULL; link = link->next)
    {
      LgwActionGroup *action_group = LGW_ACTIONGROUP (link->data);
      if (action_group != NULL)
      {
        lgw_actiongroup_add_to_map (action_group, action_map);
      }
    }
}


void
lgw_window_set_actions (LgwWindow *self,
                        GList     *action_group_list)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_WINDOW (self));

    lgw_window_clear_actions (self);
    lgw_window_add_actions (self, action_group_list);
}

