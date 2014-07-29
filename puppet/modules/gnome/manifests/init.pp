

class gnome ($display_manager = undef) {
  notify { "gnome": }
  ->
  class { "gnome::install": }
  ->
  class { "gnome::config": }

  contain 'gnome::install', 'gnome::config'
}
