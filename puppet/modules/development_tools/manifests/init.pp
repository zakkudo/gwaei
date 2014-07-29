
class development_tools {
  notify { "development_tools": }
  ->
  class { 'development_tools::install': }

  contain development_tools::install
}
