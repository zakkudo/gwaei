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

typedef enum {
  CLASS_SIGNALID_ROW_CHANGED,
  CLASS_SIGNALID_ROW_INSERTED,
  CLASS_SIGNALID_ROW_DELETED,
  CLASS_SIGNALID_ROWS_REORDERED,
  TOTAL_CLASS_SIGNALIDS
} ClassSignalId;

struct _Data {
  LwMorphologyString *query;
  LwSearchStatus status;                  //!< Used to test if a search is in progress.
  LwMorphologyEngine *morphologyengine;
  GMutex mutex;
  GThread *thread;
  LwDictionary *dictionary;
  GQueue *results;
  LwProgress *progress;

  GRegex *regex;
};

struct _Config {
  LwSearchFlag flags;
  gint max_results;
};

struct _LwSearchPrivate {
  struct _Data data;
  struct _Config config;
};

struct _LwSearchClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
  guint signalid[TOTAL_CLASS_SIGNALIDS];
};

#define LW_SEARCH_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_SEARCH, LwSearchPrivate));

G_END_DECLS

#endif




