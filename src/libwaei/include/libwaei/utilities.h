#ifndef LW_UTILITIES_INCLUDED
#define LW_UTILITIES_INCLUDED

G_BEGIN_DECLS

gboolean lw_util_is_japanese_locale (void);
gchar* lw_util_get_query_from_args (int argc, char** argv);
gchar* lw_util_get_environment_data_folder (void);

G_END_DECLS

#endif
