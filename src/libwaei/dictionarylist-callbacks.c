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
//! @file dictionarylist-callbacks.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <glib.h>
#include <glib/gstdio.h>

#include <libwaei/gettext.h>
#include <libwaei/libwaei.h>

#include <libwaei/dictionarylist-private.h>


void
lw_dictionarylist_connect_signals (LwDictionaryList *dictionary_list)
{
    //Sanity checks
    g_return_if_fail (dictionary_list != NULL);

    //Declarations
    LwDictionaryListPrivate *priv = NULL;

    //Initializations
    priv = dictionary_list->priv;

    if (priv->signalid[SIGNALID_INSERTED] == 0)
    {
      priv->signalid[SIGNALID_INSERTED] = g_signal_connect (
        G_OBJECT (dictionary_list),
        "inserted",
        G_CALLBACK (lw_dictionarylist_inserted_cb),
        NULL
      );
    }

    if (priv->signalid[SIGNALID_DELETED] == 0)
    {
      priv->signalid[SIGNALID_DELETED] = g_signal_connect (
        G_OBJECT (dictionary_list),
        "deleted",
        G_CALLBACK (lw_dictionarylist_deleted_cb),
        NULL
      );
    }

    if (priv->signalid[SIGNALID_REORDERED] == 0)
    {
      priv->signalid[SIGNALID_REORDERED] = g_signal_connect (
        G_OBJECT (dictionary_list),
        "reordered",
        G_CALLBACK (lw_dictionarylist_reordered_cb),
        NULL
      );
    }
}


void
lw_dictionarylist_disconnect_signals (LwDictionaryList *dictionary_list)
{
    //Sanity checks
    g_return_if_fail (dictionary_list != NULL);

    //Declarations
    LwDictionaryListPrivate *priv = NULL;

    //Initializations
    priv = dictionary_list->priv;

    {
      gint i = 0;
      for (i = 0; i < TOTAL_SIGNALIDS; i++)
      {
        if (priv->signalid[i] != 0)
        {
          g_signal_handler_disconnect (G_OBJECT (dictionary_list), priv->signalid[i]);
          priv->signalid[i] = 0;
        }
      }
    }
}


void
lw_dictionarylist_inserted_cb (LwDictionaryList *dictionary_list,
                               gint              position,
                               gpointer          data)
{
    //Declarations
    LwDictionary *dictionary = NULL;

    //Initializations
    dictionary = lw_dictionarylist_get_dictionary_by_position (dictionary_list, position);

    lw_dictionarylist_menumodel_insert (dictionary_list, dictionary, position);
}


void
lw_dictionarylist_deleted_cb (LwDictionaryList *dictionary_list,
                              gint              position,
                              gpointer          data)
{
    //Declarations
    GMenuModel *menu_model = NULL;
    GMenu *menu = NULL;

    //Initializations
    menu_model = lw_dictionarylist_get_menumodel (dictionary_list);
    menu = G_MENU (menu_model);

    g_menu_remove (menu, position);
}


void
lw_dictionarylist_reordered_cb (LwDictionaryList *dictionary_list,
                                gint             *new_order,
                               gpointer          data)
{
  lw_dictionarylist_sync_menumodel (dictionary_list);
}


