
class git {
  notify { "git": }
  ->
  class { 'git::install': }

  contain 'git::install'
}

