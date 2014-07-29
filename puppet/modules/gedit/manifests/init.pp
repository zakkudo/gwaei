
class gedit ($display_manager = 'gdm') {
  notify { "gedit": }
  ->
  class { 'gedit::install': }

  contain 'gedit::install'
}
