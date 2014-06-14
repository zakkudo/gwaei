#ifndef LW_PROGRESS_PRIVATE_INCLUDED
#define LW_PROGRESS_PRIVATE_INCLUDED

G_BEGIN_DECLS

#include <libwaei/word.h>

typedef enum {
    PROP_0,
    PROP_ERROR,
    PROP_ERRORED,
    PROP_CANCELLABLE,
    PROP_CANCELLED,
    PROP_PRIMARY_MESSAGE,
    PROP_SECONDARY_MESSAGE,
    PROP_STEP_MESSAGE,
    PROP_COMPLETED,
    PROP_CURRENT_PROGRESS,
    PROP_TOTAL_PROGRESS,
    PROP_PROGRESS_FRACTION,
    TOTAL_PROPS
} Props;


typedef enum {
  CLASS_SIGNALID_PROGRESS_CHANGED,
  TOTAL_CLASS_SIGNALIDS
} ClassSignalId;


struct _Data {
  GMutex mutex;

  GCancellable *cancellable;
  GError *error;

  gdouble previous_progress;
  gdouble current_progress;
  gdouble total_progress;

  gdouble ratio_delta;

  gint64 start_time;

  gchar *job_title;
  gchar *step_message;
  gchar *primary_message;
  gchar *secondary_message;
  gchar *units;

  gboolean complete;
};

struct _Config {
  gchar *filename;
  gdouble required_ratio_delta;
};

struct _LwProgressPrivate {
  struct _Data data;
  struct _Config config;
};

struct _LwProgressClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
  guint signalid[TOTAL_CLASS_SIGNALIDS];
};

#define LW_PROGRESS_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_PROGRESS, LwProgressPrivate));

void lw_progress_sync_ratio_delta (LwProgress *progress);

G_END_DECLS

#endif

