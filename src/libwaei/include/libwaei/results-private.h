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
  gint length;
};

struct _LwResultsClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
  guint signalid[TOTAL_CLASS_SIGNALIDS];
};


static void lw_results_set_dictionarycache (LwResults * self, LwDictionaryCache * dictionary_cache);
static void lw_results_set_sequence (LwResults * self, GSequence * sequence);

static gint * lw_results_normalize_indices (LwResults * self);

G_END_DECLS

#endif





