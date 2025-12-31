/**
 * forg - File Organizer CLI Application
 * 
 * A command-line tool that organizes files in the current directory
 * into categorized subfolders based on file extensions.
 * 
 * Features:
 * - Automatic file categorization by extension
 * - Prefix support for subdirectory names
 * - Recursive processing with depth control
 * - Verbose mode for detailed logging
 * - Collision handling with automatic renaming
 * - Color-coded output
 * 
 * Usage:
 *   forg               - Organize files in current directory (top-level only)
 *   forg -r            - Recursive - process current dir and all subdirs
 *   forg -r --depth 2  - Process up to 2 levels deep
 *   forg -p PREFIX     - Organize with prefix for folder names
 *   forg -v            - Verbose mode (show detailed logs)
 *   forg -h            - Show help message
 * 
 * Categories:
 *   Images, Videos, Audio, Documents, Archives, Code, Executables, Others
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <chrono>

namespace fs = std::filesystem;

// ANSI color codes for terminal output
const std::string COLOR_GREEN = "\033[32m";
const std::string COLOR_YELLOW = "\033[33m";
const std::string COLOR_RED = "\033[31m";
const std::string COLOR_BLUE = "\033[34m";
const std::string COLOR_RESET = "\033[0m";
const std::string COLOR_BOLD = "\033[1m";

// Program metadata
const std::string VERSION = "1.0.0";
const std::string PROGRAM_NAME = "forg";

// File extension to category mapping
const std::map<std::string, std::vector<std::string>> FILE_CATEGORIES = {
    {"Images", {"jpg", "jpeg", "png", "gif", "bmp", "tiff", "svg", "webp", "ico", "psd", "ai", "eps"}},
    {"Videos", {"mp4", "mkv", "avi", "mov", "wmv", "flv", "webm", "m4v", "mpeg", "mpg", "3gp", "rmvb"}},
    {"Audio", {"mp3", "wav", "flac", "aac", "ogg", "m4a", "wma", "aiff", "mid", "midi"}},
    {"Documents", {"pdf", "doc", "docx", "txt", "rtf", "odt", "xls", "xlsx", "ppt", "pptx", "csv", "md", "markdown", "log"}},
    {"Archives", {"zip", "rar", "7z", "tar", "gz", "bz2", "xz", "iso", "dmg", "pkg", "deb", "rpm"}},
    {"Code", {"cpp", "c", "h", "hpp", "py", "js", "ts", "html", "htm", "css", "scss", "java", "go", "rs", "rb", "php", "swift", "kt", "scala", "sh", "bash", "json", "xml", "yaml", "yml", "toml", "ini", "cfg", "conf"}},
    {"Executables", {"exe", "app", "bin", "msi", "run", "elf", "so", "dll", "dylib"}},
    {"Database", {"sql", "db", "sqlite", "mdb", "accdb", "frm", "ibd"}},
    {"Books", {"epub", "mobi", "azw", "azw3", "fb2", "djvu", "chm"}}
};

// Skip patterns - files and directories to ignore
const std::vector<std::string> SKIP_PATTERNS = {
    // Version control directories
    ".git", ".svn", ".hg", ".bzr",
    // IDE and editor directories
    ".vscode", ".idea", ".vs",
    // Build directories
    "build", "dist", "node_modules", ".cache", "__pycache__",
    // Special system files
    ".DS_Store", "Thumbs.db", ".Spotlight-V100", ".Trashes"
};

struct Config {
    std::string prefix = "";
    bool verbose = false;
    bool dryRun = false;
    bool recursive = false;
    int depth = 1;  // Default depth: current directory only
    bool showHelp = false;
    bool showVersion = false;
};

/**
 * Convert string to lowercase
 */
std::string toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

/**
 * Extract file extension from filename (without the dot)
 */
std::string getFileExtension(const std::string& filename) {
    size_t dotPos = filename.rfind('.');
    if (dotPos == std::string::npos || dotPos == filename.length() - 1) {
        return "";  // No extension found
    }
    return toLowerCase(filename.substr(dotPos + 1));
}

/**
 * Get category for a given file extension
 */
std::string getCategory(const std::string& extension) {
    if (extension.empty()) {
        return "Others";
    }
    
    for (const auto& [category, extensions] : FILE_CATEGORIES) {
        if (std::find(extensions.begin(), extensions.end(), extension) != extensions.end()) {
            return category;
        }
    }
    
    return "Others";
}

/**
 * Check if a file or directory should be skipped
 */
bool shouldSkip(const fs::path& path) {
    std::string filename = path.filename().string();
    
    // Skip hidden files (starting with dot)
    if (!filename.empty() && filename[0] == '.') {
        return true;
    }
    
    // Skip patterns
    for (const auto& pattern : SKIP_PATTERNS) {
        if (filename == pattern) {
            return true;
        }
    }
    
    return false;
}

/**
 * Generate a unique filename to avoid collisions
 */
fs::path getUniquePath(const fs::path& dir, const std::string& filename) {
    fs::path targetPath = dir / filename;
    
    if (!fs::exists(targetPath)) {
        return targetPath;
    }
    
    // File exists, generate a new name with counter
    size_t dotPos = filename.rfind('.');
    std::string baseName, extension;
    
    if (dotPos != std::string::npos && dotPos > 0) {
        baseName = filename.substr(0, dotPos);
        extension = filename.substr(dotPos);
    } else {
        baseName = filename;
        extension = "";
    }
    
    int counter = 1;
    while (true) {
        std::string newName = baseName + "_" + std::to_string(counter) + extension;
        fs::path newPath = dir / newName;
        
        if (!fs::exists(newPath)) {
            return newPath;
        }
        counter++;
        
        // Safety limit to prevent infinite loop
        if (counter > 1000) {
            // Use timestamp as fallback
            auto now = std::chrono::system_clock::now();
            auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()).count();
            std::string newName = baseName + "_" + std::to_string(timestamp) + extension;
            return dir / newName;
        }
    }
}

/**
 * Print colored output
 */
void printMessage(const std::string& message, const std::string& color) {
    std::cout << color << message << COLOR_RESET << std::endl;
}

void printMessage(const std::string& message) {
    std::cout << message << std::endl;
}

/**
 * Print help information
 */
void printHelp() {
    std::cout << COLOR_BOLD << "File Organizer - forg v" << VERSION << COLOR_RESET << std::endl;
    std::cout << std::endl;
    std::cout << "A command-line tool that organizes files into categorized subfolders." << std::endl;
    std::cout << std::endl;
    std::cout << COLOR_BOLD << "Usage:" << COLOR_RESET << std::endl;
    std::cout << "  forg [options]" << std::endl;
    std::cout << std::endl;
    std::cout << COLOR_BOLD << "Directory Options:" << COLOR_RESET << std::endl;
    std::cout << "  forg               # Top-level files only (default)" << std::endl;
    std::cout << "  forg -r            # Recursive - process all files in current dir and subdirs" << std::endl;
    std::cout << "  forg -r --depth N  # Process up to N levels deep (N >= 1)" << std::endl;
    std::cout << std::endl;
    std::cout << COLOR_BOLD << "Options:" << COLOR_RESET << std::endl;
    std::cout << "  -r, --recursive            # Enable recursive directory traversal" << std::endl;
    std::cout << "  -d, --depth <number>       # Maximum depth for recursion (default: 1)" << std::endl;
    std::cout << "  -p, --prefix <text>        # Add a prefix to category folder names" << std::endl;
    std::cout << "  -v, --verbose              # Show detailed progress information" << std::endl;
    std::cout << "  -n, --dry-run              # Preview what would be done without making changes" << std::endl;
    std::cout << "  -h, --help                 # Show this help message" << std::endl;
    std::cout << "  --version                  # Show version information" << std::endl;
    std::cout << std::endl;
    std::cout << COLOR_BOLD << "Examples:" << COLOR_RESET << std::endl;
    std::cout << "  forg                           # Organize top-level files only" << std::endl;
    std::cout << "  forg -r                        # Organize all files recursively" << std::endl;
    std::cout << "  forg -r --depth 2              # Organize files up to 2 levels deep" << std::endl;
    std::cout << "  forg -p backup_                # Organize with 'backup_' prefix" << std::endl;
    std::cout << "  forg -v                        # Organize with verbose output" << std::endl;
    std::cout << "  forg -p sorted_ -v             # Combine options" << std::endl;
    std::cout << std::endl;
    std::cout << COLOR_BOLD << "Categories:" << COLOR_RESET << std::endl;
    
    for (const auto& [category, extensions] : FILE_CATEGORIES) {
        std::cout << "  " << COLOR_BLUE << category << COLOR_RESET << ": ";
        
        // Print first few extensions
        size_t count = std::min(static_cast<size_t>(5), extensions.size());
        for (size_t i = 0; i < count; i++) {
            std::cout << extensions[i];
            if (i < count - 1) std::cout << ", ";
        }
        if (extensions.size() > 5) {
            std::cout << " + " << (extensions.size() - 5) << " more";
        }
        std::cout << std::endl;
    }
    
    std::cout << "  " << COLOR_BLUE << "Others" << COLOR_RESET << ": Files with unrecognized extensions" << std::endl;
}

/**
 * Parse command line arguments
 */
Config parseArguments(int argc, char* argv[]) {
    Config config;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            config.showHelp = true;
        } else if (arg == "--version") {
            config.showVersion = true;
        } else if (arg == "-v" || arg == "--verbose") {
            config.verbose = true;
        } else if (arg == "-n" || arg == "--dry-run") {
            config.dryRun = true;
        } else if (arg == "-r" || arg == "--recursive") {
            config.recursive = true;
        } else if ((arg == "-d" || arg == "--depth") && i + 1 < argc) {
            try {
                config.depth = std::stoi(argv[++i]);
                if (config.depth < 1) {
                    std::cerr << COLOR_YELLOW << "Warning: depth must be >= 1, using default" << COLOR_RESET << std::endl;
                    config.depth = 1;
                }
            } catch (...) {
                std::cerr << COLOR_YELLOW << "Warning: invalid depth value, using default" << COLOR_RESET << std::endl;
                i++;
            }
        } else if ((arg == "-p" || arg == "--prefix") && i + 1 < argc) {
            config.prefix = argv[++i];
        } else {
            std::cerr << COLOR_YELLOW << "Unknown option: " << arg << COLOR_RESET << std::endl;
            std::cerr << "Use 'forg --help' for usage information." << std::endl;
        }
    }
    
    return config;
}

/**
 * Collect files recursively up to specified depth
 */
void collectFiles(const fs::path& dir, int currentDepth, int maxDepth,
                  std::vector<std::pair<fs::path, std::string>>& filesToMove,
                  const std::string& programName, const Config& config) {
    
    if (currentDepth > maxDepth) {
        return;
    }
    
    try {
        for (const auto& entry : fs::directory_iterator(dir)) {
            if (entry.is_directory()) {
                // Skip directories that match skip patterns
                if (shouldSkip(entry.path())) {
                    if (config.verbose) {
                        printMessage("Skipping: " + entry.path().string(), COLOR_YELLOW);
                    }
                    continue;
                }
                
                // Recursively process subdirectories
                if (config.recursive && currentDepth < maxDepth) {
                    collectFiles(entry.path(), currentDepth + 1, maxDepth, 
                                filesToMove, programName, config);
                } else if (config.verbose) {
                    printMessage("Skipping directory: " + entry.path().filename().string(), COLOR_YELLOW);
                }
            } else {
                // Process file
                std::string filename = entry.path().filename().string();
                
                // Skip the program itself
                if (filename == programName || filename == "forg") {
                    if (config.verbose) {
                        printMessage("Skipping program file: " + filename, COLOR_YELLOW);
                    }
                    continue;
                }
                
                // Skip files matching skip patterns
                if (shouldSkip(entry.path())) {
                    if (config.verbose) {
                        printMessage("Skipping: " + filename, COLOR_YELLOW);
                    }
                    continue;
                }
                
                std::string extension = getFileExtension(filename);
                std::string category = getCategory(extension);
                
                filesToMove.push_back({entry.path(), category});
            }
        }
    } catch (const fs::filesystem_error& e) {
        printMessage("Error accessing directory " + dir.string() + ": " + e.what(), COLOR_RED);
    }
}

/**
 * Main organization logic
 */
int organizeFiles(const Config& config, const std::string& programName) {
    fs::path currentDir = fs::current_path();
    
    // Statistics
    std::map<std::string, int> movedCount;
    int skippedCount = 0;
    int errorCount = 0;
    
    printMessage("Organizing files in: " + currentDir.string(), COLOR_BLUE);
    
    if (config.recursive) {
        printMessage("Recursive mode enabled (max depth: " + std::to_string(config.depth) + ")", COLOR_BLUE);
    }
    
    if (!config.prefix.empty()) {
        printMessage("Using prefix: " + config.prefix, COLOR_BLUE);
    }
    
    if (config.dryRun) {
        printMessage("[DRY RUN MODE - No changes will be made]", COLOR_YELLOW);
    }
    
    std::cout << std::endl;
    
    // Get list of files to process
    std::vector<std::pair<fs::path, std::string>> filesToMove;
    
    if (config.recursive) {
        // Collect files recursively
        collectFiles(currentDir, 1, config.depth, filesToMove, programName, config);
    } else {
        // Collect only top-level files (original behavior)
        for (const auto& entry : fs::directory_iterator(currentDir)) {
            if (entry.is_directory()) {
                if (config.verbose) {
                    printMessage("Skipping directory: " + entry.path().filename().string(), COLOR_YELLOW);
                }
                continue;
            }
            
            std::string filename = entry.path().filename().string();
            
            // Skip the program itself
            if (filename == programName || filename == "forg") {
                if (config.verbose) {
                    printMessage("Skipping program file: " + filename, COLOR_YELLOW);
                }
                continue;
            }
            
            // Skip files matching skip patterns
            if (shouldSkip(entry.path())) {
                if (config.verbose) {
                    printMessage("Skipping: " + filename, COLOR_YELLOW);
                }
                skippedCount++;
                continue;
            }
            
            std::string extension = getFileExtension(filename);
            std::string category = getCategory(extension);
            
            filesToMove.push_back({entry.path(), category});
        }
    }
    
    if (filesToMove.empty()) {
        printMessage("No files to organize.", COLOR_YELLOW);
        return 0;
    }
    
    // Process files
    for (const auto& [filePath, category] : filesToMove) {
        std::string filename = filePath.filename().string();
        std::string extension = getFileExtension(filename);
        
        // Build category folder name with optional prefix
        std::string folderName = config.prefix.empty() ? category : config.prefix + category;
        fs::path targetDir = currentDir / folderName;
        
        // Create directory if it doesn't exist
        if (!config.dryRun) {
            try {
                fs::create_directories(targetDir);
            } catch (const fs::filesystem_error& e) {
                printMessage("Error creating directory " + folderName + ": " + e.what(), COLOR_RED);
                errorCount++;
                continue;
            }
        }
        
        // Get unique target path
        fs::path targetPath = getUniquePath(targetDir, filename);
        
        // Move the file
        if (config.verbose) {
            std::cout << COLOR_GREEN << "  Moving: " << COLOR_RESET << filename << std::endl;
            std::cout << COLOR_GREEN << "    From: " << COLOR_RESET << filePath.string() << std::endl;
            std::cout << COLOR_GREEN << "    To:   " << COLOR_RESET << targetPath.string() << std::endl;
        }
        
        if (!config.dryRun) {
            try {
                fs::rename(filePath, targetPath);
                printMessage("  ✓ " + filename + " -> " + folderName, COLOR_GREEN);
            } catch (const fs::filesystem_error& e) {
                printMessage("  ✗ Error moving " + filename + ": " + e.what(), COLOR_RED);
                errorCount++;
                continue;
            }
        } else {
            printMessage("  → " + filename + " -> " + folderName, COLOR_BLUE);
        }
        
        movedCount[category]++;
    }
    
    // Print summary
    std::cout << std::endl;
    printMessage(COLOR_BOLD + "Organization Complete!" + COLOR_RESET, COLOR_GREEN);
    std::cout << std::endl;
    
    std::cout << std::left << std::setw(20) << "Category" << "Files" << std::endl;
    std::cout << std::string(30, '-') << std::endl;
    
    for (const auto& [category, count] : movedCount) {
        std::cout << std::left << std::setw(20) << category << count << std::endl;
    }
    
    std::cout << std::string(30, '-') << std::endl;
    std::cout << std::left << std::setw(20) << "Total" << filesToMove.size() << std::endl;
    
    if (skippedCount > 0) {
        std::cout << std::endl;
        printMessage("Skipped: " + std::to_string(skippedCount) + " files/directories", COLOR_YELLOW);
    }
    
    if (errorCount > 0) {
        std::cout << std::endl;
        printMessage("Errors: " + std::to_string(errorCount), COLOR_RED);
        return 1;
    }
    
    return 0;
}

int main(int argc, char* argv[]) {
    // Store program name for later use
    std::string programName = fs::path(argv[0]).filename().string();
    
    Config config = parseArguments(argc, argv);
    
    if (config.showHelp) {
        printHelp();
        return 0;
    }
    
    if (config.showVersion) {
        std::cout << PROGRAM_NAME << " v" << VERSION << std::endl;
        std::cout << "Built with C++17 and standard library only." << std::endl;
        return 0;
    }
    
    return organizeFiles(config, programName);
}
