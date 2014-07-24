#ifndef LW_DICTIONARYINSTALLSTATE_INCLUDED
#define LW_DICTIONARYINSTALLSTATE_INCLUDED

G_BEGIN_DECLS

struct _LwDictionaryInstallState {
  gchar *name;
  LwFile **files;
  gint length;
  gboolean is_temporary;
};


struct _LwDictionaryInstallStateHistory {
  GList *states;
};

typedef struct _LwDictionaryInstallState LwDictionaryInstallState;
typedef struct _LwDictionaryInstallStateHistory LwDictionaryInstallStateHistory;

typedef void (*LwDictionaryInstallStateHistoryForeachFunc) (LwDictionaryInstallStateHistory *self, LwDictionaryInstallState *state, gpointer user_data);


// Methods 

LwDictionaryInstallState* lw_dictionaryinstallstate_new_glist (const gchar *NAME, gboolean is_temporary, GList *paths);

LwDictionaryInstallStateHistory* lw_dictionaryinstallstatehistory_new (void);
void lw_dictionaryinstallstatehistory_free (LwDictionaryInstallStateHistory *self);

void lw_dictionaryinstallstatehistory_add (LwDictionaryInstallStateHistory *self, LwDictionaryInstallState *state);
void lw_dictionaryinstallstatehistory_add_paths (LwDictionaryInstallStateHistory *self, const gchar *NAME, gboolean is_temporary, GList *paths);
LwDictionaryInstallState* lw_dictionaryinstallstatehistory_current (LwDictionaryInstallStateHistory *self);
void lw_dictionaryinstallstatehistory_clear (LwDictionaryInstallStateHistory *self);
void lw_dictionaryinstallstatehistory_foreach (LwDictionaryInstallStateHistory *self, LwDictionaryInstallStateHistoryForeachFunc func, gpointer user_data);

G_END_DECLS

#endif
