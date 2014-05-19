#ifndef LW_PROGRESS_INCLUDED
#define LW_PROGRESS_INCLUDED

#include <gio/gio.h>

G_BEGIN_DECLS

//Boilerplate
typedef struct _LwProgress LwProgress;
typedef struct _LwProgressClass LwProgressClass;
typedef struct _LwProgressPrivate LwProgressPrivate;
typedef struct _LwProgressClassPrivate LwProgressClassPrivate;

#define LW_TYPE_PROGRESS              (lw_progress_get_type())
#define LW_PROGRESS(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_PROGRESS, LwProgress))
#define LW_PROGRESS_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_PROGRESS, LwProgressClass))
#define LW_IS_PROGRESS(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_PROGRESS))
#define LW_IS_PROGRESS_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_PROGRESS))
#define LW_PROGRESS_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_PROGRESS, LwProgressClass))

typedef gint (*LwProgressCallback) (LwProgress *progress, gpointer data);

struct _LwProgress {
  GObject object;
  LwProgressPrivate *priv;
};

struct _LwProgressClass {
  GObjectClass parent_class;
  LwProgressClassPrivate *priv;

  //Signals
  void (*changed) (LwProgress* progress, gpointer data);
};


//Methods
LwProgress* lw_progress_new (void);
GType lw_progress_get_type (void) G_GNUC_CONST;

gboolean lw_progress_should_abort (LwProgress *self);

void lw_progress_cancel (LwProgress *self);
gboolean lw_progress_is_cancelled (LwProgress *self);

void lw_progress_set_cancellable (LwProgress *self, GCancellable *cancellable);
GCancellable* lw_progress_get_cancellable (LwProgress *self);

void lw_progress_set_error (LwProgress *self, GError *error);
void lw_progress_take_error (LwProgress *self, GError **source);
GError* lw_progress_get_error (LwProgress  *self);
gboolean lw_progress_errored (LwProgress *self);

void lw_progress_set_completed (LwProgress *self, gboolean complete);
gboolean lw_progress_completed (LwProgress *self);

void lw_progress_set_current (LwProgress *self, gdouble current_progress);
void lw_progress_set_total (LwProgress *self, gdouble total_progress);

gdouble lw_progress_get_fraction (LwProgress *self);

void lw_progress_set_primary_message (LwProgress *self, const gchar *MESSAGE);
void lw_progress_set_primary_message_printf (LwProgress *self, const gchar *FORMAT, ...);
const gchar* lw_progress_get_primary_message (LwProgress *self);

void lw_progress_set_secondary_message (LwProgress *self, const gchar *MESSAGE);
void lw_progress_set_secondary_message_printf (LwProgress *self, const gchar *FORMAT, ...);
const gchar* lw_progress_get_secondary_message (LwProgress *self);

void lw_progress_set_step_message (LwProgress *self, const gchar *MESSAGE);
void lw_progress_set_step_message_printf (LwProgress *self, const gchar *FORMAT, ...);
const gchar* lw_progress_get_step_message (LwProgress *self);

G_END_DECLS

#endif
