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
static void gw_mainwindow_initialize_header (GwMainWindow *window);
static void gw_mainwindow_initialize_vocabularywidget (GwMainWindow *window);
static void gw_mainwindow_initialize_body (GwMainWindow *window);

static gchar* gw_mainwindow_get_symbolic_icon_name_if_exists (const gchar* ICON_NAME);

static void gw_mainwindow_init_actionable_interface (LgwActionableInterface *iface);

G_DEFINE_TYPE_WITH_CODE (GwMainWindow, gw_mainwindow, LGW_TYPE_WINDOW,
                         G_IMPLEMENT_INTERFACE (LGW_TYPE_ACTIONABLE, gw_mainwindow_init_actionable_interface));

//!
//! @brief Sets up the variables in main-interface.c and main-callbacks.c for use
//!
GtkWindow* 
gw_mainwindow_new (GtkApplication *application)
{
    //Sanity checks
    g_return_val_if_fail (application != NULL, NULL);

    //Declarations
    GwMainWindow *window = NULL;

    //Initializations
    window = GW_MAINWINDOW (g_object_new (GW_TYPE_MAINWINDOW,
                                          "type",        GTK_WINDOW_TOPLEVEL,
                                          "application", application,
                                          "shows-menubar", TRUE,
                                           NULL));

    return GTK_WINDOW (window);
}


static void 
gw_mainwindow_init (GwMainWindow *window)
{
    window->priv = GW_MAINWINDOW_GET_PRIVATE (window);
    memset(window->priv, 0, sizeof(GwMainWindowPrivate));

    GwMainWindowPrivate *priv;
    priv = window->priv;
}


static void
gw_mainwindow_init_actionable_interface (LgwActionableInterface *iface)
{
    iface->get_actions = gw_mainwindow_get_actions;
    iface->set_actiongroup = gw_mainwindow_set_actiongroup;
}


static void
gw_mainwindow_set_property (GObject      *object,
                            guint         property_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
    //Declarations
    GwMainWindow *main_window = NULL;
    LgwActionable *actionable = NULL;
    GwMainWindowPrivate *priv = NULL;

    //Initializations
    main_window = GW_MAINWINDOW (object);
    actionable = LGW_ACTIONABLE (object);
    priv = main_window->priv;

    switch (property_id)
    {
      case PROP_ACTIONS:
        lgw_actionable_set_actiongroup (actionable, g_value_get_pointer (value));
        break;
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
    GwMainWindow *main_window = NULL;
    LgwActionable *actionable = NULL;
    GwMainWindowPrivate *priv = NULL;

    //Initializations
    main_window = GW_MAINWINDOW (object);
    actionable = LGW_ACTIONABLE (object);
    priv = main_window->priv;

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
    GwMainWindow *window = NULL;
    GwMainWindowPrivate *priv = NULL;

    //Initializations
    window = GW_MAINWINDOW (object);
    priv = window->priv;

    G_OBJECT_CLASS (gw_mainwindow_parent_class)->finalize (object);
}


static void 
gw_mainwindow_constructed (GObject *object)
{
    //Sanity checks
    g_return_if_fail (object != NULL);

    //Declarations
    GwMainWindow *window = NULL;
    GwMainWindowPrivate *priv = NULL;

    //Chain the parent class
    {
      G_OBJECT_CLASS (gw_mainwindow_parent_class)->constructed (object);
    }

    //Initializations
    window = GW_MAINWINDOW (object);
    priv = window->priv;

    gw_mainwindow_initialize_header (window);
    gw_mainwindow_initialize_vocabularywidget (window);
    gw_mainwindow_initialize_body (window);

    //Set up the gtk window
    gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_MOUSE);
    gtk_window_set_default_size (GTK_WINDOW (window), 620, 500);
    gtk_window_set_icon_name (GTK_WINDOW (window), "gwaei");

    gw_mainwindow_connect_signals (window);
}


static void
gw_mainwindow_dispose (GObject *object)
{
    //Declarations
    GwMainWindow *main_window = NULL;
    GwMainWindowPrivate *priv = NULL;

    //Initializations
    main_window = GW_MAINWINDOW (object);
    priv = main_window->priv;

    gw_mainwindow_disconnect_signals (main_window);

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
gw_mainwindow_initialize_header (GwMainWindow *window)
{
    //Sanity checks
    g_return_if_fail (window != NULL);
    GwMainWindowPrivate *priv = window->priv;

    //Setup header bar
    {
      GtkWidget *bar = gtk_header_bar_new ();
      priv->ui.header_bar = GTK_HEADER_BAR (bar);
      gtk_header_bar_set_show_close_button (priv->ui.header_bar, TRUE);

      //Setup stack switcher
      {
        GtkWidget *switcher = gtk_stack_switcher_new ();
        priv->ui.stack_switcher = GTK_STACK_SWITCHER (switcher);
        gtk_header_bar_set_custom_title (priv->ui.header_bar, switcher);
      }

      //Setup menu button
      {
        GtkWidget *menu_button = lgw_window_get_menu_button (LGW_WINDOW (window));
        gtk_header_bar_pack_end (priv->ui.header_bar, menu_button);
        {
          gchar* icon_name = lgw_get_symbolic_icon_name_if_exists ("emblem-system");
          GtkWidget *image = gtk_image_new_from_icon_name ("emblem-system-symbolic", GTK_ICON_SIZE_MENU);
          gtk_button_set_image (GTK_BUTTON (menu_button), image);
          if (icon_name != NULL) g_free (icon_name); icon_name = NULL;
        }
      }

      //Finalize and show
      gtk_window_set_titlebar (GTK_WINDOW (window), bar);
      gtk_widget_show_all (bar);
    }
}


static void
gw_mainwindow_initialize_vocabularywidget (GwMainWindow *window)
{
    //Sanity checks
    g_return_if_fail (window != NULL);
    GwMainWindowPrivate *priv = window->priv;

    {
      GtkWidget *bar = gtk_search_bar_new ();
      priv->ui.vocabulary_widget = GTK_SEARCH_BAR (bar);
      //gtk_search_bar_set_search_mode (priv->ui.vocabulary_widget, TRUE);

      {
        GtkWidget *entry = gtk_search_entry_new ();
        gtk_container_add (GTK_CONTAINER (bar), entry);
        gtk_widget_show (entry);
      }

      gtk_widget_show (bar);
    }

/*
    GwKanjiPad
    GwRadicalTable
    GwVocabularyWidget
    */
}


static void
gw_mainwindow_initialize_body (GwMainWindow *window)
{
    //Sanity checks
    g_return_if_fail (window != NULL);

    //Declarations
    GwMainWindowPrivate *priv = window->priv;

    {
      GtkWidget *stack = gtk_stack_new ();
      priv->ui.stack = GTK_STACK (stack);
      gtk_stack_set_homogeneous (priv->ui.stack, TRUE);
      lgw_window_pack_start (LGW_WINDOW (window), stack, TRUE, TRUE, 0); 
      gtk_stack_switcher_set_stack (priv->ui.stack_switcher, priv->ui.stack);
      gtk_stack_set_transition_type (priv->ui.stack, GTK_STACK_TRANSITION_TYPE_CROSSFADE);

      {
        {
          GtkWidget* search_widget = lgw_searchwidget_new ();
          priv->ui.search_widget = LGW_SEARCHWIDGET (search_widget);
          gtk_stack_add_titled (priv->ui.stack, search_widget, "dictionary", gettext("Lookup"));
          gtk_widget_show (search_widget);
        }

        gtk_stack_add_titled (priv->ui.stack, GTK_WIDGET (priv->ui.vocabulary_widget), "vocabulary", gettext("Study"));
      }

      gtk_widget_show (stack);
    }

    lgw_searchwidget_set_search_mode (priv->ui.search_widget, TRUE);
}


void
gw_mainwindow_set_actiongroup (LgwActionable *actionable,
                               LgwActionGroup *action_group)
{
    //Sanity checks
    g_return_val_if_fail (actionable != NULL, NULL);

    //Declarations
    GwMainWindow *main_window = NULL;
    GwMainWindowPrivate *priv = NULL;
    GwMainWindowClass *klass = NULL;
    GwMainWindowClassPrivate *klasspriv = NULL;
    GList *list = NULL;

    //Initializations
    main_window = GW_MAINWINDOW (actionable);
    priv = main_window->priv;
    klass = GW_MAINWINDOW_GET_CLASS (main_window);
    klasspriv = klass->priv;

    if (priv->data.action_group_list != NULL)
    {
      g_list_free (priv->data.action_group_list);
      priv->data.action_group_list = NULL;
    }

    if (priv->data.action_group != NULL)
    {
        lgw_actiongroup_free (priv->data.action_group);
        priv->data.action_group = NULL;
    }

    priv->data.action_group = action_group;

    {
      GtkWidget *widget = gtk_stack_get_visible_child (priv->ui.stack);
      if (LGW_IS_ACTIONABLE (widget))
      {
        LgwActionable *actionable = LGW_ACTIONABLE (widget);
        GList *actions = lgw_actionable_get_actions (actionable);
        if (actions != NULL)
        {
          GList *copy = g_list_copy (actions);
          priv->data.action_group_list = g_list_concat (copy, priv->data.action_group_list);
        }
      }
    }

    if (action_group != NULL)
    {
      priv->data.action_group_list = g_list_prepend (priv->data.action_group_list, action_group);
    }

    {
      GActionMap *action_map = G_ACTION_MAP (main_window);
      GList *action_group_list = lgw_actionable_get_actions (actionable);
      lgw_window_set_actions (LGW_WINDOW (main_window), action_group_list);
    }
}


void
gw_mainwindow_sync_actions (GwMainWindow *main_window)
{
    //Sanity checks
    g_return_val_if_fail (main_window != NULL, NULL);

    //Declarations
    GwMainWindowPrivate *priv = NULL;
    LgwActionable *actionable = NULL;
    GtkWidget *widget = NULL;
    gboolean has_focus = FALSE;

    //Initializations
    priv = main_window->priv;
    actionable = LGW_ACTIONABLE (main_window);
    widget = GTK_WIDGET (main_window);
    has_focus = gtk_widget_is_focus (widget);

    {
      static GActionEntry entries[] = {
        { "toggle-menubar-show", gw_mainwindow_menubar_show_toggled_cb, NULL, "false", NULL },
        { "close", gw_mainwindow_close_cb, NULL, NULL, NULL }
      };
      LgwActionGroup *action_group = lgw_actiongroup_static_new (entries, G_N_ELEMENTS (entries), widget);
      lgw_actionable_set_actiongroup (actionable, action_group);
    }
}


GList*
gw_mainwindow_get_actions (LgwActionable *actionable)
{
    //Sanity checks
    g_return_val_if_fail (actionable != NULL, NULL);

    //Declarations
    GwMainWindow *main_window = NULL;
    GwMainWindowPrivate *priv = NULL;

    //Initializations
    main_window = GW_MAINWINDOW (actionable);
    priv = main_window->priv;

    return priv->data.action_group_list;
}



