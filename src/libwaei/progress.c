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
//!  @file progress.c
//!
//!  @brief LwDictionary objects represent a loaded dictionary that the program
//!         can use to carry out searches.  You can uninstall dictionaries
//!         by using the object, but you cannot install them. LwDictInst
//!         objects exist for that purpose.
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <glib.h>
#include <glib/gstdio.h>

#include <libwaei/gettext.h>
#include <libwaei/libwaei.h>

#include <libwaei/dictionary-private.h>



LwProgress* 
lw_progress_new (GCancellable       *cancellable,
                 LwProgressCallback  cb, 
                 gpointer            data)
{
    //Initializations
    LwProgress *temp = NULL;

    temp = g_new0 (LwProgress, 1); if (temp == NULL) goto errored;
  
    temp->cb = cb;
    temp->data = data;
    temp->current_progress = -1.0;
    temp->total_progress = -1.0;
    temp->ratio_delta = 0.0;
    temp->required_ratio_delta = 0.01;
    temp->reached_ratio_delta = FALSE;
    temp->current_step = -1;
    temp->total_steps = -1;
    temp->finished = FALSE;
    temp->start_time = g_get_monotonic_time ();
    temp->cancellable = cancellable; if (cancellable != NULL) g_object_ref_sink (cancellable);

    g_mutex_init (&temp->mutex);

    return temp;

errored:

    lw_progress_free (temp); temp = NULL;

    return temp;
}


void 
lw_progress_free (LwProgress *progress) 
{
    if (progress == NULL) return; 

    if (progress->job_title != NULL) g_free (progress->job_title);
    if (progress->primary_message != NULL) g_free (progress->primary_message);
    if (progress->secondary_message != NULL) g_free (progress->secondary_message); 
    if (progress->step_message != NULL) g_free (progress->step_message); 
    if (progress->units != NULL) g_free (progress->units);
    if (progress->source_filename != NULL) g_free (progress->source_filename);
    if (progress->target_filename != NULL) g_free (progress->target_filename);


    if (progress->error != NULL) g_error_free (progress->error);
    if (progress->cancellable != NULL) g_object_unref (progress->cancellable); 

    g_mutex_clear (&progress->mutex);

    memset(progress, 0, sizeof(LwProgress));

    g_free (progress);
}


void
lw_progress_set_fraction (LwProgress *progress,
                          gdouble     current_progress,
                          gdouble     total_progress)
{
    //Sanity checks
    g_return_if_fail (progress != NULL);

    g_mutex_lock (&progress->mutex);

    if (progress->current_progress != current_progress ||
        progress->total_progress != total_progress)
    {
      progress->current_progress = current_progress;
      if (progress->total_progress != total_progress) progress->previous_progress = current_progress;
      progress->total_progress = total_progress;
      progress->ratio_delta = fabs((progress->previous_progress / progress->total_progress) - (progress->current_progress / progress->total_progress));
      progress->reached_ratio_delta = (progress->current_progress == 0 || (progress->current_progress / progress->total_progress == 1.0) || progress->ratio_delta > progress->required_ratio_delta);
      progress->changed = TRUE;
    }

    g_mutex_unlock (&progress->mutex);
}


void
lw_progress_set_required_ratio_delta (LwProgress *progress,
                                             gdouble     delta)
{
  //Sanity checks
  g_return_if_fail (progress != NULL);

  progress->required_ratio_delta = delta;
}


void
lw_progress_set_object (LwProgress *progress, 
                        gpointer    object)
{
    //Sanity checks
    g_return_if_fail (progress != NULL);

    g_mutex_lock (&progress->mutex);

    if (progress->object != object) progress->changed = TRUE;
    progress->object = object;

    g_mutex_unlock (&progress->mutex);
}


gboolean
lw_progress_changed (LwProgress *progress)
{
    //Sanity checks
    g_return_val_if_fail (progress != NULL, FALSE);

    g_mutex_lock (&progress->mutex);

    gboolean changed = (progress->changed && progress->reached_ratio_delta);

    g_mutex_unlock (&progress->mutex);

    return changed;
}


void
lw_progress_clear_changed (LwProgress *progress)
{
    //Sanity checks
    g_return_if_fail (progress != NULL);

    g_mutex_lock (&progress->mutex);

    progress->changed = FALSE;
    progress->previous_progress = progress->current_progress;
    progress->reached_ratio_delta = FALSE;

    g_mutex_unlock (&progress->mutex);
}


void
lw_progress_run_callback (LwProgress *progress)
{
    //Sanity checks
    g_return_if_fail (progress != NULL);

    g_mutex_lock (&progress->mutex);

    if (progress->cb != NULL)
    {
      progress->cb (progress->object, progress, progress->data);
    }

    g_mutex_unlock (&progress->mutex);
}


gdouble
lw_progress_get_fraction (LwProgress *progress)
{
    //Sanity checks
    g_return_if_fail (progress != NULL);

    g_mutex_lock (&progress->mutex);

    gdouble fraction = progress->current_progress / progress->total_progress;
    if (progress->finished) fraction = 1.0;

    g_mutex_unlock (&progress->mutex);

    return fraction;
}


void
lw_progress_set_finished (LwProgress *progress,
                          gboolean    finished)
{
    //Sanity checks
    g_return_if_fail (progress != NULL);

    g_mutex_lock (&progress->mutex);
    
    if (progress->finished != finished)
    {
      progress->finished = finished;
      progress->changed = TRUE;
    }

    g_mutex_unlock (&progress->mutex);
}


gboolean
lw_progress_is_finished (LwProgress *progress)
{
    //Sanity checks
    g_return_val_if_fail (progress != NULL, FALSE);

    g_mutex_lock (&progress->mutex);

    gboolean finished = progress->finished;

    g_mutex_unlock (&progress->mutex);

    return finished;
}


gboolean
lw_progress_errored (LwProgress *progress)
{
    //Sanity checks
    g_return_val_if_fail (progress != NULL, FALSE);

    g_mutex_lock (&progress->mutex);

    gboolean errored = (progress->error != NULL);

    g_mutex_unlock (&progress->mutex);

    return errored;
}


gboolean
lw_progress_is_cancelled (LwProgress *progress)
{
    //Sanity checks
    g_return_val_if_fail (progress != NULL, FALSE);

    g_mutex_lock (&progress->mutex);

    gboolean is_cancelled = (progress->cancellable != NULL && g_cancellable_is_cancelled (progress->cancellable));

    g_mutex_unlock (&progress->mutex);

    return is_cancelled;
}


void
lw_progress_cancel (LwProgress *progress)
{
    //Sanity checks
    g_return_if_fail (progress != NULL);

    g_mutex_lock (&progress->mutex);

    if (progress->cancellable != NULL) 
    {
      g_cancellable_cancel (progress->cancellable);
    }

    g_mutex_unlock (&progress->mutex);
}


gboolean 
lw_progress_should_abort (LwProgress *progress)
{
    //Sanity checks
    g_return_val_if_fail (progress != NULL, FALSE);

    return (lw_progress_is_cancelled (progress) || lw_progress_errored (progress));
}


void
lw_progress_set_primary_message (LwProgress  *progress, 
                                 const gchar *FORMAT, 
                                 ...)
{
    //Sanity checks
    g_return_if_fail (progress != NULL);
    g_return_if_fail (FORMAT != NULL);

    g_mutex_lock (&progress->mutex);

    //Declarations
    va_list args;
    gchar *message = NULL;

    va_start (args, FORMAT);

    //Initializations
    message = g_strdup_vprintf (FORMAT, args); if (message == NULL) goto errored;
    if (progress->primary_message == NULL || strcmp(progress->primary_message, message) != 0) progress->changed = TRUE;
    if (progress->primary_message != NULL) g_free (progress->primary_message);
    progress->primary_message = message; message = NULL;

errored:

    va_end (args);

    if (message != NULL) g_free (message); message = NULL;

    g_mutex_unlock (&progress->mutex);
}


const gchar*
lw_progress_get_primary_message (LwProgress *progress)
{
    //Sanity checks
    g_return_if_fail (progress != NULL);

    g_mutex_lock (&progress->mutex);

    gchar *primary_message = progress->primary_message;
    if (primary_message == NULL) primary_message = "";

    g_mutex_unlock (&progress->mutex);

    return primary_message;
}


void
lw_progress_set_secondary_message (LwProgress  *progress, 
                                   const gchar *FORMAT,
                                   ...)
{
    //Sanity checks
    g_return_if_fail (progress != NULL);
    g_return_if_fail (FORMAT != NULL);

    g_mutex_lock (&progress->mutex);

    //Declarations
    va_list args;
    gchar *message = NULL;

    va_start (args, FORMAT);

    message = g_strdup_vprintf (FORMAT, args); if (message == NULL) goto errored;
    if (progress->secondary_message == NULL || strcmp(progress->secondary_message, message) != 0) progress->changed = TRUE;
    if (progress->secondary_message != NULL) g_free (progress->secondary_message);
    progress->secondary_message = message; message = NULL;

errored:

    va_end (args);

    if (message != NULL) g_free (message); message = NULL;

    g_mutex_unlock (&progress->mutex);
}


const gchar*
lw_progress_get_secondary_message (LwProgress *progress)
{
    //Sanity checks
    g_return_if_fail (progress != NULL);

    g_mutex_lock (&progress->mutex);

    gchar *secondary_message = progress->secondary_message;
    if (secondary_message == NULL) secondary_message = "";

    g_mutex_unlock (&progress->mutex);

    return secondary_message;
}


void
lw_progress_set_step_message (LwProgress  *progress,
                              const gchar *FORMAT,
                              gint         current_step,
                              gint         total_steps)
{
    //Sanity checks
    g_return_if_fail (progress != NULL);
    g_return_if_fail (FORMAT != NULL);

    g_mutex_lock (&progress->mutex);

    //Declarations
    gchar *message = NULL;

    progress->current_step = current_step;
    progress->total_steps = total_steps;
    message = g_strdup_printf (FORMAT, current_step + 1, total_steps); if (message == NULL) goto errored;
    if (progress->step_message == NULL || strcmp(message, progress->step_message) != 0) progress->changed = TRUE;
    if (progress->step_message != NULL) g_free (progress->step_message);
    progress->step_message = message; message = NULL;

errored:

    if (message != NULL) g_free (message); message = NULL;

    g_mutex_unlock (&progress->mutex);
}


const gchar*
lw_progress_get_step_message (LwProgress *progress)
{
    //Sanity checks
    g_return_if_fail (progress != NULL);

    g_mutex_lock (&progress->mutex);

    gchar *step_message = progress->step_message;
    if (step_message == NULL) step_message = "";

    g_mutex_unlock (&progress->mutex);

    return step_message;
}

