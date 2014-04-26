#ifndef LW_DICTIONARYINSTALLER_INCLUDED
#define LW_DICTIONARYINSTALLER_INCLUDED

G_BEGIN_DECLS

typedef enum {
  LW_DICTIONARYINSTALLER_STATUS_UNINSTALLED,
  LW_DICTIONARYINSTALLER_STATUS_DOWNLOADING,
  LW_DICTIONARYINSTALLER_STATUS_DECOMPRESSING,
  LW_DICTIONARYINSTALLER_STATUS_ENCODING,
  LW_DICTIONARYINSTALLER_STATUS_POSTPROCESSING,
  LW_DICTIONARYINSTALLER_STATUS_FINISHING,
  LW_DICTIONARYINSTALLER_STATUS_INSTALLED,
  TOTAL_LW_DICTIONARYINSTALLER_STATUSES
} LwDictionaryInstallerStatus;


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
  LwDictionaryInstallPrivate *priv;
};

struct _LwDictionaryInstallClass {
  GObjectClass parent_class;
  LwDictionaryInstallClassPrivate *priv;
};


//Methods
LwDictionaryInstall* lw_dictionaryinstall_new (const gchar* NAME);
GType lw_dictionaryinstall_get_type (void) G_GNUC_CONST;


G_END_DECLS

#endif
