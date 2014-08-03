#ifndef LW_RESULTBUFFER_INCLUDED
#define LW_RESULTBUFFER_INCLUDED 

G_BEGIN_DECLS

struct _LwResultBuffer {
  gchar **strv;
  gint index;
  gint length;
};

typedef struct _LwResultBuffer LwResultBuffer;

void lw_resultbuffer_init (LwResultBuffer *self, gint length);
gchar** lw_resultbuffer_clear (LwResultBuffer *self, gboolean free_strv);

void lw_resultbuffer_collapse (LwResultBuffer *self);
void lw_resultbuffer_add (LwResultBuffer *self, gchar const * TEXT);

gint lw_resultbuffer_length (LwResultBuffer *self);


G_END_DECLS

#endif

