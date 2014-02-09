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
//! @file searchwidget-callbacks.c
//!
//! @brief To be written
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include <glib-object.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>
#include <libgwaei/searchwidget-private.h>


void
lgw_searchwidget_connect_signals (LgwSearchWidget *search_widget)
{
    //Sanity checks
    g_return_if_fail (search_widget != NULL);

    //Declarations
    LgwSearchWidgetPrivate *priv = NULL;

    //Initializations
    priv = search_widget->priv;

    if (priv->data.signalid[SIGNALID_SEARCHENTRY_ACTIONS] == 0)
    {
      priv->data.signalid[SIGNALID_SEARCHENTRY_ACTIONS] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.search_entry),
          "notify::actions",
          G_CALLBACK (lgw_searchwidget_child_actions_property_changed_cb),
          search_widget
      );
    }

    if (priv->data.signalid[SIGNALID_RESULTSVIEW_ACTIONS] == 0)
    {
      priv->data.signalid[SIGNALID_RESULTSVIEW_ACTIONS] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.results_view),
          "notify::actions",
          G_CALLBACK (lgw_searchwidget_child_actions_property_changed_cb),
          search_widget
      );
    }
}


void
lgw_searchwidget_disconnect_signals (LgwSearchWidget *search_widget)
{
    //Sanity checks
    g_return_if_fail (search_widget != NULL);

    //Declarations
    LgwSearchWidgetPrivate *priv = NULL;

    //Initializations
    priv = search_widget->priv;

    if (priv->data.signalid[SIGNALID_SEARCHENTRY_ACTIONS] != 0)
    {
      g_signal_handler_disconnect (G_OBJECT (priv->ui.search_entry), priv->data.signalid[SIGNALID_SEARCHENTRY_ACTIONS]);
      priv->data.signalid[SIGNALID_SEARCHENTRY_ACTIONS] = 0;
    }

    if (priv->data.signalid[SIGNALID_RESULTSVIEW_ACTIONS] != 0)
    {
      g_signal_handler_disconnect (G_OBJECT (priv->ui.results_view), priv->data.signalid[SIGNALID_RESULTSVIEW_ACTIONS]);
      priv->data.signalid[SIGNALID_RESULTSVIEW_ACTIONS] = 0;
    }
}


void
lgw_searchwidget_child_actions_property_changed_cb (LgwSearchWidget *search_widget,
                                                    GParamSpec      *pspec,
                                                    LgwActionable   *actionable)
{
    //Sanity checks
    g_return_if_fail (search_widget != NULL);
    g_return_if_fail (actionable != NULL);

    lgw_actionable_sync_actions (LGW_ACTIONABLE (search_widget));
}
