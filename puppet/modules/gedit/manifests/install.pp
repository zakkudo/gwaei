
class gedit::install inherits gedit {
  $packages = [
   "gedit"
  ]
  package { $packages:
    ensure => "installed",
    require => Class['gnome'],
  }
}
