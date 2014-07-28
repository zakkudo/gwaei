
class gdm::service inherits gdm {
  service { "gdm":
    ensure => "running",
    enable => true,
    require => Class['gdm::config'],
  }
}
