#ifndef LW_DICTIONARY_INCLUDED
#define LW_DICTIONARY_INCLUDED

#include "row.h"
#include "dictionarycache.h"
#include "progress.h"
#include "utf8.h"
#include "parsed.h"
#include "indexed.h"

G_BEGIN_DECLS

//Boilerplate
typedef struct _LwDictionary LwDictionary;
typedef struct _LwDictionaryClass LwDictionaryClass;
typedef struct _LwDictionaryPrivate LwDictionaryPrivate;
typedef struct _LwDictionaryClassPrivate LwDictionaryClassPrivate;

#define LW_TYPE_DICTIONARY              (lw_dictionary_get_type())
#define LW_DICTIONARY(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_DICTIONARY, LwDictionary))
#define LW_DICTIONARY_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_DICTIONARY, LwDictionaryClass))
#define LW_IS_DICTIONARY(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_DICTIONARY))
#define LW_IS_DICTIONARY_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_DICTIONARY))
#define LW_DICTIONARY_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_DICTIONARY, LwDictionaryClass))
#define LW_DICTIONARY_CHECKSUM G_CHECKSUM_SHA512 

struct _LwDictionary {
  GObject object;
  LwDictionaryPrivate *priv;
  Row row; 
};

struct _LwDictionaryClass {
  GObjectClass parent_class;
  LwDictionaryClassPrivate *priv;
};

typedef enum {
  LW_DICTIONARYCOLUMNHANDLING_UNUSED, //!< An unused field
  LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH, //!< Key is indexed and is included by default for all searches
  LW_DICTIONARYCOLUMNHANDLING_FILTER_ONLY, //!< Key is indexed, but is only included when queried explicitly
  TOTAL_DICTIONARYCOLUMNHANDLING
} LwDictionaryColumnHandling;
GType lw_dictionarycolumnhandling_get_type (void);
#define LW_TYPE_DICTIONARYCOLUMNHANDLING (lw_dictionarycolumnhandling_get_type ())


#define LW_DICTIONARYCOLUMNHANDLINGNAME_UNUSED "Unused"
#define LW_DICTIONARYCOLUMNHANDLINGNAME_INDEX_AND_SEARCH "Index and Search"
#define LW_DICTIONARYCOLUMNHANDLINGNAME_FILTER_ONLY "Filter Only"

#define LW_DICTIONARYCOLUMNHANDLINGNICK_UNUSED "unused"
#define LW_DICTIONARYCOLUMNHANDLINGNICK_INDEX_AND_SEARCH "index-and-search"
#define LW_DICTIONARYCOLUMNHANDLINGNICK_FILTER_ONLY "filter-only"


//Methods
LwDictionary * lw_dictionary_new (GType type, gchar const * FILENAME);
GType lw_dictionary_get_type (void) G_GNUC_CONST;

//Properties

gchar const* lw_dictionary_get_path (LwDictionary *self);
void lw_dictionary_set_filename (LwDictionary *self, gchar const *FILENAME);
gchar const* lw_dictionary_get_filename (LwDictionary *self);

gchar const* lw_dictionary_get_name (LwDictionary *self);

LwProgress* lw_dictionary_get_progress (LwDictionary *self);
void lw_dictionary_set_progress (LwDictionary *self, LwProgress *progress);

size_t lw_dictionary_length (LwDictionary *self);

gchar const* lw_dictionary_get_checksum (LwDictionary *self);
gchar const* lw_dictionary_get_contents (LwDictionary *self);

gint lw_dictionary_total_columns (LwDictionary *self);
gchar const * lw_dictionary_get_column_language (LwDictionary *self, gint column_num);
LwDictionaryColumnHandling lw_dictionary_get_column_handling (LwDictionary *self, gint column_num);

gboolean lw_dictionary_equals (LwDictionary *dictionary1, LwDictionary *dictionary2);

gchar** lw_dictionary_get_installed_idlist (GType type_filter);
gchar* lw_dictionary_build_id_from_type (GType type, gchar const *FILENAME);
gchar const* lw_dictionary_get_id (LwDictionary *self);

gchar* lw_dictionary_build_directory (GType type);

void lw_dictionary_set_cachetree (LwDictionary *self, GTree *tree);
GTree* lw_dictionary_get_cachetree (LwDictionary *self);

LwDictionaryCache* lw_dictionary_get_cache (LwDictionary *self, LwProgress *progress, LwUtf8Flag flags);
void lw_dictionary_set_cache (LwDictionary *self, LwDictionaryCache *cache);

gchar * const * lw_dictionary_lines (LwDictionary *self);
gint lw_dictionary_num_lines (LwDictionary *self);

gboolean  lw_dictionary_uninstall (LwDictionary *self);

LwParsed* lw_dictionary_parse (LwDictionary * self, LwCacheFile * cache_file, LwProgress * progress);

gchar const * lw_dictionary_get_install_directory ();
gchar * lw_dictionary_build_path_by_type_and_name (GType type, gchar const * FILENAME);


gint * lw_dictionary_calculate_applicable_columns_for_text (LwDictionary * self, gchar const * TEXT);

G_END_DECLS

#include <libwaei/dictionary-callbacks.h>
#include "search.h"

#endif
