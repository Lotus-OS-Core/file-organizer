#!/bin/bash
#
# forg - File Organizer Uninstallation Script
# 
# This script removes the forg file organizer from /usr/local/bin
#

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
BINARY_NAME="forg"
MAN_PAGE="forg.1"
INSTALL_PATH="/usr/local/bin/${BINARY_NAME}"
MAN_DIR="/usr/local/share/man/man1"
MAN_PATH="${MAN_DIR}/${MAN_PAGE}"

# Print functions
print_step() {
    echo -e "${BLUE}[STEP]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[OK]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Banner
echo -e "${BLUE}╔═══════════════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║${NC}  ${GREEN}forg${NC} - File Organizer Uninstaller v1.0.0          ${BLUE}║${NC}"
echo -e "${BLUE}╚═══════════════════════════════════════════════════════════╝${NC}"
echo ""

# Check if binary exists
print_step "Checking if ${BINARY_NAME} is installed..."
if [ ! -f "${INSTALL_PATH}" ]; then
    print_warning "${BINARY_NAME} is not installed at ${INSTALL_PATH}"
    echo "Nothing to uninstall."
    exit 0
fi
print_success "Found ${BINARY_NAME} at ${INSTALL_PATH}"

# Check if we need sudo for removal
print_step "Checking removal permissions..."
if [ ! -w "${INSTALL_PATH}" ]; then
    print_warning "Removal from ${INSTALL_PATH} requires root privileges"
    SUDO="sudo"
else
    SUDO=""
fi

# Remove the binary
print_step "Removing ${BINARY_NAME}..."
if ${SUDO} rm -f "${INSTALL_PATH}"; then
    print_success "Binary removed from ${INSTALL_PATH}"
else
    print_error "Failed to remove binary"
    exit 1
fi

# Remove man page
print_step "Checking for man page..."
MAN_REMOVED=false
if [ -f "${MAN_PATH}.gz" ]; then
    if ${SUDO} rm -f "${MAN_PATH}.gz"; then
        print_success "Man page removed: ${MAN_PATH}.gz"
        MAN_REMOVED=true
    fi
fi
if [ -f "${MAN_PATH}" ]; then
    if ${SUDO} rm -f "${MAN_PATH}"; then
        print_success "Man page removed: ${MAN_PATH}"
        MAN_REMOVED=true
    fi
fi
if [ "${MAN_REMOVED}" = false ]; then
    print_warning "No man page found to remove"
fi

# Final message
echo ""
echo -e "${GREEN}═══════════════════════════════════════════════════════════${NC}"
echo -e "${GREEN}  Uninstallation Complete!${NC}"
echo -e "${GREEN}═══════════════════════════════════════════════════════════${NC}"
echo ""
echo -e "  ${YELLOW}Removed:${NC} ${INSTALL_PATH}"
if [ "${MAN_REMOVED}" = true ]; then
    echo -e "  ${YELLOW}Removed:${NC} Man page"
fi
echo ""
echo -e "  Thank you for using ${GREEN}forg${NC}!"
echo ""
