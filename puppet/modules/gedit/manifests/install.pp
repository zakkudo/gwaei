
class gedit::install inherits gedit {
  package { "gedit":
    ensure => "installed",
  }
}
