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
//! @file searchentry-actionable-interface.c
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

#include <libgwaei/searchentry-private.h>


static void
_rebuild_actiongroup (LgwActionable *actionable)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_SEARCHENTRY (actionable), NULL);

    //Declarations
    LgwSearchEntry *self = NULL;
    LgwSearchEntryPrivate *priv = NULL;
    GtkWidget *widget = NULL;
    gboolean has_focus = FALSE;
    gboolean has_selection = FALSE;

    //Initializations
    self = LGW_SEARCHENTRY (actionable);
    priv = self->priv;
    widget = GTK_WIDGET (self);
    has_focus = gtk_widget_has_focus (GTK_WIDGET (priv->ui.search_entry)); 
    has_selection = gtk_editable_get_selection_bounds (GTK_EDITABLE (priv->ui.search_entry), NULL, NULL);

    if (priv->data.action_group == NULL)
    {
      priv->data.action_group = lgw_actiongroup_new (widget);
    }

    {
      static GActionEntry entries[] = {
        { "copy", lgw_searchentry_copy_cb, NULL, NULL, NULL },
        { "cut", lgw_searchentry_cut_cb, NULL, NULL, NULL },
      };
      gint length = G_N_ELEMENTS (entries);
      if (has_focus && has_selection) lgw_actiongroup_add_entries (priv->data.action_group, entries, length, NULL);
      else lgw_actiongroup_remove_entries (priv->data.action_group, entries, length, NULL);
    }

    {
      static GActionEntry entries[] = {
        { "paste", lgw_searchentry_paste_cb, NULL, NULL, NULL },
      };
      gint length = G_N_ELEMENTS (entries);
      if (has_focus) lgw_actiongroup_add_entries (priv->data.action_group, entries, length, NULL);
      else lgw_actiongroup_remove_entries (priv->data.action_group, entries, length, NULL);
    }

    {
      static GActionEntry entries[] = {
        { "insert-unknown-character", lgw_searchentry_insert_unknown_character_cb, NULL, NULL, NULL },
        { "insert-word-edge-character", lgw_searchentry_insert_word_edge_cb, NULL, NULL, NULL },
        { "insert-not-word-edge-character", lgw_searchentry_insert_not_word_edge_cb, NULL, NULL, NULL },
        { "insert-and-character", lgw_searchentry_insert_and_cb, NULL, NULL, NULL },
        { "insert-or-character", lgw_searchentry_insert_or_cb, NULL, NULL, NULL },
        { "clear", lgw_searchentry_clear_search_cb, NULL, NULL, NULL },
      };
      gint length = G_N_ELEMENTS (entries);
      if (has_focus && has_selection) lgw_actiongroup_add_entries (priv->data.action_group, entries, length, NULL);
      else lgw_actiongroup_remove_entries (priv->data.action_group, entries, length, NULL);
    }
}


static void
_rebuild_actiongrouplist (LgwActionable *actionable)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_SEARCHENTRY (actionable), NULL);

    //Declarations
    LgwSearchEntry *self = NULL;
    LgwSearchEntryPrivate *priv = NULL;

    //Initializations
    self = LGW_SEARCHENTRY (actionable);
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
    g_return_val_if_fail (LGW_IS_SEARCHENTRY (actionable), NULL);

    _rebuild_actiongroup (actionable);
    _rebuild_actiongrouplist (actionable);
}


static GList*
_get_actions (LgwActionable *actionable)
{
    //Sanity checks
    g_return_val_if_fail (actionable != NULL, NULL);

    //Declarations
    LgwSearchEntry *self = NULL;
    LgwSearchEntryPrivate *priv = NULL;

    //Initializations
    self = LGW_SEARCHENTRY (actionable);
    priv = self->priv;

    if (priv->data.action_group_list == NULL || priv->data.action_group == NULL)
    {
      _sync_actions (actionable);
    }

    return priv->data.action_group_list;
}


void
lgw_searchentry_implement_actionable_interface (LgwActionableInterface *iface)
{
    iface->get_actions = _get_actions;
    iface->sync_actions = _sync_actions;
}

