
class git::install inherits git {
  package { "git":
    ensure => "installed",
  }
}
