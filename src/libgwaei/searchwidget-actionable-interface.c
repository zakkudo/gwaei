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
#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>

#include <libgwaei/searchwidget-private.h>


static void
lgw_searchwidget_rebuild_actiongroup (LgwActionable *actionable)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_SEARCHWIDGET (actionable), NULL);

    //Declarations
    LgwSearchWidget *self = NULL;
    LgwSearchWidgetPrivate *priv = NULL;
    GtkWidget *widget = NULL;

    //Initializations
    self = LGW_SEARCHWIDGET (actionable);
    priv = self->priv;
    widget = GTK_WIDGET (self);

    if (priv->data.action_group == NULL)
    {
        priv->data.action_group = lgw_actiongroup_new (widget);
    }
}


static void
lgw_searchwidget_rebuild_actiongrouplist (LgwActionable *actionable)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_SEARCHWIDGET (actionable), NULL);

    //Declarations
    LgwSearchWidget *self = NULL;
    LgwSearchWidgetPrivate *priv = NULL;

    //Initializations
    self = LGW_SEARCHWIDGET (actionable);
    priv = self->priv;

    if (priv->data.action_group_list != NULL)
    {
      g_list_free (priv->data.action_group_list);
      priv->data.action_group_list = NULL;
    }

    {
      GList *actions = lgw_actionable_get_actions (LGW_ACTIONABLE (priv->ui.results_view));
      if (actions != NULL)
      {
        priv->data.action_group_list = g_list_concat (g_list_copy (actions), priv->data.action_group_list);
      }
    }

    priv->data.action_group_list = g_list_prepend (priv->data.action_group_list, priv->data.action_group);
}


static void
lgw_searchwidget_sync_actions (LgwActionable *actionable)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_SEARCHWIDGET (actionable), NULL);

    lgw_searchwidget_rebuild_actiongroup (actionable);
    lgw_searchwidget_rebuild_actiongrouplist (actionable);
}


static GList*
lgw_searchwidget_get_actions (LgwActionable *actionable)
{
    //Sanity checks
    g_return_val_if_fail (actionable != NULL, NULL);

    //Declarations
    LgwSearchWidget *self = NULL;
    LgwSearchWidgetPrivate *priv = NULL;

    //Initializations
    self = LGW_SEARCHWIDGET (actionable);
    priv = self->priv;

    if (priv->data.action_group_list == NULL)
    {
      lgw_searchwidget_sync_actions (actionable);
    }

    return priv->data.action_group_list;
}


void
lgw_searchwidget_implement_actionable_interface (LgwActionableInterface *iface) {
    iface->get_actions = lgw_searchwidget_get_actions;
    iface->sync_actions = lgw_searchwidget_sync_actions;
}

