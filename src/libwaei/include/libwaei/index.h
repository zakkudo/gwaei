#ifndef LW_INDEX_INCLUDED
#define LW_INDEX_INCLUDED

G_BEGIN_DECLS

typedef guint32 LwOffset;
typedef LwOffset LwOffsets;

G_END_DECLS

#include "io.h"
#include "dictionarydata.h"

G_BEGIN_DECLS

#define LW_INDEX_CHECKSUM_TYPE G_CHECKSUM_SHA256
#define LW_OFFSET_FORMAT G_GUINT32_FORMAT
#define GPOINTER_TO_OFFSET(x) GPOINTER_TO_UINT(x)
#define LW_OFFSET_TO_POINTER(x) GUINT_TO_POINTER(x)

typedef enum {
  LW_INDEX_TABLE_RAW,        //< Level 0 (The raw form of the world) 
  LW_INDEX_TABLE_NORMALIZED, //< Level 1 (The case/furigana insensitive form of the word)
  LW_INDEX_TABLE_STEM,       //< Level 1 (The unconjugated form of the word)
  LW_INDEX_TABLE_CANONICAL,  //< Level 3 (combines all of the above)
  TOTAL_LW_INDEX_TABLES,
  LW_INDEX_TABLE_INVALID
} LwIndexTableType;

typedef enum {
  LW_INDEX_FLAG_RAW = (1 << 0),
  LW_INDEX_FLAG_CASE_INSENSITIVE  = (1 << 1),
  LW_INDEX_FLAG_FURIGANA_INSENSITIVE  = (1 << 1),
  LW_INDEX_FLAG_STEM_INSENSITIVE = (1 << 2),
  LW_INDEX_FLAG_POPULAR_ONLY = (1 << 3),
  LW_INDEX_FLAG_NORMALIZED = (LW_INDEX_FLAG_CASE_INSENSITIVE | LW_INDEX_FLAG_FURIGANA_INSENSITIVE),
  LW_INDEX_FLAG_CANONICAL = (LW_INDEX_FLAG_CASE_INSENSITIVE | LW_INDEX_FLAG_FURIGANA_INSENSITIVE | LW_INDEX_FLAG_STEM_INSENSITIVE)
} LwIndexFlag;

struct _LwIndex {
  gchar *buffer[TOTAL_LW_INDEX_TABLES];
  const gchar *checksum;
  GHashTable *table[TOTAL_LW_INDEX_TABLES];
  gchar *path;
  LwMorphologyEngine *morphologyengine;
};
typedef struct _LwIndex LwIndex;


LwIndex* lw_index_new (LwMorphologyEngine *morphologyengine);
void lw_index_free (LwIndex *index);

GList* lw_index_search (LwIndex *index, LwMorphologyString *morphologystring, LwIndexFlag flags, LwDictionaryData *dictionarydata);
gboolean lw_index_data_is_valid (LwIndex *index, LwDictionaryData *dictionarydata);

void lw_index_append_data_offset (LwIndex *index, const gchar *KEY, LwOffset offset);
LwOffsets* lw_index_get_data_offsets (LwIndex *index, const gchar *KEY);
LwOffset lw_index_get_data_offsets_length (LwIndex *index, const gchar *KEY);

void lw_index_add_string (LwIndex *index, const gchar *TEXT, LwOffset offset);

gboolean lw_index_are_equal (LwIndex *index1, LwIndex *index2);

void lw_index_write (LwIndex *index, const gchar* PATH, LwProgress *progress);
void lw_index_read (LwIndex *index, const gchar* PATH, LwProgress *progress);
void lw_index_create (LwIndex *index, LwDictionaryData *dictionarydata, LwProgress *progress);

gboolean lw_index_exists (const gchar *PATH);

GList* lw_index_get_matches_for_morphologylist (LwIndex *index, LwIndexTableType type, LwMorphologyString *morphologystring);

const gchar* lw_index_table_type_to_string (LwIndexTableType type);

G_END_DECLS

#endif
