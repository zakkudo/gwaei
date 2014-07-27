# -*- mode: ruby -*-
# vi: set ft=ruby :
Vagrant.configure('2') do |config|
  config.vm.define "gwaei3" do |v|
    v.vm.box = "chef/fedora-20"
    v.vm.hostname = 'gWaei3'
    v.vm.provider :virtualbox do |vb| 
      vb.customize [
        "modifyvm", :id,
        "--memory", "1024",
        "--vram", "64",
        "--accelerate3d", "on",
      ]
      vb.gui = true;
    end
  end

$install_script = <<SCRIPT
yum install deltarpm -y -q
yum localinstall --nogpgcheck http://download1.rpmfusion.org/free/fedora/rpmfusion-free-release-$(rpm -E %fedora).noarch.rpm http://download1.rpmfusion.org/nonfree/fedora/rpmfusion-nonfree-release-$(rpm -E %fedora).noarch.rpm -y -q
yum install VirtualBox-guest puppet -y -q
yum update -y -q
SCRIPT

  config.vm.provision "shell", inline: $install_script

  config.vm.provision :puppet  do |puppet|
    puppet.manifests_path = "puppet/manifests"
    puppet.manifest_file = "site.pp"
    puppet.module_path = "puppet/modules"
  end
end

