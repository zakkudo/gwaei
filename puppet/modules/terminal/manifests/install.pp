
class terminal::install inherits terminal {
  package { "gnome-terminal":
    ensure => "installed",
  }
}
