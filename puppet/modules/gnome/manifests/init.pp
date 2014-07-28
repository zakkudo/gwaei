

class gnome {

#  $packages = [
#    "gcc",
#    "autoconf",
#    "automake",
#    "make",
#    "intltool",
#    "gettext",
#    "rarian-compat",
#    "gnome-doc-utils",
#    "dconf",
#    "gsettings-desktop-schemas",
#    "gtk3-devel",
#    "libstdc++-devel",
#    "libtool",
#  ]
  include gnome::install, gnome::config
}
