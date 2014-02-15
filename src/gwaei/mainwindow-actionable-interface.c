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


static void
gw_mainwindow_rebuild_actiongroup (LgwActionable *actionable)
{
    //Sanity checks
    g_return_val_if_fail (GW_IS_MAINWINDOW (actionable), NULL);

    //Declarations
    GwMainWindow *self = NULL;
    GwMainWindowPrivate *priv = NULL;
    GtkWidget *widget = NULL;

    //Initializations
    self = GW_MAINWINDOW (actionable);
    priv = self->priv;
    widget = GTK_WIDGET (self);

    if (priv->data.action_group == NULL)
    {
        priv->data.action_group = lgw_actiongroup_new (widget);
    }

    {
      static GActionEntry entries[] = {
        { "toggle-menubar-show", gw_mainwindow_menubar_show_toggled_cb, NULL, "false", NULL },
        { "close", gw_mainwindow_close_cb, NULL, NULL, NULL }
      };
      gint length = G_N_ELEMENTS (entries);
      lgw_actiongroup_add_entries (priv->data.action_group, entries, length, NULL);
    }
}


static void
gw_mainwindow_rebuild_actiongrouplist (LgwActionable *actionable)
{
    //Sanity checks
    g_return_val_if_fail (GW_IS_MAINWINDOW (actionable), NULL);

    //Declarations
    GwMainWindow *self = NULL;
    GwMainWindowPrivate *priv = NULL;

    //Initializations
    self = GW_MAINWINDOW (actionable);
    priv = self->priv;

    if (priv->data.action_group_list != NULL)
    {
      g_list_free (priv->data.action_group_list);
      priv->data.action_group_list = NULL;
    }

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

    {
      LgwActionable *actionable = LGW_ACTIONABLE (priv->ui.search_entry);
      GList *actions = lgw_actionable_get_actions (actionable);
      if (actions != NULL)
      {
        GList *copy = g_list_copy (actions);
        priv->data.action_group_list = g_list_concat (copy, priv->data.action_group_list);
      }
    }

    priv->data.action_group_list = g_list_prepend (priv->data.action_group_list, priv->data.action_group);
}


static void
gw_mainwindow_sync_actions (LgwActionable *actionable)
{
    //Sanity checks
    g_return_val_if_fail (GW_IS_MAINWINDOW (actionable), NULL);
    g_return_val_if_fail (G_IS_ACTION_MAP (actionable), NULL);

    //Declarations
    GwMainWindow *self = NULL;
    GActionMap *action_map = NULL;

    //Initializations
    self = GW_MAINWINDOW (actionable);
    action_map = G_ACTION_MAP (actionable);

    gw_mainwindow_rebuild_actiongroup (actionable);
    gw_mainwindow_rebuild_actiongrouplist (actionable);

    {
      GActionMap *action_map = G_ACTION_MAP (actionable);
      GList *action_group_list = lgw_actionable_get_actions (actionable);
      lgw_window_set_actions (LGW_WINDOW (self), action_group_list);
    }
}


static GList*
gw_mainwindow_get_actions (LgwActionable *actionable)
{
    //Sanity checks
    g_return_val_if_fail (GW_IS_MAINWINDOW (actionable), NULL);

    //Declarations
    GwMainWindow *self = NULL;
    GwMainWindowPrivate *priv = NULL;

    //Initializations
    self = GW_MAINWINDOW (actionable);
    priv = self->priv;

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
    iface->sync_actions = gw_mainwindow_sync_actions;
}

