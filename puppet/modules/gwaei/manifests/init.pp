
class gwaei ($display_manager = undef) {
  notify { "gwaei": }
  ->
  class { 'gwaei::install': }

  contain 'gwaei::install'
}
