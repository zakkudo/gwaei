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
//! @file searchentry.c
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

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>

#include <libgwaei/searchentry-private.h>


static void
lgw_searchentry_set_actiongroup (LgwActionable  *actionable,
                                 LgwActionGroup *action_group)
{
    //Sanity checks
    g_return_val_if_fail (actionable != NULL, NULL);

    //Declarations
    LgwSearchEntry *search_entry = NULL;
    LgwSearchEntryPrivate *priv = NULL;
    LgwSearchEntryClass *klass = NULL;
    LgwSearchEntryClassPrivate *klasspriv = NULL;
    GList *list = NULL;

    //Initializations
    search_entry = LGW_SEARCHENTRY (actionable);
    priv = search_entry->priv;
    klass = LGW_SEARCHENTRY_GET_CLASS (search_entry);
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


void
lgw_searchentry_sync_actions (LgwActionable *actionable)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_SEARCHENTRY (actionable), NULL);

    //Declarations
    LgwSearchEntry *search_entry = NULL;
    LgwSearchEntryPrivate *priv = NULL;
    GtkWidget *widget = NULL;
    gboolean has_focus = FALSE;

    //Initializations
    search_entry = LGW_SEARCHENTRY (actionable);
    priv = search_entry->priv;
    widget = GTK_WIDGET (search_entry);
    has_focus = gtk_widget_has_focus (GTK_WIDGET (priv->ui.search_entry)); 

    if (has_focus)
    {
      static GActionEntry entries[] = {
        { "copy", lgw_searchentry_copy_cb, NULL, NULL, NULL },
        { "cut", lgw_searchentry_cut_cb, NULL, NULL, NULL },
        { "paste", lgw_searchentry_paste_cb, NULL, NULL, NULL },
        { "insert-unknown-character", lgw_searchentry_insert_unknown_character_cb, NULL, NULL, NULL },
        { "insert-word-edge-character", lgw_searchentry_insert_word_edge_cb, NULL, NULL, NULL },
        { "insert-not-word-edge-character", lgw_searchentry_insert_not_word_edge_cb, NULL, NULL, NULL },
        { "insert-and-character", lgw_searchentry_insert_and_cb, NULL, NULL, NULL },
        { "insert-or-character", lgw_searchentry_insert_or_cb, NULL, NULL, NULL },
        { "clear", lgw_searchentry_clear_search_cb, NULL, NULL, NULL },
      };
      LgwActionGroup *action_group = lgw_actiongroup_static_new (entries, G_N_ELEMENTS (entries), widget);
      lgw_actionable_set_actiongroup (LGW_ACTIONABLE (search_entry), action_group);
    }
    else 
    {
      static GActionEntry entries[] = {
        { "insert-unknown-character", lgw_searchentry_insert_unknown_character_cb, NULL, NULL, NULL },
        { "insert-word-edge-character", lgw_searchentry_insert_word_edge_cb, NULL, NULL, NULL },
        { "insert-not-word-edge-character", lgw_searchentry_insert_not_word_edge_cb, NULL, NULL, NULL },
        { "insert-and-character", lgw_searchentry_insert_and_cb, NULL, NULL, NULL },
        { "insert-or-character", lgw_searchentry_insert_or_cb, NULL, NULL, NULL },
        { "clear", lgw_searchentry_clear_search_cb, NULL, NULL, NULL },
      };
      LgwActionGroup *action_group = lgw_actiongroup_static_new (entries, G_N_ELEMENTS (entries), widget);
      lgw_actionable_set_actiongroup (LGW_ACTIONABLE (search_entry), action_group);
    }
}


static GList*
lgw_searchentry_get_actions (LgwActionable *actionable)
{
    //Sanity checks
    g_return_val_if_fail (actionable != NULL, NULL);

    //Declarations
    LgwSearchEntry *search_entry = NULL;
    LgwSearchEntryPrivate *priv = NULL;

    //Initializations
    search_entry = LGW_SEARCHENTRY (actionable);
    priv = search_entry->priv;

    return priv->data.action_group_list;
}


void
lgw_searchentry_implement_actionable_interface (LgwActionableInterface *iface)
{
    iface->get_actions = lgw_searchentry_get_actions;
    iface->set_actiongroup = lgw_searchentry_set_actiongroup;
    iface->sync_actions = lgw_searchentry_sync_actions;
}

