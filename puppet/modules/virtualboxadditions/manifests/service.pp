
class virtualboxadditions::service inherits virtualboxadditions {
  $services = [
    'vboxadd-x11'
  ]
  service { $services:
    ensure => 'running',
    enable => true,
    require => Class['virtualboxadditions::install'],
  }
}
