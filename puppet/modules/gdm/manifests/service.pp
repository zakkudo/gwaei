
class gdm::service inherits gdm {
  service { "gdm":
    provider => "systemd",
    ensure => "running",
    enable => true,
    require => Class['gdm::config'],
  }
}
