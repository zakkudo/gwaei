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
//! @file searchentry-callbacks.c
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


void
lgw_searchentry_connect_signals (LgwSearchEntry *entry)
{
    //Sanity checks
    g_return_if_fail (entry != NULL);

    //Declarations
    LgwSearchEntryPrivate *priv = NULL;

    //Initializations
    priv = entry->priv;

    priv->data.signalid[SIGNALID_CHANGED] = g_signal_connect (
        GTK_EDITABLE (priv->ui.search_entry),
        "changed",
        G_CALLBACK (lgw_searchentry_changed_cb),
        NULL
    );

    priv->data.signalid[SIGNALID_ACTIVATED] = g_signal_connect (
        GTK_ENTRY (priv->ui.search_entry),
        "activate",
        G_CALLBACK (lgw_searchentry_activated_cb),
        NULL
    );
}


void
lgw_searchentry_disconnect_signals (LgwSearchEntry *entry)
{
    //Sanity checks
    g_return_if_fail (entry != NULL);

    //Declarations
    LgwSearchEntryPrivate *priv = NULL;
    
    //Initializations
    priv = entry->priv;

    {
      gint i = 0;
      while (i < TOTAL_SIGNALIDS)
      {
        if (priv->data.signalid[i] != 0)
        {
          g_signal_handler_disconnect (G_OBJECT (priv->ui.search_entry), priv->data.signalid[i]);
          priv->data.signalid[i] = 0;
        }
        i++;
      }
    }
}


void
lgw_searchentry_activated_cb (GtkEntry *entry,
                              gpointer  data)
{
    printf("BREAK activated\n");
}


void
lgw_searchentry_changed_cb (GtkEditable *editable,
                            gpointer     data)
{
    printf("BREAK changed\n");
}
