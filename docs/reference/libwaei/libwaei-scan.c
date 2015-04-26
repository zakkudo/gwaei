#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <glib-object.h>

extern GType lw_application_get_type (void);
extern GType lw_cachefile_get_type (void);
extern GType lw_command_get_type (void);
extern GType lw_condition_get_type (void);
extern GType lw_dependancy_get_type (void);
extern GType lw_dictionary_get_type (void);
extern GType lw_dictionarycache_get_type (void);
extern GType lw_dictionarycolumnhandling_get_type (void);
extern GType lw_dictionaryinstall_get_type (void);
extern GType lw_dictionaryinstalllist_get_type (void);
extern GType lw_dictionarylist_get_type (void);
extern GType lw_dictionarymodule_get_type (void);
extern GType lw_indexed_get_type (void);
extern GType lw_installsubcommand_get_type (void);
extern GType lw_mappedfile_get_type (void);
extern GType lw_morphologyengine_get_type (void);
extern GType lw_parenthesisnode_get_type (void);
extern GType lw_parsed_get_type (void);
extern GType lw_preferences_get_type (void);
extern GType lw_progress_get_type (void);
extern GType lw_search_get_type (void);
extern GType lw_searchflag_get_type (void);
extern GType lw_searchstatus_get_type (void);
extern GType lw_searchsubcommand_get_type (void);
extern GType lw_serializable_get_type (void);
extern GType lw_subcommand_get_type (void);
extern GType lw_uninstallsubcommand_get_type (void);
extern GType lw_utf8flag_get_type (void);
extern GType lw_vocabulary_get_type (void);
extern GType lw_word_get_type (void);
extern GType lw_wordfield_get_type (void);

#ifdef GTK_IS_WIDGET_CLASS
#include <gtk/gtk.h>
#endif
GType object_types[32];

static GType *
get_object_types (void)
{
    gpointer g_object_class;
    gint i = 0;
    object_types[i++] = lw_application_get_type ();
    object_types[i++] = lw_cachefile_get_type ();
    object_types[i++] = lw_command_get_type ();
    object_types[i++] = lw_condition_get_type ();
    object_types[i++] = lw_dependancy_get_type ();
    object_types[i++] = lw_dictionary_get_type ();
    object_types[i++] = lw_dictionarycache_get_type ();
    object_types[i++] = lw_dictionarycolumnhandling_get_type ();
    object_types[i++] = lw_dictionaryinstall_get_type ();
    object_types[i++] = lw_dictionaryinstalllist_get_type ();
    object_types[i++] = lw_dictionarylist_get_type ();
    object_types[i++] = lw_dictionarymodule_get_type ();
    object_types[i++] = lw_indexed_get_type ();
    object_types[i++] = lw_installsubcommand_get_type ();
    object_types[i++] = lw_mappedfile_get_type ();
    object_types[i++] = lw_morphologyengine_get_type ();
    object_types[i++] = lw_parenthesisnode_get_type ();
    object_types[i++] = lw_parsed_get_type ();
    object_types[i++] = lw_preferences_get_type ();
    object_types[i++] = lw_progress_get_type ();
    object_types[i++] = lw_search_get_type ();
    object_types[i++] = lw_searchflag_get_type ();
    object_types[i++] = lw_searchstatus_get_type ();
    object_types[i++] = lw_searchsubcommand_get_type ();
    object_types[i++] = lw_serializable_get_type ();
    object_types[i++] = lw_subcommand_get_type ();
    object_types[i++] = lw_uninstallsubcommand_get_type ();
    object_types[i++] = lw_utf8flag_get_type ();
    object_types[i++] = lw_vocabulary_get_type ();
    object_types[i++] = lw_word_get_type ();
    object_types[i++] = lw_wordfield_get_type ();
    object_types[i] = 0;

    /* reference the GObjectClass to initialize the param spec pool
     * potentially needed by interfaces. See http://bugs.gnome.org/571820 */
    g_object_class = g_type_class_ref (G_TYPE_OBJECT);

    /* Need to make sure all the types are loaded in and initialize
     * their signals and properties.
     */
    for (i=0; object_types[i]; i++)
      {
        if (G_TYPE_IS_CLASSED (object_types[i]))
          g_type_class_ref (object_types[i]);
        if (G_TYPE_IS_INTERFACE (object_types[i]))
          g_type_default_interface_ref (object_types[i]);
      }

    g_type_class_unref (g_object_class);

    return object_types;
}

/*
 * This uses GObject type functions to output signal prototypes and the object
 * hierarchy.
 */

/* The output files */
const gchar *signals_filename = "./libwaei.signals.new";
const gchar *hierarchy_filename = "./libwaei.hierarchy.new";
const gchar *interfaces_filename = "./libwaei.interfaces.new";
const gchar *prerequisites_filename = "./libwaei.prerequisites.new";
const gchar *args_filename = "./libwaei.args.new";


static void output_signals (void);
static void output_object_signals (FILE *fp,
                                   GType object_type);
static void output_object_signal (FILE *fp,
                                  const gchar *object_class_name,
                                  guint signal_id);
static const gchar * get_type_name (GType type,
                                    gboolean * is_pointer);
static void output_object_hierarchy (void);
static void output_hierarchy (FILE *fp,
                              GType type,
                              guint level);

static void output_object_interfaces (void);
static void output_interfaces (FILE *fp,
                               GType type);

static void output_interface_prerequisites (void);
static void output_prerequisites (FILE *fp,
                                  GType type);

static void output_args (void);
static void output_object_args (FILE *fp, GType object_type);

int
main (int argc, char *argv[])
{
  #if !GLIB_CHECK_VERSION(2,35,0)
  g_type_init();
  #endif
  g_type_class_ref(G_TYPE_OBJECT);

  get_object_types ();

  output_signals ();
  output_object_hierarchy ();
  output_object_interfaces ();
  output_interface_prerequisites ();
  output_args ();

  return 0;
}


static void
output_signals (void)
{
  FILE *fp;
  gint i;

  fp = fopen (signals_filename, "w");
  if (fp == NULL)
    {
      g_warning ("Couldn't open output file: %s : %s", signals_filename, g_strerror(errno));
      return;
    }

  for (i = 0; object_types[i]; i++)
    output_object_signals (fp, object_types[i]);

  fclose (fp);
}

static gint
compare_signals (const void *a, const void *b)
{
  const guint *signal_a = a;
  const guint *signal_b = b;

  return strcmp (g_signal_name (*signal_a), g_signal_name (*signal_b));
}

/* This outputs all the signals of one object. */
static void
output_object_signals (FILE *fp, GType object_type)
{
  const gchar *object_class_name;
  guint *signals, n_signals;
  guint sig;

  if (G_TYPE_IS_INSTANTIATABLE (object_type) ||
      G_TYPE_IS_INTERFACE (object_type))
    {

      object_class_name = g_type_name (object_type);

      signals = g_signal_list_ids (object_type, &n_signals);
      qsort (signals, n_signals, sizeof (guint), compare_signals);

      for (sig = 0; sig < n_signals; sig++)
        {
           output_object_signal (fp, object_class_name, signals[sig]);
        }
      g_free (signals);
   }
}


/* This outputs one signal. */
static void
output_object_signal (FILE *fp,
                      const gchar *object_name,
                      guint signal_id)
{
  GSignalQuery query_info;
  const gchar *type_name, *ret_type, *object_arg, *arg_name;
  gchar *pos, *object_arg_lower;
  gboolean is_pointer;
  gchar buffer[1024];
  guint i, param;
  gint param_num, widget_num, event_num, callback_num;
  gint *arg_num;
  gchar signal_name[128];
  gchar flags[16];

  /*  g_print ("Object: %s Signal: %u\n", object_name, signal_id);*/

  param_num = 1;
  widget_num = event_num = callback_num = 0;

  g_signal_query (signal_id, &query_info);

  /* Output the signal object type and the argument name. We assume the
     type is a pointer - I think that is OK. We remove "Gtk" or "Gnome" and
     convert to lower case for the argument name. */
  pos = buffer;
  sprintf (pos, "%s ", object_name);
  pos += strlen (pos);

  /* Try to come up with a sensible variable name for the first arg
   * It chops off 2 know prefixes :/ and makes the name lowercase
   * It should replace lowercase -> uppercase with '_'
   * GFileMonitor -> file_monitor
   * GIOExtensionPoint -> extension_point
   * GtkTreeView -> tree_view
   * if 2nd char is upper case too
   *   search for first lower case and go back one char
   * else
   *   search for next upper case
   */
  if (!strncmp (object_name, "Gtk", 3))
      object_arg = object_name + 3;
  else if (!strncmp (object_name, "Gnome", 5))
      object_arg = object_name + 5;
  else
      object_arg = object_name;

  object_arg_lower = g_ascii_strdown (object_arg, -1);
  sprintf (pos, "*%s\n", object_arg_lower);
  pos += strlen (pos);
  if (!strncmp (object_arg_lower, "widget", 6))
    widget_num = 2;
  g_free(object_arg_lower);

  /* Convert signal name to use underscores rather than dashes '-'. */
  strncpy (signal_name, query_info.signal_name, 127);
  signal_name[127] = '\0';
  for (i = 0; signal_name[i]; i++)
    {
      if (signal_name[i] == '-')
        signal_name[i] = '_';
    }

  /* Output the signal parameters. */
  for (param = 0; param < query_info.n_params; param++)
    {
      type_name = get_type_name (query_info.param_types[param] & ~G_SIGNAL_TYPE_STATIC_SCOPE, &is_pointer);

      /* Most arguments to the callback are called "arg1", "arg2", etc.
         GtkWidgets are called "widget", "widget2", ...
         GtkCallbacks are called "callback", "callback2", ... */
      if (!strcmp (type_name, "GtkWidget"))
        {
          arg_name = "widget";
          arg_num = &widget_num;
        }
      else if (!strcmp (type_name, "GtkCallback")
               || !strcmp (type_name, "GtkCCallback"))
        {
          arg_name = "callback";
          arg_num = &callback_num;
        }
      else
        {
          arg_name = "arg";
          arg_num = &param_num;
        }
      sprintf (pos, "%s ", type_name);
      pos += strlen (pos);

      if (!arg_num || *arg_num == 0)
        sprintf (pos, "%s%s\n", is_pointer ? "*" : " ", arg_name);
      else
        sprintf (pos, "%s%s%i\n", is_pointer ? "*" : " ", arg_name,
                 *arg_num);
      pos += strlen (pos);

      if (arg_num)
        {
          if (*arg_num == 0)
            *arg_num = 2;
          else
            *arg_num += 1;
        }
    }

  pos = flags;
  /* We use one-character flags for simplicity. */
  if (query_info.signal_flags & G_SIGNAL_RUN_FIRST)
    *pos++ = 'f';
  if (query_info.signal_flags & G_SIGNAL_RUN_LAST)
    *pos++ = 'l';
  if (query_info.signal_flags & G_SIGNAL_RUN_CLEANUP)
    *pos++ = 'c';
  if (query_info.signal_flags & G_SIGNAL_NO_RECURSE)
    *pos++ = 'r';
  if (query_info.signal_flags & G_SIGNAL_DETAILED)
    *pos++ = 'd';
  if (query_info.signal_flags & G_SIGNAL_ACTION)
    *pos++ = 'a';
  if (query_info.signal_flags & G_SIGNAL_NO_HOOKS)
    *pos++ = 'h';
  *pos = 0;

  /* Output the return type and function name. */
  ret_type = get_type_name (query_info.return_type & ~G_SIGNAL_TYPE_STATIC_SCOPE, &is_pointer);

  fprintf (fp,
           "<SIGNAL>\n<NAME>%s::%s</NAME>\n<RETURNS>%s%s</RETURNS>\n<FLAGS>%s</FLAGS>\n%s</SIGNAL>\n\n",
           object_name, query_info.signal_name, ret_type, is_pointer ? "*" : "", flags, buffer);
}


/* Returns the type name to use for a signal argument or return value, given
   the GtkType from the signal info. It also sets is_pointer to TRUE if the
   argument needs a '*' since it is a pointer. */
static const gchar *
get_type_name (GType type, gboolean * is_pointer)
{
  const gchar *type_name;

  *is_pointer = FALSE;
  type_name = g_type_name (type);

  switch (type) {
  case G_TYPE_NONE:
  case G_TYPE_CHAR:
  case G_TYPE_UCHAR:
  case G_TYPE_BOOLEAN:
  case G_TYPE_INT:
  case G_TYPE_UINT:
  case G_TYPE_LONG:
  case G_TYPE_ULONG:
  case G_TYPE_FLOAT:
  case G_TYPE_DOUBLE:
  case G_TYPE_POINTER:
    /* These all have normal C type names so they are OK. */
    return type_name;

  case G_TYPE_STRING:
    /* A GtkString is really a gchar*. */
    *is_pointer = TRUE;
    return "gchar";

  case G_TYPE_ENUM:
  case G_TYPE_FLAGS:
    /* We use a gint for both of these. Hopefully a subtype with a decent
       name will be registered and used instead, as GTK+ does itself. */
    return "gint";

  case G_TYPE_BOXED:
    /* The boxed type shouldn't be used itself, only subtypes. Though we
       return 'gpointer' just in case. */
    return "gpointer";

  case G_TYPE_PARAM:
    /* A GParam is really a GParamSpec*. */
    *is_pointer = TRUE;
    return "GParamSpec";

#if GLIB_CHECK_VERSION (2, 25, 9)
  case G_TYPE_VARIANT:
    *is_pointer = TRUE;
    return "GVariant";
#endif

default:
    break;
  }

  /* For all GObject subclasses we can use the class name with a "*",
     e.g. 'GtkWidget *'. */
  if (g_type_is_a (type, G_TYPE_OBJECT))
    *is_pointer = TRUE;

  /* Also catch non GObject root types */
  if (G_TYPE_IS_CLASSED (type))
    *is_pointer = TRUE;

  /* All boxed subtypes will be pointers as well. */
  /* Exception: GStrv */
  if (g_type_is_a (type, G_TYPE_BOXED) &&
      !g_type_is_a (type, G_TYPE_STRV))
    *is_pointer = TRUE;

  /* All pointer subtypes will be pointers as well. */
  if (g_type_is_a (type, G_TYPE_POINTER))
    *is_pointer = TRUE;

  /* But enums are not */
  if (g_type_is_a (type, G_TYPE_ENUM) ||
      g_type_is_a (type, G_TYPE_FLAGS))
    *is_pointer = FALSE;

  return type_name;
}


/* This outputs the hierarchy of all objects which have been initialized,
   i.e. by calling their XXX_get_type() initialization function. */
static void
output_object_hierarchy (void)
{
  FILE *fp;
  gint i,j;
  GType root, type;
  GType root_types[32] = { G_TYPE_INVALID, };

  fp = fopen (hierarchy_filename, "w");
  if (fp == NULL)
    {
      g_warning ("Couldn't open output file: %s : %s", hierarchy_filename, g_strerror(errno));
      return;
    }
  output_hierarchy (fp, G_TYPE_OBJECT, 0);
  output_hierarchy (fp, G_TYPE_INTERFACE, 0);

  for (i=0; object_types[i]; i++) {
    root = object_types[i];
    while ((type = g_type_parent (root))) {
      root = type;
    }
    if ((root != G_TYPE_OBJECT) && (root != G_TYPE_INTERFACE)) {
      for (j=0; root_types[j]; j++) {
        if (root == root_types[j]) {
          root = G_TYPE_INVALID; break;
        }
      }
      if(root) {
        root_types[j] = root;
        output_hierarchy (fp, root, 0);
      }
    }
  }

  fclose (fp);
}

/* This is called recursively to output the hierarchy of a object. */
static void
output_hierarchy (FILE  *fp,
                  GType  type,
                  guint   level)
{
  guint i;
  GType *children;
  guint n_children;

  if (!type)
    return;

  for (i = 0; i < level; i++)
    fprintf (fp, "  ");
  fprintf (fp, "%s\n", g_type_name (type));

  children = g_type_children (type, &n_children);

  for (i=0; i < n_children; i++)
    output_hierarchy (fp, children[i], level + 1);

  g_free (children);
}

static void output_object_interfaces (void)
{
  guint i;
  FILE *fp;

  fp = fopen (interfaces_filename, "w");
  if (fp == NULL)
    {
      g_warning ("Couldn't open output file: %s : %s", interfaces_filename, g_strerror(errno));
      return;
    }
  output_interfaces (fp, G_TYPE_OBJECT);

  for (i = 0; object_types[i]; i++)
    {
      if (!g_type_parent (object_types[i]) &&
          (object_types[i] != G_TYPE_OBJECT) &&
          G_TYPE_IS_INSTANTIATABLE (object_types[i]))
        {
          output_interfaces (fp, object_types[i]);
        }
    }
  fclose (fp);
}

static void
output_interfaces (FILE  *fp,
                   GType  type)
{
  guint i;
  GType *children, *interfaces;
  guint n_children, n_interfaces;

  if (!type)
    return;

  interfaces = g_type_interfaces (type, &n_interfaces);

  if (n_interfaces > 0)
    {
      fprintf (fp, "%s", g_type_name (type));
      for (i=0; i < n_interfaces; i++)
          fprintf (fp, " %s", g_type_name (interfaces[i]));
      fprintf (fp, "\n");
     }
  g_free (interfaces);

  children = g_type_children (type, &n_children);

  for (i=0; i < n_children; i++)
    output_interfaces (fp, children[i]);

  g_free (children);
}

static void output_interface_prerequisites (void)
{
  FILE *fp;

  fp = fopen (prerequisites_filename, "w");
  if (fp == NULL)
    {
      g_warning ("Couldn't open output file: %s : %s", prerequisites_filename, g_strerror(errno));
      return;
    }
  output_prerequisites (fp, G_TYPE_INTERFACE);
  fclose (fp);
}

static void
output_prerequisites (FILE  *fp,
                      GType  type)
{
#if GLIB_CHECK_VERSION(2,1,0)
  guint i;
  GType *children, *prerequisites;
  guint n_children, n_prerequisites;

  if (!type)
    return;

  prerequisites = g_type_interface_prerequisites (type, &n_prerequisites);

  if (n_prerequisites > 0)
    {
      fprintf (fp, "%s", g_type_name (type));
      for (i=0; i < n_prerequisites; i++)
          fprintf (fp, " %s", g_type_name (prerequisites[i]));
      fprintf (fp, "\n");
     }
  g_free (prerequisites);

  children = g_type_children (type, &n_children);

  for (i=0; i < n_children; i++)
    output_prerequisites (fp, children[i]);

  g_free (children);
#endif
}

static void
output_args (void)
{
  FILE *fp;
  gint i;

  fp = fopen (args_filename, "w");
  if (fp == NULL)
    {
      g_warning ("Couldn't open output file: %s : %s", args_filename, g_strerror(errno));
      return;
    }

  for (i = 0; object_types[i]; i++) {
    output_object_args (fp, object_types[i]);
  }

  fclose (fp);
}

static gint
compare_param_specs (const void *a, const void *b)
{
  GParamSpec *spec_a = *(GParamSpec **)a;
  GParamSpec *spec_b = *(GParamSpec **)b;

  return strcmp (g_param_spec_get_name (spec_a), g_param_spec_get_name (spec_b));
}

/* Its common to have unsigned properties restricted
 * to the signed range. Therefore we make this look
 * a bit nicer by spelling out the max constants.
 */

/* Don't use "==" with floats, it might trigger a gcc warning.  */
#define GTKDOC_COMPARE_FLOAT(x, y) (x <= y && x >= y)

static gchar*
describe_double_constant (gdouble value)
{
  gchar *desc;

  if (GTKDOC_COMPARE_FLOAT (value, G_MAXDOUBLE))
    desc = g_strdup ("G_MAXDOUBLE");
  else if (GTKDOC_COMPARE_FLOAT (value, G_MINDOUBLE))
    desc = g_strdup ("G_MINDOUBLE");
  else if (GTKDOC_COMPARE_FLOAT (value, -G_MAXDOUBLE))
    desc = g_strdup ("-G_MAXDOUBLE");
  else if (GTKDOC_COMPARE_FLOAT (value, G_MAXFLOAT))
    desc = g_strdup ("G_MAXFLOAT");
  else if (GTKDOC_COMPARE_FLOAT (value, G_MINFLOAT))
    desc = g_strdup ("G_MINFLOAT");
  else if (GTKDOC_COMPARE_FLOAT (value, -G_MAXFLOAT))
    desc = g_strdup ("-G_MAXFLOAT");
  else{
    /* make sure floats are output with a decimal dot irrespective of
    * current locale. Use formatd since we want human-readable numbers
    * and do not need the exact same bit representation when deserialising */
    desc = g_malloc0 (G_ASCII_DTOSTR_BUF_SIZE);
    g_ascii_formatd (desc, G_ASCII_DTOSTR_BUF_SIZE, "%g", value);
  }

  return desc;
}

static gchar*
describe_signed_constant (gsize size, gint64 value)
{
  gchar *desc = NULL;

  switch (size) {
    case 2:
      if (sizeof (int) == 2) {
        if (value == G_MAXINT)
          desc = g_strdup ("G_MAXINT");
        else if (value == G_MININT)
          desc = g_strdup ("G_MININT");
      }
      break;
    case 4:
      if (sizeof (int) == 4) {
        if (value == G_MAXINT)
          desc = g_strdup ("G_MAXINT");
        else if (value == G_MININT)
          desc = g_strdup ("G_MININT");
      }
      if (value == G_MAXLONG)
        desc = g_strdup ("G_MAXLONG");
      else if (value == G_MINLONG)
        desc = g_strdup ("G_MINLONG");
      break;
    case 8:
      if (value == G_MAXINT64)
        desc = g_strdup ("G_MAXINT64");
      else if (value == G_MININT64)
        desc = g_strdup ("G_MININT64");
      break;
    default:
      break;
  }
  if (!desc)
    desc = g_strdup_printf ("%" G_GINT64_FORMAT, value);

  return desc;
}

static gchar*
describe_unsigned_constant (gsize size, guint64 value)
{
  gchar *desc = NULL;

  switch (size) {
    case 2:
      if (sizeof (int) == 2) {
        if (value == (guint64)G_MAXINT)
          desc = g_strdup ("G_MAXINT");
        else if (value == G_MAXUINT)
          desc = g_strdup ("G_MAXUINT");
      }
      break;
    case 4:
      if (sizeof (int) == 4) {
        if (value == (guint64)G_MAXINT)
          desc = g_strdup ("G_MAXINT");
        else if (value == G_MAXUINT)
          desc = g_strdup ("G_MAXUINT");
      }
      if (value == (guint64)G_MAXLONG)
        desc = g_strdup ("G_MAXLONG");
      else if (value == G_MAXULONG)
        desc = g_strdup ("G_MAXULONG");
      break;
    case 8:
      if (value == G_MAXINT64)
        desc = g_strdup ("G_MAXINT64");
      else if (value == G_MAXUINT64)
        desc = g_strdup ("G_MAXUINT64");
      break;
    default:
      break;
  }
  if (!desc)
    desc = g_strdup_printf ("%" G_GUINT64_FORMAT, value);

  return desc;
}

static gchar*
describe_type (GParamSpec *spec)
{
  gchar *desc;
  gchar *lower;
  gchar *upper;

  if (G_IS_PARAM_SPEC_CHAR (spec))
    {
      GParamSpecChar *pspec = G_PARAM_SPEC_CHAR (spec);

      lower = describe_signed_constant (sizeof(gchar), pspec->minimum);
      upper = describe_signed_constant (sizeof(gchar), pspec->maximum);
      if (pspec->minimum == G_MININT8 && pspec->maximum == G_MAXINT8)
        desc = g_strdup ("");
      else if (pspec->minimum == G_MININT8)
        desc = g_strdup_printf ("<= %s", upper);
      else if (pspec->maximum == G_MAXINT8)
        desc = g_strdup_printf (">= %s", lower);
      else
        desc = g_strdup_printf ("[%s,%s]", lower, upper);
      g_free (lower);
      g_free (upper);
    }
  else if (G_IS_PARAM_SPEC_UCHAR (spec))
    {
      GParamSpecUChar *pspec = G_PARAM_SPEC_UCHAR (spec);

      lower = describe_unsigned_constant (sizeof(guchar), pspec->minimum);
      upper = describe_unsigned_constant (sizeof(guchar), pspec->maximum);
      if (pspec->minimum == 0 && pspec->maximum == G_MAXUINT8)
        desc = g_strdup ("");
      else if (pspec->minimum == 0)
        desc = g_strdup_printf ("<= %s", upper);
      else if (pspec->maximum == G_MAXUINT8)
        desc = g_strdup_printf (">= %s", lower);
      else
        desc = g_strdup_printf ("[%s,%s]", lower, upper);
      g_free (lower);
      g_free (upper);
    }
  else if (G_IS_PARAM_SPEC_INT (spec))
    {
      GParamSpecInt *pspec = G_PARAM_SPEC_INT (spec);

      lower = describe_signed_constant (sizeof(gint), pspec->minimum);
      upper = describe_signed_constant (sizeof(gint), pspec->maximum);
      if (pspec->minimum == G_MININT && pspec->maximum == G_MAXINT)
        desc = g_strdup ("");
      else if (pspec->minimum == G_MININT)
        desc = g_strdup_printf ("<= %s", upper);
      else if (pspec->maximum == G_MAXINT)
        desc = g_strdup_printf (">= %s", lower);
      else
        desc = g_strdup_printf ("[%s,%s]", lower, upper);
      g_free (lower);
      g_free (upper);
    }
  else if (G_IS_PARAM_SPEC_UINT (spec))
    {
      GParamSpecUInt *pspec = G_PARAM_SPEC_UINT (spec);

      lower = describe_unsigned_constant (sizeof(guint), pspec->minimum);
      upper = describe_unsigned_constant (sizeof(guint), pspec->maximum);
      if (pspec->minimum == 0 && pspec->maximum == G_MAXUINT)
        desc = g_strdup ("");
      else if (pspec->minimum == 0)
        desc = g_strdup_printf ("<= %s", upper);
      else if (pspec->maximum == G_MAXUINT)
        desc = g_strdup_printf (">= %s", lower);
      else
        desc = g_strdup_printf ("[%s,%s]", lower, upper);
      g_free (lower);
      g_free (upper);
    }
  else if (G_IS_PARAM_SPEC_LONG (spec))
    {
      GParamSpecLong *pspec = G_PARAM_SPEC_LONG (spec);

      lower = describe_signed_constant (sizeof(glong), pspec->minimum);
      upper = describe_signed_constant (sizeof(glong), pspec->maximum);
      if (pspec->minimum == G_MINLONG && pspec->maximum == G_MAXLONG)
        desc = g_strdup ("");
      else if (pspec->minimum == G_MINLONG)
        desc = g_strdup_printf ("<= %s", upper);
      else if (pspec->maximum == G_MAXLONG)
        desc = g_strdup_printf (">= %s", lower);
      else
        desc = g_strdup_printf ("[%s,%s]", lower, upper);
      g_free (lower);
      g_free (upper);
    }
  else if (G_IS_PARAM_SPEC_ULONG (spec))
    {
      GParamSpecULong *pspec = G_PARAM_SPEC_ULONG (spec);

      lower = describe_unsigned_constant (sizeof(gulong), pspec->minimum);
      upper = describe_unsigned_constant (sizeof(gulong), pspec->maximum);
      if (pspec->minimum == 0 && pspec->maximum == G_MAXULONG)
        desc = g_strdup ("");
      else if (pspec->minimum == 0)
        desc = g_strdup_printf ("<= %s", upper);
      else if (pspec->maximum == G_MAXULONG)
        desc = g_strdup_printf (">= %s", lower);
      else
        desc = g_strdup_printf ("[%s,%s]", lower, upper);
      g_free (lower);
      g_free (upper);
    }
  else if (G_IS_PARAM_SPEC_INT64 (spec))
    {
      GParamSpecInt64 *pspec = G_PARAM_SPEC_INT64 (spec);

      lower = describe_signed_constant (sizeof(gint64), pspec->minimum);
      upper = describe_signed_constant (sizeof(gint64), pspec->maximum);
      if (pspec->minimum == G_MININT64 && pspec->maximum == G_MAXINT64)
        desc = g_strdup ("");
      else if (pspec->minimum == G_MININT64)
        desc = g_strdup_printf ("<= %s", upper);
      else if (pspec->maximum == G_MAXINT64)
        desc = g_strdup_printf (">= %s", lower);
      else
        desc = g_strdup_printf ("[%s,%s]", lower, upper);
      g_free (lower);
      g_free (upper);
    }
  else if (G_IS_PARAM_SPEC_UINT64 (spec))
    {
      GParamSpecUInt64 *pspec = G_PARAM_SPEC_UINT64 (spec);

      lower = describe_unsigned_constant (sizeof(guint64), pspec->minimum);
      upper = describe_unsigned_constant (sizeof(guint64), pspec->maximum);
      if (pspec->minimum == 0 && pspec->maximum == G_MAXUINT64)
        desc = g_strdup ("");
      else if (pspec->minimum == 0)
        desc = g_strdup_printf ("<= %s", upper);
      else if (pspec->maximum == G_MAXUINT64)
        desc = g_strdup_printf (">= %s", lower);
      else
        desc = g_strdup_printf ("[%s,%s]", lower, upper);
      g_free (lower);
      g_free (upper);
    }
  else if (G_IS_PARAM_SPEC_FLOAT (spec))
    {
      GParamSpecFloat *pspec = G_PARAM_SPEC_FLOAT (spec);

      lower = describe_double_constant (pspec->minimum);
      upper = describe_double_constant (pspec->maximum);
      if (GTKDOC_COMPARE_FLOAT (pspec->minimum, -G_MAXFLOAT))
        {
          if (GTKDOC_COMPARE_FLOAT (pspec->maximum, G_MAXFLOAT))
            desc = g_strdup ("");
          else
            desc = g_strdup_printf ("<= %s", upper);
        }
      else if (GTKDOC_COMPARE_FLOAT (pspec->maximum, G_MAXFLOAT))
        desc = g_strdup_printf (">= %s", lower);
      else
        desc = g_strdup_printf ("[%s,%s]", lower, upper);
      g_free (lower);
      g_free (upper);
    }
  else if (G_IS_PARAM_SPEC_DOUBLE (spec))
    {
      GParamSpecDouble *pspec = G_PARAM_SPEC_DOUBLE (spec);

      lower = describe_double_constant (pspec->minimum);
      upper = describe_double_constant (pspec->maximum);
      if (GTKDOC_COMPARE_FLOAT (pspec->minimum, -G_MAXDOUBLE))
        {
          if (GTKDOC_COMPARE_FLOAT (pspec->maximum, G_MAXDOUBLE))
            desc = g_strdup ("");
          else
            desc = g_strdup_printf ("<= %s", upper);
        }
      else if (GTKDOC_COMPARE_FLOAT (pspec->maximum, G_MAXDOUBLE))
        desc = g_strdup_printf (">= %s", lower);
      else
        desc = g_strdup_printf ("[%s,%s]", lower, upper);
      g_free (lower);
      g_free (upper);
    }
#if GLIB_CHECK_VERSION (2, 12, 0)
  else if (G_IS_PARAM_SPEC_GTYPE (spec))
    {
      GParamSpecGType *pspec = G_PARAM_SPEC_GTYPE (spec);
      gboolean is_pointer;

      desc = g_strdup (get_type_name (pspec->is_a_type, &is_pointer));
    }
#endif
#if GLIB_CHECK_VERSION (2, 25, 9)
  else if (G_IS_PARAM_SPEC_VARIANT (spec))
    {
      GParamSpecVariant *pspec = G_PARAM_SPEC_VARIANT (spec);
      gchar *variant_type;

      variant_type = g_variant_type_dup_string (pspec->type);
      desc = g_strdup_printf ("GVariant<%s>", variant_type);
      g_free (variant_type);
    }
#endif
  else
    {
      desc = g_strdup ("");
    }

  return desc;
}

static gchar*
describe_default (GParamSpec *spec)
{
  gchar *desc;

  if (G_IS_PARAM_SPEC_CHAR (spec))
    {
      GParamSpecChar *pspec = G_PARAM_SPEC_CHAR (spec);

      desc = g_strdup_printf ("%d", pspec->default_value);
    }
  else if (G_IS_PARAM_SPEC_UCHAR (spec))
    {
      GParamSpecUChar *pspec = G_PARAM_SPEC_UCHAR (spec);

      desc = g_strdup_printf ("%u", pspec->default_value);
    }
  else if (G_IS_PARAM_SPEC_BOOLEAN (spec))
    {
      GParamSpecBoolean *pspec = G_PARAM_SPEC_BOOLEAN (spec);

      desc = g_strdup_printf ("%s", pspec->default_value ? "TRUE" : "FALSE");
    }
  else if (G_IS_PARAM_SPEC_INT (spec))
    {
      GParamSpecInt *pspec = G_PARAM_SPEC_INT (spec);

      desc = g_strdup_printf ("%d", pspec->default_value);
    }
  else if (G_IS_PARAM_SPEC_UINT (spec))
    {
      GParamSpecUInt *pspec = G_PARAM_SPEC_UINT (spec);

      desc = g_strdup_printf ("%u", pspec->default_value);
    }
  else if (G_IS_PARAM_SPEC_LONG (spec))
    {
      GParamSpecLong *pspec = G_PARAM_SPEC_LONG (spec);

      desc = g_strdup_printf ("%ld", pspec->default_value);
    }
  else if (G_IS_PARAM_SPEC_LONG (spec))
    {
      GParamSpecULong *pspec = G_PARAM_SPEC_ULONG (spec);

      desc = g_strdup_printf ("%lu", pspec->default_value);
    }
  else if (G_IS_PARAM_SPEC_INT64 (spec))
    {
      GParamSpecInt64 *pspec = G_PARAM_SPEC_INT64 (spec);

      desc = g_strdup_printf ("%" G_GINT64_FORMAT, pspec->default_value);
    }
  else if (G_IS_PARAM_SPEC_UINT64 (spec))
    {
      GParamSpecUInt64 *pspec = G_PARAM_SPEC_UINT64 (spec);

      desc = g_strdup_printf ("%" G_GUINT64_FORMAT, pspec->default_value);
    }
  else if (G_IS_PARAM_SPEC_UNICHAR (spec))
    {
      GParamSpecUnichar *pspec = G_PARAM_SPEC_UNICHAR (spec);

      if (g_unichar_isprint (pspec->default_value))
        desc = g_strdup_printf ("'%c'", pspec->default_value);
      else
        desc = g_strdup_printf ("%u", pspec->default_value);
    }
  else if (G_IS_PARAM_SPEC_ENUM (spec))
    {
      GParamSpecEnum *pspec = G_PARAM_SPEC_ENUM (spec);

      GEnumValue *value = g_enum_get_value (pspec->enum_class, pspec->default_value);
      if (value)
        desc = g_strdup_printf ("%s", value->value_name);
      else
        desc = g_strdup_printf ("%d", pspec->default_value);
    }
  else if (G_IS_PARAM_SPEC_FLAGS (spec))
    {
      GParamSpecFlags *pspec = G_PARAM_SPEC_FLAGS (spec);
      guint default_value;
      GString *acc;

      default_value = pspec->default_value;
      acc = g_string_new ("");

      while (default_value)
        {
          GFlagsValue *value = g_flags_get_first_value (pspec->flags_class, default_value);

          if (!value)
            break;

          if (acc->len > 0)
            g_string_append (acc, " | ");
          g_string_append (acc, value->value_name);

          default_value &= ~value->value;
        }

      if (default_value == 0)
        desc = g_string_free (acc, FALSE);
      else
        {
          desc = g_strdup_printf ("%d", pspec->default_value);
          g_string_free (acc, TRUE);
        }
    }
  else if (G_IS_PARAM_SPEC_FLOAT (spec))
    {
      GParamSpecFloat *pspec = G_PARAM_SPEC_FLOAT (spec);

      /* make sure floats are output with a decimal dot irrespective of
       * current locale. Use formatd since we want human-readable numbers
       * and do not need the exact same bit representation when deserialising */
      desc = g_malloc0 (G_ASCII_DTOSTR_BUF_SIZE);
      g_ascii_formatd (desc, G_ASCII_DTOSTR_BUF_SIZE, "%g",
          pspec->default_value);
    }
  else if (G_IS_PARAM_SPEC_DOUBLE (spec))
    {
      GParamSpecDouble *pspec = G_PARAM_SPEC_DOUBLE (spec);

      /* make sure floats are output with a decimal dot irrespective of
       * current locale. Use formatd since we want human-readable numbers
       * and do not need the exact same bit representation when deserialising */
      desc = g_malloc0 (G_ASCII_DTOSTR_BUF_SIZE);
      g_ascii_formatd (desc, G_ASCII_DTOSTR_BUF_SIZE, "%g",
          pspec->default_value);
    }
  else if (G_IS_PARAM_SPEC_STRING (spec))
    {
      GParamSpecString *pspec = G_PARAM_SPEC_STRING (spec);

      if (pspec->default_value)
        {
          gchar *esc = g_strescape (pspec->default_value, NULL);

          desc = g_strdup_printf ("\"%s\"", esc);

          g_free (esc);
        }
      else
        desc = g_strdup_printf ("NULL");
    }
#if GLIB_CHECK_VERSION (2, 25, 9)
  else if (G_IS_PARAM_SPEC_VARIANT (spec))
    {
      GParamSpecVariant *pspec = G_PARAM_SPEC_VARIANT (spec);

      if (pspec->default_value)
        desc = g_variant_print (pspec->default_value, TRUE);
      else
        desc = g_strdup ("NULL");
    }
#endif
  else
    {
      desc = g_strdup ("");
    }

  return desc;
}


static void
output_object_args (FILE *fp, GType object_type)
{
  gpointer class;
  const gchar *object_class_name;
  guint arg;
  gchar flags[16], *pos;
  GParamSpec **properties;
  guint n_properties;
  gboolean child_prop;
  gboolean style_prop;
  gboolean is_pointer;
  const gchar *type_name;
  gchar *type_desc;
  gchar *default_value;

  if (G_TYPE_IS_OBJECT (object_type))
    {
      class = g_type_class_peek (object_type);
      if (!class)
        return;

      properties = g_object_class_list_properties (class, &n_properties);
    }
#if GLIB_MAJOR_VERSION > 2 || (GLIB_MAJOR_VERSION == 2 && GLIB_MINOR_VERSION >= 3)
  else if (G_TYPE_IS_INTERFACE (object_type))
    {
      class = g_type_default_interface_ref (object_type);

      if (!class)
        return;

      properties = g_object_interface_list_properties (class, &n_properties);
    }
#endif
  else
    return;

  object_class_name = g_type_name (object_type);

  child_prop = FALSE;
  style_prop = FALSE;

  while (TRUE) {
    qsort (properties, n_properties, sizeof (GParamSpec *), compare_param_specs);
    for (arg = 0; arg < n_properties; arg++)
      {
        GParamSpec *spec = properties[arg];
        const gchar *nick, *blurb, *dot;

        if (spec->owner_type != object_type)
          continue;

        pos = flags;
        /* We use one-character flags for simplicity. */
        if (child_prop && !style_prop)
             *pos++ = 'c';
        if (style_prop)
             *pos++ = 's';
        if (spec->flags & G_PARAM_READABLE)
           *pos++ = 'r';
        if (spec->flags & G_PARAM_WRITABLE)
          *pos++ = 'w';
        if (spec->flags & G_PARAM_CONSTRUCT)
          *pos++ = 'x';
        if (spec->flags & G_PARAM_CONSTRUCT_ONLY)
          *pos++ = 'X';
        *pos = 0;

        nick = g_param_spec_get_nick (spec);
        blurb = g_param_spec_get_blurb (spec);

        dot = "";
        if (blurb) {
          int str_len = strlen (blurb);
          if (str_len > 0  && blurb[str_len - 1] != '.')
            dot = ".";
        }

        type_desc = describe_type (spec);
        default_value = describe_default (spec);
        type_name = get_type_name (spec->value_type, &is_pointer);
        fprintf (fp, "<ARG>\n<NAME>%s::%s</NAME>\n<TYPE>%s%s</TYPE>\n<RANGE>%s</RANGE>\n<FLAGS>%s</FLAGS>\n<NICK>%s</NICK>\n<BLURB>%s%s</BLURB>\n<DEFAULT>%s</DEFAULT>\n</ARG>\n\n",
                 object_class_name, g_param_spec_get_name (spec), type_name, is_pointer ? "*" : "", type_desc, flags, nick ? nick : "(null)", blurb ? blurb : "(null)", dot, default_value);
        g_free (type_desc);
        g_free (default_value);
      }

    g_free (properties);

#ifdef GTK_IS_CONTAINER_CLASS
    if (!child_prop && GTK_IS_CONTAINER_CLASS (class)) {
      properties = gtk_container_class_list_child_properties (class, &n_properties);
      child_prop = TRUE;
      continue;
    }
#endif

#ifdef GTK_IS_CELL_AREA_CLASS
    if (!child_prop && GTK_IS_CELL_AREA_CLASS (class)) {
      properties = gtk_cell_area_class_list_cell_properties (class, &n_properties);
      child_prop = TRUE;
      continue;
    }
#endif

#ifdef GTK_IS_WIDGET_CLASS
#if GTK_CHECK_VERSION(2,1,0)
    if (!style_prop && GTK_IS_WIDGET_CLASS (class)) {
      properties = gtk_widget_class_list_style_properties (GTK_WIDGET_CLASS (class), &n_properties);
      style_prop = TRUE;
      continue;
    }
#endif
#endif

    break;
  }
}
