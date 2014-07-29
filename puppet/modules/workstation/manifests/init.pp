
class workstation {
  $display_manager = Service['gdm']

  notify { "workstation": }
  ->
  class { 'japanese': }
  ->
  class { 'vim': }
  ->
  class { 'git': }
  ->
  class { 'development_tools': }
  ->
  class { 'gnome': }
  ->
  class { 'virtualboxadditions': }
  ->
  class { 'gwaei': }
  ->
  class { 'gedit': }
  ->
  class { 'terminal': }
  ->
  class { 'gdm': }

  contain 'japanese', 'vim', 'git', 'development_tools', 'gnome', 'virtualboxadditions', 'gwaei', 'gedit', 'terminal', 'gdm'
}

