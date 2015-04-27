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

/**
 * SECTION:progress
 * @short_description: An object to track task progress and errors
 * @title: LwProgress
 *
 * This object provides a standardized threadsave interface for methods to
 * provide progress information to a ui.  This includes percent, primary
 * messages, secondary messages etc.  Methods using this object can also pass
 * errors or recieve cancellation requests through the same object.
 */

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

#include <libwaei/progress-private.h>

G_DEFINE_TYPE (LwProgress, lw_progress, G_TYPE_OBJECT)


/**
 * lw_progress_new:
 *
 * Creates a new #LwProgress object.
 *
 * Returns: a new #LwProgress object that can be freed with g_object_unref()
 */
LwProgress* 
lw_progress_new ()
{
    //Declarations
    LwProgress *self = NULL;
    GCancellable *cancellable = NULL;

    //Initializations
    cancellable = g_cancellable_new ();
    self = LW_PROGRESS (g_object_new (LW_TYPE_PROGRESS, "cancellable", cancellable, NULL));

    return self;
}


static void 
lw_progress_init (LwProgress *self)
{
    self->priv = LW_PROGRESS_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LwProgressPrivate));

    //Declarations
    LwProgressPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    priv->data.current_progress = -1.0;
    priv->data.total_progress = -1.0;
    priv->data.ratio_delta = 0.0;
    priv->data.complete = FALSE;
    priv->data.start_time = g_get_monotonic_time ();

    priv->config.required_ratio_delta = 0.01;

    g_mutex_init (&priv->data.mutex);
}


static void 
lw_progress_set_property (GObject        *object,
                            guint         property_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
    //Declarations
    LwProgress *self = NULL;
    LwProgressPrivate *priv = NULL;

    //Initializations
    self = LW_PROGRESS (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_ERROR:
        lw_progress_set_error (self, g_value_get_object (value));
        break;
      case PROP_CANCELLABLE:
        lw_progress_set_cancellable (self, g_value_get_object (value));
        break;
      case PROP_PRIMARY_MESSAGE:
        lw_progress_set_primary_message (self, g_value_get_string (value));
        break;
      case PROP_SECONDARY_MESSAGE:
        lw_progress_set_secondary_message (self, g_value_get_string (value));
        break;
      case PROP_STEP_MESSAGE:
        lw_progress_set_step_message (self, g_value_get_string (value));
        break;
      case PROP_COMPLETED:
        lw_progress_set_completed (self, g_value_get_boolean (value));
        break;
      case PROP_CURRENT_PROGRESS:
        lw_progress_set_current (self, g_value_get_double (value));
        break;
      case PROP_TOTAL_PROGRESS:
        lw_progress_set_total (self, g_value_get_double (value));
        break;
      case PROP_CHUNK_SIZE:
        lw_progress_set_chunk_size (self, g_value_get_ulong (value));
        break;
      case PROP_PREFERED_CHUNK_SIZE:
        lw_progress_set_prefered_chunk_size (self, g_value_get_ulong (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_progress_get_property (GObject    *object,
                          guint       property_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
    //Declarations
    LwProgress *self = NULL;
    LwProgressPrivate *priv = NULL;

    //Initializations
    self = LW_PROGRESS (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_ERROR:
        g_value_set_object (value, lw_progress_get_error (self));
        break;
      case PROP_ERRORED:
        g_value_set_boolean (value, lw_progress_errored (self));
        break;
      case PROP_CANCELLABLE:
        g_value_set_object (value, lw_progress_get_cancellable (self));
        break;
      case PROP_CANCELLED:
        g_value_set_boolean (value, lw_progress_is_cancelled (self));
        break;
      case PROP_PRIMARY_MESSAGE:
        g_value_set_string (value, lw_progress_get_primary_message (self));
        break;
      case PROP_SECONDARY_MESSAGE:
        g_value_set_string (value, lw_progress_get_secondary_message (self));
        break;
      case PROP_STEP_MESSAGE:
        g_value_set_string (value, lw_progress_get_step_message (self));
        break;
      case PROP_COMPLETED:
        g_value_set_boolean (value, lw_progress_completed (self));
        break;
      case PROP_CURRENT_PROGRESS:
        g_value_set_double (value, lw_progress_get_current (self));
        break;
      case PROP_TOTAL_PROGRESS:
        g_value_set_double (value, lw_progress_get_total (self));
        break;
      case PROP_PROGRESS_FRACTION:
        g_value_set_double (value, lw_progress_get_fraction (self));
        break;
      case PROP_CHUNK_SIZE:
        g_value_set_ulong (value, lw_progress_get_chunk_size (self));
        break;
      case PROP_PREFERED_CHUNK_SIZE:
        g_value_set_ulong (value, lw_progress_get_prefered_chunk_size (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_progress_finalize (GObject *object)
{
    //Declarations
    LwProgress *self = NULL;
    LwProgressPrivate *priv = NULL;

    //Initalizations
    self = LW_PROGRESS (object);
    priv = self->priv;

    if (priv->data.primary_message != NULL) g_free (priv->data.primary_message);
    if (priv->data.secondary_message != NULL) g_free (priv->data.secondary_message); 
    if (priv->data.step_message != NULL) g_free (priv->data.step_message); 
    if (priv->data.units != NULL) g_free (priv->data.units);

    if (priv->data.error != NULL) g_error_free (priv->data.error);
    if (priv->data.cancellable != NULL) g_object_unref (priv->data.cancellable); 

    g_mutex_clear (&priv->data.mutex);

    G_OBJECT_CLASS (lw_progress_parent_class)->finalize (object);
}


static void
lw_progress_dispose (GObject *object)
{
    //Declarations
    LwProgress *self = NULL;

    //Initializations
    self = LW_PROGRESS (object);

    G_OBJECT_CLASS (lw_progress_parent_class)->dispose (object);
}


static void
lw_progress_class_init (LwProgressClass *klass)
{
    //Declarations
    GObjectClass *object_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    klass->priv = g_new0 (LwProgressClassPrivate, 1);
    object_class->set_property = lw_progress_set_property;
    object_class->get_property = lw_progress_get_property;
    object_class->dispose = lw_progress_dispose;
    object_class->finalize = lw_progress_finalize;

    g_type_class_add_private (object_class, sizeof (LwProgressPrivate));

    LwProgressClassPrivate *klasspriv = klass->priv;

    klasspriv->signalid[CLASS_SIGNALID_PROGRESS_CHANGED] = g_signal_new (
        "self-changed",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwProgressClass, changed),
        NULL, NULL,
        g_cclosure_marshal_VOID__VOID,
        G_TYPE_NONE, 0
    );

    klasspriv->pspec[PROP_ERROR] = g_param_spec_boxed (
        "error",
        "FIlename construct prop",
        "Set the filename",
        G_TYPE_ERROR,
        G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_ERROR, klasspriv->pspec[PROP_ERROR]);

    klasspriv->pspec[PROP_ERRORED] = g_param_spec_boolean (
        "errored",
        "FIlename construct prop",
        "Set the filename",
        FALSE,
        G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_ERRORED, klasspriv->pspec[PROP_ERRORED]);

    klasspriv->pspec[PROP_CANCELLABLE] = g_param_spec_object (
        "cancellable",
        "FIlename construct prop",
        "Set the filename",
        G_TYPE_CANCELLABLE,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_CANCELLABLE, klasspriv->pspec[PROP_CANCELLABLE]);

    klasspriv->pspec[PROP_CANCELLED] = g_param_spec_boolean (
        "cancelled",
        "FIlename construct prop",
        "Set the filename",
        FALSE,
        G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_CANCELLED, klasspriv->pspec[PROP_CANCELLED]);

    klasspriv->pspec[PROP_PRIMARY_MESSAGE] = g_param_spec_string (
        "primary-message",
        "FIlename construct prop",
        "Set the filename",
        "",
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_PRIMARY_MESSAGE, klasspriv->pspec[PROP_PRIMARY_MESSAGE]);

    klasspriv->pspec[PROP_SECONDARY_MESSAGE] = g_param_spec_string (
        "secondary-message",
        "changed construct prop",
        "Set the changed",
        "",
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_SECONDARY_MESSAGE, klasspriv->pspec[PROP_SECONDARY_MESSAGE]);

    klasspriv->pspec[PROP_COMPLETED] = g_param_spec_boolean (
        "completed",
        "changed construct prop",
        "Set the changed",
        FALSE,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_COMPLETED, klasspriv->pspec[PROP_COMPLETED]);

    klasspriv->pspec[PROP_CURRENT_PROGRESS] = g_param_spec_double (
        "current-progress",
        "changed construct prop",
        "Set the changed",
        0.0,
        G_MAXDOUBLE,
        0.0,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_CURRENT_PROGRESS, klasspriv->pspec[PROP_CURRENT_PROGRESS]);

    klasspriv->pspec[PROP_TOTAL_PROGRESS] = g_param_spec_double (
        "total-progress",
        "changed construct prop",
        "Set the changed",
        0.0,
        G_MAXDOUBLE,
        0.0,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_TOTAL_PROGRESS, klasspriv->pspec[PROP_TOTAL_PROGRESS]);

    klasspriv->pspec[PROP_PROGRESS_FRACTION] = g_param_spec_double (
        "progress-fraction",
        "changed construct prop",
        "Set the changed",
        0.0,
        G_MAXDOUBLE,
        0.0,
        G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_PROGRESS_FRACTION, klasspriv->pspec[PROP_PROGRESS_FRACTION]);

    klasspriv->pspec[PROP_CHUNK_SIZE] = g_param_spec_ulong (
        "chunk-size",
        "chunk size for transactions",
        "Set the changed",
        0,
        G_MAXSIZE,
        lw_io_get_pagesize (),
        G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_CHUNK_SIZE, klasspriv->pspec[PROP_CHUNK_SIZE]);

    klasspriv->pspec[PROP_PREFERED_CHUNK_SIZE] = g_param_spec_ulong (
        "prefered-chunk-size",
        "chunk size for transactions",
        "Set the changed",
        0,
        G_MAXSIZE,
        lw_io_get_pagesize (),
        G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_PREFERED_CHUNK_SIZE, klasspriv->pspec[PROP_PREFERED_CHUNK_SIZE]);
}


void
lw_progress_set_cancellable (LwProgress   *self,
                             GCancellable *cancellable)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PROGRESS (self));

    //Declarations
    LwProgressPrivate *priv = NULL;
    LwProgressClass *klass = NULL;
    LwProgressClassPrivate *klasspriv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    klass = LW_PROGRESS_GET_CLASS (self);
    klasspriv = klass->priv;
    changed = (cancellable != priv->data.cancellable);

    if (cancellable != NULL)
    {
      g_object_ref (cancellable);
    }

    if (priv->data.cancellable != NULL)
    {
      g_object_remove_weak_pointer (
        G_OBJECT (priv->data.cancellable),
        (gpointer*) &(priv->data.cancellable)
      );
      g_object_unref (priv->data.cancellable);
    }

    priv->data.cancellable = cancellable;

    if (priv->data.cancellable != NULL)
    {
      g_object_add_weak_pointer (
        G_OBJECT (priv->data.cancellable),
        (gpointer*) &(priv->data.cancellable)
      );
    }

    if (changed)
    {
      g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_CANCELLABLE]);
    }
    
}


GCancellable*
lw_progress_get_cancellable (LwProgress *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_PROGRESS (self), NULL);

    //Declarations
    LwProgressPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.cancellable;
}


gdouble
lw_progress_get_current (LwProgress *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_PROGRESS (self), 0);

    //Declarations
    LwProgressPrivate *priv = NULL;
    gdouble current_progress = 0.0;

    //Initializations
    priv = self->priv;

    g_mutex_lock (&priv->data.mutex);

    current_progress = priv->data.current_progress;

    g_mutex_unlock (&priv->data.mutex);

    return current_progress;
}
                         

void
lw_progress_set_current (LwProgress *self,
                         gdouble     current_progress)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PROGRESS (self));

    //Declarations
    LwProgressPrivate *priv = NULL;
    LwProgressClass *klass = NULL;
    LwProgressClassPrivate *klasspriv = NULL;
    gboolean changed = FALSE;
    gdouble epsilon = 0.0000001;
    gboolean is_locked = FALSE;

    //Initializations
    priv = self->priv;
    klass = LW_PROGRESS_GET_CLASS (self);
    klasspriv = klass->priv;

    g_mutex_lock (&priv->data.mutex);
    is_locked = TRUE;

    if (current_progress < 0)
    {
      current_progress = 0;
    }
    if (current_progress > priv->data.total_progress)
    {
      current_progress = priv->data.total_progress;
    }

    changed = (fabs(priv->data.current_progress - current_progress) > epsilon);
    if (!changed) goto errored;

    priv->data.current_progress = current_progress;

errored:

    if (is_locked)
    {
      g_mutex_unlock (&priv->data.mutex);
      is_locked = FALSE;
    }

    if (changed) 
    {
      g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_CURRENT_PROGRESS]);
      lw_progress_sync_ratio_delta (self);
    }

    return;
}


gboolean
lw_progress_get_total (LwProgress *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_PROGRESS (self), NULL);

    //Declarations
    LwProgressPrivate *priv = NULL;
    gdouble total_progress = 0.0;

    //Initializations
    priv = self->priv;

    g_mutex_lock (&priv->data.mutex);

    total_progress = priv->data.total_progress;

    g_mutex_unlock (&priv->data.mutex);

    return total_progress;
}


void
lw_progress_set_total (LwProgress *self,
                       gdouble     total_progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_PROGRESS (self), NULL);

    //Declarations
    LwProgressPrivate *priv = NULL;
    LwProgressClass *klass = NULL;
    LwProgressClassPrivate *klasspriv = NULL;
    gboolean changed = FALSE;
    gboolean sync_current_progress = FALSE;
    gdouble epsilon = 0.0000001;

    //Initializations
    priv = self->priv;
    klass = LW_PROGRESS_GET_CLASS (self);
    klasspriv = klass->priv;

    if (total_progress < 0.0)
    {
      total_progress = 0.0;
      sync_current_progress = TRUE;
    }

    priv->data.total_progress = total_progress;

    if (priv->data.current_progress > priv->data.total_progress)
    {
      sync_current_progress = TRUE;
    }

    changed = (fabs(priv->data.total_progress - total_progress) > epsilon);
    if (!changed) goto errored;

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_TOTAL_PROGRESS]);

    if (sync_current_progress)
    {
      lw_progress_set_current (self, priv->data.total_progress);
    }

    lw_progress_sync_ratio_delta (self);

errored:

    return;
}


gdouble
lw_progress_get_fraction (LwProgress *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PROGRESS (self));

    //Declarations
    LwProgressPrivate *priv = NULL;
    gdouble fraction = 0.0;

    //Initializations
    priv = self->priv;

    g_mutex_lock (&priv->data.mutex);

    fraction = priv->data.current_progress / priv->data.total_progress;
    if (priv->data.complete) fraction = 1.0;

    g_mutex_unlock (&priv->data.mutex);

    return fraction;
}


void
lw_progress_sync_ratio_delta (LwProgress *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PROGRESS (self));

    //Declarations
    LwProgressPrivate *priv = NULL;
    LwProgressClass *klass = NULL;
    LwProgressClassPrivate *klasspriv = NULL;
    gboolean ratio_reached = FALSE;
    gdouble previous_fraction = 0.0;
    gdouble current_progress = 0.0;
    gdouble delta = 0.0;
    gboolean reached = FALSE;

    //Initializations
    priv = self->priv;
    klass = LW_PROGRESS_GET_CLASS (self);
    klasspriv = klass->priv;

    if (priv->data.total_progress != 0)
    {
      previous_fraction = (priv->data.previous_progress / priv->data.total_progress);
      current_progress = (priv->data.current_progress / priv->data.total_progress);
      delta = fabs(previous_fraction - current_progress);
    }

    reached = (
        delta > priv->config.required_ratio_delta ||
        priv->data.current_progress == 0.0 ||
        priv->data.current_progress == priv->data.total_progress
    );
    if (!reached) goto errored;

    g_signal_emit (
      G_OBJECT (self),
      klasspriv->signalid[CLASS_SIGNALID_PROGRESS_CHANGED], 
      0
    );

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_PROGRESS_FRACTION]);

errored:

    return;
}


void
lw_progress_set_required_ratio_delta (LwProgress *self,
                                      gdouble     delta)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PROGRESS (self));

    //Declarations
    LwProgressPrivate *priv = NULL;
    LwProgressClass *klass = NULL;
    LwProgressClassPrivate *klasspriv = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_PROGRESS_GET_CLASS (self);
    klasspriv = klass->priv;

    priv->config.required_ratio_delta = delta;

    g_signal_emit (
      G_OBJECT (self),
      klasspriv->signalid[CLASS_SIGNALID_PROGRESS_CHANGED], 
      0
    );
}


void
lw_progress_set_completed (LwProgress *self,
                          gboolean    complete)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PROGRESS (self));

    //Declarations
    LwProgressPrivate *priv = NULL;
    LwProgressClass *klass = NULL;
    LwProgressClassPrivate *klasspriv = NULL;
    gboolean changed = FALSE;
    gboolean is_locked = FALSE;

    //Initializations
    priv = self->priv;
    klass = LW_PROGRESS_GET_CLASS (self);
    klasspriv = klass->priv;

    g_mutex_lock (&priv->data.mutex);
    is_locked = TRUE;

    changed = (priv->data.complete != complete);
    if (!changed) goto errored;
    
    priv->data.complete = complete;

errored:

    if (is_locked) g_mutex_unlock (&self->priv->data.mutex);
    if (changed) g_signal_emit (
      G_OBJECT (self),
      klasspriv->signalid[CLASS_SIGNALID_PROGRESS_CHANGED], 
      0
    );

    return;
}


gboolean
lw_progress_completed (LwProgress *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PROGRESS (self));

    //Declarations
    LwProgressPrivate *priv = NULL;
    gboolean complete = FALSE;

    //Initializations
    priv = self->priv;

    g_mutex_lock (&priv->data.mutex);

    complete = priv->data.complete;

    g_mutex_unlock (&priv->data.mutex);

    return complete;
}


void
lw_progress_set_error (LwProgress *self,
                       GError     *_error)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PROGRESS (self));

    //Declarations
    LwProgressPrivate *priv = NULL;
    GError *error = NULL;

    //Initializations
    priv = self->priv;
    if (error != NULL && error != _error)
    {
      error = g_error_copy (_error);
    }

    lw_progress_take_error (self, error);
}


void
lw_progress_take_error (LwProgress *self,
                        GError     *error)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PROGRESS (self));

    //Declarations
    LwProgressPrivate *priv = NULL;
    LwProgressClass *klass = NULL;
    LwProgressClassPrivate *klasspriv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    klass = LW_PROGRESS_GET_CLASS (self);
    klasspriv = klass->priv;
    changed = (error != priv->data.error);
    if (!changed) goto errored;

    g_clear_error (&priv->data.error);
    priv->data.error = error;

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_ERROR]);

errored:
  
    return;
}


GError*
lw_progress_get_error (LwProgress  *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PROGRESS (self));

    //Declarations
    LwProgressPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.error;
}


/**
 * lw_progress_errored:
 *
 * Method to query if an error set set such as by lw_progress_set_error()
 * or lw_progress_take_error(), though lw_progress_should_abort() is usually
 * the prefered way to check if a task should abort.  lw_progress_clear()
 * will reset the cancelled state.  You can get the #GError though
 * lw_progress_get_error(). lw_progress_clear() will reset the errored state.
 *
 * Returns: #TRUE if the operation had an error set
 */
gboolean
lw_progress_errored (LwProgress *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_PROGRESS (self), FALSE);

    //Declarations
    LwProgressPrivate *priv = NULL;
    gboolean errored = FALSE;

    //Initialization
    priv = self->priv;

    g_mutex_lock (&priv->data.mutex);

    errored = (priv->data.error != NULL);

    g_mutex_unlock (&priv->data.mutex);

    return errored;
}


/**
 * lw_progress_is_cancelled:
 *
 * Used to query specifically if lw_progress_cancel() was called,
 * though lw_progress_should_abort() is usually the prefered way to
 * check if a task should abort.  lw_progress_clear() will reset
 * the cancelled state.
 *
 * Returns: #TRUE if the operation was cancelled
 */
gboolean
lw_progress_is_cancelled (LwProgress *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PROGRESS (self));

    //Declarations
    LwProgressPrivate *priv = NULL;
    gboolean is_cancelled = FALSE;

    //Initializations
    priv = self->priv;

    g_mutex_lock (&priv->data.mutex);

    is_cancelled = (priv->data.cancellable != NULL && g_cancellable_is_cancelled (priv->data.cancellable));

    g_mutex_unlock (&priv->data.mutex);

    return is_cancelled;
}


/**
 * lw_progress_cancel:
 *
 * Use to signal that a task should abort and return as soon as possible.
 * This method will usually be connected to a cancel button on a ui.
 * lw_progress_clear() will reset the errored state.
 */
void
lw_progress_cancel (LwProgress *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PROGRESS (self));

    //Declarations
    LwProgressPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    g_mutex_lock (&priv->data.mutex);

    if (priv->data.cancellable != NULL) 
    {
      g_cancellable_cancel (priv->data.cancellable);
    }

    g_mutex_unlock (&priv->data.mutex);
}

 

/**
 * lw_progress_should_abort:
 *
 * Provides an easy way to query a #LwProgress object if an error occured or
 * a cancellation request was initiated.  When either occur, this method will
 * start returning TRUE.  When this method returns TRUE, most methods should
 * stop their processing, cleanup and return. lw_progress_clear() can be used
 * to reset the errored/cancelled state.
 *
 * Returns: #TRUE if an error occured or if the task was cancelled.
 */
gboolean 
lw_progress_should_abort (LwProgress *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PROGRESS (self));

    //Declarations
    LwProgressPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return (lw_progress_is_cancelled (self) || lw_progress_errored (self));
}


void
lw_progress_set_primary_message (LwProgress *self,
                                 const gchar *MESSAGE)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PROGRESS (self));

    //Declarations
    LwProgressPrivate *priv = NULL;
    LwProgressClass *klass = NULL;
    LwProgressClassPrivate *klasspriv = NULL;
    gboolean changed = FALSE;
    gboolean is_locked = FALSE;

    //Initializations
    priv = self->priv;
    klass = LW_PROGRESS_GET_CLASS (self);
    klasspriv = klass->priv;

    g_mutex_lock (&self->priv->data.mutex);
    is_locked = TRUE;

    changed = (g_strcmp0 (MESSAGE, priv->data.primary_message) != 0);
    if (!changed) goto errored;

    g_free (priv->data.primary_message);
    priv->data.primary_message = g_strdup (MESSAGE);

errored:

    if (is_locked) g_mutex_unlock (&self->priv->data.mutex);
    if (changed) g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_PRIMARY_MESSAGE]);
}


void
lw_progress_set_primary_message_vprintf (LwProgress  *self,
                                         const gchar *FORMAT,
                                         va_list      va)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PROGRESS (self));
    g_return_if_fail (FORMAT != NULL);

    //Declarations
    LwProgressPrivate *priv = NULL;
    gchar *message = NULL;

    //Initializations
    priv = self->priv;
    message = g_strdup_vprintf (FORMAT, va); 
    if (message == NULL) goto errored;

    lw_progress_set_primary_message (self, message);

errored:

    if (message != NULL) g_free (message); message = NULL;
}


void
lw_progress_set_primary_message_printf (LwProgress  *self, 
                                        const gchar *FORMAT, 
                                        ...)
{
    //Declarations
    va_list va;

    //Initializations
    va_start(va, FORMAT);

    lw_progress_set_primary_message_vprintf (self, FORMAT, va);

    va_end (va);
}


const gchar*
lw_progress_get_primary_message (LwProgress *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PROGRESS (self));

    //Declarations
    LwProgressPrivate *priv = NULL;
    gchar *primary_message = NULL;

    //Initializations
    priv = self->priv;

    g_mutex_lock (&priv->data.mutex);

    primary_message = priv->data.primary_message;
    if (primary_message == NULL) primary_message = "";

    g_mutex_unlock (&priv->data.mutex);

    return primary_message;
}


void
lw_progress_set_secondary_message (LwProgress  *self,
                                   const gchar *MESSAGE)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    //Declarations
    LwProgressPrivate *priv = NULL;
    LwProgressClass *klass = NULL;
    LwProgressClassPrivate *klasspriv = NULL;
    gboolean changed = FALSE;
    gboolean is_locked = FALSE;

    //Initializations
    priv = self->priv;
    klass = LW_PROGRESS_GET_CLASS (self);
    klasspriv = klass->priv;

    g_mutex_lock (&self->priv->data.mutex);
    is_locked = TRUE;

    changed = (g_strcmp0 (MESSAGE, priv->data.secondary_message) != 0);
    if (!changed) goto errored;

    g_free (priv->data.secondary_message);
    priv->data.secondary_message = g_strdup (MESSAGE);

errored:

    if (is_locked) g_mutex_unlock (&self->priv->data.mutex);
    if (changed) g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_SECONDARY_MESSAGE]);

    return;
}


void
lw_progress_set_secondary_message_vprintf (LwProgress  *self,
                                           const gchar *FORMAT,
                                           va_list      va)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (FORMAT != NULL);

    //Declarations
    LwProgressPrivate *priv = NULL;
    gchar *message = NULL;

    //Initializations
    priv = self->priv;
    message = g_strdup_vprintf (FORMAT, va); 
    if (message == NULL) goto errored;

    lw_progress_set_secondary_message (self, message);
    
errored:

    if (message != NULL) g_free (message); message = NULL;
}


void
lw_progress_set_secondary_message_printf (LwProgress  *self, 
                                          const gchar *FORMAT,
                                          ...)
{
    //Declarations
    va_list va;

    //Initializations
    va_start(va, FORMAT);

    lw_progress_set_secondary_message_vprintf (self, FORMAT, va);

    va_end (va);
}


const gchar*
lw_progress_get_secondary_message (LwProgress *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PROGRESS (self));

    //Declarations
    LwProgressPrivate *priv = NULL;
    gchar *secondary_message = NULL;

    //Initializations
    priv = self->priv;

    g_mutex_lock (&self->priv->data.mutex);

    secondary_message = priv->data.secondary_message;
    if (secondary_message == NULL) secondary_message = "";

    g_mutex_unlock (&priv->data.mutex);

    return secondary_message;
}


void
lw_progress_set_step_message (LwProgress  *self,
                              const gchar *MESSAGE)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PROGRESS (self));

    //Declarations
    LwProgressPrivate *priv = NULL;
    LwProgressClass *klass = NULL;
    LwProgressClassPrivate *klasspriv = NULL;
    gboolean changed = FALSE;
    gboolean is_locked = FALSE;

    //Initializations
    priv = self->priv;
    klass = LW_PROGRESS_GET_CLASS (self);
    klasspriv = klass->priv;

    g_mutex_lock (&self->priv->data.mutex);
    is_locked = TRUE;

    changed = (g_strcmp0(MESSAGE, priv->data.step_message) != 0);
    if (changed == FALSE) goto errored;
    
    g_free (priv->data.step_message);
    priv->data.step_message = g_strdup (MESSAGE);

errored:

    if (is_locked) g_mutex_unlock (&self->priv->data.mutex);
    if (changed) g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_STEP_MESSAGE]);
  
    return;
}


void
lw_progress_set_step_message_vprintf (LwProgress  *self,
                                      const gchar *FORMAT,
                                      va_list      va)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PROGRESS (self));
    g_return_if_fail (FORMAT != NULL);

    //Declarations
    LwProgressPrivate *priv = NULL;
    gchar *message = NULL;

    //Initializations
    priv = self->priv;
    message = g_strdup_vprintf (FORMAT, va);
    if (message == NULL) goto errored;

    lw_progress_set_step_message (self, message);

errored:

    if (message != NULL) g_free (message); message = NULL;
}


void
lw_progress_set_step_message_printf (LwProgress  *self,
                                     const gchar *FORMAT,
                                     ...)
{
    //Declarations
    va_list va;

    //Initializations
    va_start(va, FORMAT);

    lw_progress_set_step_message_vprintf (self, FORMAT, va);

    va_end (va);
}


const gchar*
lw_progress_get_step_message (LwProgress *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PROGRESS (self));

    //Declarations
    LwProgressPrivate *priv = NULL;
    gchar *step_message = NULL;

    //Initializations
    priv = self->priv;

    g_mutex_lock (&self->priv->data.mutex);

    step_message = priv->data.step_message;
    if (step_message == NULL) step_message = "";

    g_mutex_unlock (&self->priv->data.mutex);

    return step_message;
}


gsize
lw_progress_get_prefered_chunk_size (LwProgress *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, 0);
    
    //Declarations
    LwProgressPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->config.prefered_chunk_size;
}


void
lw_progress_set_prefered_chunk_size (LwProgress *self,
                                     gsize       prefered_chunk_size)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, 0);
    
    //Declarations
    LwProgressPrivate *priv = NULL;
    LwProgressClass *klass = NULL;
    LwProgressClassPrivate *klasspriv = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_PROGRESS_GET_CLASS (self);
    klasspriv = klass->priv;

    if (priv->config.prefered_chunk_size == prefered_chunk_size) goto errored;

    priv->config.prefered_chunk_size = prefered_chunk_size;

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_PREFERED_CHUNK_SIZE]);

errored:

    return;
}


gsize
lw_progress_get_chunk_size (LwProgress *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, 0);
    
    //Declarations
    LwProgressPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.chunk_size;
}


void
lw_progress_set_chunk_size (LwProgress *self,
                            gsize       chunk_size)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, 0);
    
    //Declarations
    LwProgressPrivate *priv = NULL;
    LwProgressClass *klass = NULL;
    LwProgressClassPrivate *klasspriv = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_PROGRESS_GET_CLASS (self);
    klasspriv = klass->priv;

    if (priv->data.chunk_size == chunk_size) goto errored;

    priv->data.chunk_size = chunk_size;

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_CHUNK_SIZE]);

errored:

    return;
}
