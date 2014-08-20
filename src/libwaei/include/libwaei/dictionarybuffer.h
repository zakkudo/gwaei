#ifndef LW_DICTIONARYBUFFER_INCLUDED
#define LW_DICTIONARYBUFFER_INCLUDED

#include "dictionaryline.h"

G_BEGIN_DECLS


//Boilerplate
typedef struct _LwDictionaryBuffer LwDictionaryBuffer;
typedef struct _LwDictionaryBufferClass LwDictionaryBufferClass;
typedef struct _LwDictionaryBufferPrivate LwDictionaryBufferPrivate;
typedef struct _LwDictionaryBufferClassPrivate LwDictionaryBufferClassPrivate;

#define LW_TYPE_DICTIONARYBUFFER              (lw_dictionarybuffer_get_type())
#define LW_DICTIONARYBUFFER(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_DICTIONARYBUFFER, LwDictionaryBuffer))
#define LW_DICTIONARYBUFFER_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_DICTIONARYBUFFER, LwDictionaryBufferClass))
#define LW_IS_DICTIONARYBUFFER(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_DICTIONARYBUFFER))
#define LW_IS_DICTIONARYBUFFER_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_DICTIONARYBUFFER))
#define LW_DICTIONARYBUFFER_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_DICTIONARYBUFFER, LwDictionaryBufferClass))

struct _LwDictionaryBuffer {
    GObject object;
    LwDictionaryBufferPrivate *priv;
};

struct _LwDictionaryBufferClass {
  GObjectClass parent_class;
  LwDictionaryBufferClassPrivate *priv;
};

typedef LwDictionaryLine**(*LwDictionaryBufferParseFunc)(LwDictionaryBuffer*, gpointer, gint*);

//Methods
LwDictionaryBuffer* lw_dictionarybuffer_new (LwUtf8NormalizeFlag flags);
GType lw_dictionarybuffer_get_type (void) G_GNUC_CONST;

LwDictionaryLine** lw_dictionarybuffer_get_lines (LwDictionaryBuffer *self, gint *num_lines);
void lw_dictionarybuffer_set_contents (LwDictionaryBuffer *self, const gchar *CHECKSUM, const gchar *CONTENTS, gint length, LwDictionaryBufferParseFunc func, gpointer data);

//Properties
LwProgress* lw_dictionarybuffer_get_progress (LwDictionaryBuffer *self);
void lw_dictionarybuffer_set_progress (LwDictionaryBuffer *self, LwProgress *progress);

LwUtf8NormalizeFlag lw_dictionarybuffer_get_flags (LwDictionaryBuffer *self);

gchar* lw_dictionarybuffer_get_contents (LwDictionaryBuffer *self);

const gchar* lw_dictionarybuffer_get_checksum (LwDictionaryBuffer *self);

G_END_DECLS

#endif
