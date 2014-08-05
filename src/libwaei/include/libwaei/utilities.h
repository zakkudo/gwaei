#ifndef LW_UTILITIES_INCLUDED
#define LW_UTILITIES_INCLUDED

G_BEGIN_DECLS

typedef enum {
  LW_PATH_BASE, 
  LW_PATH_DICTIONARY,
  LW_PATH_PLUGIN,
  LW_PATH_CACHE,
  LW_PATH_INDEX,
  LW_PATH_VOCABULARY,
  TOTAL_LW_PATHS
} LwFolderPath;

gchar*  lw_util_build_filename (const LwFolderPath PATH, const char *FILENAME);
gboolean lw_util_is_japanese_locale (void);
gchar* lw_util_get_query_from_args (int argc, char** argv);
gchar* lw_util_get_environment_data_folder (void);

G_END_DECLS

#endif
