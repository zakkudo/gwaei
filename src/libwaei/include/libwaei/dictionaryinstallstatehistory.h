#ifndef LW_DICTIONARYINSTALLSTATEHISTORY_INCLUDED
#define LW_DICTIONARYINSTALLSTATEHISTORY_INCLUDED

#include "dictionaryinstallstate.h"

G_BEGIN_DECLS

typedef struct _LwDictionaryInstallStateHistory LwDictionaryInstallStateHistory;

#define LW_DICTIONARYINSTALLSTATEHISTORY(obj) (LwDictionaryInstallStateHistory*)obj

typedef void (*LwDictionaryInstallStateHistoryForeachFunc) (LwDictionaryInstallStateHistory *self, LwDictionaryInstallState *state, gpointer user_data);

// Methods 

LwDictionaryInstallStateHistory* lw_dictionaryinstallstatehistory_new (void);
void lw_dictionaryinstallstatehistory_free (LwDictionaryInstallStateHistory *self);

void lw_dictionaryinstallstatehistory_add (LwDictionaryInstallStateHistory *self, LwDictionaryInstallState *state);
void lw_dictionaryinstallstatehistory_add_named_paths (LwDictionaryInstallStateHistory *self, const gchar *NAME, gboolean is_temporary, GList *paths);
LwDictionaryInstallState* lw_dictionaryinstallstatehistory_current (LwDictionaryInstallStateHistory *self);
void lw_dictionaryinstallstatehistory_clear (LwDictionaryInstallStateHistory *self);
void lw_dictionaryinstallstatehistory_foreach (LwDictionaryInstallStateHistory *self, LwDictionaryInstallStateHistoryForeachFunc func, gpointer user_data);

G_END_DECLS

#endif
