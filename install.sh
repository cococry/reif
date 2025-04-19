#!/bin/bash

echo "- Installing Dependencies..."

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
    build-essential \
    pkg-config \
    libgl1-mesa-dev \
    libx11-dev \
    libxrandr-dev \
    libxrender-dev \
    libglfw3-dev \
    libfreetype6-dev \
    libharfbuzz-dev

  }

install_redhat() {
  sudo dnf groupinstall "Development Tools"
  sudo dnf install \
    pkgconf-pkg-config \
    mesa-libGL-devel \
    libX11-devel \
    libXrandr-devel \
    libXrender-devel \
    glfw-devel \
    freetype-devel \
    harfbuzz-devel
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
  sudo zypper install -t pattern devel_basis
  sudo zypper install \
    pkgconfig \
    Mesa-libGL-devel \
    libX11-devel \
    libXrandr-devel \
    libXrender-devel \
    glfw-devel \
    freetype2-devel \
    harfbuzz-devel

  }

install_void() {
  sudo xbps-install -y \
    base-devel \
    pkg-config \
    mesa \
    libX11-devel \
    libXrandr-devel \
    libXrender-devel \
    glfw-devel \
  freetype-devel \
  harfbuzz-devel
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

git clone https://github.com/cococry/runara vendor/runara 

echo "- Compiling RUNARA"
make -C vendor/runara 
echo "- Installing RUNARA"
sudo make -C vendor/runara install

echo "- Compiling REIF"
make 
echo "- Installing REIF"
sudo make install

if [ $? -eq 0 ]; then
echo -e "\033[32m- Installation successfull!\033[0m"
fi
