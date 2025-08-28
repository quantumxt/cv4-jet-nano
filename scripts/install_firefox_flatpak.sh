#!/bin/sh

sudo apt update
echo "<< Installing Flatpak"
sudo apt install -y flatpak gnome-software-plugin-flatpak
echo "<< Add flatpak repo"
flatpak remote-add --if-not-exists flathub https://dl.flathub.org/repo/flathub.flatpakrepo
echo "<< Installing Firefox"
flatpak install flathub org.mozilla.firefox
echo 'export XDG_DATA_DIRS="$XDG_DATA_DIRS:/var/lib/flatpak/exports/share"' >> ~/.xsessionrc
echo -e "\n\nInstallation complete! Reboot the Jetson to use Firefox"
