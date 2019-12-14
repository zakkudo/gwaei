#ifndef W_APPLICATION_PRIVATE_INCLUDED
#define W_APPLICATION_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
  PROP_0,
  PROP_PREFERENCES,
  TOTAL_PROPS
} Props;

struct _Data {
  LwDictionaryList *dictionarylist;
  LwDictionaryInstallList *dictionaryinstalllist;
  LwPreferences *preferences;
};

struct _WApplicationPrivate {
  struct _Data data;
};

struct _WApplicationClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
};

#define W_APPLICATION_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), W_TYPE_APPLICATION, WApplicationPrivate))


G_END_DECLS

#endif
