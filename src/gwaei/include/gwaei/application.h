#ifndef GW_APPLICATION_INCLUDED
#define GW_APPLICATION_INCLUDED

G_BEGIN_DECLS

//Boilerplate
typedef struct _GwApplication GwApplication;
typedef struct _GwApplicationClass GwApplicationClass;
typedef struct _GwApplicationPrivate GwApplicationPrivate;

struct _GwSearchWindow; //Forward declaration

#define GW_TYPE_APPLICATION              (gw_application_get_type())
#define GW_APPLICATION(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), GW_TYPE_APPLICATION, GwApplication))
#define GW_APPLICATION_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), GW_TYPE_APPLICATION, GwApplicationClass))
#define GW_IS_APPLICATION(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), GW_TYPE_APPLICATION))
#define GW_IS_APPLICATION_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), GW_TYPE_APPLICATION))
#define GW_APPLICATION_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), GW_TYPE_APPLICATION, GwApplicationClass))

#define GW_APPLICATION_MAX_FONT_MAGNIFICATION  6
#define GW_APPLICATION_MIN_FONT_MAGNIFICATION -6
#define GW_APPLICATION_DEFAULT_FONT_MAGNIFICATION 0
#define GW_APPLICATION_FONT_ZOOM_STEP 2
#define GW_APPLICATION_MAX_FONT_SIZE 100
#define GW_APPLICATION_MIN_FONT_SIZE 6
#define GW_APPLICATION_DEFAULT_FONT_SIZE 12
#define GW_APPLICATION_DEFAULT_FONT "Sans 12"

typedef enum {
  GW_APPLICATION_SIGNALID_MATCH_FG,
  GW_APPLICATION_SIGNALID_MATCH_BG,
  GW_APPLICATION_SIGNALID_HEADER_FG,
  GW_APPLICATION_SIGNALID_HEADER_BG,
  GW_APPLICATION_SIGNALID_COMMENT_FG,
  TOTAL_GW_APPLICATION_SIGNALIDS
} GwApplicationSignalId;

typedef enum {
  GW_APP_RESOLUTION_NO_ERRORS,
  GW_APP_RESOLUTION_OUT_OF_MEMORY
} GwApplicationResolution;


struct _GwApplication {
  GtkApplication self;
  GwApplicationPrivate *priv;
};

struct _GwApplicationClass {
  GtkApplicationClass parent_class;
  guint signalid[TOTAL_GW_APPLICATION_SIGNALIDS];
};

//Methods
GApplication* gw_application_new (void);
GType gw_application_get_type (void) G_GNUC_CONST;

GwApplicationResolution gw_application_run (GwApplication*);
void gw_application_parse_args (GwApplication*, int*, char***);
void gw_application_quit (GwApplication*);

GtkWindow* gw_application_get_window_by_type (GwApplication *self, const GType TYPE);

const gchar* gw_application_get_program_name (GwApplication *self);
void gw_application_map_actions (GActionMap *map, GwApplication *self);

LgwDictionaryListStore* gw_application_get_installed_dictionaryliststore (GwApplication *self);

LwPreferences* gw_application_get_preferences (GwApplication *self);

LgwVocabularyListStore* gw_application_get_vocabularyliststore (GwApplication *self);

G_END_DECLS

#include "application-callbacks.h"

#endif
