#ifndef LW_DICTIONARYBUFFER_PRIVATE_INCLUDED
#define LW_DICTIONARYBUFFER_PRIVATE_INCLUDED

G_BEGIN_DECLS

#include <libwaei/word.h>

typedef enum {
    PROP_0,
    PROP_BUFFER,
    PROP_FLAGS,
    TOTAL_PROPS
} Props;


struct _LwDictionaryBufferPrivate {
    GMappedFile *mapped_file;
    gchar *contents;
    gsize length;
    gint fd;

    LwUtf8NormalizeFlag flags;

    LwDictionaryLine *lines;
    gint num_lines;
};

struct _LwDictionaryBufferClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
};

#define LW_DICTIONARYBUFFER_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_DICTIONARYBUFFER, LwDictionaryBufferPrivate));

//Methods

void lw_dictionarybuffer_set_flags (LwDictionaryBuffer *self, LwUtf8NormalizeFlag flags);

G_END_DECLS

#endif


