#ifndef LW_VOCABULARY_PRIVATE_INCLUDED
#define LW_VOCABULARY_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
    PROP_0,
    PROP_FILENAME,
    PROP_CHANGED,
    TOTAL_PROPS
} Props;


typedef enum {
  CLASS_SIGNALID_CHANGED,
  CLASS_SIGNALID_INSERTED,
  CLASS_SIGNALID_DELETED,
  CLASS_SIGNALID_REORDERED,
  TOTAL_CLASS_SIGNALIDS
} ClassSignalId;


struct _LwVocabularyPrivate {
  gchar *filename;
  GList *list; //<!-- A Glist of LwWord
  gboolean changed; //<!-- hint of there are any savable changes
  gdouble progress;
};

struct _LwVocabularyClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
  guint signalid[TOTAL_CLASS_SIGNALIDS];
};

#define LW_VOCABULARY_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_VOCABULARY, LwVocabularyPrivate));

G_END_DECLS

#endif


