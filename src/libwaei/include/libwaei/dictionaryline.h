#ifndef LW_DICTIONARYLINE_INCLUDED
#define LW_DICTIONARYLINE_INCLUDED 

G_BEGIN_DECLS

//Single form

typedef struct _LwDictionaryLine LwDictionaryLine;

#define LW_DICTIONARYLINE(obj) (LwDictionaryLine*)obj

//Methods
void lw_dictionaryline_set (LwDictionaryLine *self, gchar const *KEY, gchar const * parsed_text);
void lw_dictionaryline_set_strv (LwDictionaryLine *self, gchar const *KEY, gchar const * * parsed_strv);

gchar const * lw_dictionaryline_get (LwDictionaryLine *self, gchar const *KEY);
gchar * const * lw_dictionaryline_get_strv (LwDictionaryLine *self, gchar const *KEY);


//List form

struct _LwDictionaryLines {
  gint num_lines;
  LwDictionaryLine *lines;
};

typedef struct _LwDictionaryLines LwDictionaryLines;

typedef void (*LwDictionaryLinesForeachFunc) (LwDictionaryLines *self, LwDictionaryLine *line, gpointer user_data);
#define LW_DICTIONARYLINES(obj) (LwDictionaryLines*)obj

LwDictionaryLines* lw_dictionarylines_new (gint num_lines);
void lw_dictionarylines_free (LwDictionaryLines *lines);

void lw_dictionarylines_foreach (LwDictionaryLines *self, LwDictionaryLinesForeachFunc func, gpointer data);
LwDictionaryLine* lw_dictionarylines_get_line (LwDictionaryLines *self, gint line_number);
gint lw_dictionarylines_length (LwDictionaryLines *self);


G_END_DECLS

#endif

