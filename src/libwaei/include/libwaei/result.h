#ifndef LW_RESULT_INCLUDED
#define LW_RESULT_INCLUDED 

G_BEGIN_DECLS

struct _LwResult {
  gchar *text;
  gint length;
  gchar *buffer;
  gchar *buffer_end;
  GHashTable *data;
};

typedef struct _LwResult LwResult;
#define LW_RESULT(obj) (LwResult*)obj

//Methods
LwResult* lw_result_new (const gchar *TEXT);
void lw_result_free (LwResult *self);

//Methods
gchar const * lw_result_get_text (LwResult *self);

gchar * lw_result_get_buffer (LwResult *self);
gboolean lw_result_buffer_owns_text (LwResult *self, const gchar *TEXT);
gboolean lw_result_buffer_owns_strv (LwResult *self, gchar **TEXT);

void lw_result_take (LwResult *self, gchar const *KEY, gchar const * parsed_text);
void lw_result_take_strv (LwResult *self, gchar const *KEY, gchar const * * parsed_strv);

gchar const * lw_result_get (LwResult *self, gchar const *KEY);
gchar * const * lw_result_get_strv (LwResult *self, gchar const *KEY);

G_END_DECLS

#endif

