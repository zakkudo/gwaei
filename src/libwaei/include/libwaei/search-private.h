#ifndef LW_SEARCH_PRIVATE_INCLUDED
#define LW_SEARCH_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
    PROP_0,
    PROP_STATUS,
    PROP_DICTIONARY,
    PROP_QUERY,
    PROP_FLAGS,
    PROP_MAX_RESULTS,
    PROP_RESULTS,
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
  LwSearchStatus status;                  //!< Used to test if a search is in progress.
  //LwMorphologyEngine *morphologyengine;
  GMutex mutex;
  GThread * thread;
  LwDictionary *dictionary;
  LwResults * results;
  LwProgress *progress;

  LwSearchFlag flags;
  gint max_results;
};

struct _LwSearchClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
};


struct _LwSearchData {
    LwSearch * search;
    LwQueryNode * root;
};
typedef struct _LwSearchData LwSearchData;


#define LW_SEARCH_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_SEARCH, LwSearchPrivate));

static void lw_search_set_results (LwSearch * self, LwResults * results);
static LwSearchData * lw_searchdata_new (LwSearch * search, GError ** error);
static void lw_searchdata_free (LwSearchData * self);
static gboolean _apply_search_columns_to_query (LwQueryNode * self, LwSearchData * data);
static void lw_searchdata_search_parsed (LwSearchData * self, LwParsed * parsed);
static LwParsed * lw_search_get_parsed (LwSearch *  self, GError ** error);
static void lw_search_set_flags (LwSearch * self, LwSearchFlag flags);

G_END_DECLS

#endif




