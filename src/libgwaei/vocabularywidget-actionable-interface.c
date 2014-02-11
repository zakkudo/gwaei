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
//! @file vocabularywidget.c
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

#include <libgwaei/vocabularywidget-private.h>


static GList* lgw_vocabularywidget_get_actions (LgwActionable *actionable);
static void lgw_vocabularywidget_set_actiongroup (LgwActionable *actionable, LgwActionGroup *action_group);
static void lgw_vocabularywidget_sync_actions (LgwActionable *actionable);


static GList*
lgw_vocabularywidget_get_actions (LgwActionable *actionable)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYWIDGET (actionable), NULL);

    //Declarations
    LgwVocabularyWidget *vocabulary_widget = NULL;
    LgwVocabularyWidgetPrivate *priv = NULL;

    //Initializations
    vocabulary_widget = LGW_VOCABULARYWIDGET (actionable);
    priv = vocabulary_widget->priv;

    if (priv->data.action_group_list == NULL)
    {
      lgw_vocabularywidget_sync_actions (actionable);
    }

    return priv->data.action_group_list;
}


static void
lgw_vocabularywidget_set_actiongroup (LgwActionable  *actionable,
                                      LgwActionGroup *action_group)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWIDGET (actionable));

    //Declarations
    LgwVocabularyWidget *vocabulary_widget = NULL;
    LgwVocabularyWidgetPrivate *priv = NULL;
    GList *list = NULL;

    //Initializations
    vocabulary_widget = LGW_VOCABULARYWIDGET (actionable);
    priv = vocabulary_widget->priv;

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

    {
      LgwActionable *actionable = LGW_ACTIONABLE (priv->ui.vocabulary_list_view);
      GList *actions = lgw_actionable_get_actions (actionable);
      if (actions != NULL)
      {
        GList *copy = g_list_copy (actions);
        priv->data.action_group_list = g_list_concat (copy, priv->data.action_group_list);
      }
    }

    {
      LgwActionable *actionable = LGW_ACTIONABLE (priv->ui.vocabulary_word_view);
      GList *actions = lgw_actionable_get_actions (actionable);
      if (actions != NULL)
      {
        GList *copy = g_list_copy (actions);
        priv->data.action_group_list = g_list_concat (copy, priv->data.action_group_list);
      }
    }
}


static void
lgw_vocabularywidget_sync_actions (LgwActionable *actionable)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWIDGET (actionable));

    //Declarations
    LgwVocabularyWidget *vocabulary_widget = NULL;
    LgwVocabularyWidgetPrivate *priv = NULL;
    GtkWidget *widget = NULL;

    //Initializations
    vocabulary_widget = LGW_VOCABULARYWIDGET (actionable);
    priv = vocabulary_widget->priv;
    widget = GTK_WIDGET (vocabulary_widget);

/*
    static GActionEntry entries[] = {
    };
    if (priv->data.action_group == NULL || !lgw_actiongroup_contains_entries (priv->data.action_group, entries, G_N_ELEMENTS (entries)))
    {
      LgwActionGroup *action_group = lgw_actiongroup_static_new (entries, G_N_ELEMENTS (entries), widget);
      lgw_vocabularywidget_set_actiongroup (actionable, action_group);
    }
*/
    lgw_vocabularywidget_set_actiongroup (actionable, NULL);
}


void
lgw_vocabularywidget_implement_actionable_interface (LgwActionableInterface *iface) {
    iface->get_actions = lgw_vocabularywidget_get_actions;
    iface->set_actiongroup = lgw_vocabularywidget_set_actiongroup;
    iface->sync_actions = lgw_vocabularywidget_sync_actions;
}

