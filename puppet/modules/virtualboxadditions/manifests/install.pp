
class virtualboxadditions::install inherits virtualboxadditions {
#  exec { 'VirtualBox-guest':
#    command => 'yum install VirtualBox-guest -q -y',
#    creates => '/etc/init.d/vboxadd-x11',
#  }
  package { 'VirtualBox-guest':
    ensure => 'installed'
  }
}
