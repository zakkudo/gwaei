
class gdm::install inherits gdm {
  $packages = [
    "gdm",
    "mesa-dri-drivers",
    "glx-utils",
    "xorg-x11-drivers",
    "xorg-x11-server-Xorg",
  ]

  package { $packages:
    ensure => "installed"
  }
}

