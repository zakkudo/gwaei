#ifndef LW_PARSEDLINE_INCLUDED
#define LW_PARSEDLINE_INCLUDED 

G_BEGIN_DECLS

//Single form

#define LW_PARSEDLINE_ERROR lw_parsedline_error_quark ()

/**
 * LwParsedLineErrorCode:
 * @LW_PARSEDLINE_ERRORCODE_DESERIALIZATION_ERROR: There was a fatal error during deserialization
 *
 * Error Codes for #LwParsedLine operations
 */
typedef enum {
  LW_PARSEDLINE_ERRORCODE_DESERIALIZATION_ERROR
} LwParsedLineErrorCode;


/**
 * LwParsedLine:
 *
 * An object for holding parsed line data.  It's contents are private
 * and should only be accessed through public methods.
 */
struct _LwParsedLine {
  /*< private >*/
  GTree *tree;
};

typedef struct _LwParsedLine LwParsedLine;

/**
 * LW_PARSEDLINE:
 * @obj: An object to cast to a #LwParsedLine
 */
#define LW_PARSEDLINE(obj) (LwParsedLine*)obj

typedef gboolean (*LwParsedLineForeachFunc) (LwParsedLine * self, gint column, gchar ** strv, gpointer data);

//Methods
void lw_parsedline_init (LwParsedLine *self);
void lw_parsedline_init_full (LwParsedLine *self, GDestroyNotify destroy_notify_func);
void lw_parsedline_clear (LwParsedLine *self);

void lw_parsedline_set_strv (LwParsedLine *self, gint id, gchar ** strv);
gchar const ** lw_parsedline_get_strv (LwParsedLine *self, gint id);

gint lw_parsedline_num_columns (LwParsedLine * self);

gsize lw_parsedline_get_serialized_length (LwParsedLine *self);
gsize lw_parsedline_serialize (LwParsedLine *self, gchar const * contents_reference_point, gchar *preallocated_buffer, GError **error);
gsize lw_parsedline_deserialize_into (LwParsedLine *self, gchar const *serialized_data, gchar const *contents_reference_point, GError **error);

void lw_parsedline_foreach (LwParsedLine * self, LwParsedLineForeachFunc func, gpointer data);

void lw_parsedline_assert_equals (LwParsedLine * self, LwParsedLine * other);


G_END_DECLS

#endif

