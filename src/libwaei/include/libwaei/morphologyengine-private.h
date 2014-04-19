#ifndef LW_MORPHOLOGYENGINE_PRIVATE_INCLUDED
#define LW_MORPHOLOGYENGINE_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
  PROP_0,
  PROP_LOCALE,
  TOTAL_PROPS
} LwMorphologyEngineProps;


struct _LwMorphologyEnginePrivate {
#ifdef HAVE_MECAB
  mecab_t *mecab;
#endif
#ifdef HAVE_HUNSPELL
  Hunhandle *hunspell;
#endif
  gchar *locale;
};

struct _LwMorphologyEngineClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
};

#define LW_MORPHOLOGYENGINE_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_MORPHOLOGYENGINE, LwMorphologyEnginePrivate));

G_END_DECLS

#endif

