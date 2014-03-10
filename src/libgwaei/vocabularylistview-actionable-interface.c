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
//! @file vocabularylistview-actionable-interface.c
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

#include <libgwaei/vocabularylistview-private.h>


static void
_rebuild_actiongroup (LgwActionable *actionable)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYLISTVIEW (actionable), NULL);

    //Declarations
    LgwVocabularyListView *self = NULL;
    LgwVocabularyListViewPrivate *priv = NULL;
    GtkWidget *widget = NULL;
    gboolean has_focus = FALSE;
    gboolean has_selection = FALSE;
    gboolean has_liststore = FALSE;

    //Initializations
    self = LGW_VOCABULARYLISTVIEW (actionable);
    priv = self->priv;
    widget = GTK_WIDGET (self);
    has_focus = gtk_widget_has_focus (GTK_WIDGET (priv->ui.tree_view)); 
    has_selection = (gtk_tree_selection_count_selected_rows (priv->data.tree_selection) > 0);
    has_liststore = (priv->data.vocabulary_list_store != NULL);

    if (priv->data.action_group == NULL)
    {
      priv->data.action_group = lgw_actiongroup_new (widget);
    }

    {
      static GActionEntry entries[] = {
        { "add-new-vocabulary-list", lgw_vocabularylistview_add_new_activated_cb, NULL, NULL, NULL },
      };
      gint length = G_N_ELEMENTS (entries);
      if (has_liststore) lgw_actiongroup_add_entries (priv->data.action_group, entries, length, NULL);
      else lgw_actiongroup_remove_entries (priv->data.action_group, entries, length, NULL);
    }

    {
      static GActionEntry entries[] = {
        { "remove-selected-vocabulary-lists", lgw_vocabularylistview_delete_selected_activated_cb, NULL, NULL, NULL }
      };
      gint length = G_N_ELEMENTS (entries);
      if (has_selection) lgw_actiongroup_add_entries (priv->data.action_group, entries, length, NULL);
      else lgw_actiongroup_remove_entries (priv->data.action_group, entries, length, NULL);
    }

    {
      static GActionEntry entries[] = {
        { "global-delete", lgw_vocabularylistview_delete_selected_activated_cb, NULL, NULL, NULL },
        { "global-copy", lgw_vocabularylistview_copy_selected_activated_cb, NULL, NULL, NULL },
        { "global-cut", lgw_vocabularylistview_cut_selected_activated_cb, NULL, NULL, NULL }
      };
      gint length = G_N_ELEMENTS (entries);
      if (has_selection && has_focus) lgw_actiongroup_add_entries (priv->data.action_group, entries, length, NULL);
      else lgw_actiongroup_remove_entries (priv->data.action_group, entries, length, NULL);
    }

    {
      static GActionEntry entries[] = {
        { "global-paste", lgw_vocabularylistview_paste_activated_cb, NULL, NULL, NULL }
      };
      gint length = G_N_ELEMENTS (entries);
      if (has_focus) lgw_actiongroup_add_entries (priv->data.action_group, entries, length, NULL);
      else lgw_actiongroup_remove_entries (priv->data.action_group, entries, length, NULL);
    }

}


static void
_rebuild_actiongrouplist (LgwActionable *actionable)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYLISTVIEW (actionable), NULL);

    //Declarations
    LgwVocabularyListView *self = NULL;
    LgwVocabularyListViewPrivate *priv = NULL;

    //Initializations
    self = LGW_VOCABULARYLISTVIEW (actionable);
    priv = self->priv;

    if (priv->data.action_group_list != NULL)
    {
      g_list_free (priv->data.action_group_list);
      priv->data.action_group_list = NULL;
    }

    priv->data.action_group_list = g_list_prepend (priv->data.action_group_list, priv->data.action_group);
}


static void
_sync_actions (LgwActionable *actionable)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYLISTVIEW (actionable), NULL);

    _rebuild_actiongroup (actionable);
    _rebuild_actiongrouplist (actionable);
}


static GList*
_get_actions (LgwActionable *actionable)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYLISTVIEW (actionable), NULL);

    //Declarations
    LgwVocabularyListView *self = NULL;
    LgwVocabularyListViewPrivate *priv = NULL;

    //Initializations
    self = LGW_VOCABULARYLISTVIEW (actionable);
    priv = self->priv;

    if (priv->data.action_group_list == NULL)
    {
      _sync_actions (actionable);
    }

    return priv->data.action_group_list;
}


void
lgw_vocabularylistview_implement_actionable_interface (LgwActionableInterface *iface)
{
    iface->get_actions = _get_actions;
    iface->sync_actions = _sync_actions;
}

