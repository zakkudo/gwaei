#ifndef LW_PROGRESS_INCLUDED
#define LW_PROGRESS_INCLUDED

#include <glib.h>
#include <gio/gio.h>

G_BEGIN_DECLS

typedef struct _LwProgress LwProgress;

#define LW_PROGRESS(obj) (LwProgress*) obj

//Callback prototype
typedef gint (*LwProgressCallback) (gpointer object, LwProgress *progress, gpointer data);

//Class
struct _LwProgress {
  GMutex mutex;

  gpointer object;
  LwProgressCallback cb;
  gpointer data;
  GCancellable *cancellable;

  gdouble previous_progress;
  gdouble current_progress;
  gdouble total_progress;

  gdouble ratio_delta;
  gdouble required_ratio_delta;
  gboolean reached_ratio_delta;

  gchar *step_message;
  gint current_step;
  gint total_steps;

  GError *error;

  gint64 start_time;

  gchar *job_title;
  gchar *primary_message;
  gchar *secondary_message;
  gchar *units;

  gchar *source_filename;
  gchar *target_filename;

  gboolean finished;
  gboolean changed;
};

//Methods
LwProgress* lw_progress_new (GCancellable *cancellable, LwProgressCallback cb, gpointer data);
void lw_progress_free (LwProgress *progress);

void lw_progress_set_fraction (LwProgress *progress, gdouble current_progress, gdouble total_progress);
gdouble lw_progress_get_fraction (LwProgress *progress);

gboolean lw_progress_changed (LwProgress *progress);
void lw_progress_clear_changed (LwProgress *progress);
gboolean lw_progress_errored (LwProgress *progress);
gboolean lw_progress_is_cancelled (LwProgress *progress);
void lw_progress_cancel (LwProgress *progress);

void lw_progress_set_object (LwProgress *progress, gpointer object);

gboolean lw_progress_should_abort (LwProgress *progress);

void lw_progress_run_callback (LwProgress *progress);

void lw_progress_set_finished (LwProgress *progress, gboolean finished);
gboolean lw_progress_is_finished (LwProgress *progress);

void lw_progress_set_primary_message (LwProgress *progress, const gchar *FORMAT, ...);
const gchar* lw_progress_get_primary_message (LwProgress *progress);
void lw_progress_set_secondary_message (LwProgress *progress, const gchar *FORMAT, ...);
const gchar* lw_progress_get_secondary_message (LwProgress *progress);

void lw_progress_set_step_message (LwProgress *progress, const gchar *FORMAT, gint current_step, gint total_steps);
const gchar* lw_progress_get_step_message (LwProgress *progress);

void lw_progress_set_required_ratio_delta (LwProgress *progress, gdouble delta);


struct _LwProgressCallbackWithData {
  LwProgressCallback cb;
  gpointer data;
  GCancellable *cancellable;
};
typedef struct _LwProgressCallbackWithData LwProgressCallbackWithData;


G_END_DECLS

#endif
