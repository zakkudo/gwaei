#ifndef LW_DICTIONARYCACHE_PRIVATE_INCLUDED
#define LW_DICTIONARYCACHE_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
    PROP_0,
    PROP_NAME,
    PROP_CONTENTS,
    PROP_LENGTH,
    PROP_FLAGS,
    PROP_PATH,
    TOTAL_PROPS
} Props;


struct _LwDictionaryCachePrivate {
  LwCacheFile *cache_file;
  LwUtf8Flag flags;
  LwDictionaryLines *lines;
  gchar *contents;
  gsize length;
};

struct _LwDictionaryCacheClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
};

#define LW_DICTIONARYCACHE_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_DICTIONARYCACHE, LwDictionaryCachePrivate));

//Methods

void lw_dictionarycache_set_flags (LwDictionaryCache *self, LwUtf8Flag flags);

G_END_DECLS

#endif


