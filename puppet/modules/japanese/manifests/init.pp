
class japanese {
  class { 'japanese::install': }

  contain 'japanese::install'
}
