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
//! @file deletevocabularylistdialog.c
//!
//! @brief To be written
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>

#include <libgwaei/deletevocabularylistdialog-private.h>


static LgwDeleteVocabularyListDialogClass *_klass = NULL;
static LgwDeleteVocabularyListDialogClassPrivate *_klasspriv = NULL;


G_DEFINE_TYPE (LgwDeleteVocabularyListDialog, lgw_deletevocabularylistdialog, GTK_TYPE_DIALOG)

//!
//! @brief Sets up the variables in main-interface.c and main-callbacks.c for use
//!
GtkWidget* 
lgw_deletevocabularylistdialog_new (GtkWindow *window)
{
  g_return_val_if_fail (window != NULL, NULL);
    //Declarations
    LgwDeleteVocabularyListDialog *dialog = NULL;

    //Initializations
    dialog = LGW_DELETEVOCABULARYLISTDIALOG (g_object_new (LGW_TYPE_DELETEVOCABULARYLISTDIALOG, "transient-for", window, NULL));

    return GTK_WIDGET (dialog);
}


static void 
lgw_deletevocabularylistdialog_init (LgwDeleteVocabularyListDialog *self)
{
    self->priv = LGW_DELETEVOCABULARYLISTDIALOG_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LgwDeleteVocabularyListDialogPrivate));
}


static void
lgw_deletevocabularylistdialog_dispose (GObject *object)
{
    //Declarations
    LgwDeleteVocabularyListDialog *self = NULL;

    //Initializations
    self = LGW_DELETEVOCABULARYLISTDIALOG (object);

    lgw_deletevocabularylistdialog_disconnect_signals (self);

    G_OBJECT_CLASS (lgw_deletevocabularylistdialog_parent_class)->dispose (object);
}


static void 
lgw_deletevocabularylistdialog_finalize (GObject *object)
{
    LgwDeleteVocabularyListDialog *self;
    LgwDeleteVocabularyListDialogPrivate *priv;

    self = LGW_DELETEVOCABULARYLISTDIALOG (object);
    priv = self->priv;

    if (priv->data.vocabulary_list_store != NULL) g_object_unref (priv->data.vocabulary_list_store); priv->data.vocabulary_list_store = NULL;

    G_OBJECT_CLASS (lgw_deletevocabularylistdialog_parent_class)->finalize (object);
}


static void
lgw_deletevocabularylistdialog_set_property (GObject      *object,
                                             guint         property_id,
                                             const GValue *value,
                                             GParamSpec   *pspec)
{
    //Declarations
    LgwDeleteVocabularyListDialog *self = NULL;
    LgwDeleteVocabularyListDialogPrivate *priv = NULL;

    //Initializations
    self = LGW_DELETEVOCABULARYLISTDIALOG (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_VOCABULARYLISTSTORE:
        lgw_deletevocabularylistdialog_set_liststore (self, g_value_get_object (value));
        break;
      case PROP_DELETE_ON_RESPONSE:
        lgw_deletevocabularylistdialog_set_delete_on_response (self, g_value_get_boolean (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
lgw_deletevocabularylistdialog_get_property (GObject      *object,
                                      guint         property_id,
                                      GValue       *value,
                                      GParamSpec   *pspec)
{
    //Declarations
    LgwDeleteVocabularyListDialog *self = NULL;
    LgwDeleteVocabularyListDialogPrivate *priv = NULL;

    //Initializations
    self = LGW_DELETEVOCABULARYLISTDIALOG (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_VOCABULARYLISTSTORE:
        g_value_set_object (value, lgw_deletevocabularylistdialog_get_liststore (self));
        break;
      case PROP_DELETE_ON_RESPONSE:
        g_value_set_boolean (value, lgw_deletevocabularylistdialog_get_delete_on_response (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
_init_accelerators (LgwDeleteVocabularyListDialog *self)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_DELETEVOCABULARYLISTDIALOG (self), NULL);

    //Declarations
    LgwDeleteVocabularyListDialogPrivate *priv = NULL;
    GtkAccelGroup *accel_group = NULL;
    GtkWindow *window = NULL;

    //Initializations
    priv = self->priv;
    accel_group = gtk_accel_group_new ();
    window = GTK_WINDOW (self);

    //Set menu accelerators
    gtk_widget_add_accelerator (GTK_WIDGET (priv->ui.cancel_button), "activate",  accel_group, (GDK_KEY_Escape), 0, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (GTK_WIDGET (priv->ui.delete_button), "activate",  accel_group, (GDK_KEY_Return), 0, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (GTK_WIDGET (priv->ui.delete_button), "activate",  accel_group, (GDK_KEY_ISO_Enter), 0, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (GTK_WIDGET (priv->ui.delete_button), "activate",  accel_group, (GDK_KEY_KP_Enter), 0, GTK_ACCEL_VISIBLE);
    gtk_window_add_accel_group (window, accel_group);
}


static void 
lgw_deletevocabularylistdialog_constructed (GObject *object)
{
    //Declarations
    LgwDeleteVocabularyListDialog *self;
    LgwDeleteVocabularyListDialogPrivate *priv;
    GtkDialog *dialog = NULL;

    //Chain the parent class
    {
      G_OBJECT_CLASS (lgw_deletevocabularylistdialog_parent_class)->constructed (object);
    }

    //Initializations
    self = LGW_DELETEVOCABULARYLISTDIALOG (object);
    priv = self->priv;
    dialog = GTK_DIALOG (self);

    {
      GtkBox *box = GTK_BOX (gtk_dialog_get_content_area (dialog));

      {
        GtkWidget *layout_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 8);
        priv->ui.layout_box = GTK_BOX (layout_box);
        gtk_container_set_border_width (GTK_CONTAINER (layout_box), 6);
        gtk_box_pack_start (box, layout_box, TRUE, TRUE, 0);
        gtk_widget_show (layout_box);

        {
          {
            GtkWidget *image = gtk_image_new_from_icon_name ("delete", GTK_ICON_SIZE_DIALOG);
            if (image != NULL)
            {
              gtk_misc_set_alignment (GTK_MISC (image), .5, 0);
              gtk_box_pack_start (priv->ui.layout_box, image, FALSE, FALSE, 0);
              gtk_widget_show (image);
            }
          }

          {
            GtkWidget *grid = gtk_grid_new ();
            priv->ui.grid = GTK_GRID (grid);
            gtk_grid_set_row_spacing (priv->ui.grid, 6);
            gtk_grid_set_column_spacing (priv->ui.grid, 6);
            gtk_box_pack_start (priv->ui.layout_box, grid, TRUE, TRUE, 0);
            gtk_widget_show (grid);
          }
        }
      }
    }

    gtk_dialog_add_buttons (
      dialog,
      gettext("_Cancel"),
      LGW_DELETEVOCABULARYLISTDIALOG_RESPONSE_CANCEL,
      gettext("_Add"),
      LGW_DELETEVOCABULARYLISTDIALOG_RESPONSE_DELETE,
      NULL
    );
    gtk_dialog_set_default_response (dialog, LGW_DELETEVOCABULARYLISTDIALOG_RESPONSE_DELETE);
    priv->ui.delete_button = GTK_BUTTON (gtk_dialog_get_widget_for_response (dialog, LGW_DELETEVOCABULARYLISTDIALOG_RESPONSE_DELETE));
    priv->ui.cancel_button = GTK_BUTTON (gtk_dialog_get_widget_for_response (dialog, LGW_DELETEVOCABULARYLISTDIALOG_RESPONSE_CANCEL));

    gtk_window_set_title (GTK_WINDOW (dialog), gettext("Delete Vocabulary List"));
    gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
    gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);

    lgw_deletevocabularylistdialog_connect_signals (self);

    _init_accelerators (self);
}


static void
lgw_deletevocabularylistdialog_class_init (LgwDeleteVocabularyListDialogClass *klass)
{
    GObjectClass *object_class;

    object_class = G_OBJECT_CLASS (klass);

    object_class->constructed = lgw_deletevocabularylistdialog_constructed;
    object_class->set_property = lgw_deletevocabularylistdialog_set_property;
    object_class->get_property = lgw_deletevocabularylistdialog_get_property;
    object_class->finalize = lgw_deletevocabularylistdialog_finalize;
    object_class->dispose = lgw_deletevocabularylistdialog_dispose;

    g_type_class_add_private (object_class, sizeof(LgwDeleteVocabularyListDialogPrivate));

    klass->priv = g_new0 (LgwDeleteVocabularyListDialogClassPrivate, 1);

    _klass = klass;
    _klasspriv = klass->priv;

    _klasspriv->pspec[PROP_VOCABULARYLISTSTORE] = g_param_spec_object (
        "vocabulary-list-store",
        "prop",
        "object",
        LGW_TYPE_VOCABULARYLISTSTORE,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_VOCABULARYLISTSTORE, _klasspriv->pspec[PROP_VOCABULARYLISTSTORE]);

    _klasspriv->pspec[PROP_DELETE_ON_RESPONSE] = g_param_spec_boolean (
        "delete-on-response",
        "prop",
        "object",
        FALSE,
        G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_DELETE_ON_RESPONSE, _klasspriv->pspec[PROP_DELETE_ON_RESPONSE]);
}


LgwVocabularyListStore*
lgw_deletevocabularylistdialog_get_liststore (LgwDeleteVocabularyListDialog *self)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_DELETEVOCABULARYLISTDIALOG (self), NULL);

    //Declarations
    LgwDeleteVocabularyListDialogPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.vocabulary_list_store;
}


void
lgw_deletevocabularylistdialog_set_liststore (LgwDeleteVocabularyListDialog *self,
                                              LgwVocabularyListStore *vocabulary_list_store)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_DELETEVOCABULARYLISTDIALOG (self));

    //Declarations
    LgwDeleteVocabularyListDialogPrivate *priv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    changed = (vocabulary_list_store != priv->data.vocabulary_list_store);
    if (!changed) goto errored;

    if (vocabulary_list_store != NULL)
    {
      g_object_ref (vocabulary_list_store);
    }

    if (priv->data.vocabulary_list_store != NULL)
    {
      g_object_remove_weak_pointer (
        G_OBJECT (priv->data.vocabulary_list_store),
        (gpointer*) &(priv->data.vocabulary_list_store)
      );
      g_object_unref (priv->data.vocabulary_list_store);
      priv->data.vocabulary_list_store = NULL;
    }

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_VOCABULARYLISTSTORE]);

errored:
  
    return;
}


gboolean
lgw_deletevocabularylistdialog_get_delete_on_response (LgwDeleteVocabularyListDialog *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_DELETEVOCABULARYLISTDIALOG (self));

    //Declarations
    LgwDeleteVocabularyListDialogPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->config.delete_on_response;
}


void
lgw_deletevocabularylistdialog_set_delete_on_response (LgwDeleteVocabularyListDialog *self,
                                                       gboolean                       delete_on_response)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_DELETEVOCABULARYLISTDIALOG (self));

    //Declarations
    LgwDeleteVocabularyListDialogPrivate *priv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    changed = (delete_on_response != priv->config.delete_on_response);
    if (!changed) goto errored;

    priv->config.delete_on_response = delete_on_response;

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_DELETE_ON_RESPONSE]);

errored:

    return;
}

