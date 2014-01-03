#ifndef LGW_TEXTTAGTABLE_PRIVATE_INCLUDED
#define LGW_TEXTTAGTABLE_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum
{
  PROP_0,
  PROP_PREFERENCES,
  TOTAL_PROPS
} Props;

typedef enum { 
  LGW_TEXTTAGTABLE_SIGNALID_MATCH_FG,
  LGW_TEXTTAGTABLE_SIGNALID_MATCH_BG,
  LGW_TEXTTAGTABLE_SIGNALID_HEADER_FG,
  LGW_TEXTTAGTABLE_SIGNALID_HEADER_BG,
  LGW_TEXTTAGTABLE_SIGNALID_COMMENT_FG,
  TOTAL_LGW_TEXTTAGTABLE_SIGNALIDS
} LgwTextTagTableColumn;

struct _Config {
  LwPreferences *preferences;
};

struct _Data {
  guint signalid[TOTAL_LGW_TEXTTAGTABLE_SIGNALIDS];
};

struct _LgwTextTagTablePrivate {
  struct _Config config;
  struct _Data data;
};

#define LGW_TEXTTAGTABLE_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_TEXTTAGTABLE, LgwTextTagTablePrivate))

struct _LgwTextTagTableClassPrivate {
    GParamSpec *pspec[TOTAL_PROPS];
};

G_END_DECLS

#include <libgwaei/texttagtable-callbacks.h>

#endif

