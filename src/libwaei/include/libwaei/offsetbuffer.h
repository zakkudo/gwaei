#ifndef LW_RESULTBUFFER_INCLUDED
#define LW_RESULTBUFFER_INCLUDED 

G_BEGIN_DECLS

typedef gsize lwoffset;

struct _LwOffsetBuffer {
  gboolean is_static;
  lwoffset *offsets;
  gint length;
};

typedef struct _LwOffsetBuffer LwOffsetBuffer;

#define LW_OFFSETBUFFER(obj) (LwOffsetBuffer*)obj

void lw_offsetbuffer_init_static (LwOffsetBuffer *self, const gchar *BUFFER, lwoffset *offsets);
void lw_offsetbuffer_init (LwOffsetBuffer *self, const gchar *BUFFER, gint length);
lwoffset* lw_offsetbuffer_clear (LwOffsetBuffer *self, gboolean free_strv);

void lw_offsetbuffer_collapse (LwOffsetBuffer *self);

lwoffset lw_offsetbuffer_get_relative (LwOffsetBuffer *self, gsize index);
void lw_offsetbuffer_add_relative (LwOffsetBuffer *self, gsize offset);

const gchar* lw_offsetbuffer_get_absolute (LwOffsetBuffer *self, gsize index);
void lw_offsetbuffer_add_absolute (LwOffsetBuffer *self, gchar const * TOKEN);

gint lw_offsetbuffer_length (LwOffsetBuffer *self);

const guchar* lw_offsetbuffer_serialize (LwOffsetBuffer *self, gsize *length);
void lw_offsetbuffer_deserialize (LwOffsetBuffer *self, const gchar *BUFFER, const guchar *data);


G_END_DECLS

#endif

