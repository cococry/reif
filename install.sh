#!/bin/bash

echo "- Installing Dependencies..."

# Function to check for the distribution
get_distro() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        DISTRO=$ID
    else
        echo "Unknown distribution"
        exit 1
    fi
}

install_debian() {
    sudo apt update
    sudo apt install -y \
        libgl1-mesa-dev \
        libharfbuzz-dev \
        libfreetype6-dev \
        zlib1g-dev \
        libpng-dev \
        libcglm-dev \
        libx11-dev
}

install_redhat() {
    sudo dnf install -y \
        mesa-libGL-devel \
        harfbuzz-devel \
        freetype-devel \
        zlib-devel \
        libpng-devel \
        cglm-devel \
        libX11-devel
}

install_arch() {
    sudo pacman -S --noconfirm \
        mesa \
        harfbuzz \
        freetype2 \
        zlib \
        libpng \
        cglm \
        libx11
}

install_suse() {
    sudo zypper install -y \
        Mesa-devel \
        harfbuzz-devel \
        freetype2-devel \
        zlib-devel \
        libpng-devel \
        cglm-devel \
        libX11-devel
}

install_void() {
    sudo xbps-install -y \
        mesa-devel \
        harfbuzz-devel \
        freetype2-devel \
        zlib-devel \
        libpng-devel \
        cglm-devel \
        libX11-devel
}

get_distro

echo "Detected distribution: $DISTRO"

case "$DISTRO" in
    ubuntu|debian)
        echo "Installing dependencies for Debian/Ubuntu..."
        install_debian
        ;;
    fedora|centos|rhel)
        echo "Installing dependencies for Fedora/CentOS/RHEL..."
        install_redhat
        ;;
    arch)
        echo "Installing dependencies for Arch Linux..."
        install_arch
        ;;
    opensuse|sles)
        echo "Installing dependencies for openSUSE/SLES..."
        install_suse
        ;;
    void)
        echo "Installing dependencies for Void Linux..."
        install_void
        ;;
    *)
        echo "Unsupported distribution: $DISTRO"
        exit 1
        ;;
esac

echo "Dependencies installation complete!"

echo "- Compiling RUNARA"

make -C vendor/runara 
sudo make -C vendor/runara install
