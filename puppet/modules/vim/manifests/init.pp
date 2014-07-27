
class vim { 
  package { "vim-enhanced":
    ensure => "installed",
  }

  package { "vim-minimal":
    ensure => "latest",
    before => Package["vim-enhanced"],
  }
}
