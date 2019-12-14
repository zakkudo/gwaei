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


void
w_command_progress_primary_message_changed_cb (WCommand   *self,
                                               GParamSpec *pspec,
                                               LwProgress *progress)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));
    g_return_if_fail (LW_IS_PROGRESS (progress));

    lw_command_print (LW_COMMAND (self), "%s\n", lw_progress_get_primary_message (progress));
}


void
w_command_progress_secondary_message_changed_cb (WCommand   *self,
                                                 GParamSpec *pspec,
                                                 LwProgress *progress)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));
    g_return_if_fail (LW_IS_PROGRESS (progress));

    //lw_command_print (LW_COMMAND (self), "BREAK w_command_progress_secondary_message_changed_cb: %s\n", lw_progress_get_secondary_message (progress));
}


void
w_command_progress_completed_changed_cb (WCommand   *self,
                                         GParamSpec *pspec,
                                         LwProgress *progress)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));
    g_return_if_fail (LW_IS_PROGRESS (progress));

    lw_command_print (LW_COMMAND (self), "BREAK w_command_progress_completed_changed_cb: %d\n", lw_progress_completed (progress));
}


void
w_command_progress_fraction_changed_cb (WCommand   *self,
                                        GParamSpec *pspec,
                                        LwProgress *progress)
{
    //Sanity checks
    g_return_if_fail (W_IS_COMMAND (self));
    g_return_if_fail (LW_IS_PROGRESS (progress));

    gint percent = (gint)(lw_progress_get_fraction (progress) * 100.0);

    if (percent >= 0)
    {
      lw_command_print (LW_COMMAND (self), "\r  %s [%d%%]", lw_progress_get_secondary_message (progress), percent);

      if (percent == 100)
      {
        lw_command_print (LW_COMMAND (self), "\n");
      }
    }
}
