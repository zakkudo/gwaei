
class development_tools::install inherits development_tools {
  $packages = [
    "gcc",
    "autoconf",
    "automake",
    "make",
    "intltool",
    "gettext",
    "rarian-compat",
    "gnome-doc-utils",
    "dconf",
    "gsettings-desktop-schemas",
    "gtk3-devel",
    "libstdc++",
    "libstdc++-devel",
    "libstdc++-static",
    "libtool",
    "gdb",
  ]

  package { $packages:
    ensure => "installed"
  }
}
