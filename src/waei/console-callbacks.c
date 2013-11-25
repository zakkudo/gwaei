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


void
w_console_progress_cb (LwDictionary *dictionary, 
                       LwProgress   *progress, 
                       WApplication *application)
{
    //Sanity checks
    g_return_if_fail (dictionary != NULL);
    g_return_if_fail (progress != NULL);
    g_return_if_fail (application != NULL);

    if (progress->primary_message != NULL)
    {
      fprintf(stderr, progress->primary_message);
      fprintf(stderr, "\n");
      g_free (progress->primary_message); progress->primary_message = NULL;
    }

    //Initializations
    if (progress->changed && progress->reached_ratio_delta && progress->secondary_message != NULL)
    {
      gdouble fraction = progress->current_progress / progress->total_progress;
      gint percent = (gint) (100.0 * fraction);

      fprintf(stderr, "\r    [%3d%%] %s", percent, progress->secondary_message); 

      if (fraction == 1.0) fprintf(stderr, "\n");

      fflush(stdout);

      progress->changed = FALSE;
      progress->previous_progress = progress->current_progress;
      progress->reached_ratio_delta = FALSE;
    }
}


gboolean 
w_console_append_result_timeout (gpointer data)
{
  //Sanity checks
  g_return_val_if_fail (data != NULL, FALSE);

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
      w_console_no_result (sdata->application, iterator);
    }
    g_main_loop_quit (sdata->loop);
    keep_appending = FALSE;
  }
  else
  {
    while (lw_searchiterator_next (iterator))
    {
      w_console_append_result (sdata->application, iterator);
    }
    keep_appending = TRUE;
  }

  return keep_appending;
}

