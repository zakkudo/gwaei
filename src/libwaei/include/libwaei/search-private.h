#ifndef LW_SEARCH_PRIVATE_INCLUDED
#define LW_SEARCH_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
    PROP_0,
    PROP_STATUS,
    PROP_DICTIONARY,
    PROP_QUERY,
    PROP_QUERY_TREE,
    PROP_FLAGS,
    PROP_MAX_RESULTS,
    PROP_PROGRESS,
    PROP_REGEX,
    TOTAL_PROPS
} Props;


typedef enum {
  SIGNALID_CHANGED,
  SIGNALID_INSERTED,
  SIGNALID_DELETED,
  SIGNALID_REORDERED,
  TOTAL_SIGNALIDS
} SignalId;


struct _LwSearchPrivate {
  gchar * query;
  LwQueryNode * query_tree;
  LwSearchStatus status;                  //!< Used to test if a search is in progress.
  GMutex mutex;
  LwDictionary * dictionary;
  LwProgress * progress;

  LwSearchFlag flags;
  gint max_results;

  GThread * thread;
  GList * results_buffer;
  guint watch_id;
  LwDictionaryCache * dictionary_cache;
};

struct _LwSearchClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
};


static void lw_search_set_results (LwSearch * self, LwResults * results);
static gboolean _apply_search_columns_to_query (LwQueryNode * self, LwSearch * search);
static void lw_search_search_parsed (LwSearch * self, LwParsed * parsed);
static LwParsed * lw_search_get_parsed (LwSearch *  self, GError ** error);
static void lw_search_set_flags (LwSearch * self, LwSearchFlag flags);

static void lw_search_set_dictionary (LwSearch * self, LwDictionary * dictionary);
static void lw_search_set_query (LwSearch * self, const gchar * QUERY);
static void lw_search_set_status (LwSearch * self, LwSearchStatus status);
static void lw_search_set_progress (LwSearch * self, LwProgress * progress);

static LwDictionaryCache * lw_search_ensure_dictionarycache (LwSearch * self);
static void lw_search_query_parsed (LwSearch * self, LwParsed * parsed);

static void lw_search_set_query_tree (LwSearch * self, LwQueryNode * query_tree);

G_END_DECLS

#endif




