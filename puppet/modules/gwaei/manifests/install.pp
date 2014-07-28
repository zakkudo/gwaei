
class gwaei::install inherits gwaei {
  $packages = [
    "hunspell-en-US",
    "hunspell-devel",
    "mecab-ipadic",
    "libcurl-devel",
    "mecab-devel",
    "kanjistrokeorders-fonts",
    "gnome-icon-theme-symbolic",
  ]

  package { $packages:
    ensure => "installed"
  }
}
