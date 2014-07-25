#ifndef LW_DICTIONARYINSTALLSTATE_INCLUDED
#define LW_DICTIONARYINSTALLSTATE_INCLUDED

G_BEGIN_DECLS

typedef struct _LwDictionaryInstallState LwDictionaryInstallState;

#define LW_DICTIONARYINSTALLSTATE(obj) (LwDictionaryInstallState*)obj

typedef void (*LwDictionaryInstallStateForeachFunc) (LwDictionaryInstallState *state, LwFilePath *filepath, gpointer user_data);

// Methods 

LwDictionaryInstallState* lw_dictionaryinstallstate_new_glist (const gchar *NAME, gboolean is_temporary, GList *paths);
void lw_dictionaryinstallstate_free (LwDictionaryInstallState *self);
void lw_dictionaryinstallstate_foreach (LwDictionaryInstallState *self, LwDictionaryInstallStateForeachFunc func, gpointer user_data);

void lw_dictionaryinstallstate_remove_files (LwDictionaryInstallState *self);

LwFilePath** lw_dictionaryinstallstate_get_filepaths (LwDictionaryInstallState *self, gint *length);
gint lw_dictionaryinstallstate_length (LwDictionaryInstallState *self);

gboolean lw_dictionaryinstallstate_is_temporary (LwDictionaryInstallState *self);

G_END_DECLS

#endif

