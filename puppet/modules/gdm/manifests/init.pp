
class gdm {
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

  file { "/etc/gdm/custom.conf":
    content => template("gdm/custom.conf.erb"),
    ensure => "present",
    require => Package["gdm"],
  }

  service { "gdm":
    ensure => "running",
    enable => true,
    require => File["/etc/gdm/custom.conf"],
  }
}

