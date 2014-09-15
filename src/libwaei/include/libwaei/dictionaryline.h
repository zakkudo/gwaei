#ifndef LW_DICTIONARYLINE_INCLUDED
#define LW_DICTIONARYLINE_INCLUDED 

G_BEGIN_DECLS

//Single form

#define LW_DICTIONARYLINE_ERROR lw_dictionaryline_error_quark ()

typedef enum {
  LW_DICTIONARYLINE_ERRORCODE_DESERIALIZATION_ERROR
} LwDictionaryLineErrorCode;

struct _LwDictionaryLine {
  GTree *tree;
};

typedef struct _LwDictionaryLine LwDictionaryLine;

#define LW_DICTIONARYLINE(obj) (LwDictionaryLine*)obj

//Methods
void lw_dictionaryline_init (LwDictionaryLine *self);
void lw_dictionaryline_clear (LwDictionaryLine *self);

void lw_dictionaryline_take_strv (LwDictionaryLine *self, gint id, gchar ** strv);
gchar const ** lw_dictionaryline_get_strv (LwDictionaryLine *self, gint id);

gsize lw_dictionaryline_get_serialized_length (LwDictionaryLine *self);
gsize lw_dictionaryline_serialize (LwDictionaryLine *self, gchar *contents_reference_point, gchar *preallocated_buffer, GError **error);
gsize lw_dictionaryline_deserialize_into (LwDictionaryLine *self, gchar const *serialized_data, gchar const *contents_reference_point, GError **error);


G_END_DECLS

#endif

