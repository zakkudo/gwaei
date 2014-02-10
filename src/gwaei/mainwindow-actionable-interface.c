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


static void gw_mainwindow_set_actiongroup (LgwActionable *actionable, LgwActionGroup *action_group);
static void gw_mainwindow_sync_actions (LgwActionable *actionable);
static GList* gw_mainwindow_get_actions (LgwActionable *actionable);


static void
gw_mainwindow_set_actiongroup (LgwActionable  *actionable,
                               LgwActionGroup *action_group)
{
    //Sanity checks
    g_return_val_if_fail (GW_IS_MAINWINDOW (actionable), NULL);

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
      printf("BREAK setting window actions\n");
      lgw_window_set_actions (LGW_WINDOW (main_window), action_group_list);
    }
}


static void
gw_mainwindow_sync_actions (LgwActionable *actionable)
{
    printf("BREAK gw_mainwindow_sync_actions\n");
    //Sanity checks
    g_return_val_if_fail (GW_IS_MAINWINDOW (actionable), NULL);

    //Declarations
    GwMainWindow *main_window = NULL;
    GwMainWindowPrivate *priv = NULL;
    GtkWidget *widget = NULL;
    gboolean has_focus = FALSE;

    //Initializations
    main_window = GW_MAINWINDOW (actionable);
    priv = main_window->priv;
    widget = GTK_WIDGET (main_window);
    has_focus = gtk_widget_is_focus (widget);

    {
      static GActionEntry entries[] = {
        { "toggle-menubar-show", gw_mainwindow_menubar_show_toggled_cb, NULL, "false", NULL },
        { "close", gw_mainwindow_close_cb, NULL, NULL, NULL }
      };
      LgwActionGroup *action_group = lgw_actiongroup_static_new (entries, G_N_ELEMENTS (entries), widget);
      gw_mainwindow_set_actiongroup (actionable, action_group);
    }
}


static GList*
gw_mainwindow_get_actions (LgwActionable *actionable)
{
    //Sanity checks
    g_return_val_if_fail (GW_IS_MAINWINDOW (actionable), NULL);

    //Declarations
    GwMainWindow *main_window = NULL;
    GwMainWindowPrivate *priv = NULL;

    //Initializations
    main_window = GW_MAINWINDOW (actionable);
    priv = main_window->priv;

    if (priv->data.action_group_list == NULL)
    {
      gw_mainwindow_sync_actions (actionable);
    }

    return priv->data.action_group_list;
}


void
gw_mainwindow_implement_actionable_interface (LgwActionableInterface *iface)
{
    iface->get_actions = gw_mainwindow_get_actions;
    iface->set_actiongroup = gw_mainwindow_set_actiongroup;
    iface->sync_actions = gw_mainwindow_sync_actions;
}

