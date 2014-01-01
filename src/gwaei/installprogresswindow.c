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
//! @file installprogresswindow.c
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

static gpointer gw_installprogresswindow_install_thread (gpointer);

typedef enum
{
  PROP_0,
  PROP_PROGRESS
} GwWindowProps;

G_DEFINE_TYPE (GwInstallProgressWindow, gw_installprogresswindow, GW_TYPE_WINDOW)

//!
//! @brief Sets up the variables in main-interface.c and main-callbacks.c for use
//!
GtkWindow* 
gw_installprogresswindow_new (GtkApplication *application)
{
    //Sanity checks
    g_return_val_if_fail (application != NULL, NULL);

    //Declarations
    GwInstallProgressWindow *window = NULL;

    //Initializations
    window = GW_INSTALLPROGRESSWINDOW (g_object_new (GW_TYPE_INSTALLPROGRESSWINDOW,
                                            "type",        GTK_WINDOW_TOPLEVEL,
                                            "application", GW_APPLICATION (application),
                                            "ui-xml",      "installprogresswindow.ui",
                                            NULL));

    return GTK_WINDOW (window);
}

static void 
gw_installprogresswindow_set_property (GObject      *object,
                                       guint         property_id,
                                       const GValue *value,
                                       GParamSpec   *pspec)
{
    GwInstallProgressWindow *window;
    GwInstallProgressWindowPrivate *priv;

    window = GW_INSTALLPROGRESSWINDOW (object);
    priv = window->priv;

    switch (property_id)
    {
      case PROP_PROGRESS:
        priv->progress = g_value_get_pointer (value);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
gw_installprogresswindow_get_property (GObject      *object,
                                       guint         property_id,
                                       GValue       *value,
                                       GParamSpec   *pspec)
{
    GwInstallProgressWindow *window;
    GwInstallProgressWindowPrivate *priv;

    window = GW_INSTALLPROGRESSWINDOW (object);
    priv = window->priv;

    switch (property_id)
    {
      case PROP_PROGRESS:
        g_value_set_pointer (value, priv->progress);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}



static void 
gw_installprogresswindow_init (GwInstallProgressWindow *window)
{
    window->priv = GW_INSTALLPROGRESSWINDOW_GET_PRIVATE (window);
    memset(window->priv, 0, sizeof(GwInstallProgressWindowPrivate));
}


static void 
gw_installprogresswindow_finalize (GObject *object)
{
    GwInstallProgressWindow *window;
    GwInstallProgressWindowPrivate *priv;

    window = GW_INSTALLPROGRESSWINDOW (object);
    priv = window->priv;
 
    if (priv->progressbar != NULL) lw_progress_free (priv->progress); priv->progress = NULL;

    G_OBJECT_CLASS (gw_installprogresswindow_parent_class)->finalize (object);
}


static void 
gw_installprogresswindow_constructed (GObject *object)
{
    //Declarations
    GwInstallProgressWindow *window;
    GwInstallProgressWindowPrivate *priv;
    GtkAccelGroup *accelgroup;

    //Chain the parent class
    {
      G_OBJECT_CLASS (gw_installprogresswindow_parent_class)->constructed (object);
    }

    window = GW_INSTALLPROGRESSWINDOW (object);
    priv = window->priv;
    accelgroup = gw_window_get_accel_group (GW_WINDOW (window));

    priv->sequence_label = GTK_LABEL (gw_window_get_object (GW_WINDOW (window), "sequence_label"));
    priv->action_label = GTK_LABEL (gw_window_get_object (GW_WINDOW (window), "action_label"));
    priv->direct_object_label = GTK_LABEL (gw_window_get_object (GW_WINDOW (window), "direct_object_label"));
    priv->progressbar = GTK_PROGRESS_BAR (gw_window_get_object (GW_WINDOW (window), "progress_progressbar"));
    priv->cancel_button = GTK_BUTTON (gw_window_get_object (GW_WINDOW (window), "cancel_button"));
    priv->progress = lw_progress_new (g_cancellable_new (), (LwProgressCallback) gw_installprogresswindow_update_progress_cb, window);


    gtk_window_set_title (GTK_WINDOW (window), gettext("Installing Dictionaries..."));
    gtk_window_set_resizable (GTK_WINDOW (window), TRUE);
    gtk_window_set_type_hint (GTK_WINDOW (window), GDK_WINDOW_TYPE_HINT_DIALOG);
    gtk_window_set_skip_taskbar_hint (GTK_WINDOW (window), TRUE);
    gtk_window_set_skip_pager_hint (GTK_WINDOW (window), TRUE);
    gtk_window_set_destroy_with_parent (GTK_WINDOW (window), TRUE);
    gtk_window_set_icon_name (GTK_WINDOW (window), "gwaei");
    gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER_ON_PARENT);
    gtk_window_set_modal (GTK_WINDOW (window), TRUE);
    gtk_window_set_default_size (GTK_WINDOW (window), 500, -1);
    gtk_window_set_has_resize_grip (GTK_WINDOW (window), FALSE);
    gtk_container_set_border_width (GTK_CONTAINER (window), 4);


    gtk_widget_add_accelerator (GTK_WIDGET (priv->cancel_button), "activate", 
      accelgroup, (GDK_KEY_W), GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (GTK_WIDGET (priv->cancel_button), "activate", 
      accelgroup, (GDK_KEY_Escape), 0, GTK_ACCEL_VISIBLE);

    gw_window_unload_xml (GW_WINDOW (window));
}


static void
gw_installprogresswindow_class_init (GwInstallProgressWindowClass *klass)
{
    //Declarations
    GParamSpec *pspec;
    GObjectClass *object_class;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);

    object_class->constructed = gw_installprogresswindow_constructed;
    object_class->finalize = gw_installprogresswindow_finalize;
    object_class->set_property = gw_installprogresswindow_set_property;
    object_class->get_property = gw_installprogresswindow_get_property;

    g_type_class_add_private (object_class, sizeof (GwInstallProgressWindowPrivate));

    pspec = g_param_spec_pointer ("progress",
                                 "An object for storing operation progress",
                                 "Operation progress",
                                 G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_PROGRESS, pspec);
}


//!
//! @brief Starts the install when the add button on the dictionary chooser is selected
//!
void 
gw_installprogresswindow_start (GwInstallProgressWindow *window)
{
    //Sanity check
    g_assert (window != NULL);

    //Declarations
    GwApplication *application;
    GwInstallProgressWindowPrivate *priv;

    //Initializations
    application = gw_window_get_application (GW_WINDOW (window));
    priv = window->priv;

    //Set the new window
    g_thread_try_new (
      "gwaei-install-thread", 
      gw_installprogresswindow_install_thread, 
      window,
      &priv->progress->error
    );

    g_timeout_add (100, gw_installprogresswindow_update_ui_timeout, window);
}


static gpointer 
gw_installprogresswindow_install_thread (gpointer data)
{
    //Declarations
    GwInstallProgressWindow *window = NULL;
    GwInstallProgressWindowPrivate *priv = NULL;
    GwApplication *application = NULL;
    GwDictionaryList *dictionarylist = NULL;
    GList *list = NULL, *link = NULL;
    LwProgress *progress = NULL;

    //Initializations
    window = GW_INSTALLPROGRESSWINDOW (data);
    if (window == NULL) return NULL;
    priv = window->priv;
    application = gw_window_get_application (GW_WINDOW (window));
    dictionarylist = gw_application_get_installable_dictionarylist (application);
    gint current_step = 0;
    gint total_steps = 0;
    list = lw_dictionarylist_get_list (LW_DICTIONARYLIST (dictionarylist));
    progress = priv->progress;

    for (link = list; link != NULL; link = link->next)
    {
      LwDictionary *dictionary = LW_DICTIONARY (link->data);
      if (dictionary != NULL && lw_dictionary_is_selected (dictionary)) total_steps++;
    }

    //Do the installation
    for (link = list; link != NULL && !lw_progress_should_abort (progress); link = link->next)
    {
      LwDictionary *dictionary = LW_DICTIONARY (link->data);
      if (dictionary != NULL && lw_dictionary_is_selected (dictionary))
      {
        const gchar *MESSAGE = gettext("Installing dictionary %d of %d");
        lw_progress_set_step_message (progress, MESSAGE, current_step, total_steps);
        lw_dictionary_install (dictionary, progress);
        current_step++;
      }
    }

    lw_progress_set_finished (progress, TRUE);

    if (list != NULL) g_list_free (list); list = NULL;

    return NULL;
}


