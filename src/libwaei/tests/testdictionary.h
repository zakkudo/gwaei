
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
