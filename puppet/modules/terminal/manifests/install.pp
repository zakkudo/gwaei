
class terminal::install inherits terminal {
  $packages = [
    "gnome-terminal",
  ]

  if ($display_manager != undef) {
    package { $packages:
      ensure => "installed",
      before => $display_manager,
    }
  }
  else {
    package { $packages:
      ensure => "installed",
    }
  }
}
