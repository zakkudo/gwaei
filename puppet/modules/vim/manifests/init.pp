
class vim { 
  notify { "vim": }
  ->
  class { 'vim::install': }
  ->
  class { 'vim::config': }

  contain 'vim::install', 'vim::config'
}
