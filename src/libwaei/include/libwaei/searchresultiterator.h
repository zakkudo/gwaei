#ifndef LW_SEARCHRESULTITERATOR_INCLUDED
#define LW_SEARCHRESULTITERATOR_INCLUDED

G_BEGIN_DECLS

#define LW_SEARCHRESULTITERATOR(object) (LwSearchResultIterator*) object

struct _LwSearchResultIterator {
  LwSearch *search;
  gchar *category;
  GList *list;
  GList *link;
  gint count;
  GHashTable *historytable;
};

typedef struct _LwSearchResultIterator LwSearchResultIterator;

LwSearchResultIterator* lw_searchresultiterator_new (LwSearch *search, const gchar *CATEGORY);
LwResult* lw_searchresultiterator_get_result (LwSearchResultIterator* iterator);
gboolean lw_searchresultiterator_next (LwSearchResultIterator* iterator);
void lw_searchresultiterator_rewind (LwSearchResultIterator* iterator);
void lw_searchresultiterator_free (LwSearchResultIterator *iterator);
void lw_searchresultiterator_free_full (LwSearchResultIterator *iterator);
gint lw_searchresultiterator_length (LwSearchResultIterator *iterator);
gint lw_searchresultiterator_count (LwSearchResultIterator *iterator);

gboolean lw_searchresultiterator_finished (LwSearchResultIterator *iterator);
gboolean lw_searchresultiterator_empty (LwSearchResultIterator *iterator);

G_END_DECLS

#endif

