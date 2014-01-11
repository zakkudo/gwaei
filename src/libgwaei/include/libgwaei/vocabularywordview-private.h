#ifndef LGW_VOCABULARYWORDVIEW_PRIVATE_INCLUDED
#define LGW_VOCABULARYWORDVIEW_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
  PROP_0,
  PROP_VOCABULARYWORDSTORE,
  TOTAL_PROPS
} Prop;

struct _LgwVocabularyWordViewPrivate {
  LgwVocabularyWordStore *vocabulary_word_store;
};

struct _LgwVocabularyWordViewClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
};

#define LGW_VOCABULARYWORDVIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_VOCABULARYWORDVIEW, LgwVocabularyWordViewPrivate))

G_END_DECLS

#endif


