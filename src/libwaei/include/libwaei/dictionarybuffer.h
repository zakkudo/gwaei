#ifndef LW_DICTIONARYBUFFER_INCLUDED
#define LW_DICTIONARYBUFFER_INCLUDED

G_BEGIN_DECLS

struct _LwDictionaryBuffer {
    gchar *buffer;
    gchar *checksum;
    gsize length;
    gchar *path;
    gsize *lines;
    gint num_lines;
    gchar **tokens;
    gint num_tokens;
};
typedef struct _LwDictionaryBuffer LwDictionaryBuffer;

#define LW_DICTIONARYBUFFER(obj) (LwDictionaryBuffer*)obj
#define LW_TYPE_DICTIONARYBUFFER (lw_result_get_type())

typedef gchar**(*LwTokenizeFunc)(gchar*, gchar**, gint*);

LwDictionaryBuffer* lw_dictionarybuffer_new (void);
GType lw_dictionarybuffer_get_type (void);
void lw_dictionarybuffer_free (LwDictionaryBuffer* self);

void lw_dictionarybuffer_unref (LwDictionaryBuffer *self);
LwDictionaryBuffer* lw_dictionarybuffer_ref (LwDictionaryBuffer *self);

void lw_dictionarybuffer_create (LwDictionaryBuffer *self, const gchar *PATH, LwTokenizeFunc tokenize);

const gchar** lw_dictionarybuffer_get_line (LwDictionaryBuffer *self, gint number, gint *num_tokens_out);

gint lw_dictionarybuffer_get_accuracy_weight_delta (LwDictionaryBuffer *self, gsize offset, LwMorphologyString *morphologystring);
const gchar* lw_dictionarybuffer_get_checksum (LwDictionaryBuffer *self);
gsize lw_dictionarybuffer_length (LwDictionaryBuffer *self);

G_END_DECLS

#endif
