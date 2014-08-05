#ifndef LW_DICTIONARYBUFFER_INCLUDED
#define LW_DICTIONARYBUFFER_INCLUDED

G_BEGIN_DECLS

struct _LwDictionaryBuffer {
    gchar *buffer;
    gchar *checksum;
    LwOffset length;
    gchar *path;
    gchar **lines;
    gint num_lines;
};
typedef struct _LwDictionaryBuffer LwDictionaryBuffer;

#define LW_DICTIONARYBUFFER(obj) (LwDictionaryBuffer*)obj

LwDictionaryBuffer* lw_dictionarybuffer_new (void);
void lw_dictionarybuffer_free (LwDictionaryBuffer* self);

void lw_dictionarybuffer_create (LwDictionaryBuffer *self, const gchar *PATH);

const gchar* lw_dictionarybuffer_start (LwDictionaryBuffer *self);
const gchar* lw_dictionarybuffer_next (LwDictionaryBuffer *self, const gchar *BUFFER);

const gchar* lw_dictionarybuffer_get_line (LwDictionaryBuffer *self, gint number);
GList* lw_dictionarybuffer_get_all_lines (LwDictionaryBuffer *self, GList *numbers);

gint lw_dictionarybuffer_get_accuracy_weight_delta (LwDictionaryBuffer *self, LwOffset offset, LwMorphologyString *morphologystring);
const gchar* lw_dictionarybuffer_get_checksum (LwDictionaryBuffer *self);
LwOffset lw_dictionarybuffer_length (LwDictionaryBuffer *self);
LwOffset lw_dictionarybuffer_get_offset (LwDictionaryBuffer *self, const gchar *BUFFER);

G_END_DECLS

#endif
