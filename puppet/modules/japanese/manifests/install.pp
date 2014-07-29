
class japanese::install inherits japanese {
  exec { 'japanese_groupinstall':
    command => 'yum groupinstall japanese-support -y -q',
  }
#  ->
#  exec { 'japanese_langinstall':
#    command => 'sudo yum langinstall ja_JP -y -q',
#  }
}
