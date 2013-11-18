#ifndef LW_MORPHOLOGY_INCLUDED
#define LW_MORPHOLOGY_INCLUDED 



//!
//! @brief Morphological analysis of input
//!
struct _LwMorphology {
  gchar *word;           //!< Original word
  gchar *normalized;     //!< Case and furigana insensitive form of the word.  Not set if same as word
  gchar *stem;           //!< Deduced (most likely) dictionary form of the word. NULL if no result. Not set if same as word
  gchar *canonical;      //!< Normalized and stem form of the word. Not set if same as normalized or stem
  gchar *spellcheck;     //!< Spellchecking results delimeted by ;.  NULL if is Japanese or it seems correct.
  gchar *explanation;    //!< Free-form explanation of the morphological analysis. NULL if none.
  gint start_offset;     //!< Bytes from the beginning of the string for the original word
  gint end_offset;       //!< Byte length of the original word
  gchar *regex_pattern;
  GRegex *regex;
};
typedef struct _LwMorphology LwMorphology;
#define LW_MORPHOLOGY(obj) (LwMorphology*)obj

//Methods
LwMorphology* lw_morphology_new (gchar *word, gchar *normalized, gchar *stem, gchar *canonical, gchar *spellcheck, gchar *explanation, gint start_offset, gint end_offset);
void lw_morphology_free (LwMorphology*);

const gchar* lw_morphology_get_raw (LwMorphology *morphology);
const gchar* lw_morphology_get_normalized (LwMorphology *morphology);
const gchar* lw_morphology_get_stem (LwMorphology *morphology);
const gchar* lw_morphology_get_canonical (LwMorphology *morphology);

gchar* lw_morphology_to_string (LwMorphology *morphology);
const gchar* lw_morphology_get_regex_pattern (LwMorphology *morphology);
GRegex* lw_morphology_get_regex (LwMorphology *morphology, GError **error);

GList* lw_morphology_get_morphologyindexlist (LwMorphology *morphology, const gchar *HAYSTACK, gint index);
gboolean lw_morphology_regex_search (LwMorphology *morphology, const gchar *HAYSTACK, GMatchInfo **match_info);

G_END_DECLS

#endif

#include "morphologylist.h"
#include "morphologyengine.h"
