#!/bin/bash

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

echo -e "${BLUE}===================================================="
echo -e "${BLUE}       REIF Installation Script - Step-by-Step"
echo -e "${BLUE}====================================================${NC}"

# Function to get the distro name
get_distro() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        DISTRO=$ID
    else
        echo -e "${RED}Unknown distribution${NC}"
        exit 1
    fi
}

# Dependency installation functions for each distribution
install_debian() {
    echo -e "${YELLOW}Installing dependencies for Debian/Ubuntu...${NC}"
    sudo apt update && sudo apt install -y \
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
    echo -e "${YELLOW}Installing dependencies for Fedora/CentOS/RHEL...${NC}"
    sudo dnf groupinstall "Development Tools" -y
    sudo dnf install -y \
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
    echo -e "${YELLOW}Installing dependencies for Arch Linux...${NC}"
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
    echo -e "${YELLOW}Installing dependencies for openSUSE/SLES...${NC}"
    sudo zypper install -t pattern devel_basis -y
    sudo zypper install -y \
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
    echo -e "${YELLOW}Installing dependencies for Void Linux...${NC}"
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

# Get the distro and display it
get_distro
echo -e "${CYAN}Detected distribution: $DISTRO${NC}"

# Install dependencies based on the distribution
case "$DISTRO" in
    ubuntu|debian)
        install_debian
        ;;
    fedora|centos|rhel)
        install_redhat
        ;;
    arch)
        install_arch
        ;;
    opensuse|sles)
        install_suse
        ;;
    void)
        install_void
        ;;
    *)
        echo -e "${RED}Unsupported distribution: $DISTRO${NC}"
        exit 1
        ;;
esac

# Cloning and installing RUNARA
echo -e "${BLUE}Cloning RUNARA...${NC}"
if [ ! -d "vendor/runara" ]; then
    git clone https://github.com/cococry/runara vendor/runara
else
    echo -e "${CYAN}RUNARA is already cloned in vendor/runara, skipping...${NC}"
fi

echo -e "${YELLOW}Compiling RUNARA...${NC}"
make -C vendor/runara

echo -e "${YELLOW}Installing RUNARA...${NC}"
sudo make -C vendor/runara install

# Compiling and Installing REIF
echo -e "${BLUE}Compiling REIF...${NC}"
make

echo -e "${BLUE}Installing REIF...${NC}"
sudo make install

# Final success message
if [ $? -eq 0 ]; then
    echo -e "${GREEN}========================================="
    echo -e "${GREEN}   Installation successful! 🎉"
    echo -e "${GREEN}=========================================${NC}"
else
    echo -e "${RED}Installation failed! Please check the error messages above.${NC}"
    exit 1
fi

