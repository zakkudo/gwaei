
class gnome::install inherits gnome {
  $packages = [
    "gnome-desktop3",
    "gnome-desktop3-devel",
    "gnome-tweak-tool",
    "gnome-screenshot",
    "ibus",
    "ibus-anthy",
    "ibus-gtk3",
    "control-center",
    "dconf-editor",
    "glade3",
    "firefox",
    "mozilla-adblockplus",
    "mozilla-noscript",
    "abattis-cantarell-fonts",
    "liberation-mono-fonts",
    "liberation-sans-fonts",
    "liberation-serif-fonts",
    "bitstream-vera-sans-fonts",
    "bitstream-vera-sans-mono-fonts",
    "bitstream-vera-serif-fonts",
    "google-droid-sans-fonts",
    "google-droid-sans-mono-fonts",
    "google-droid-serif-fonts",
    "levien-inconsolata-fonts"
  ]

  package { $packages:
    ensure => "installed"
  }
}

