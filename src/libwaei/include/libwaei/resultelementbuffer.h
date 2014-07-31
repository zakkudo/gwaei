#ifndef LW_RESULTELEMENTBUFFER_INCLUDED
#define LW_RESULTELEMENTBUFFER_INCLUDED 

G_BEGIN_DECLS

struct _LwResultElementBuffer {
  gchar **strv;
  gint index;
  gint length;
};

typedef struct _LwResultElementBuffer LwResultElementBuffer;

void lw_resultelementbuffer_init (LwResultElementBuffer *self, gint length);
void lw_resultelementbuffer_clear (LwResultElementBuffer *self);

void lw_resultelementbuffer_shrink (LwResultElementBuffer *self);
void lw_resultelementbuffer_add (LwResultElementBuffer *self, gchar const * TEXT);


G_END_DECLS

#endif

