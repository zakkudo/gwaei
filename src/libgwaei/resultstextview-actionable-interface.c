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
//! @file resultstextview.c
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

#include <libgwaei/resultstextview-private.h>


static void lgw_resultstextview_set_actiongroup (LgwActionable *actionable, LgwActionGroup *action_group);
static void lgw_resultstextview_sync_actions (LgwActionable *actionable);
static GList* lgw_resultstextview_get_actions (LgwActionable *actionable);


static void
lgw_resultstextview_set_actiongroup (LgwActionable  *actionable,
                                     LgwActionGroup *action_group)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_ACTIONABLE (actionable), NULL);

    //Declarations
    LgwResultsTextView *results_text_view = NULL;
    LgwResultsTextViewPrivate *priv = NULL;
    LgwResultsTextViewClass *klass = NULL;
    LgwResultsTextViewClassPrivate *klasspriv = NULL;
    GList *list = NULL;

    //Initializations
    results_text_view = LGW_RESULTSTEXTVIEW (actionable);
    priv = results_text_view->priv;
    klass = LGW_RESULTSTEXTVIEW_GET_CLASS (results_text_view);
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

    if (action_group != NULL)
    {
      priv->data.action_group_list = g_list_prepend (priv->data.action_group_list, action_group);
    }
}


static void
lgw_resultstextview_sync_actions (LgwActionable *actionable)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_RESULTSTEXTVIEW (actionable), NULL);

    //Declarations
    LgwResultsTextView *results_text_view = NULL;
    LgwResultsTextViewPrivate *priv = NULL;
    GtkWidget *widget = NULL;
    gboolean has_focus = FALSE;

    //Initializations
    results_text_view = LGW_RESULTSTEXTVIEW (actionable);
    priv = results_text_view->priv;
    widget = GTK_WIDGET (results_text_view);
    has_focus = gtk_widget_is_focus (GTK_WIDGET (priv->ui.text_view));

    if (has_focus)
    {
      static GActionEntry entries[] = {
        { "copy", lgw_resultstextview_copy_cb, NULL, NULL, NULL }
      };
      LgwActionGroup *action_group = lgw_actiongroup_static_new (entries, G_N_ELEMENTS (entries), widget);
      lgw_resultstextview_set_actiongroup (actionable, action_group);
    }
    else 
    {
      lgw_resultstextview_set_actiongroup (actionable, NULL);
    }

}


static GList*
lgw_resultstextview_get_actions (LgwActionable *actionable)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_ACTIONABLE (actionable), NULL);

    //Declarations
    LgwResultsTextView *results_text_view = NULL;
    LgwResultsTextViewPrivate *priv = NULL;

    //Initializations
    results_text_view = LGW_RESULTSTEXTVIEW (actionable);
    priv = results_text_view->priv;

    if (priv->data.action_group_list == NULL)
    {
      lgw_resultstextview_sync_actions (actionable);
    }

    return priv->data.action_group_list;
}


void
lgw_resultstextview_implement_actionable_interface (LgwActionableInterface *iface)
{
    iface->get_actions = lgw_resultstextview_get_actions;
    iface->set_actiongroup = lgw_resultstextview_set_actiongroup;
    iface->sync_actions = lgw_resultstextview_sync_actions;
}


