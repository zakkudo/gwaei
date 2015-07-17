#include <libwaei/dictionarymodule.h>

//Boilerplate
typedef struct _LwTestDictionary LwTestDictionary;
typedef struct _LwTestDictionaryClass LwTestDictionaryClass;
typedef struct _LwTestDictionaryPrivate LwTestDictionaryPrivate;

#define LW_TYPE_TESTDICTIONARY              (lw_testdictionary_get_type())
#define LW_TESTDICTIONARY(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_TESTDICTIONARY, LwTestDictionary))
#define LW_TESTDICTIONARY_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_TESTDICTIONARY, LwTestDictionaryClass))
#define LW_IS_TESTDICTIONARY(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_TESTDICTIONARY))
#define LW_IS_TESTDICTIONARY_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_TESTDICTIONARY))
#define LW_TESTDICTIONARY_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_TESTDICTIONARY, LwTestDictionaryClass))

struct _LwTestDictionary {
  LwDictionary object;
};

struct _LwTestDictionaryClass {
  LwDictionaryClass parent_class;
};

GType lw_testdictionary_get_type (void) G_GNUC_CONST;


G_DEFINE_TYPE (LwTestDictionary, lw_testdictionary, LW_TYPE_DICTIONARY)

static void
lw_testdictionary_init (LwTestDictionary * testdictionar){}
static void
lw_testdictionary_constructed (GObject *object)
{
    //Chain the parent class
    {
      G_OBJECT_CLASS (lw_testdictionary_parent_class)->constructed (object);
    }
}
static void
lw_testdictionary_finalize (GObject *object)
{
    G_OBJECT_CLASS (lw_testdictionary_parent_class)->finalize (object);
}
static void
lw_testdictionary_class_init (LwTestDictionaryClass *klass)
{
    //Declarations
    GObjectClass *object_class;
    LwDictionaryClass *dictionary_class;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = lw_testdictionary_finalize;
    object_class->constructed = lw_testdictionary_constructed;
}


struct _DictionaryConfig {
  gchar * cachetmpl;
  gchar * cachedir;
  gchar * datatmpl;
  gchar * datadir;
};
typedef struct _DictionaryConfig DictionaryConfig;

#define DICTIONARYMODULEDIR ".." G_DIR_SEPARATOR_S ".." G_DIR_SEPARATOR_S "dictionary" G_DIR_SEPARATOR_S ".libs"
#define TESTDICTIONARYDATADIR ".." G_DIR_SEPARATOR_S ".." G_DIR_SEPARATOR_S "dictionary" G_DIR_SEPARATOR_S "tests" G_DIR_SEPARATOR_S "data"

static DictionaryConfig *
dictionary_config_new (void)
{
    DictionaryConfig * self = g_new0 (DictionaryConfig, 1);

    self->cachetmpl = g_build_filename (g_get_tmp_dir (), "testcachedir-XXXXXX", NULL);
    self->cachedir = g_mkdtemp_full (self->cachetmpl, 0700);

    self->datatmpl = g_build_filename (g_get_tmp_dir (), "testdatadir-XXXXXX", NULL);
    self->datadir = g_mkdtemp_full (self->datatmpl, 0700);

    g_setenv("DICTIONARYCACHEDIR", self->cachedir, TRUE);
    g_setenv("DICTIONARYDATADIR", self->datadir, TRUE);
    g_setenv ("DICTIONARYLIB_SEARCHPATH", DICTIONARYMODULEDIR, TRUE);

    lw_dictionarymodule_new (DICTIONARYMODULEDIR G_DIR_SEPARATOR_S "libedictionary." G_MODULE_SUFFIX);
    lw_dictionarymodule_new (DICTIONARYMODULEDIR G_DIR_SEPARATOR_S "libkanjidictionary." G_MODULE_SUFFIX);
    lw_dictionarymodule_new (DICTIONARYMODULEDIR G_DIR_SEPARATOR_S "libradicalsdictionary." G_MODULE_SUFFIX);
    lw_dictionarymodule_new (DICTIONARYMODULEDIR G_DIR_SEPARATOR_S "libexampledictionary." G_MODULE_SUFFIX);
    lw_dictionarymodule_new (DICTIONARYMODULEDIR G_DIR_SEPARATOR_S "libunknowndictionary." G_MODULE_SUFFIX);

    return self;
}

static void
dictionary_config_free (DictionaryConfig * self)
{
    if (self == NULL) return;

    g_remove (self->cachedir);
    g_free (self->cachedir);

    g_remove (self->datadir);
    g_free(self->datadir);

    g_free (self);
}
