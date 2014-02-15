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
//! @file mainwindow.c
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


//Static declarations
static void gw_mainwindow_initialize_header (GwMainWindow *self);
static void gw_mainwindow_initialize_body (GwMainWindow *self);

static gchar* gw_mainwindow_get_symbolic_icon_name_if_exists (const gchar* ICON_NAME);

static void gw_mainwindow_init_actionable_interface (LgwActionableInterface *iface);

G_DEFINE_TYPE_WITH_CODE (GwMainWindow, gw_mainwindow, LGW_TYPE_WINDOW,
                         G_IMPLEMENT_INTERFACE (LGW_TYPE_ACTIONABLE, gw_mainwindow_implement_actionable_interface));

//!
//! @brief Sets up the variables in main-interface.c and main-callbacks.c for use
//!
GtkWindow* 
gw_mainwindow_new (GtkApplication *application)
{
    //Sanity checks
    g_return_val_if_fail (application != NULL, NULL);

    //Declarations
    GwMainWindow *self = NULL;

    //Initializations
    self = GW_MAINWINDOW (g_object_new (GW_TYPE_MAINWINDOW,
                                          "type",        GTK_WINDOW_TOPLEVEL,
                                          "application", application,
                                          "shows-menubar", TRUE,
                                           NULL));

    return GTK_WINDOW (self);
}


static void 
gw_mainwindow_init (GwMainWindow *self)
{
    self->priv = GW_MAINWINDOW_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(GwMainWindowPrivate));

    GwMainWindowPrivate *priv;
    priv = self->priv;
}


static void
gw_mainwindow_set_property (GObject      *object,
                            guint         property_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
    //Declarations
    GwMainWindow *self = NULL;
    LgwActionable *actionable = NULL;
    GwMainWindowPrivate *priv = NULL;

    //Initializations
    self = GW_MAINWINDOW (object);
    actionable = LGW_ACTIONABLE (object);
    priv = self->priv;

    switch (property_id)
    {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
gw_mainwindow_get_property (GObject      *object,
                            guint         property_id,
                            GValue       *value,
                            GParamSpec   *pspec)
{
    //Declarations
    GwMainWindow *self = NULL;
    LgwActionable *actionable = NULL;
    GwMainWindowPrivate *priv = NULL;

    //Initializations
    self = GW_MAINWINDOW (object);
    actionable = LGW_ACTIONABLE (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_ACTIONS:
        g_value_set_pointer (value, lgw_actionable_get_actions (actionable));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
gw_mainwindow_finalize (GObject *object)
{
    //Declarations
    GwMainWindow *self = NULL;
    GwMainWindowPrivate *priv = NULL;

    //Initializations
    self = GW_MAINWINDOW (object);
    priv = self->priv;

    G_OBJECT_CLASS (gw_mainwindow_parent_class)->finalize (object);
}


static void 
gw_mainwindow_constructed (GObject *object)
{
    //Sanity checks
    g_return_if_fail (object != NULL);

    //Declarations
    GwMainWindow *self = NULL;
    GwMainWindowPrivate *priv = NULL;

    //Chain the parent class
    {
      G_OBJECT_CLASS (gw_mainwindow_parent_class)->constructed (object);
    }

    //Initializations
    self = GW_MAINWINDOW (object);
    priv = self->priv;

    gw_mainwindow_initialize_header (self);
    gw_mainwindow_initialize_body (self);

    //Set up the gtk self
    gtk_window_set_position (GTK_WINDOW (self), GTK_WIN_POS_MOUSE);
    gtk_window_set_default_size (GTK_WINDOW (self), 620, 500);
    gtk_window_set_icon_name (GTK_WINDOW (self), "gwaei");

    gw_mainwindow_connect_signals (self);
}


static void
gw_mainwindow_dispose (GObject *object)
{
    //Declarations
    GwMainWindow *self = NULL;
    GwMainWindowPrivate *priv = NULL;

    //Initializations
    self = GW_MAINWINDOW (object);
    priv = self->priv;

    gw_mainwindow_disconnect_signals (self);

    if (priv->data.action_group != NULL)
    {
      lgw_actiongroup_free (priv->data.action_group);
      priv->data.action_group = NULL;
    }

    if (priv->data.action_group_list != NULL)
    {
      g_list_free (priv->data.action_group_list);
      priv->data.action_group_list = NULL;
    }

    G_OBJECT_CLASS (gw_mainwindow_parent_class)->dispose (object);
}



static void
gw_mainwindow_class_init (GwMainWindowClass *klass)
{
    GObjectClass *object_class;

    object_class = G_OBJECT_CLASS (klass);

    object_class->constructed = gw_mainwindow_constructed;
    object_class->set_property = gw_mainwindow_set_property;
    object_class->get_property = gw_mainwindow_get_property;
    object_class->dispose = gw_mainwindow_dispose;
    object_class->finalize = gw_mainwindow_finalize;

    g_type_class_add_private (object_class, sizeof (GwMainWindowPrivate));

    g_object_class_override_property (object_class, PROP_ACTIONS, "actions");
}


static void
gw_mainwindow_initialize_header (GwMainWindow *self)
{
    //Sanity checks
    g_return_if_fail (GW_IS_MAINWINDOW (self));
    GwMainWindowPrivate *priv = self->priv;

    //Setup header bar
    {
      GtkWidget *bar = gtk_header_bar_new ();
      priv->ui.header_bar = GTK_HEADER_BAR (bar);
      gtk_header_bar_set_show_close_button (priv->ui.header_bar, TRUE);

      //Setup seach entry
      {
        GtkWidget *search_entry = lgw_searchentry_new ();
        priv->ui.search_entry = LGW_SEARCHENTRY (search_entry);
        //priv->ui.stack_switcher = GTK_STACK_SWITCHER (switcher);
        gtk_header_bar_set_custom_title (priv->ui.header_bar, search_entry);
        gtk_widget_show_all (search_entry);
      }

      //Search toggle button
      {
        GtkWidget *stack_switcher = gtk_stack_switcher_new ();
        priv->ui.stack_switcher = GTK_STACK_SWITCHER (stack_switcher);
        gtk_header_bar_pack_end (priv->ui.header_bar, stack_switcher);
      }

      //Setup menu button
      {
        GtkWidget *menu_button = lgw_window_get_menu_button (LGW_WINDOW (self));
        gtk_header_bar_pack_end (priv->ui.header_bar, menu_button);
        {
          gchar* icon_name = lgw_get_symbolic_icon_name_if_exists ("emblem-system");
          GtkWidget *image = gtk_image_new_from_icon_name (icon_name, GTK_ICON_SIZE_MENU);
          gtk_button_set_image (GTK_BUTTON (menu_button), image);
          if (icon_name != NULL) g_free (icon_name); icon_name = NULL;
        }
      }

      //Finalize and show
      gtk_window_set_titlebar (GTK_WINDOW (self), bar);
      gtk_widget_show_all (bar);
    }
}


static void
gw_mainwindow_initialize_body (GwMainWindow *self)
{
    //Sanity checks
    g_return_if_fail (GW_IS_MAINWINDOW (self));

    //Declarations
    GwMainWindowPrivate *priv = self->priv;

    {
      GtkWidget *stack = gtk_stack_new ();
      priv->ui.stack = GTK_STACK (stack);
      gtk_stack_set_homogeneous (priv->ui.stack, TRUE);
      lgw_window_pack_start (LGW_WINDOW (self), stack, TRUE, TRUE, 0); 
      gtk_stack_set_transition_type (priv->ui.stack, GTK_STACK_TRANSITION_TYPE_CROSSFADE);

      {
        GtkWidget* search_widget = lgw_searchwidget_new ();
        priv->ui.search_widget = LGW_SEARCHWIDGET (search_widget);

        {
          gchar* icon_name = lgw_get_symbolic_icon_name_if_exists ("edit-find");
          gtk_container_add_with_properties (
            GTK_CONTAINER (stack), search_widget, 
            "name", "dictionary", 
            "title", gettext("Lookup"), 
            "icon-name", icon_name,
            NULL
          );
          if (icon_name != NULL) g_free (icon_name); icon_name = NULL;
        }

        gtk_widget_show (search_widget);
      }

      {
        GtkWidget* vocabulary_widget = lgw_vocabularywidget_new ();
        priv->ui.vocabulary_widget = LGW_VOCABULARYWIDGET (vocabulary_widget);

        {
          gchar* icon_name = lgw_get_symbolic_icon_name_if_exists ("view-list");
          gtk_container_add_with_properties (
            GTK_CONTAINER (stack), vocabulary_widget, 
            "name", "vocabulary", 
            "title", gettext("Study"), 
            "icon-name", icon_name,
            NULL
          );
          if (icon_name != NULL) g_free (icon_name); icon_name = NULL;
        }

        gtk_widget_show (vocabulary_widget);
      }

      gtk_widget_show (stack);

    }

    gtk_stack_switcher_set_stack (priv->ui.stack_switcher, priv->ui.stack);
}




