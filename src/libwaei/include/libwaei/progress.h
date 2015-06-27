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


#define LW_PROGRESS_GOTO_ERRORED_IF_SHOULD_ABORT(self) if (self != NULL && lw_progress_should_abort (self)) goto errored;

#define LW_PROGRESS_RETURN_IF_SHOULD_ABORT(self) if (self != NULL && lw_progress_should_abort (self))\
    {\
      return;\
    }

#define LW_PROGRESS_RETURN_VAL_IF_SHOULD_ABORT(self, value) if (self != NULL && lw_progress_should_abort (self))\
    {\
      return value;\
    }

#define LW_PROGRESS_START(self, total) (self != NULL) ? lw_progress_get_chunk_size (self) : total;\
    if (self != NULL)\
    {\
      lw_progress_set_current (self, 0);\
      lw_progress_set_total (self, total);\
    }

#define LW_PROGRESS_FINISH(self, current) if (self != NULL)\
    {\
      lw_progress_set_current (self, current);\
    }

#define LW_PROGRESS_TAKE_ERROR(self, error) if (error != NULL)\
    {\
      if (self != NULL)\
      {\
        lw_progress_take_error (self, error);\
        error = NULL;\
      }\
      g_clear_error (&error);\
      goto errored;\
    }

#define LW_PROGRESS_UPDATE(self, current, total, chunk, max_chunk, error) if (error != NULL)\
    {\
      if (self != NULL)\
      {\
        lw_progress_take_error (self, error);\
        error = NULL;\
      }\
      g_clear_error (&error);\
      goto errored;\
    }\
    if (self != NULL)\
    {\
      if (G_UNLIKELY (chunk >= max_chunk) && G_LIKELY (max_chunk > 0))\
      {\
        if (G_UNLIKELY (lw_progress_should_abort (self))) goto errored;\
        lw_progress_set_current (self, current);\
        chunk = 0;\
      }\
    }

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

//Properties

void lw_progress_cancel (LwProgress *self);
gboolean lw_progress_is_cancelled (LwProgress *self);

void lw_progress_set_error (LwProgress *self, GError *error);
void lw_progress_take_error (LwProgress *self, GError *error);
GError* lw_progress_get_error (LwProgress  *self);
gboolean lw_progress_errored (LwProgress *self);

void lw_progress_set_completed (LwProgress *self, gboolean completed);
gboolean lw_progress_completed (LwProgress *self);

gdouble lw_progress_get_current (LwProgress *self);
void lw_progress_set_current (LwProgress *self, gdouble current_progress);

gdouble lw_progress_get_total (LwProgress *self);
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

gsize lw_progress_get_prefered_chunk_size (LwProgress *self);
void lw_progress_set_prefered_chunk_size (LwProgress *self, gsize prefered_chunk_size);

gsize lw_progress_get_chunk_size (LwProgress *self);
void lw_progress_set_chunk_size (LwProgress *self, gsize chunk_size);

G_END_DECLS

#endif
