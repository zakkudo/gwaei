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
void lw_dictionaryline_set_offsets (LwDictionaryLine *self, gint id, lwoffset *offsets);
lwoffset* lw_dictionaryline_get_offsets (LwDictionaryLine *self, gint id);

gsize lw_dictionaryline_get_serialization_length (LwDictionaryLine *self);
void lw_dictionaryline_serialize (LwDictionaryLine *self, guchar *preallocated_buffer, gsize buffer_length);


G_END_DECLS

#endif

