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
 * @Title: LwProgress
 * @include: libwaei/progress.h
 *
 * This object provides a standardized threadsafe interface for methods to
 * provide progress information to a ui.  This includes percent, primary
 * messages, secondary messages etc.  Methods using this object can also pass
 * errors or receive cancellation requests through the same object.
 *
 * A simple sample program:
 * |[<!-- langauge="C" -->
 * void action_that_requires_progress (LwProgress * progress) {
 *    int i = 0;
 *    lw_progress_set_primary_message (progress, "Actioning...");
 *    lw_progress_set_total_progress (progress, 100);
 *    for (i = 0; i < 100; i++) {
 *       lw_progress_set_current (progress, i);
 *    }
 *    lw_progress_set_completed (progress, TRUE);
 * }
 *
 * void show_progress (LwProgress * progress, gpointer data) {
 *   if (lw_progress_get_current (progress) == 0.0) {
 *     printf("%s\n", lw_progress_get_primary_message (progress));
 *   }
 *   printf("%d completed...\n", (gint)(lw_progress_get_fraction (progress) * 100));
 *   if (lw_progress_completed (progress) {
 *     printf("DONE\n");
 *   }
 * }
 *
 * gint main (int argc, gchar *argv[]) {
 *   LwProgress * progress = lw_progress_new ()
 *   g_signal_connect (progress, "progress-changed", G_CALLBACK (show_progress), NULL);
 *   action_that_requires_progress (progress);
 *   g_object_unref (progress);
 * }
 * ]|
 *
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


static void lw_progress_set_cancellable (LwProgress * self, GCancellable * cancellable);
static GCancellable * lw_progress_get_cancellable (LwProgress * self);


/**
 * lw_progress_new:
 *
 * Creates a new #LwProgress object.
 *
 * Returns: (transfer full): A new #LwProgress object that can be freed with g_object_unref()
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
    g_object_unref (cancellable);

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

    priv->current_progress = 0.0;
    priv->total_progress = 0.0;
    priv->ratio_delta = 0.0;
    priv->completed = FALSE;
    priv->start_time = g_get_monotonic_time ();
    priv->chunk_size = lw_io_get_pagesize ();

    priv->required_ratio_delta = 0.001;

    g_mutex_init (&priv->mutex);
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

    if (priv->primary_message != NULL) g_free (priv->primary_message);
    if (priv->secondary_message != NULL) g_free (priv->secondary_message); 
    if (priv->step_message != NULL) g_free (priv->step_message); 
    if (priv->units != NULL) g_free (priv->units);

    if (priv->error != NULL) g_error_free (priv->error);
    if (priv->cancellable != NULL) g_object_unref (priv->cancellable); 

    g_mutex_clear (&priv->mutex);

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

    /**
     * LwProgress::progress-changed:
     * @widget: the widget that received the signal
     * @foo: some foo
     * @bar: some bar
     *
     * The ::foobarized signal is emitted each time someone tries to foobarize @widget.
     */
    klasspriv->signalid[CLASS_SIGNALID_PROGRESS_CHANGED] = g_signal_new (
        "progress-changed",
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
    /**
     * LwProgress:error:
     *
     * Here you can document a property.
     */
    g_object_class_install_property (object_class, PROP_ERROR, klasspriv->pspec[PROP_ERROR]);

    klasspriv->pspec[PROP_ERRORED] = g_param_spec_boolean (
        "errored",
        "FIlename construct prop",
        "Set the filename",
        FALSE,
        G_PARAM_READABLE
    );
    /**
     * LwProgress:errored:
     *
     * Here you can document a property.
     */
    g_object_class_install_property (object_class, PROP_ERRORED, klasspriv->pspec[PROP_ERRORED]);

    klasspriv->pspec[PROP_CANCELLABLE] = g_param_spec_object (
        "cancellable",
        "FIlename construct prop",
        "Set the filename",
        G_TYPE_CANCELLABLE,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    /**
     * LwProgress:cancellable:
     *
     * Here you can document a property.
     */
    g_object_class_install_property (object_class, PROP_CANCELLABLE, klasspriv->pspec[PROP_CANCELLABLE]);

    klasspriv->pspec[PROP_CANCELLED] = g_param_spec_boolean (
        "cancelled",
        "FIlename construct prop",
        "Set the filename",
        FALSE,
        G_PARAM_READABLE
    );
    /**
     * LwProgress:cancelled:
     *
     * Here you can document a property.
     */
    g_object_class_install_property (object_class, PROP_CANCELLED, klasspriv->pspec[PROP_CANCELLED]);

    klasspriv->pspec[PROP_PRIMARY_MESSAGE] = g_param_spec_string (
        "primary-message",
        "FIlename construct prop",
        "Set the filename",
        "",
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    /**
     * LwProgress:primary-message:
     *
     * Here you can document a property.
     */
    g_object_class_install_property (object_class, PROP_PRIMARY_MESSAGE, klasspriv->pspec[PROP_PRIMARY_MESSAGE]);

    klasspriv->pspec[PROP_SECONDARY_MESSAGE] = g_param_spec_string (
        "secondary-message",
        "changed construct prop",
        "Set the changed",
        "",
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    /**
     * LwProgress:secondary-message:
     *
     * Here you can document a property.
     */
    g_object_class_install_property (object_class, PROP_SECONDARY_MESSAGE, klasspriv->pspec[PROP_SECONDARY_MESSAGE]);

    klasspriv->pspec[PROP_STEP_MESSAGE] = g_param_spec_string (
        "step-message",
        "changed construct prop",
        "Set the changed",
        "",
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    /**
     * LwProgress:step-message:
     *
     * Here you can document a property.
     */
    g_object_class_install_property (object_class, PROP_STEP_MESSAGE, klasspriv->pspec[PROP_STEP_MESSAGE]);

    klasspriv->pspec[PROP_COMPLETED] = g_param_spec_boolean (
        "completed",
        "changed construct prop",
        "Set the changed",
        FALSE,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    /**
     * LwProgress:completed:
     *
     * Here you can document a property.
     */
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
    /**
     * LwProgress:current-progress:
     *
     * Here you can document a property.
     */
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
    /**
     * LwProgress:total-progress:
     *
     * Here you can document a property.
     */
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
    /**
     * LwProgress:progress-fraction:
     *
     * Here you can document a property.
     */
    g_object_class_install_property (object_class, PROP_PROGRESS_FRACTION, klasspriv->pspec[PROP_PROGRESS_FRACTION]);

    klasspriv->pspec[PROP_CHUNK_SIZE] = g_param_spec_ulong (
        "chunk-size",
        "chunk size for transactions",
        "Set the changed",
        0,
        G_MAXSIZE,
        0,
        G_PARAM_READWRITE
    );
    /**
     * LwProgress:chunk-size:
     *
     * Here you can document a property.
     */
    g_object_class_install_property (object_class, PROP_CHUNK_SIZE, klasspriv->pspec[PROP_CHUNK_SIZE]);

    klasspriv->pspec[PROP_PREFERED_CHUNK_SIZE] = g_param_spec_ulong (
        "prefered-chunk-size",
        "chunk size for transactions",
        "Set the changed",
        0,
        G_MAXSIZE,
        0,
        G_PARAM_READWRITE
    );
    /**
     * LwProgress:prefered-chunk-size:
     *
     * Here you can document a property.
     */
    g_object_class_install_property (object_class, PROP_PREFERED_CHUNK_SIZE, klasspriv->pspec[PROP_PREFERED_CHUNK_SIZE]);
}


static void
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
    changed = (cancellable != priv->cancellable);

    if (cancellable != NULL)
    {
      g_object_ref (cancellable);
    }

    if (priv->cancellable != NULL)
    {
      g_object_remove_weak_pointer (
        G_OBJECT (priv->cancellable),
        (gpointer*) &(priv->cancellable)
      );
      g_object_unref (priv->cancellable);
    }

    priv->cancellable = cancellable;

    if (priv->cancellable != NULL)
    {
      g_object_add_weak_pointer (
        G_OBJECT (priv->cancellable),
        (gpointer*) &(priv->cancellable)
      );
    }

    if (changed)
    {
      g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_CANCELLABLE]);
    }
    
}


static GCancellable*
lw_progress_get_cancellable (LwProgress *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_PROGRESS (self), NULL);

    //Declarations
    LwProgressPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->cancellable;
}


static void
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

    if (priv->total_progress != 0)
    {
      previous_fraction = (priv->previous_progress / priv->total_progress);
      current_progress = (priv->current_progress / priv->total_progress);
      delta = fabs(previous_fraction - current_progress);
    }

    reached = (
        delta > priv->required_ratio_delta ||
        priv->current_progress == 0.0 ||
        priv->current_progress == priv->total_progress
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


static void
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

    priv->required_ratio_delta = delta;

    g_signal_emit (
      G_OBJECT (self),
      klasspriv->signalid[CLASS_SIGNALID_PROGRESS_CHANGED], 
      0
    );
}


/**
 * lw_progress_get_current:
 * @self: a #LwProgress
 *
 * Returns the current progress as a #gdouble.  The number will generally
 * represent bytes in a file that can be paired against
 * lw_progress_get_total() for a progress percentage.
 *
 * Returns: A #gdouble representing the number of bytes read
 */
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

    g_mutex_lock (&priv->mutex);

    current_progress = priv->current_progress;

    g_mutex_unlock (&priv->mutex);

    return current_progress;
}
                         

/**
 * lw_progress_set_current:
 * @self: a #LwProgress
 * @current_progress: The new current progress
 * 
 * Sets the #gdouble representing the current progress.  If the number
 * is less than 0, it will be rounded up to 0.  If it is greater than the
 * total progress, it will be set to be equal to the total progress. When
 * this value causes a change larger thant he ratio delta int he progress
 * fraction, the "progress-changed" signal is emitted.
 */
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

    g_mutex_lock (&priv->mutex);
    is_locked = TRUE;

    if (current_progress < 0)
    {
      current_progress = 0;
    }
    if (current_progress > priv->total_progress)
    {
      current_progress = priv->total_progress;
    }

    changed = (fabs(priv->current_progress - current_progress) > epsilon);
    if (!changed) goto errored;

    priv->current_progress = current_progress;

errored:

    if (is_locked)
    {
      g_mutex_unlock (&priv->mutex);
      is_locked = FALSE;
    }

    if (changed) 
    {
      g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_CURRENT_PROGRESS]);
      lw_progress_sync_ratio_delta (self);
    }

    return;
}


/**
 * lw_progress_get_total:
 * @self: a #LwProgress
 * 
 * Gets the total progress represented by a #gdouble. The double will generally
 * represent the total bytes in a file.  This value can be used with
 * lw_progress_get_current() to calculate a fraction of completion.
 *
 * Returns: A #gdouble representing the possible total progress
 */
gdouble
lw_progress_get_total (LwProgress *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_PROGRESS (self), 0.0);

    //Declarations
    LwProgressPrivate *priv = NULL;
    gdouble total_progress = 0.0;

    //Initializations
    priv = self->priv;

    g_mutex_lock (&priv->mutex);

    total_progress = priv->total_progress;

    g_mutex_unlock (&priv->mutex);

    return total_progress;
}


/**
 * lw_progress_set_total:
 * @self: a #LwProgress
 * @total_progress: The new maximium progress
 *
 * Sets the current total progress. If the value is less than 0.0, it is
 * rounded up to 0.0.  
 */
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
    gboolean truncate_current_progress = FALSE;
    gdouble epsilon = 0.0000001;

    //Initializations
    priv = self->priv;
    klass = LW_PROGRESS_GET_CLASS (self);
    klasspriv = klass->priv;

    if (total_progress < 0.0)
    {
      total_progress = 0.0;
      truncate_current_progress = TRUE;
    }

    changed = (fabs(priv->total_progress - total_progress) > epsilon);
    if (!changed) goto errored;

    priv->total_progress = total_progress;

    if (priv->current_progress > priv->total_progress)
    {
      truncate_current_progress = TRUE;
    }


    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_TOTAL_PROGRESS]);

    if (truncate_current_progress)
    {
      lw_progress_set_current (self, priv->total_progress);
    }
    else if (!truncate_current_progress && changed && priv->current_progress > 0.0)
    {
      g_signal_emit (
        G_OBJECT (self),
        klasspriv->signalid[CLASS_SIGNALID_PROGRESS_CHANGED], 
        0
      );
    }

    lw_progress_sync_ratio_delta (self);

errored:

    return;
}


/**
 * lw_progress_get_fraction:
 * @self: a #LwProgress
 *
 * The current fraction progress calculated by dividing the curent progress by
 * the total progress.  After progress as been set to completed
 * with lw_progress_set_completed(), this method will always return 1.0.
 *
 * Returns: The current completion fraction that can be used for progress bars
 */
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

    g_mutex_lock (&priv->mutex);

    if (priv->completed)
    {
      fraction = 1.0;
    }
    else if (priv->total_progress == 0.0)
    {
      fraction = 0.0;
    }
    else
    {
      fraction = priv->current_progress / priv->total_progress;
    }

    g_mutex_unlock (&priv->mutex);

    return fraction;
}


/**
 * lw_progress_set_completed:
 * @self: a #LwProgress
 * @completed: The new completed setting as a #gboolean
 *
 * Sets the completed flag which is used to hint a progress dialog should be
 * be closed or a progress item should be marked completed. Once this is sete
 * to true, any call to lw_progress_get_fraction() will return 1.0.  Whenever
 * the value of @completed changes, the "progress-changed" signal is propogated
 * regardless of if the fraction changed.
 */
void
lw_progress_set_completed (LwProgress *self,
                           gboolean    completed)
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

    g_mutex_lock (&priv->mutex);
    is_locked = TRUE;

    changed = (priv->completed != completed);
    if (!changed) goto errored;
    
    priv->completed = completed;

errored:

    if (is_locked) g_mutex_unlock (&self->priv->mutex);
    if (changed) g_signal_emit (
      G_OBJECT (self),
      klasspriv->signalid[CLASS_SIGNALID_PROGRESS_CHANGED], 
      0
    );

    return;
}


/**
 * lw_progress_completed:
 * @self: a #LwProgress
 *
 * Returns: The current completed flag as a #gboolean
 */
gboolean
lw_progress_completed (LwProgress *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PROGRESS (self));

    //Declarations
    LwProgressPrivate *priv = NULL;
    gboolean completed = FALSE;

    //Initializations
    priv = self->priv;

    g_mutex_lock (&priv->mutex);

    completed = priv->completed;

    g_mutex_unlock (&priv->mutex);

    return completed;
}


/**
 * lw_progress_set_error:
 * @self: a #LwProgress
 * @error: The error to set a copy of
 *
 * Makes a copy of the passed @error and sets it to the #LwProgress object.
 * Setting an error is roughly equivalent to raising an exception, and thus
 * should cause the task to cancel and return as soon as possible. This can be
 * checked with the lw_progress_should_abort() method.
 */
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
    if (_error != NULL && error != _error)
    {
      error = g_error_copy (_error);
    }

    lw_progress_take_error (self, error);
}


/**
 * lw_progress_take_error:
 * @self: a #LwProgress
 * @error: The error to take ownership of
 *
 * Takes ownership of the passed  @error and sets it to the #LwProgress object.
 * Setting an error is roughly equivalent to raising an exception, and thus
 * should cause the task to cancel and return as soon as possible.  This can be
 * checked with the lw_progress_should_abort() method.
 */
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
    changed = (error != priv->error);
    if (!changed) goto errored;

    g_clear_error (&priv->error);
    priv->error = error;

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_ERROR]);

errored:
  
    return;
}


/**
 * lw_progress_get_error:
 * @self: a #LwProgress
 *
 * Returns: The currently set #GError or %NULL
 */
GError*
lw_progress_get_error (LwProgress  *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PROGRESS (self));

    //Declarations
    LwProgressPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->error;
}


/**
 * lw_progress_errored:
 * @self: a #LwProgress
 *
 * Method to query if an error set set such as by lw_progress_set_error()
 * or lw_progress_take_error(), though lw_progress_should_abort() is usually
 * the prefered way to check if a task should abort. You can get the #GError
 * though lw_progress_get_error().
 *
 * Returns: %TRUE if the operation had an error set
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

    g_mutex_lock (&priv->mutex);

    errored = (priv->error != NULL);

    g_mutex_unlock (&priv->mutex);

    return errored;
}


/**
 * lw_progress_is_cancelled:
 * @self: a #LwProgress
 *
 * Used to query specifically if lw_progress_cancel() was called,
 * though lw_progress_should_abort() is usually the prefered way to
 * check if a task should abort.
 *
 * Returns: %TRUE if the operation was cancelled
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

    g_mutex_lock (&priv->mutex);

    is_cancelled = (priv->cancellable != NULL && g_cancellable_is_cancelled (priv->cancellable));

    g_mutex_unlock (&priv->mutex);

    return is_cancelled;
}


/**
 * lw_progress_cancel:
 * @self: a #LwProgress
 *
 * Use to signal that a task should abort and return as soon as possible.
 * This method will usually be connected to a cancel button on a ui.
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

    g_mutex_lock (&priv->mutex);

    if (priv->cancellable != NULL) 
    {
      g_cancellable_cancel (priv->cancellable);
    }

    g_mutex_unlock (&priv->mutex);
}

 

/**
 * lw_progress_should_abort:
 * @self: a #LwProgress
 *
 * Provides an easy way to query a #LwProgress object if an error occured or
 * a cancellation request was initiated.  When either occur, this method will
 * start returning TRUE.  When this method returns TRUE, most methods should
 * stop their processing, cleanup and return.
 *
 * Returns: %TRUE if an error occured or if the task was cancelled.
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


/**
 * lw_progress_set_primary_message:
 * @self: a #LwProgress
 * @MESSAGE: The new primary message
 *
 * Sets the primary message which is used to inform the user of the currently
 * running action.  This would be a message like "Installing..." "Loading..."
 * etc.
 *
 */
void
lw_progress_set_primary_message (LwProgress *self,
                                 gchar const *MESSAGE)
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

    g_mutex_lock (&self->priv->mutex);
    is_locked = TRUE;

    changed = (g_strcmp0 (MESSAGE, priv->primary_message) != 0);
    if (!changed) goto errored;

    g_free (priv->primary_message);
    priv->primary_message = g_strdup (MESSAGE);

errored:

    if (is_locked) g_mutex_unlock (&self->priv->mutex);
    if (changed) g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_PRIMARY_MESSAGE]);
}


/**
 * lw_progress_set_primary_message_vprintf:
 * @self: a #LwProgress
 * @FORMAT: A printf format string
 *
 * vprintf version of lw_progress_set_primary_message()
 */
void
lw_progress_set_primary_message_vprintf (LwProgress  *self,
                                         gchar const *FORMAT,
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


/**
 * lw_progress_set_primary_message_printf:
 * @self: a #LwProgress
 * @FORMAT: A printf format string
 * @...: The arguments to fullfill the format string
 *
 * printf() version of lw_progress_set_primary_message()
 */
void
lw_progress_set_primary_message_printf (LwProgress  *self, 
                                        gchar const *FORMAT, 
                                        ...)
{
    //Declarations
    va_list va;

    //Initializations
    va_start(va, FORMAT);

    lw_progress_set_primary_message_vprintf (self, FORMAT, va);

    va_end (va);
}


/**
 * lw_progress_get_primary_message:
 * @self: a #LwProgress
 *
 * Returns: The currently set primary message or an empty string.  This method will never return %NULL
 */
gchar const*
lw_progress_get_primary_message (LwProgress *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PROGRESS (self));

    //Declarations
    LwProgressPrivate *priv = NULL;
    gchar *primary_message = NULL;

    //Initializations
    priv = self->priv;

    g_mutex_lock (&priv->mutex);

    primary_message = priv->primary_message;
    if (primary_message == NULL) primary_message = "";

    g_mutex_unlock (&priv->mutex);

    return primary_message;
}


/**
 * lw_progress_set_secondary_message:
 * @self: a #LwProgress
 * @MESSAGE: The new secondary message
 *
 * Sets the secondary message which is used to inform the user of the currently
 * running action.  If the primary message is "Installing...", the secondary
 * message would be something like "Downloading...", or "Decompressing...".
 *
 */
void
lw_progress_set_secondary_message (LwProgress  *self,
                                   gchar const *MESSAGE)
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

    g_mutex_lock (&self->priv->mutex);
    is_locked = TRUE;

    changed = (g_strcmp0 (MESSAGE, priv->secondary_message) != 0);
    if (!changed) goto errored;

    g_free (priv->secondary_message);
    priv->secondary_message = g_strdup (MESSAGE);

errored:

    if (is_locked) g_mutex_unlock (&self->priv->mutex);
    if (changed) g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_SECONDARY_MESSAGE]);

    return;
}


/**
 * lw_progress_set_secondary_message_vprintf:
 * @self: a #LwProgress
 * @FORMAT: A printf format string
 * @va: a #va_list as generated with va_start()
 *
 * vprintf() version of lw_progress_set_secondary_message()
 */
void
lw_progress_set_secondary_message_vprintf (LwProgress  *self,
                                           gchar const *FORMAT,
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


/**
 * lw_progress_set_secondary_message_printf:
 * @self: a #LwProgress
 * @FORMAT: A printf format string
 * @...: The arguments to fullfill the format string
 *
 * printf() version of lw_progress_set_secondary_message()
 */
void
lw_progress_set_secondary_message_printf (LwProgress  *self, 
                                          gchar const *FORMAT,
                                          ...)
{
    //Declarations
    va_list va;

    //Initializations
    va_start(va, FORMAT);

    lw_progress_set_secondary_message_vprintf (self, FORMAT, va);

    va_end (va);
}


/**
 * lw_progress_get_secondary_message:
 * @self: a #LwProgress
 *
 * Returns: The currently set secondary message or an empty string.  This method will never return %NULL
 */
gchar const*
lw_progress_get_secondary_message (LwProgress *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PROGRESS (self));

    //Declarations
    LwProgressPrivate *priv = NULL;
    gchar *secondary_message = NULL;

    //Initializations
    priv = self->priv;

    g_mutex_lock (&self->priv->mutex);

    secondary_message = priv->secondary_message;
    if (secondary_message == NULL) secondary_message = "";

    g_mutex_unlock (&priv->mutex);

    return secondary_message;
}


/**
 * lw_progress_set_step_message:
 * @self: a #LwProgress
 * @MESSAGE: The new secondary message
 *
 * Sets the secondary message which is used to inform the user of the currently
 * running action.  If the primary message is "Installing...", the secondary
 * message would be something like "Downloading...", or "Decompressing...".
 *
 */
void
lw_progress_set_step_message (LwProgress  *self,
                              gchar const *MESSAGE)
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

    g_mutex_lock (&self->priv->mutex);
    is_locked = TRUE;

    changed = (g_strcmp0(MESSAGE, priv->step_message) != 0);
    if (changed == FALSE) goto errored;
    
    g_free (priv->step_message);
    priv->step_message = g_strdup (MESSAGE);

errored:

    if (is_locked) g_mutex_unlock (&self->priv->mutex);
    if (changed) g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_STEP_MESSAGE]);
  
    return;
}



/**
 * lw_progress_set_step_message_vprintf:
 * @self: a #LwProgress
 * @FORMAT: A printf format string
 * @va: a #va_list as generated with va_start()
 * 
 *
 * vprintf() version of lw_progress_set_step_message()
 */
void
lw_progress_set_step_message_vprintf (LwProgress  *self,
                                      gchar const *FORMAT,
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


/**
 * lw_progress_set_step_message_printf:
 * @self: a #LwProgress
 * @FORMAT: A printf format string
 * @...: The arguments to fullfill the format string
 *
 * printf() version of lw_progress_set_step_message()
 */
void
lw_progress_set_step_message_printf (LwProgress  *self,
                                     gchar const *FORMAT,
                                     ...)
{
    //Declarations
    va_list va;

    //Initializations
    va_start(va, FORMAT);

    lw_progress_set_step_message_vprintf (self, FORMAT, va);

    va_end (va);
}


/**
 * lw_progress_get_step_message:
 * @self: a #LwProgress
 *
 * Returns: The currently set step message or an empty string.  This method will never return %NULL
 */
gchar const*
lw_progress_get_step_message (LwProgress *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PROGRESS (self));

    //Declarations
    LwProgressPrivate *priv = NULL;
    gchar *step_message = NULL;

    //Initializations
    priv = self->priv;

    g_mutex_lock (&self->priv->mutex);

    step_message = priv->step_message;
    if (step_message == NULL) step_message = "";

    g_mutex_unlock (&self->priv->mutex);

    return step_message;
}


/**
 * lw_progress_get_prefered_chunk_size:
 * @self: a #LwProgress
 *
 * Returns: The current prefered chunk size or 0 to signify it isn't set
 */
gsize
lw_progress_get_prefered_chunk_size (LwProgress *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, 0);
    
    //Declarations
    LwProgressPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->prefered_chunk_size;
}


/**
 * lw_progress_set_prefered_chunk_size:
 * @self: a #LwProgress
 * @prefered_chunk_size: The new prefered chunksize
 *
 * When set, prefered chunksize overrides the normal chunksize for 
 * progress operations guaranteeing it will always be the same.
 */
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

    if (priv->prefered_chunk_size == prefered_chunk_size) goto errored;

    priv->prefered_chunk_size = prefered_chunk_size;

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_PREFERED_CHUNK_SIZE]);

errored:

    return;
}


/**
 * lw_progress_get_chunk_size:
 * @self: a #LwProgress
 *
 * Returns: The prefered chunk size if available, otherwise returning the direct value.
 */
gsize
lw_progress_get_chunk_size (LwProgress *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, 0);
    
    //Declarations
    LwProgressPrivate *priv = NULL;
    gsize chunk_size = 0.0;

    //Initializations
    priv = self->priv;
    if (priv->prefered_chunk_size != 0.0)
    {
      chunk_size = priv->prefered_chunk_size;
    }
    else
    {
      chunk_size = priv->chunk_size;
    }

    return chunk_size;
}


/**
 * lw_progress_set_chunk_size:
 * @self: a #LwProgress
 * @chunk_size: The new chunk_size
 *
 * This value is updated by #LwProgress to try to make progress
 * progression smooth, but not use too much cpu space and reduce
 * waits for mutexes.  If you want to set a chunk size that won't
 * be overwritten, please use lw_progress_set_prefered_chunk_size().
 */
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

    if (priv->chunk_size == chunk_size) goto errored;

    priv->chunk_size = chunk_size;

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_CHUNK_SIZE]);

errored:

    return;
}
