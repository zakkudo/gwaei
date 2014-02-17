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
//! @file vocabularywordview.c
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

#include <libgwaei/vocabularywordview-private.h>


static void
lgw_vocabularywordview_rebuild_actiongroup (LgwActionable *actionable)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYWORDVIEW (actionable), NULL);

    //Declarations
    LgwVocabularyWordView *self = NULL;
    LgwVocabularyWordViewPrivate *priv = NULL;
    GtkWidget *widget = NULL;
    gboolean has_focus = FALSE;
    gboolean has_selection = FALSE;

    //Initializations
    self = LGW_VOCABULARYWORDVIEW (actionable);
    priv = self->priv;
    widget = GTK_WIDGET (self);
    has_focus = gtk_widget_has_focus (GTK_WIDGET (priv->ui.tree_view)); 
    has_selection = (gtk_tree_selection_count_selected_rows (priv->data.tree_selection) > 0);

    if (priv->data.action_group == NULL)
    {
      priv->data.action_group = lgw_actiongroup_new (widget);
    }

    {
      static GActionEntry entries[] = {
        { "add-new-word", lgw_vocabularywordview_add_new_activated_cb, NULL, NULL, NULL },
      };
      gint length = G_N_ELEMENTS (entries);
      lgw_actiongroup_add_entries (priv->data.action_group, entries, length, NULL);
    }

    {
      static GActionEntry entries[] = {
        { "remove-selected-words", lgw_vocabularywordview_remove_selected_activated_cb, NULL, NULL, NULL }
      };
      gint length = G_N_ELEMENTS (entries);
      if (has_selection) lgw_actiongroup_add_entries (priv->data.action_group, entries, length, NULL);
      else lgw_actiongroup_remove_entries (priv->data.action_group, entries, length, NULL);
    }
}


static void
lgw_vocabularywordview_rebuild_actiongrouplist (LgwActionable *actionable)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYWORDVIEW (actionable), NULL);

    //Declarations
    LgwVocabularyWordView *self = NULL;
    LgwVocabularyWordViewPrivate *priv = NULL;

    //Initializations
    self = LGW_VOCABULARYWORDVIEW (actionable);
    priv = self->priv;

    if (priv->data.action_group_list != NULL)
    {
      g_list_free (priv->data.action_group_list);
      priv->data.action_group_list = NULL;
    }

    priv->data.action_group_list = g_list_prepend (priv->data.action_group_list, priv->data.action_group);
}


static void
lgw_vocabularywordview_sync_actions (LgwActionable *actionable)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYWORDVIEW (actionable), NULL);

    lgw_vocabularywordview_rebuild_actiongroup (actionable);
    lgw_vocabularywordview_rebuild_actiongrouplist (actionable);
}


static GList*
lgw_vocabularywordview_get_actions (LgwActionable *actionable)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYWORDVIEW (actionable), NULL);

    //Declarations
    LgwVocabularyWordView *self = NULL;
    LgwVocabularyWordViewPrivate *priv = NULL;

    //Initializations
    self = LGW_VOCABULARYWORDVIEW (actionable);
    priv = self->priv;

    if (priv->data.action_group_list == NULL)
    {
      lgw_vocabularywordview_sync_actions (actionable);
    }

    return priv->data.action_group_list;
}


void
lgw_vocabularywordview_implement_actionable_interface (LgwActionableInterface *iface)
{
    iface->get_actions = lgw_vocabularywordview_get_actions;
    iface->sync_actions = lgw_vocabularywordview_sync_actions;
}

