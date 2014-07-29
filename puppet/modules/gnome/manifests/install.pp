
class gnome::install inherits gnome {
  $packages = [
    'mesa-demos',
    'glx-utils',
    'mesa-dri-drivers',
    'xorg-x11-utils',
    'xorg-x11-drivers',
    'xorg-x11-xinit',
    'xorg-x11-server-Xorg',
    'gnome-desktop3',
    'gnome-desktop3-devel',
    'ibus-anthy',
    'anthy',
    'liberation-mono-fonts',
    'liberation-sans-fonts',
    'liberation-serif-fonts',
    'wine-fonts',
    'terminus-fonts-console',
    'ipa-gothic-fonts',
    'ipa-mincho-fonts',
    'ipa-pgothic-fonts',
    'ipa-pmincho-fonts',
    'dejavu-sans-fonts',
    'dejavu-serif-fonts',
    'dejavu-sans-mono-fonts',
  ]
  package { $packages:
    ensure => 'installed',
  }
}

