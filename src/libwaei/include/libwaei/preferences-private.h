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

G_END_DECLS

#endif
