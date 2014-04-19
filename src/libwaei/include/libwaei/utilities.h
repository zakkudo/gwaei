#ifndef LW_UTILITIES_INCLUDED
#define LW_UTILITIES_INCLUDED

G_BEGIN_DECLS

typedef enum {
  LW_NORMALIZATION_CASE_INSENSITIVE = (1 << 0),
  LW_NORMALIZATION_FURIGANA_INSENSITIVE = (1 << 1),
  LW_NORMALIZATION_INSENSITIVE = (LW_NORMALIZATION_CASE_INSENSITIVE | LW_NORMALIZATION_FURIGANA_INSENSITIVE)
} LwNormalizationFlags;

typedef enum {
  LW_PATH_BASE, 
  LW_PATH_DICTIONARY,
  LW_PATH_PLUGIN,
  LW_PATH_CACHE,
  LW_PATH_INDEX,
  LW_PATH_VOCABULARY,
  TOTAL_LW_PATHS
} LwFolderPath;


typedef enum {
//  LW_COMPRESSION_ZIP, //Unsupported since you can't tell what the file will be named
  LW_COMPRESSION_GZIP,
  LW_COMPRESSION_NONE,
  LW_COMPRESSION_TOTAL
} LwCompression;

typedef enum {
  LW_ENCODING_UTF8,
  LW_ENCODING_EUC_JP,
  LW_ENCODING_SHIFT_JS,
  LW_ENCODING_TOTAL
} LwEncoding;


gchar* lw_util_build_filename (const LwFolderPath, const char*);
const char* lw_util_get_compressionname (const LwCompression);
const char* lw_util_get_encodingname (const LwEncoding);


const char* lw_util_next_hira_char_from_roma (const char*);
char* lw_util_roma_to_hira (const char*, char*);
gboolean lw_util_str_roma_to_hira (const char*, char*, int);

gboolean lw_util_is_hiragana_str (const char*);
gboolean lw_util_is_util_kanji_str (const char*);
gboolean lw_util_is_katakana_str (const char*);
gboolean lw_util_is_romaji_str (const char*);
gboolean lw_util_is_kanji_ish_str (const char*);
gboolean lw_util_is_kanji_str (const char*);
gboolean lw_util_is_furigana_str (const char*);
gboolean lw_util_is_yojijukugo_str (const const char*);
gboolean lw_util_string_has_japanese (const gchar *INPUT);

void lw_util_str_shift_kata_to_hira (char*);
void lw_util_str_shift_hira_to_kata (char*);

gchar* lw_util_furiganafold (const gchar*);

gboolean lw_util_all_chars_are_in_range (char*, int, int);

void lw_util_sanitize_string (gchar *buffer);

gchar* lw_util_normalize_string (const gchar *TEXT, LwNormalizationFlags flags);

gboolean lw_util_contains_halfwidth_japanese (const gchar*);
gchar* lw_util_enlarge_halfwidth_japanese (const gchar*);

gboolean lw_util_is_japanese_locale (void);

gchar* lw_util_get_query_from_args (int, char**);

gchar* lw_strjoinv (gchar, gchar**, gint);
gchar* lw_util_collapse_string (const gchar*);

gchar* lw_util_delimit_script_changes (const gchar*, const gchar*, gboolean);
gchar* lw_util_delimit_whitespace (const gchar*, const gchar*);
gchar* lw_util_delimit_radicals (const gchar*, const gchar*);

GRegex* lw_regex_new (const gchar*, const gchar*, GError**);

gchar* lw_util_get_environment_data_folder (void);

gboolean lw_util_is_regex_pattern (const gchar *TEXT, GError **error);

gint lw_util_replace_linebreaks_with_nullcharacter (gchar *TEXT);
gchar* lw_util_convert_printf_pattern_to_regex_pattern (const gchar *PATTERN);
gint* lw_util_get_numbers (const gchar *TEXT);


G_END_DECLS

#endif
