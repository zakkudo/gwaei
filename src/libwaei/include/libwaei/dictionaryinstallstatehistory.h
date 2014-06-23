#ifndef LW_DICTIONARYINSTALLSTATE_INCLUDED
#define LW_DICTIONARYINSTALLSTATE_INCLUDED

G_BEGIN_DECLS


struct _LwDictionaryInstallState {
  gchar *name;
  gchar *path;
};


struct _LwDictionaryInstallStateHistory {
  GList *states;
};

typedef struct _LwDictionaryInstallState LwDictionaryInstallState;
typedef struct _LwDictionaryInstallStateHistory LwDictionaryInstallStateHistory;
typedef void (*LwDictionaryInstallStateHistoryForeachFunc) (LwDictionaryInstallStateHistory *self, LwDictionaryInstallState *state, gpointer user_data);


// Methods 

LwDictionaryInstallStateHistory* lw_dictionaryinstallstatehistory_new (void);
void lw_dictionaryinstallstatehistory_free (LwDictionaryInstallStateHistory *self);

void lw_dictionaryinstallstatehistory_add (LwDictionaryInstallStateHistory *self, LwDictionaryInstallState *state);


G_END_DECLS

#endif