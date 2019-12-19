#ifndef LW_PARSED_LINE_INCLUDED
#define LW_PARSED_LINE_INCLUDED 

G_BEGIN_DECLS

//Single form

#define LW_PARSED_LINE_ERROR lw_parsed_line_error_quark ()
GQuark lw_parsed_line_error_quark (void);

/**
 * LwParsedLineErrorCode:
 * @LW_PARSED_LINE_ERRORCODE_DESERIALIZATION_ERROR: There was a fatal error during deserialization
 *
 * Error Codes for #LwParsedLine operations
 */
typedef enum {
  LW_PARSED_LINE_ERRORCODE_DESERIALIZATION_ERROR
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
 * LW_PARSED_LINE:
 * @obj: An object to cast to a #LwParsedLine
 */
#define LW_PARSED_LINE(obj) (LwParsedLine*)obj

typedef gboolean (*LwParsedLineForeachFunc) (LwParsedLine * self, gint column, gchar ** strv, gpointer data);

//Methods
void lw_parsed_line_init (LwParsedLine *self);
void lw_parsed_line_init_full (LwParsedLine *self, GDestroyNotify destroy_notify_func);
void lw_parsed_line_clear (LwParsedLine *self);

void lw_parsed_line_set_strv (LwParsedLine *self, gint id, gchar ** strv);
gchar const ** lw_parsed_line_get_strv (LwParsedLine *self, gint id);

gint lw_parsed_line_num_columns (LwParsedLine * self);

gsize lw_parsed_line_get_serialized_length (LwParsedLine *self);
gsize lw_parsed_line_serialize (LwParsedLine *self, gchar const * contents_reference_point, gchar *preallocated_buffer, GError **error);
gsize lw_parsed_line_deserialize_into (LwParsedLine *self, gchar const *serialized_data, gchar const *contents_reference_point, GError **error);

void lw_parsed_line_foreach (LwParsedLine * self, LwParsedLineForeachFunc func, gpointer data);

void lw_parsed_line_assert_equals (LwParsedLine * self, LwParsedLine * other);


G_END_DECLS

#endif

