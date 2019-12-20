#ifndef LW_DICTIONARY_INCLUDED
#define LW_DICTIONARY_INCLUDED

#include <gio/gio.h>
#include "dictionary-cache.h"
#include "progress.h"
#include "utf8.h"
#include "parsed.h"
#include "progress.h"
#include "list.h"

G_BEGIN_DECLS

//Boilerplate
#define LW_TYPE_DICTIONARY lw_dictionary_get_type ()
G_DECLARE_DERIVABLE_TYPE (LwDictionary, lw_dictionary, LW, DICTIONARY, LwList)

struct _LwDictionaryClass {
    LwList parent_class;

    //Virtual methods
    gsize (* count_lines) (gchar * buffer, gsize buffer_length, gsize * max_line_length, LwProgress * progress);
    GQuark (* get_column_language) (gint column_num);
    LwDictionaryColumnHandling (* get_column_handling) (gint column_num);
    gchar* (* columnize_line) (LwDictionary * self, gchar * buffer, gchar ** tokens, gsize * num_tokens);
    void (* load_columns) (LwDictionary * self, char * buffer, gchar ** tokens, gint num_tokens, LwParsedLine * line);
    gint* (* calculate_applicable_columns_for_text) (char const * TEXT);

    GType (* get_column_type) ();
    gint (* get_n_columns) ();
};

#define LW_DICTIONARY_CHECKSUM G_CHECKSUM_SHA512 

//Methods
LwDictionary * lw_dictionary_new (GType type, gchar const * FILENAME);

//Properties

void lw_dictionary_set_name (LwDictionary * self, gchar const * NAME);
gchar const * lw_dictionary_get_name (LwDictionary * self);

gchar const * lw_dictionary_get_contents_path (LwDictionary * self);
void lw_dictionary_set_contents_filename (LwDictionary * self, gchar const * FILENAME);
gchar const * lw_dictionary_get_contents_filename (LwDictionary * self);

size_t lw_dictionary_contents_length (LwDictionary * self);

gchar const * lw_dictionary_get_contents_checksum (LwDictionary * self);
gchar const * lw_dictionary_get_contents (LwDictionary * self);

gint lw_dictionary_total_columns (LwDictionaryClass * klass);
GQuark lw_dictionary_get_column_language (LwDictionaryClass * klass, gint column_num);
LwDictionaryColumnHandling lw_dictionary_get_column_handling (LwDictionaryClass * klass, gint column_num);

gchar const* lw_dictionary_get_id (LwDictionary * self);

gboolean  lw_dictionary_uninstall (LwDictionary * self);

gchar const * lw_dictionary_get_install_directory ();
gchar * lw_dictionary_build_contents_path_by_type_and_name (GType type, gchar const * FILENAME);


gint * lw_dictionary_calculate_applicable_columns_for_text (LwDictionaryClass * klass, gchar const * TEXT);
GType lw_dictionary_get_columnid_type (LwDictionaryClass * klass);

LwDictionaryCache * lw_dictionary_ensure_cache_by_utf8flags (LwDictionary * self, LwUtf8Flag flags, LwProgress * progress);

G_END_DECLS

#endif
