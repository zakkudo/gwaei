
class virtualboxadditions {
  notify { 'virtualboxadditions': }
  ->
  class { 'virtualboxadditions::install': }
  ->
  class { 'virtualboxadditions::service': }

  contain 'virtualboxadditions::install', 'virtualboxadditions::service'
}
