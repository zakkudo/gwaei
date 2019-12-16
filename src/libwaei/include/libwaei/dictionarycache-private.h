#ifndef LW_DICTIONARYCACHE_PRIVATE_INCLUDED
#define LW_DICTIONARYCACHE_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
    PROP_0,
    PROP_FLAGS,
    PROP_PARSED,
    PROP_INDEXED,
    PROP_DICTIONARY_NAME,
    PROP_DICTIONARY_TYPE,
    PROP_PROGRESS,
    TOTAL_PROPS
} Props;


struct _LwDictionaryCachePrivate {
  GType dictionary_type;
  gchar *dictionary_name;
  LwUtf8Flag flags;
  LwParsed *parsed;
  LwIndexed *indexed;
  LwProgress * progress;
  GMutex mutex;
};

struct _LwDictionaryCacheClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
};

//Methods

static void lw_dictionarycache_set_flags (LwDictionaryCache *self, LwUtf8Flag flags);

static gchar* lw_dictionarycache_write_normalized_temporary_file (LwDictionaryCache *self, gchar const *CHECKSUM, gchar const *CONTENTS, gsize content_length, LwProgress  *progress);

static void lw_dictionarycache_clear (LwDictionaryCache *self);

static void lw_dictionarycache_set_dictionary_name (LwDictionaryCache *self, gchar const *DICTIONARY_NAME);

static void lw_dictionarycache_set_parsed (LwDictionaryCache *self, LwParsed *parsed);
static void lw_dictionarycache_set_indexed (LwDictionaryCache *self, LwIndexed *indexed);

static gchar* lw_dictionarycache_build_filename (LwDictionaryCache *self, gchar const *TYPE);
static gchar* lw_dictionarycache_build_path (LwDictionaryCache *self, gchar const *TYPE);

static void lw_dictionarycache_set_progress (LwDictionaryCache * self, LwProgress * progress);
static void lw_dictionarycache_set_dictionary_type (LwDictionaryCache * self, GType dictionary_type);

G_END_DECLS

#endif


