
class gdm::install inherits gdm {
  $packages = [
    "gdm",
  ]

  package { $packages:
    ensure => "installed"
  }
}

