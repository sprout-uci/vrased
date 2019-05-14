#!/bin/sh
# https://spot.lrde.epita.fr/install.html
wget -q -O - https://www.lrde.epita.fr/repo/debian.gpg | apt-key add -
echo 'deb http://www.lrde.epita.fr/repo/debian/ stable/' >> /etc/apt/sources.list
apt-get update
apt-get install spot libspot-dev spot-doc python3-spot
