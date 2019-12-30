#ifndef LW_DICTIONARY_CACHE_INCLUDED
#define LW_DICTIONARY_CACHE_INCLUDED

#include "utf8.h"
#include "parsed.h"
#include "progress.h"

G_BEGIN_DECLS

#define LW_DICTIONARY_CACHE_ERROR lw_dictionary_cache_error_quark ()

typedef enum {
  LW_DICTIONARY_CACHE_ERRORCODE_CORRUPT_CONTENTS
} LwDictionaryCacheErrorCode;

//Boilerplate
#define LW_TYPE_DICTIONARY_CACHE lw_dictionary_cache_get_type ()
G_DECLARE_FINAL_TYPE (LwDictionaryCache, lw_dictionary_cache, LW, DICTIONARY_CACHE, GObject)

typedef LwParsed*(*LwDictionaryCacheParseFunc)(LwCacheFile * cache_file, gpointer data);

//Properties
LwUtf8Flag lw_dictionary_cache_get_flags (LwDictionaryCache * self);
gchar const * lw_dictionary_cache_get_name (LwDictionaryCache * self);
GType lw_dictionary_cache_get_dictionary_type (LwDictionaryCache * self);
LwProgress * lw_dictionary_cache_get_progress (LwDictionaryCache * self);

//Methods
LwDictionaryCache * lw_dictionary_cache_new (gchar const * NAME, GType * dictionary_type, LwUtf8Flag flags);

void lw_dictionary_cache_write (LwDictionaryCache * self, gchar const * CHECKSUM, gchar const * CONTENTS, gsize content_length, LwDictionaryCacheParseFunc parse, gpointer data, LwProgress * progress);
gboolean lw_dictionary_cache_read (LwDictionaryCache * self, gchar const * EXPECTED_CHECKSUM, LwProgress * progress);
gboolean lw_dictionary_cache_trylock (LwDictionaryCache * self);
void lw_dictionary_cache_lock (LwDictionaryCache * self);
void lw_dictionary_cache_unlock (LwDictionaryCache * self);

G_END_DECLS

#endif
