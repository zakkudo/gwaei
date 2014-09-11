#ifndef LW_DICTIONARYLINE_INCLUDED
#define LW_DICTIONARYLINE_INCLUDED 

G_BEGIN_DECLS

//Single form

struct _LwDictionaryLine {
  GTree *tree;
};

typedef struct _LwDictionaryLine LwDictionaryLine;

#define LW_DICTIONARYLINE(obj) (LwDictionaryLine*)obj

//Methods
void lw_dictionaryline_init (LwDictionaryLine *self);
void lw_dictionaryline_clear (LwDictionaryLine *self);

void lw_dictionaryline_take_strv (LwDictionaryLine *self, gint id, gchar ** strv);
gchar const ** lw_dictionaryline_get_offsets (LwDictionaryLine *self, gint id);

gsize lw_dictionaryline_get_serialized_length (LwDictionaryLine *self);
gsize lw_dictionaryline_serialize (LwDictionaryLine *self, gchar *preallocated_buffer, gsize buffer_length);


G_END_DECLS

#endif

