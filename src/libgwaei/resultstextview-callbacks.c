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
//! @file resultstextview-callbacks.c
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

#include <libgwaei/resultstextview-private.h>


void
lgw_resultstextview_connect_signals (LgwResultsTextView *results_text_view)
{
    //Sanity checks
    g_return_if_fail (results_text_view != NULL);

    //Declarations
    LgwResultsTextViewPrivate *priv = NULL;
    
    //Initializations
    priv = results_text_view->priv;

    if (priv->data.signalid[SIGNALID_FOCUS_IN_EVENT] == 0)
    {
      priv->data.signalid[SIGNALID_FOCUS_IN_EVENT] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.text_view),
          "focus-in-event",
          G_CALLBACK (lgw_resultstextview_focus_in_event_cb),
          results_text_view
      );
    }

    if (priv->data.signalid[SIGNALID_FOCUS_OUT_EVENT] == 0)
    {
      priv->data.signalid[SIGNALID_FOCUS_OUT_EVENT] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.text_view),
          "focus-out-event",
          G_CALLBACK (lgw_resultstextview_focus_out_event_cb),
          results_text_view
      );
    }
}


void
lgw_resultstextview_disconnect_signals (LgwResultsTextView *results_text_view)
{
    //Sanity checks
    g_return_if_fail (results_text_view != NULL);

    //Declarations
    LgwResultsTextViewPrivate *priv = NULL;
    
    //Initializations
    priv = results_text_view->priv;

    if (priv->data.signalid[SIGNALID_FOCUS_IN_EVENT] != 0)
    {
      g_signal_handler_disconnect (G_OBJECT (priv->ui.text_view), priv->data.signalid[SIGNALID_FOCUS_IN_EVENT]);
      priv->data.signalid[SIGNALID_FOCUS_IN_EVENT] = 0;
    }

    if (priv->data.signalid[SIGNALID_FOCUS_OUT_EVENT] != 0)
    {
      g_signal_handler_disconnect (G_OBJECT (priv->ui.text_view), priv->data.signalid[SIGNALID_FOCUS_OUT_EVENT]);
      priv->data.signalid[SIGNALID_FOCUS_OUT_EVENT] = 0;
    }
}


gboolean
lgw_resultstextview_focus_in_event_cb (LgwResultsTextView *results_text_view,
                                       GdkEvent           *event,
                                       GtkTextView        *internal_text_view)
{

    printf("BREAK0 resultstextview focus in event\n");

    lgw_resultstextview_sync_actions (results_text_view);

    printf("BREAK1 resultstextview focus in event\n");

    return FALSE;
}


gboolean
lgw_resultstextview_focus_out_event_cb (LgwResultsTextView *results_text_view,
                                        GdkEvent           *event,
                                        GtkTextView        *internal_text_view)
{
    printf("BREAK0 resultstextview focus out event\n");

    lgw_resultstextview_sync_actions (results_text_view);

    printf("BREAK1 resultstextview focus out event\n");

    return FALSE;
}


void
lgw_resultstextview_copy_cb (GSimpleAction *action,
                             GVariant      *parameter,
                             gpointer       data)
{
    //Sanity checks
    g_return_if_fail (data != NULL);

    //Declarations
    LgwResultsTextView *results_text_view = LGW_RESULTSTEXTVIEW (data);

    //Initializations

    printf("BREAK lgw_resultstextview\n");
}


