#!/bin/sh


if ! getent group docker > /dev/null 2>&1; then
    sudo groupadd docker
fi
echo "<< Adding [$USER] to docker group"
sudo usermod -aG docker $USER
echo "<< Refreshing docker group"
#newgrp docker
#echo "<< Test run sample docker"
#docker run hello-world
newgrp docker <<EOF
    echo "<< Run sample docker"
    docker run hello-world
    echo "<< Removing sample docker"
    docker rmi hello-world:latest -f
EOF
