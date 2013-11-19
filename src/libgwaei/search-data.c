/******************************************************************************
    AUTHOR:
    KanjiPad - Japanese handwriting recognition front end
    Copyright (C) 1997 Owen Taylor
    File heavily modified and updated by Zachary Dovel.

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
//! @file search-data.c
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

#include <gwaei/gwaei.h>
#include <gwaei/gettext.h>


GwSearchData* 
gw_searchdata_new (GtkTextView *view, GwSearchWindow *window)
{
    //Sanity checks
    g_return_val_if_fail (view != NULL, NULL);
    g_return_val_if_fail (window != NULL, NULL);

    GwSearchData *temp;
    temp = g_new0 (GwSearchData, 1);

    if (temp != NULL)
    {
      temp->window = window;
      temp->view = view;
      temp->result = NULL;

      temp->cursor_position = gw_searchwindow_entry_get_position (window);
    }

    return temp;
}


void 
gw_searchdata_free (GwSearchData *data)
{
    if (data == NULL) return;

    if (data->result != NULL) lw_result_free (data->result);
    memset(data, 0, sizeof(GwSearchData));

    free (data);
}


void 
gw_searchdata_set_result (GwSearchData *data, LwResult *result)
{
    g_assert (data != NULL);

    if (data->result != NULL) 
      lw_result_free (data->result);
    data->result = result;
}

LwResult* 
gw_searchdata_get_result (GwSearchData *data)
{
  return data->result;
}


