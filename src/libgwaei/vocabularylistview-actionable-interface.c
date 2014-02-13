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
//! @file vocabularylistview.c
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


static GList* lgw_vocabularylistview_get_actions (LgwActionable *actionable);
static void lgw_vocabularylistview_sync_actions (LgwActionable *actionable);
static void lgw_vocabularylistview_set_actiongroup (LgwActionable *actionable, LgwActionGroup *action_group);


static GList*
lgw_vocabularylistview_get_actions (LgwActionable *actionable)
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
      lgw_vocabularylistview_sync_actions (actionable);
    }

    return priv->data.action_group_list;
}


static void
lgw_vocabularylistview_set_actiongroup (LgwActionable *actionable,
                                        LgwActionGroup *action_group)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (actionable));

    //Declarations
    LgwVocabularyListView *self = NULL;
    LgwVocabularyListViewPrivate *priv = NULL;
    GList *list = NULL;

    //Initializations
    self = LGW_VOCABULARYLISTVIEW (actionable);
    priv = self->priv;

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
lgw_vocabularylistview_sync_actions (LgwActionable *actionable)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (actionable));

    //Declarations
    LgwVocabularyListView *self = NULL;
    LgwVocabularyListViewPrivate *priv = NULL;
    LgwVocabularyListStore *vocabulary_list_store = NULL;
    GtkWidget *widget = NULL;
    gboolean has_selection = FALSE;

    //Initializations
    self = LGW_VOCABULARYLISTVIEW (actionable);
    priv = self->priv;
    vocabulary_list_store = lgw_vocabularylistview_get_liststore (self);
    widget = GTK_WIDGET (self);
    has_selection = (gtk_tree_selection_count_selected_rows (priv->data.tree_selection) > 0);

    printf("BREAK lgw_vocabularylistview_sync_actions %d\n", has_selection);

    if (has_selection)
    {
      static GActionEntry entries[] = {
        { "add-new-vocabulary-list", lgw_vocabularylistview_add_new_activated_cb, NULL, NULL, NULL },
        { "remove-selected-vocabulary-lists", lgw_vocabularylistview_delete_selected_activated_cb, NULL, NULL, NULL }
      };
      LgwActionGroup *action_group = lgw_actiongroup_static_new (entries, G_N_ELEMENTS (entries), widget);
      lgw_vocabularylistview_set_actiongroup (actionable, action_group);
    }
    else
    {
      static GActionEntry entries[] = {
        { "add-new-vocabulary-list", lgw_vocabularylistview_add_new_activated_cb, NULL, NULL, NULL }
      };
      LgwActionGroup *action_group = lgw_actiongroup_static_new (entries, G_N_ELEMENTS (entries), widget);
      lgw_vocabularylistview_set_actiongroup (actionable, action_group);
    }
}


void
lgw_vocabularylistview_implement_actionable_interface (LgwActionableInterface *iface)
{
    iface->get_actions = lgw_vocabularylistview_get_actions;
    iface->set_actiongroup = lgw_vocabularylistview_set_actiongroup;
    iface->sync_actions = lgw_vocabularylistview_sync_actions;
}

