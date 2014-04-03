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
//!  @file deletevocabularylistdialog-callbacks.c
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


void
lgw_deletevocabularylistdialog_response_cb (LgwDeleteVocabularyListDialog *self,
                                            gint                           response_id,
                                            GtkDialog                     *dialog)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_DELETEVOCABULARYLISTDIALOG (self));

    //Declarations
    LgwDeleteVocabularyListDialogPrivate *priv = NULL;

    //Initializations

errored:

    printf("BREAK response is %d\n", response_id);
}


void
lgw_deletevocabularylistdialog_connect_signals (LgwDeleteVocabularyListDialog *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_DELETEVOCABULARYLISTDIALOG (self));

    //Declarations
    LgwDeleteVocabularyListDialogPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (priv->data.signalid[SIGNALID_RESPONSE] == 0)
    {
        priv->data.signalid[SIGNALID_RESPONSE] = g_signal_connect_swapped (
          G_OBJECT (self),
          "response",
          G_CALLBACK (lgw_deletevocabularylistdialog_response_cb),
          self
        );
    }
}


void
lgw_deletevocabularylistdialog_disconnect_signals (LgwDeleteVocabularyListDialog *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_DELETEVOCABULARYLISTDIALOG (self));

    //Declarations
    LgwDeleteVocabularyListDialogPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (priv->data.signalid[SIGNALID_RESPONSE] != 0)
    {
      g_signal_handler_disconnect (
        G_OBJECT (self),
        priv->data.signalid[SIGNALID_RESPONSE]
      );
      priv->data.signalid[SIGNALID_RESPONSE] = 0;
    }
}

