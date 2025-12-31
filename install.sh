#!/bin/bash
#
# forg - File Organizer Installation Script
#
# This script compiles and installs the forg file organizer to /usr/local/bin
# It automatically detects available C++ standards and compiler capabilities
#

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SOURCE_FILE="main.cpp"
BINARY_NAME="forg"
MAN_PAGE="forg.1"
INSTALL_DIR="/usr/local/bin"
INSTALL_PATH="${INSTALL_DIR}/${BINARY_NAME}"
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

print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

# Banner
echo -e "${BLUE}╔═══════════════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║${NC}  ${GREEN}forg${NC} - File Organizer Installer v1.0.0              ${BLUE}║${NC}"
echo -e "${BLUE}╚═══════════════════════════════════════════════════════════╝${NC}"
echo ""

# Detect Linux distribution for package installation
detect_distro() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        case "$ID" in
            ubuntu|debian|linuxmint|pop)
                echo "debian"
                ;;
            fedora|rhel|centos|rocky|alma)
                echo "rhel"
                ;;
            arch|manjaro|endeavouros)
                echo "arch"
                ;;
            opensuse*|suse)
                echo "suse"
                ;;
            *)
                echo "unknown"
                ;;
        esac
    else
        echo "unknown"
    fi
}

# Install required development packages
install_dependencies() {
    local distro=$(detect_distro)
    print_warning "Attempting to install required development packages..."

    case "$distro" in
        debian)
            print_step "Detected Debian/Ubuntu-based system"
            if command -v apt-get &> /dev/null; then
                print_info "Running: sudo apt-get update && sudo apt-get install -y g++ build-essential libstdc++-12-dev"
                if sudo apt-get update && sudo apt-get install -y g++ build-essential libstdc++-12-dev; then
                    print_success "Dependencies installed successfully"
                    return 0
                else
                    print_error "Failed to install packages"
                    return 1
                fi
            fi
            ;;
        rhel)
            print_step "Detected RedHat/Fedora-based system"
            if command -v dnf &> /dev/null; then
                print_info "Running: sudo dnf install -y gcc-c++ make libstdc++-devel"
                if sudo dnf install -y gcc-c++ make libstdc++-devel; then
                    print_success "Dependencies installed successfully"
                    return 0
                else
                    print_error "Failed to install packages"
                    return 1
                fi
            fi
            ;;
        arch)
            print_step "Detected Arch Linux-based system"
            if command -v pacman &> /dev/null; then
                print_info "Running: sudo pacman -S --noconfirm gcc"
                if sudo pacman -S --noconfirm gcc; then
                    print_success "Dependencies installed successfully"
                    return 0
                else
                    print_error "Failed to install packages"
                    return 1
                fi
            fi
            ;;
        suse)
            print_step "Detected openSUSE-based system"
            if command -v zypper &> /dev/null; then
                print_info "Running: sudo zypper install -y gcc-c++ libstdc++6"
                if sudo zypper install -y gcc-c++ libstdc++6; then
                    print_success "Dependencies installed successfully"
                    return 0
                else
                    print_error "Failed to install packages"
                    return 1
                fi
            fi
            ;;
        *)
            print_error "Could not detect your Linux distribution"
            return 1
            ;;
    esac
}

# Check if source file exists
print_step "Checking source file..."
if [ ! -f "${SCRIPT_DIR}/${SOURCE_FILE}" ]; then
    print_error "Source file '${SOURCE_FILE}' not found in ${SCRIPT_DIR}"
    exit 1
fi
print_success "Source file found: ${SOURCE_FILE}"

# Check if man page exists
print_step "Checking man page..."
if [ ! -f "${SCRIPT_DIR}/${MAN_PAGE}" ]; then
    print_warning "Man page '${MAN_PAGE}' not found, skipping man page installation"
    MAN_PAGE_INSTALLED=false
else
    print_success "Man page found: ${MAN_PAGE}"
    MAN_PAGE_INSTALLED=true
fi

# Check for g++ compiler
print_step "Checking for g++ compiler..."
if ! command -v g++ &> /dev/null; then
    install_dependencies
    if ! command -v g++ &> /dev/null; then
        print_error "g++ compiler not found and could not be installed"
        exit 1
    fi
fi

# Get g++ version
GXX_VERSION=$(g++ --version | head -n1)
print_success "Found: ${GXX_VERSION}"

# Try different compilation approaches
CXX_STANDARD=""
CXX_FLAGS=""
COMPILE_CMD=""
COMPILATION_SUCCESS=false

print_step "Testing C++17 compilation..."

# Array of compilation approaches to try (in order of preference)
declare -a COMPILE_ATTEMPTS=(
    "g++ -std=c++17 -O2 -Wall -Wextra -o BINARY SOURCE -lstdc++fs"
    "g++ -std=c++17 -O2 -Wall -Wextra -o BINARY SOURCE"
    "g++ -std=c++2a -O2 -Wall -Wextra -o BINARY SOURCE -lstdc++fs"
    "g++ -std=c++20 -O2 -Wall -Wextra -o BINARY SOURCE"
)

# Try each compilation approach
for attempt in "${COMPILE_ATTEMPTS[@]}"; do
    # Replace placeholders with actual values
    cmd="${attempt/BINARY/${SCRIPT_DIR}/${BINARY_NAME}}"
    cmd="${cmd/SOURCE/${SCRIPT_DIR}/${SOURCE_FILE}}"

    print_info "Trying: ${cmd}"

    if $cmd 2>/dev/null; then
        if [ -f "${SCRIPT_DIR}/${BINARY_NAME}" ]; then
            # Extract the standard used
            if [[ "$cmd" == *"c++17"* ]]; then
                CXX_STANDARD="c++17"
            elif [[ "$cmd" == *"c++2a"* ]]; then
                CXX_STANDARD="c++2a (experimental C++20)"
            elif [[ "$cmd" == *"c++20"* ]]; then
                CXX_STANDARD="c++20"
            fi

            # Extract extra flags
            if [[ "$cmd" == *"-lstdc++fs"* ]]; then
                CXX_FLAGS="-lstdc++fs"
            fi

            COMPILE_CMD="$cmd"
            COMPILATION_SUCCESS=true
            print_success "Compilation successful with ${CXX_STANDARD}"
            break
        fi
    fi
done

# If all automatic attempts failed, try with pthread
if [ "$COMPILATION_SUCCESS" = false ]; then
    print_warning "Standard approaches failed, trying alternative method..."

    alt_cmd="g++ -std=c++17 -O2 -Wall -Wextra -pthread -o ${SCRIPT_DIR}/${BINARY_NAME} ${SCRIPT_DIR}/${SOURCE_FILE} 2>&1"
    alt_output=$(eval $alt_cmd)

    if [ -f "${SCRIPT_DIR}/${BINARY_NAME}" ]; then
        COMPILATION_SUCCESS=true
        CXX_STANDARD="c++17 (with pthread)"
        COMPILE_CMD="$alt_cmd"
        print_success "Compilation successful with ${CXX_STANDARD}"
    else
        print_info "Compiler output: ${alt_output}"
    fi
fi

# Final fallback: try compiling actual source to get detailed error
if [ "$COMPILATION_SUCCESS" = false ]; then
    print_warning "All automatic methods failed, analyzing actual error..."
    print_step "Running compilation to see detailed errors..."
    compile_output=$(g++ -std=c++17 -O2 -Wall -Wextra -o "${SCRIPT_DIR}/${BINARY_NAME}" "${SCRIPT_DIR}/${SOURCE_FILE}" 2>&1)
    echo "$compile_output"

    # Check for common issues
    if echo "$compile_output" | grep -q "filesystem"; then
        print_error "std::filesystem is not available on this system"
        print_info "This is likely due to an incomplete libstdc++ installation"

        # Try installing dependencies
        print_step "Attempting to install missing libraries..."
        install_dependencies

        # Try one more time after install
        print_step "Retrying compilation after package installation..."
        if g++ -std=c++17 -O2 -Wall -Wextra -o "${SCRIPT_DIR}/${BINARY_NAME}" "${SCRIPT_DIR}/${SOURCE_FILE}" 2>/dev/null; then
            COMPILATION_SUCCESS=true
            CXX_STANDARD="c++17"
            print_success "Compilation successful after installing dependencies!"
        fi
    fi
fi

# If still failed, give up with helpful message
if [ "$COMPILATION_SUCCESS" = false ]; then
    echo ""
    print_error "============================================"
    print_error "  COMPILATION FAILED"
    print_error "============================================"
    echo ""
    echo "Your system has g++ ${GXX_VERSION} but std::filesystem support"
    echo "is not working properly."
    echo ""
    echo "This is a known issue on some systems where the C++ standard"
    echo "library headers are incomplete."
    echo ""
    echo "SOLUTION:"
    echo "========="
    echo ""
    echo "1. Try installing the development package:"
    echo "   sudo apt update && sudo apt install libstdc++-12-dev"
    echo ""
    echo "2. Or upgrade your system packages:"
    echo "   sudo apt update && sudo apt upgrade"
    echo ""
    echo "3. Or try manually with explicit linking:"
    echo "   g++ -std=c++17 -O2 -Wall -Wextra -o forg main.cpp -lstdc++fs"
    echo ""
    echo "4. Or install from Ubuntu toolchain PPA (if on older Ubuntu):"
    echo "   sudo add-apt-repository ppa:ubuntu-toolchain-r/test"
    echo "   sudo apt update && sudo apt install g++-11"
    echo ""
    echo "============================================"
    exit 1
fi

# Check if binary was created
if [ ! -f "${SCRIPT_DIR}/${BINARY_NAME}" ]; then
    print_error "Binary not created after successful compilation check"
    exit 1
fi

print_success "Binary created: ${SCRIPT_DIR}/${BINARY_NAME}"

# Check if we need sudo for installation
print_step "Checking installation permissions..."
if [ ! -w "${INSTALL_DIR}" ]; then
    print_warning "Installation to ${INSTALL_DIR} requires root privileges"
    SUDO="sudo"
else
    SUDO=""
fi

# Install the binary
print_step "Installing ${BINARY_NAME} to ${INSTALL_PATH}..."
if ${SUDO} cp "${SCRIPT_DIR}/${BINARY_NAME}" "${INSTALL_PATH}"; then
    print_success "Binary installed to ${INSTALL_PATH}"
else
    print_error "Failed to install binary"
    exit 1
fi

# Set permissions
print_step "Setting executable permissions..."
if ${SUDO} chmod +x "${INSTALL_PATH}"; then
    print_success "Permissions set"
else
    print_error "Failed to set permissions"
    exit 1
fi

# Verify installation
print_step "Verifying installation..."
if [ -x "${INSTALL_PATH}" ]; then
    print_success "Installation verified"
else
    print_error "Installation verification failed"
    exit 1
fi

# Install man page
if [ "${MAN_PAGE_INSTALLED}" = true ]; then
    print_step "Installing man page..."

    # Check if man directory exists, create if not
    if [ ! -d "${MAN_DIR}" ]; then
        ${SUDO} mkdir -p "${MAN_DIR}"
    fi

    # Check if gzip is available for compression
    if command -v gzip &> /dev/null; then
        # Install compressed man page
        if ${SUDO} gzip -c "${SCRIPT_DIR}/${MAN_PAGE}" > "${MAN_PATH}.gz"; then
            print_success "Man page installed (compressed): ${MAN_PATH}.gz"
        else
            print_warning "Failed to install compressed man page, trying uncompressed..."
            ${SUDO} cp "${SCRIPT_DIR}/${MAN_PAGE}" "${MAN_PATH}"
            print_success "Man page installed: ${MAN_PATH}"
        fi
    else
        # Install uncompressed man page
        if ${SUDO} cp "${SCRIPT_DIR}/${MAN_PAGE}" "${MAN_PATH}"; then
            print_success "Man page installed: ${MAN_PATH}"
        else
            print_warning "Failed to install man page"
        fi
    fi
fi

# Final message
echo ""
echo -e "${GREEN}═══════════════════════════════════════════════════════════${NC}"
echo -e "${GREEN}  Installation Complete!${NC}"
echo -e "${GREEN}═══════════════════════════════════════════════════════════${NC}"
echo ""
echo -e "  ${YELLOW}Compiler:${NC} ${GXX_VERSION}"
if [ -n "$CXX_STANDARD" ]; then
    echo -e "  ${YELLOW}Standard:${NC} ${CXX_STANDARD}"
fi
if [ -n "$CXX_FLAGS" ]; then
    echo -e "  ${YELLOW}Flags:${NC} ${CXX_FLAGS}"
fi
echo ""
echo -e "  ${YELLOW}Usage:${NC}"
echo "    forg               # Organize files in current directory"
echo "    forg -r            # Recursive mode (process subdirectories)"
echo "    forg -r --depth 2  # Process up to 2 levels deep"
echo "    forg -p PREFIX_    # Organize with prefix"
echo "    forg -v            # Verbose mode"
echo "    forg -h            # Show help"
echo ""
echo -e "  ${YELLOW}Documentation:${NC}"
echo "    forg --help        # Quick help"
echo "    man forg           # Full manual page"
echo ""
echo -e "  ${YELLOW}Categories:${NC}"
echo "    Images, Videos, Audio, Documents, Archives, Code, Executables, etc."
echo ""
echo -e "  To uninstall, run: ${GREEN}./uninstall.sh${NC}"
echo ""
