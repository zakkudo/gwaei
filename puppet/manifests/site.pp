Exec { path => "/usr/bin:/usr/sbin/:/bin:/sbin" }

node default {
  include gnome
  include gdm
  include gwaei
  include vim
  include git
  include gedit
  include terminal
}


#  package { $packages:
#    ensure => "installed",
#  }
#}


#class rpmfusion {
#http://rpmfusion.org/Configuration

#http://download1.rpmfusion.org/free/fedora/rpmfusion-free-release-20.noarch.rpm
#http://download1.rpmfusion.org/nonfree/fedora/rpmfusion-nonfree-release-20.noarch.rpm

#yum localinstall --nogpgcheck http://download1.rpmfusion.org/free/fedora/rpmfusion-free-release-$(rpm -E %fedora).noarch.rpm http://download1.rpmfusion.org/nonfree/fedora/rpmfusion-nonfree-release-$(rpm -E %fedora).noarch.rpm -y
#yum install VirtualBox-guest

