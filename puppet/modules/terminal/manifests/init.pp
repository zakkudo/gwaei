
class terminal ($display_manager = undef, $username = 'vagrant') {
  notify { "terminal": }
  ->
  class { 'terminal::install': }
  ->
  class { 'terminal::config': }
  ->
  class { 'terminal::service': }

  contain 'terminal::install', 'terminal::config', 'terminal::service'
}
