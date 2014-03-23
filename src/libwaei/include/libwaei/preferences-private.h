#ifndef LW_PREFERENCES_PRIVATE_INCLUDED
#define LW_PREFERENCES_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum
{
  PROP_0,
  PROP_BACKEND,
} Props;

struct _LwPreferencesPrivate {
  GHashTable *settings;
  GSettingsBackend *backend;
};

#define LW_PREFERENCES_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_PREFERENCES, LwPreferencesPrivate));

G_END_DECLS

#endif
