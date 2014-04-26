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
//! @file console-callbacks.c
//!
//! @brief To be written
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <glib.h>

#include <waei/gettext.h>
#include <waei/waei.h>

#include <waei/command-private.h>


void
w_command_progress_cb (LwDictionary *dictionary, 
                       LwProgress   *progress, 
                       WCommand     *self)
{
  TODO
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY (dictionary));
    g_return_if_fail (progress != NULL);
    g_return_if_fail (W_IS_COMMAND (self));

    //Declarations
    WCommandPrivate *priv = NULL;
    GApplicationCommandLine *command_line = NULL;

    //Initializations
    priv = self->priv;
    command_line = priv->data.command_line;
    if (command_line == NULL) goto errored;

    if (progress->primary_message != NULL)
    {
      g_application_command_line_printerr (command_line, progress->primary_message);
      g_application_command_line_printerr (command_line, "\n");
      g_free (progress->primary_message); progress->primary_message = NULL;
    }

    //Initializations
    if (progress->changed && progress->reached_ratio_delta && progress->secondary_message != NULL)
    {
      gdouble fraction = progress->current_progress / progress->total_progress;
      gint percent = (gint) (100.0 * fraction);

      g_application_command_line_printerr (command_line, "\r    [%3d%%] %s", percent, progress->secondary_message); 

      if (fraction == 1.0) g_application_command_line_printerr (command_line, "\n");

      progress->changed = FALSE;
      progress->previous_progress = progress->current_progress;
      progress->reached_ratio_delta = FALSE;
    }

errored:

    return;
}


gboolean 
w_command_append_result_timeout (gpointer data)
{
  //Sanity checks
  g_return_val_if_fail (data != NULL, FALSE);
/*TODO
  //Declarations
  LwSearchIterator *iterator = NULL;
  LwSearch *search = NULL;
  LwSearchStatus status = 0;
  WSearchData *sdata = NULL;
  gboolean keep_appending = TRUE;

  //Initializations
  iterator = LW_SEARCHITERATOR (data);
  search = iterator->search;
  status = lw_search_get_status (search);
  sdata = W_SEARCHDATA (lw_search_get_data (search));

  if (lw_searchiterator_finished (iterator))
  {
    if (lw_searchiterator_empty (iterator))
    {
      w_command_no_result (sdata->application, iterator);
    }
    g_main_loop_quit (sdata->loop);
    keep_appending = FALSE;
  }
  else
  {
    while (lw_searchiterator_next (iterator))
    {
      w_command_append_result (sdata->application, iterator);
    }
    keep_appending = TRUE;
  }

  return keep_appending;
  */
  return FALSE;
}

