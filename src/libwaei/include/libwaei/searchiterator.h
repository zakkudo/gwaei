#ifndef LW_SEARCHITERATOR_INCLUDED
#define LW_SEARCHITERATOR_INCLUDED

G_BEGIN_DECLS

#define LW_SEARCHITERATOR(object) (LwSearchIterator*) object

struct _LwSearchIterator {
  LwSearch *search;
  gchar *category;
  GList *list;
  GList *link;
  gint count;
  GHashTable *historytable;
};

typedef struct _LwSearchIterator LwSearchIterator;

LwSearchIterator* lw_searchiterator_new (LwSearch *search, const gchar *CATEGORY);
LwResult* lw_searchiterator_get_result (LwSearchIterator* iterator);
gboolean lw_searchiterator_next (LwSearchIterator* iterator);
void lw_searchiterator_rewind (LwSearchIterator* iterator);
void lw_searchiterator_free (LwSearchIterator *iterator);
void lw_searchiterator_free_full (LwSearchIterator *iterator);
gint lw_searchiterator_length (LwSearchIterator *iterator);
gint lw_searchiterator_count (LwSearchIterator *iterator);

gboolean lw_searchiterator_finished (LwSearchIterator *iterator);
gboolean lw_searchiterator_empty (LwSearchIterator *iterator);

G_END_DECLS

#endif

