#ifndef LW_PARSEDLINE_INCLUDED
#define LW_PARSEDLINE_INCLUDED 

G_BEGIN_DECLS

//Single form

#define LW_PARSEDLINE_ERROR lw_parsedline_error_quark ()

typedef enum {
  LW_PARSEDLINE_ERRORCODE_DESERIALIZATION_ERROR
} LwParsedLineErrorCode;

struct _LwParsedLine {
  GTree *tree;
};

typedef struct _LwParsedLine LwParsedLine;

#define LW_PARSEDLINE(obj) (LwParsedLine*)obj

//Methods
void lw_parsedline_init (LwParsedLine *self);
void lw_parsedline_init_static (LwParsedLine *self);
void lw_parsedline_clear (LwParsedLine *self);

void lw_parsedline_take_strv (LwParsedLine *self, gint id, gchar ** strv);
gchar const ** lw_parsedline_get_strv (LwParsedLine *self, gint id);

gsize lw_parsedline_get_serialized_length (LwParsedLine *self);
gsize lw_parsedline_serialize (LwParsedLine *self, gchar *contents_reference_point, gchar *preallocated_buffer, GError **error);
gsize lw_parsedline_deserialize_into (LwParsedLine *self, gchar const *serialized_data, gchar const *contents_reference_point, GError **error);


G_END_DECLS

#endif

