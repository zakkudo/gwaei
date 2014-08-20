#ifndef LW_DICTIONARYLINE_INCLUDED
#define LW_DICTIONARYLINE_INCLUDED 

G_BEGIN_DECLS


struct _LwDictionaryLine {
  GTree *tree;
};
typedef struct _LwDictionaryLine LwDictionaryLine;

#define LW_DICTIONARYLINE(obj) (LwDictionaryLine*)obj
#define LW_TYPE_DICTIONARYLINE (lw_dictionaryline_get_type())

//Methods
LwDictionaryLine* lw_dictionaryline_new (const gchar *TEXT);
void lw_dictionaryline_free (LwDictionaryLine *self);
GType lw_dictionaryline_get_type (void);
LwDictionaryLine* lw_dictionaryline_copy (LwDictionaryLine *self);

//Methods
void lw_dictionaryline_set (LwDictionaryLine *self, gchar const *KEY, gchar const * parsed_text);
void lw_dictionaryline_set_strv (LwDictionaryLine *self, gchar const *KEY, gchar const * * parsed_strv);

gchar const * lw_dictionaryline_get (LwDictionaryLine *self, gchar const *KEY);
gchar * const * lw_dictionaryline_get_strv (LwDictionaryLine *self, gchar const *KEY);


G_END_DECLS

#endif

