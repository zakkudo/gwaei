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
//! @file installprogresswindow-callbacks.c
//!
//! @brief To be written
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>

#include <gtk/gtk.h>
 
#include <gwaei/gettext.h>
#include <gwaei/gwaei.h>
#include <gwaei/installprogresswindow-private.h>


G_MODULE_EXPORT void 
gw_installprogresswindow_cancel_cb (GtkWidget *widget, 
                                    gpointer   data)
{
    //Sanity checks
    g_return_if_fail (widget != NULL);

    //Declarations
    GwInstallProgressWindow *window;
    GwInstallProgressWindowPrivate *priv;

    //Initializations
    window = GW_INSTALLPROGRESSWINDOW (gtk_widget_get_ancestor (GTK_WIDGET (data), GW_TYPE_INSTALLPROGRESSWINDOW));
    g_return_if_fail (window != NULL);
    priv = window->priv;

    lw_progress_cancel (priv->progress);
}


void 
gw_installprogresswindow_update_progress_cb (LwDictionary            *dictionary, 
                                             LwProgress              *progress, 
                                             GwInstallProgressWindow *window)
{
}


static void
gw_installprogresswindow_finish (GwInstallProgressWindow *window)
{
    //Sanity checks
    g_return_if_fail (window != NULL);

    //Declarations
    GwInstallProgressWindowPrivate *priv;
    GwApplication *application;
    LwDictionaryList *dictionarylist;
    LwPreferences *preferences;
    LwMorphologyEngine *morphologyengine;

    //Initializations
    priv = window->priv;
    application = gw_window_get_application (GW_WINDOW (window));
    dictionarylist = LW_DICTIONARYLIST (gw_application_get_installed_dictionarylist (application));
    preferences = gw_application_get_preferences (application);
    morphologyengine = gw_application_get_morphologyengine (application);

    lw_dictionarylist_clear (dictionarylist);
    lw_dictionarylist_load_installed (dictionarylist, morphologyengine);
    lw_dictionarylist_load_order (dictionarylist);
}


static void
gw_installprogresswindow_sync_progress (GwInstallProgressWindow *window)
{
    //Sanity checks
    g_return_if_fail (window != NULL);

    //Declarations
    GwInstallProgressWindowPrivate *priv = NULL;
    GwApplication *application = NULL;
    LwProgress *progress = NULL;
    const gchar *PRIMARY_MESSAGE = NULL;
    const gchar *SECONDARY_MESSAGE = NULL;
    const gchar *PROGRESSBAR_MESSAGE = NULL;
    gdouble fraction = 0.0;

    //Initializations
    application = gw_window_get_application (GW_WINDOW (window));
    priv = window->priv;
    progress = priv->progress;
    PRIMARY_MESSAGE = lw_progress_get_step_message (progress);
    SECONDARY_MESSAGE = lw_progress_get_primary_message (progress);
    PROGRESSBAR_MESSAGE = lw_progress_get_secondary_message (progress);
    fraction = lw_progress_get_fraction (progress);

    gtk_label_set_text (priv->sequence_label, PRIMARY_MESSAGE);
    gtk_label_set_text (priv->direct_object_label, SECONDARY_MESSAGE);
    gtk_label_set_text (priv->action_label, PROGRESSBAR_MESSAGE);
    gtk_progress_bar_set_fraction (priv->progressbar, fraction);
    gtk_progress_bar_set_show_text (priv->progressbar, TRUE);

    gw_installprogresswindow_finish (window);
}


//!
//! @brief Callback to update the install dialog progress.  The data passed to it should be
//!        in the form of a LwDictionary.  If it is NULL, the progress window will be closed.
//!
G_MODULE_EXPORT gboolean 
gw_installprogresswindow_update_ui_timeout (gpointer data)
{
    //Sanity check
    g_return_val_if_fail (data != NULL, FALSE);

    //Declarations
    GwInstallProgressWindow *window = NULL;
    GwInstallProgressWindowPrivate *priv = NULL;
    GwApplication *application = NULL;
    gboolean status = FALSE;
    
    //Initializations
    window = GW_INSTALLPROGRESSWINDOW (gtk_widget_get_ancestor (GTK_WIDGET (data), GW_TYPE_INSTALLPROGRESSWINDOW));
    g_return_val_if_fail (window != NULL, FALSE);
    priv = window->priv;
    application = gw_window_get_application (GW_WINDOW (window));

    //The install is complete close the window
    if (lw_progress_should_abort (priv->progress) || lw_progress_is_finished (priv->progress))
    {
      gw_application_handle_error (application, NULL, TRUE, &priv->progress->error);
      status = FALSE;
      gtk_widget_destroy (GTK_WIDGET (window));
    }
    else
    {
      gw_installprogresswindow_sync_progress (window);
      status = TRUE;
    }

    return status;
}

