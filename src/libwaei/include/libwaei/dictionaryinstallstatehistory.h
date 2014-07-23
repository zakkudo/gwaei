#ifndef LW_DICTIONARYINSTALLSTATE_INCLUDED
#define LW_DICTIONARYINSTALLSTATE_INCLUDED

G_BEGIN_DECLS

struct _LwDictionaryInstallStateFile {
  gchar *path;
  gchar *basename;
  const gchar *SUFFIX;
  gchar *suffixless;
};

typedef struct _LwDictionaryInstallStateFile LwDictionaryInstallStateFile;

struct _LwDictionaryInstallState {
  gchar *name;
  LwDictionaryInstallStateFile **files;
  gint length;
};


struct _LwDictionaryInstallStateHistory {
  GList *states;
};

typedef struct _LwDictionaryInstallState LwDictionaryInstallState;
typedef struct _LwDictionaryInstallStateHistory LwDictionaryInstallStateHistory;

typedef void (*LwDictionaryInstallStateHistoryForeachFunc) (LwDictionaryInstallStateHistory *self, LwDictionaryInstallState *state, gpointer user_data);


// Methods 

LwDictionaryInstallState* lw_dictionaryinstallstate_new_glist (const gchar *NAME, GList *paths);

LwDictionaryInstallStateHistory* lw_dictionaryinstallstatehistory_new (void);
void lw_dictionaryinstallstatehistory_free (LwDictionaryInstallStateHistory *self);

void lw_dictionaryinstallstatehistory_add (LwDictionaryInstallStateHistory *self, LwDictionaryInstallState *state);
void lw_dictionaryinstallstatehistory_add_paths (LwDictionaryInstallStateHistory *self, const gchar *NAME, GList *paths);

LwDictionaryInstallState* lw_dictionaryinstallstatehistory_current (LwDictionaryInstallStateHistory *self);

void lw_dictionaryinstallstatehistory_clear (LwDictionaryInstallStateHistory *self);

G_END_DECLS

#endif
