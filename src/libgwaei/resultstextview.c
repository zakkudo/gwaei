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
//! @file resultstextview.c
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


//Static declarations
static void lgw_resultstextview_connect_signals (LgwResultsTextView*);
static void lgw_resultstextview_disconnect_signals (LgwResultsTextView*);

G_DEFINE_TYPE (LgwResultsTextView, lgw_resultstextview, GTK_TYPE_BOX)


//!
//! @brief Sets up the variables in main-interface.c and main-callbacks.c for use
//!
GtkWidget* 
lgw_resultstextview_new ()
{
    //Declarations
    LgwResultsTextView *widget = NULL;

    //Initializations
    widget = LGW_RESULTSTEXTVIEW (g_object_new (LGW_TYPE_RESULTSTEXTVIEW, NULL));

    return GTK_WIDGET (widget);
}


static void 
lgw_resultstextview_init (LgwResultsTextView *widget)
{
    widget->priv = LGW_RESULTSTEXTVIEW_GET_PRIVATE (widget);
    memset(widget->priv, 0, sizeof(LgwResultsTextViewPrivate));

    LgwResultsTextViewPrivate *priv;
    priv = widget->priv;
}


static void 
lgw_resultstextview_finalize (GObject *object)
{
    LgwResultsTextView *widget;
    LgwResultsTextViewPrivate *priv;

    widget = LGW_RESULTSTEXTVIEW (object);
    priv = widget->priv;

    G_OBJECT_CLASS (lgw_resultstextview_parent_class)->finalize (object);
}


static void 
lgw_resultstextview_constructed (GObject *object)
{
    //Sanity checks
    g_return_if_fail (object != NULL);

    //Declarations
    LgwResultsTextView *view = NULL;
    LgwResultsTextViewPrivate *priv = NULL;
    LgwResultsTextViewClass *klass = NULL;
    LgwResultsTextViewClassPrivate *klasspriv = NULL;

    //Chain the parent class
    {
      G_OBJECT_CLASS (lgw_resultstextview_parent_class)->constructed (object);
    }

    //Initializations
    view = LGW_RESULTSTEXTVIEW (object);
    priv = view->priv;
    priv->ui.box = GTK_BOX (view);
    klass = LGW_RESULTSTEXTVIEW_GET_CLASS (view);
    klasspriv = klass->priv;
    g_return_if_fail (klasspriv->text_tag_table != NULL);

    {
        GtkWidget *text_view = gtk_text_view_new ();
        priv->ui.text_view = GTK_TEXT_VIEW (text_view);
        gtk_box_pack_start (priv->ui.box, text_view, TRUE, TRUE, 0);
        gtk_widget_show (text_view);

        GtkTextBuffer *text_buffer = gtk_text_buffer_new (klasspriv->text_tag_table);
        priv->ui.text_buffer = GTK_TEXT_BUFFER (text_buffer);
        gtk_text_view_set_buffer (priv->ui.text_view, text_buffer);
        g_object_unref (text_buffer);
    }
}


static void
lgw_resultstextview_class_init (LgwResultsTextViewClass *klass)
{
    //Sanity checks
    g_return_if_fail (klass != NULL);

    //Declarations
    GObjectClass *object_class = NULL;
    LgwResultsTextViewClassPrivate *klasspriv = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->constructed = lgw_resultstextview_constructed;
    object_class->finalize = lgw_resultstextview_finalize;
    klasspriv = g_new0 (LgwResultsTextViewClassPrivate, 1);

    klasspriv->text_tag_table = lgw_texttagtable_new ();

    g_type_class_add_private (object_class, sizeof (LgwResultsTextViewPrivate));
    klass->priv = klasspriv;

/*
    klass->signalid[GW_ADDVOCABULARYWINDOW_CLASS_SIGNALID_WORD_ADDED] = g_signal_new (
        "word-added",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (GwAddVocabularyWindowClass, word_added),
        NULL, NULL,
        g_cclosure_marshal_VOID__VOID,
        G_TYPE_NONE, 0
    );
*/
}

