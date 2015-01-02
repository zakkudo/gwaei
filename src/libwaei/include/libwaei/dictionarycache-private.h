#ifndef LW_DICTIONARYCACHE_PRIVATE_INCLUDED
#define LW_DICTIONARYCACHE_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
    PROP_0,
    PROP_NAME,
    PROP_FLAGS,
    PROP_PARSED,
    PROP_INDEXED,
    TOTAL_PROPS
} Props;


struct _LwDictionaryCachePrivate {
  gchar *name;
  LwUtf8Flag flags;
  LwParsed *parsed;
  LwIndexed *indexed;
};

struct _LwDictionaryCacheClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
};

#define LW_DICTIONARYCACHE_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_DICTIONARYCACHE, LwDictionaryCachePrivate));

//Methods

static void lw_dictionarycache_set_flags (LwDictionaryCache *self, LwUtf8Flag flags);

static gchar* lw_dictionarycache_write_normalized_temporary_file (LwDictionaryCache *self, const gchar *CONTENTS, gsize content_length, LwProgress  *progress);

static void lw_dictionarycache_clear (LwDictionaryCache *self);

static void lw_dictionarycache_set_name (LwDictionaryCache *self, const gchar *NAME);

G_END_DECLS

#endif


