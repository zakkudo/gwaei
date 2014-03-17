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
//! @file vocabularyliststore-callbacks.c
//!
//! @brief To be written
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib/gstdio.h>
#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>

#include <libgwaei/vocabularyliststore-private.h>


static void
lgw_vocabularyliststore_sync_order_cb (GSettings *settings,
                                       gchar     *key,
                                       gpointer   data)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (data));

    //Declarations
    LgwVocabularyListStore *self = NULL;
    LgwVocabularyListStorePrivate *priv = NULL;
    gchar *order = NULL;
    
    //Initializations
    self = LGW_VOCABULARYLISTSTORE (data);
    priv = self->priv;
    order = lw_preferences_get_string (settings, key);
    if (order == NULL) goto errored;

    g_signal_handler_block (self, priv->data.signalid[SIGNALID_INTERNAL_ORDER_CHANGED]);
    lgw_vocabularyliststore_set_order (self, order);
    g_signal_handler_unblock (self, priv->data.signalid[SIGNALID_INTERNAL_ORDER_CHANGED]);

errored:

    g_free (order); order = NULL; 
}


void
lgw_vocabularyliststore_connect_signals (LgwVocabularyListStore *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (self));

    //Declarations
    LgwVocabularyListStorePrivate *priv = NULL;
    LwPreferences *preferences = NULL;

    //Initializations
    priv = self->priv;
    preferences = lgw_vocabularyliststore_get_preferences (self);

    if (priv->data.signalid[SIGNALID_INTERNAL_ORDER_CHANGED] == 0)
    {
      priv->data.signalid[SIGNALID_INTERNAL_ORDER_CHANGED] = lw_preferences_add_change_listener_by_schema (
          preferences,
          LW_SCHEMA_VOCABULARY,
          LW_KEY_LIST_ORDER,
          lgw_vocabularyliststore_sync_order_cb,
          self
      );
      priv->data.signalid[SIGNALID_INTERNAL_ORDER_CHANGED] = 0;
    }
}


void
lgw_vocabularyliststore_disconnect_signals (LgwVocabularyListStore *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTSTORE (self));

    //Declarations
    LgwVocabularyListStorePrivate *priv = NULL;
    LwPreferences *preferences = NULL;

    //Initializations
    priv = self->priv;
    preferences = lgw_vocabularyliststore_get_preferences (self);

    if (priv->data.signalid[SIGNALID_INTERNAL_ORDER_CHANGED] != 0)
    {
      lw_preferences_remove_change_listener_by_schema (
        preferences,
        LW_SCHEMA_VOCABULARY,
        priv->data.signalid[SIGNALID_INTERNAL_ORDER_CHANGED]
      );
      priv->data.signalid[SIGNALID_INTERNAL_ORDER_CHANGED] = 0;
    }
}

