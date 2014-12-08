#ifndef LW_REGEX_INCLUDED
#define LW_REGEX_INCLUDED

G_BEGIN_DECLS

void lw_regex_initialize (void);
void lw_regex_free (void);

typedef enum {
  LW_RE_NUMBER,
  LW_RE_STROKES,
  LW_RE_GRADE,
  LW_RE_FREQUENCY,
  LW_RE_JLPT,
  LW_RE_PARENTHESES,
  LW_RE_KANJI_DICTIONARY_SPACERS,
  LW_RE_JAPANESE,
  LW_RE_ENGLISH,
  LW_RE_SECTION,
  LW_RE_CONTIGUOUS,
  LW_RE_TOTAL
} LwRegexDataIndex;

gchar* lw_regex_remove_parenthesis (const gchar* TEXT);
gchar* lw_regex_remove_kanji_dictionary_spacers (const gchar* TEXT);

gboolean lw_regex_get_japanese_matches (const gchar *HAYSTACK, GMatchInfo **match_info);
gboolean lw_regex_get_sections (const gchar *HAYSTACK, GMatchInfo **match_info);
gboolean lw_regex_get_contiguous (const gchar *HAYSTACK, GMatchInfo **match_info);

G_END_DECLS

#endif
