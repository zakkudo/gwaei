#ifndef LW_DICTIONARYCACHE_INCLUDED
#define LW_DICTIONARYCACHE_INCLUDED

#include "dictionaryline.h"

G_BEGIN_DECLS

#define LW_DICTIONARYCACHE_ERROR lw_dictionarycache_error_quark ()

typedef enum {
  LW_DICTIONARYCACHE_ERRORCODE_CORRUPT_CONTENTS
} LwDictionaryCacheErrorCode;

//Boilerplate
typedef struct _LwDictionaryCache LwDictionaryCache;
typedef struct _LwDictionaryCacheClass LwDictionaryCacheClass;
typedef struct _LwDictionaryCachePrivate LwDictionaryCachePrivate;
typedef struct _LwDictionaryCacheClassPrivate LwDictionaryCacheClassPrivate;

#define LW_TYPE_DICTIONARYCACHE              (lw_dictionarycache_get_type())
#define LW_DICTIONARYCACHE(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_DICTIONARYCACHE, LwDictionaryCache))
#define LW_DICTIONARYCACHE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_DICTIONARYCACHE, LwDictionaryCacheClass))
#define LW_IS_DICTIONARYCACHE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_DICTIONARYCACHE))
#define LW_IS_DICTIONARYCACHE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_DICTIONARYCACHE))
#define LW_DICTIONARYCACHE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_DICTIONARYCACHE, LwDictionaryCacheClass))

typedef LwParsed*(*LwDictionaryCacheParseFunc)(gchar*, gsize, gpointer);

struct _LwDictionaryCache {
    GObject object;
    LwDictionaryCachePrivate *priv;
};

struct _LwDictionaryCacheClass {
  GObjectClass parent_class;
  LwDictionaryCacheClassPrivate *priv;
};


//Methods
LwDictionaryCache* lw_dictionarycache_new (const gchar *NAME, LwUtf8Flag flags);
GType lw_dictionarycache_get_type (void) G_GNUC_CONST;

LwParsed* lw_dictionarycache_get_lines (LwDictionaryCache *self);

void lw_dictionarycache_set_contents (LwDictionaryCache *self, gchar const *CHECKSUM, gchar const *contents, gsize content_length, LwDictionaryCacheParseFunc parse, gpointer data, LwProgress *progress);
const gchar* lw_dictionarycache_get_contents (LwDictionaryCache *self);

gboolean lw_dictionarycache_validate (LwDictionaryCache *self, const gchar *EXPECTED_CHECKSUM, LwProgress *progress);
gboolean lw_dictionarycache_is_modifiable (LwDictionaryCache *self);

void lw_dictionarycache_write (LwDictionaryCache *self, const gchar *CHECKSUM, gchar const *CONTENTS, gsize content_length, LwDictionaryCacheParseFunc parse, gpointer data, LwProgress *progress);
void lw_dictionarycache_read (LwDictionaryCache *self, gchar const *EXPECTED_CHECKSUM, LwProgress *progress);

//Properties
LwProgress* lw_dictionarycache_get_progress (LwDictionaryCache *self);
void lw_dictionarycache_set_progress (LwDictionaryCache *self, LwProgress *progress);

LwUtf8Flag lw_dictionarycache_get_flags (LwDictionaryCache *self);


const gchar* lw_dictionarycache_get_checksum (LwDictionaryCache *self);

gsize lw_dictionarycache_length (LwDictionaryCache *self);

void lw_dictionarycache_set_name (LwDictionaryCache *self, const gchar *NAME);
const gchar* lw_dictionarycache_get_name (LwDictionaryCache *self);

gchar* lw_dictionarycache_build_filename (LwDictionaryCache *self, gchar const *TYPE);
gchar* lw_dictionarycache_build_path (LwDictionaryCache *self, gchar const *TYPE);
void lw_dictionarycache_sync_path (LwDictionaryCache *self);

void lw_dictionarycache_set_path (LwDictionaryCache *self, const gchar *PATH);
const gchar* lw_dictionarycache_get_path (LwDictionaryCache *self);

void lw_dictionarycache_set_parsed (LwDictionaryCache *self, LwParsed *parsed);
LwParsed* lw_dictionarycache_get_parsed (LwDictionaryCache *self);

LwIndexed* lw_dictionarycache_get_indexed (LwDictionaryCache *self);
void lw_dictionarycache_set_indexed (LwDictionaryCache *self, LwIndexed *indexed);

void lw_dictionarycache_set_normalized_cachefile (LwDictionaryCache *self, LwCacheFile *cachefile);
LwCacheFile* lw_dictionarycache_get_normalized_cachefile (LwDictionaryCache *self);

void lw_dictionarycache_set_normalized_cachefile (LwDictionaryCache *self, LwCacheFile *cachefile);
LwCacheFile* lw_dictionarycache_get_normalized_cachefile (LwDictionaryCache *self);

LwCacheFile* lw_dictionarycache_get_parsed_cachefile (LwDictionaryCache *self);
void lw_dictionarycache_set_parsed_cachefile (LwDictionaryCache *self, LwCacheFile *cachefile);

void lw_dictionarycache_set_indexed_cachefile (LwDictionaryCache *self, LwCacheFile *cachefile);
LwCacheFile* lw_dictionarycache_get_indexed_cachefile (LwDictionaryCache *self);

void lw_dictionarycache_clear (LwDictionaryCache *self);

gchar* lw_dictionarycache_write_normalized_temporary_file (LwDictionaryCache *self, const gchar *CONTENTS, gsize content_length, LwProgress  *progress);

G_END_DECLS

#endif
