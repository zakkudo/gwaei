#ifndef LW_DICTIONARYINSTALL_INCLUDED
#define LW_DICTIONARYINSTALL_INCLUDED

#include "dictionaryinstallstatehistory.h"

G_BEGIN_DECLS

//Boilerplate
typedef struct _LwDictionaryInstall LwDictionaryInstall;
typedef struct _LwDictionaryInstallClass LwDictionaryInstallClass;
typedef struct _LwDictionaryInstallPrivate LwDictionaryInstallPrivate;
typedef struct _LwDictionaryInstallClassPrivate LwDictionaryInstallClassPrivate;

#define LW_TYPE_DICTIONARYINSTALL              (lw_dictionaryinstall_get_type())
#define LW_DICTIONARYINSTALL(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_DICTIONARYINSTALL, LwDictionaryInstall))
#define LW_DICTIONARYINSTALL_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_DICTIONARYINSTALL, LwDictionaryInstallClass))
#define LW_IS_DICTIONARYINSTALL(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_DICTIONARYINSTALL))
#define LW_IS_DICTIONARYINSTALL_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_DICTIONARYINSTALL))
#define LW_DICTIONARYINSTALL_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_DICTIONARYINSTALL, LwDictionaryInstallClass))


struct _LwDictionaryInstall {
  GObject object;
};

struct _LwDictionaryInstallClass {
  GObjectClass parent_class;
  LwDictionaryInstallClassPrivate *priv;
};


//Methods
LwDictionaryInstall* lw_dictionaryinstall_new (void);
GType lw_dictionaryinstall_get_type (void) G_GNUC_CONST;

void lw_dictionaryinstall_set_name (LwDictionaryInstall *self, const gchar *NAME);
const gchar* lw_dictionaryinstall_get_name (LwDictionaryInstall *self);

void lw_dictionaryinstall_set_description (LwDictionaryInstall *self, const gchar *DESCRIPTION);
const gchar* lw_dictionaryinstall_get_description (LwDictionaryInstall *self);

void lw_dictionaryinstall_set_text_encoding (LwDictionaryInstall *self, const gchar *TEXT_ENCODING);
const gchar* lw_dictionaryinstall_get_text_encoding (LwDictionaryInstall *self);

void lw_dictionaryinstall_set_gtype (LwDictionaryInstall *self, GType gtype);
GType lw_dictionaryinstall_get_gtype (LwDictionaryInstall *self);

void lw_dictionaryinstall_set_download_key (LwDictionaryInstall *self, const gchar *DOWNLOAD_KEY);
const gchar* lw_dictionaryinstall_get_download_key (LwDictionaryInstall *self);

void lw_dictionaryinstall_set_download_uri (LwDictionaryInstall *self, const gchar *DOWNLOAD_URI);
const gchar* lw_dictionaryinstall_get_download_uri (LwDictionaryInstall *self);
void lw_dictionaryinstall_sync_download_uri (LwDictionaryInstall *self);

void lw_dictionaryinstall_set_preferences (LwDictionaryInstall *self, LwPreferences *preferences);
LwPreferences* lw_dictionaryinstall_get_preferences (LwDictionaryInstall *self);

void lw_dictionaryinstall_set_merge_radicals_into_kanji (LwDictionaryInstall *self, gboolean merge);
gboolean lw_dictionaryinstall_get_merge_radicals_into_kanji (LwDictionaryInstall *self);
gboolean lw_dictionaryinstall_merge_radicals_into_kanji (LwDictionaryInstall *self);

void lw_dictionaryinstall_set_progress (LwDictionaryInstall *self, LwProgress *progress);
LwProgress* lw_dictionaryinstall_get_progress (LwDictionaryInstall *self);

void lw_dictionaryinstall_set_split_places_from_names (LwDictionaryInstall *self, gboolean split);
gboolean lw_dictionaryinstall_get_split_places_from_names (LwDictionaryInstall *self);
gboolean lw_dictionaryinstall_split_places_from_names (LwDictionaryInstall *self);

void lw_dictionaryinstall_set_dependancies (LwDictionaryInstall *self, GList* dependencies);
GList* lw_dictionaryinstall_get_dependancies (LwDictionaryInstall *self);
void lw_dictionaryinstall_set_simple_boolean_dependancy (LwDictionaryInstall *self, const gchar *NAME, const gchar *PROPERTY_NAME, gboolean VALUE);

const gchar* lw_dictionaryinstall_get_id (LwDictionaryInstall *self);
void lw_dictionaryinstall_set_id (LwDictionaryInstall *self, const gchar *ID);
void lw_dictionaryinstall_sync_id (LwDictionaryInstall *self);

const gchar* lw_dictionaryinstall_get_install_path (LwDictionaryInstall *self);
void lw_dictionaryinstall_sync_install_path (LwDictionaryInstall *self);

gboolean lw_dictionaryinstall_install (LwDictionaryInstall *self);

G_END_DECLS

#endif
