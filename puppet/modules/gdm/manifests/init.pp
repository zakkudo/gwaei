
class gdm {
  notify { "gdm": }
  ->
  class { 'gdm::install': }
  ->
  class { 'gdm::config': }
  ->
  class { 'gdm::service': }

  contain 'gdm::install', 'gdm::config', 'gdm::service'
}

