#ifndef LW_RESULTS_PRIVATE_INCLUDED
#define LW_RESULTS_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
    PROP_0,
    PROP_SEQUENCE,
    PROP_DICTIONARYCACHE,
    TOTAL_PROPS
} Props;


typedef enum {
  CLASS_SIGNALID_ROW_CHANGED,
  CLASS_SIGNALID_ROW_INSERTED,
  CLASS_SIGNALID_ROW_DELETED,
  CLASS_SIGNALID_ROWS_REORDERED,
  TOTAL_CLASS_SIGNALIDS
} ClassSignalId;


typedef enum {
  SIGNALID_UNUSED,
  TOTAL_SIGNALIDS
} SignalId;


struct _LwResultsPrivate {
  GSequence * sequence;
  LwDictionaryCache * dictionary_cache;
};

struct _LwResultsClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
  guint signalid[TOTAL_CLASS_SIGNALIDS];
};


#define LW_RESULTS_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_RESULTS, LwResultsPrivate));

G_END_DECLS

#endif





