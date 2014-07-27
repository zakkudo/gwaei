node gnome_development_environment {
  include gnome
  include gdm
}

node default inherits gtk_development_environment {
}
