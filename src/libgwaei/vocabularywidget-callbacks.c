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
//! @file vocabularywidget-callbacks.c
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


void
lgw_vocabularywidget_connect_signals (LgwVocabularyWidget *self)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    //Declarations
    LgwVocabularyWidgetPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (priv->data.signalid[SIGNALID_VOCABULARYLISTVIEW_ACTIONS] == 0)
    {
      priv->data.signalid[SIGNALID_VOCABULARYLISTVIEW_ACTIONS] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.vocabulary_list_view),
          "notify::actions",
          G_CALLBACK (lgw_vocabularywidget_child_actions_property_changed_cb),
          self
      );
    }

    if (priv->data.signalid[SIGNALID_VOCABULARYWORDVIEW_ACTIONS] == 0)
    {
      priv->data.signalid[SIGNALID_VOCABULARYWORDVIEW_ACTIONS] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.vocabulary_word_view),
          "notify::actions",
          G_CALLBACK (lgw_vocabularywidget_child_actions_property_changed_cb),
          self
      );
    }
}


void
lgw_vocabularywidget_disconnect_signals (LgwVocabularyWidget *self)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    //Declarations
    LgwVocabularyWidgetPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (priv->data.signalid[SIGNALID_VOCABULARYLISTVIEW_ACTIONS] != 0)
    {
      g_signal_handler_disconnect (G_OBJECT (priv->ui.vocabulary_list_view), priv->data.signalid[SIGNALID_VOCABULARYLISTVIEW_ACTIONS]);
      priv->data.signalid[SIGNALID_VOCABULARYLISTVIEW_ACTIONS] = 0;
    }

    if (priv->data.signalid[SIGNALID_VOCABULARYWORDVIEW_ACTIONS] != 0)
    {
      g_signal_handler_disconnect (G_OBJECT (priv->ui.vocabulary_word_view), priv->data.signalid[SIGNALID_VOCABULARYWORDVIEW_ACTIONS]);
      priv->data.signalid[SIGNALID_VOCABULARYWORDVIEW_ACTIONS] = 0;
    }
}


void
lgw_vocabularywidget_child_actions_property_changed_cb (LgwVocabularyWidget *self,
                                                        GParamSpec          *pspec,
                                                        LgwActionable       *actionable)
{
  printf("lgw_vocabualrywidget_child_actions_property_changed_cb\n");
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWIDGET (self));
    g_return_if_fail (actionable != NULL);

    lgw_actionable_sync_actions (LGW_ACTIONABLE (self));
}

