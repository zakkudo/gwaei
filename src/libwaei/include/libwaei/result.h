#ifndef LW_RESULT_INCLUDED
#define LW_RESULT_INCLUDED 

G_BEGIN_DECLS

struct _LwResult {
  GHashTable *data;
  GList *lexicon;
  gboolean common;
};

typedef struct _LwResult LwResult;
#define LW_RESULT(obj) (LwResult*)obj

//Methods
LwResult* lw_result_new (void);
void lw_result_free (LwResult *self);

void lw_result_take_data (LwResult *self, const gchar *KEY, GList *data);
GList* lw_result_get_data (LwResult *self, const gchar *KEY);

void lw_result_take_lexicon (LwResult *self, GList *lexicon);
GList* lw_result_get_lexicon (LwResult *self);

G_END_DECLS

#endif

