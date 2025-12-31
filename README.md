# forg - File Organizer

A command-line tool that organizes files in the current directory into categorized subfolders based on their file extensions. Written in C++ using only standard library components.

## Features

- **Automatic File Categorization**: Files are organized into categories based on their extensions
- **Recursive Processing**: Process files in subdirectories with configurable depth
- **Prefix Support**: Add custom prefixes to category folder names
- **Collision Handling**: Automatically renames files to avoid overwrites
- **Skip Patterns**: Automatically ignores hidden files, version control directories, and system files
- **Color-Coded Output**: Visual feedback with colored terminal output
- **Dry Run Mode**: Preview what would be done without making changes
- **Verbose Logging**: Detailed progress information when needed

## Installation

### Prerequisites

- C++17 compatible compiler (g++ 7+ or clang 5+)
- Linux or Unix-like operating system
- Root privileges for system-wide installation

### Installation Steps

1. Clone or download the source files
2. Run the installation script:

```bash
cd /path/to/forg
bash install.sh
```

The installation script will:
- Check for required dependencies
- Compile the application with optimizations
- Install to `/usr/local/bin/forg`
- Set appropriate permissions

### Manual Compilation

If you prefer to compile manually:

```bash
g++ -std=c++17 -O2 -Wall -Wextra -o forg main.cpp
sudo mv forg /usr/local/bin/
sudo chmod +x /usr/local/bin/forg
```

## Uninstallation

To remove forg from your system:

```bash
cd /path/to/forg
bash uninstall.sh
```

Or manually:

```bash
sudo rm /usr/local/bin/forg
```

## Usage

### Basic Commands

```bash
# Organize top-level files only (default)
forg

# Recursive - process all files in current dir and subdirs
forg -r

# Process up to 2 levels deep
forg -r --depth 2

# Organize with prefix for folder names
forg -p backup_

# Verbose mode - show detailed progress
forg -v

# Dry run - preview without making changes
forg -n
```

### Command Options

| Option | Description |
|--------|-------------|
| `-r, --recursive` | Enable recursive directory traversal |
| `-d, --depth <number>` | Maximum depth for recursion (default: 1) |
| `-p, --prefix <text>` | Add prefix to category folder names |
| `-v, --verbose` | Show detailed progress information |
| `-n, --dry-run` | Preview without making changes |
| `-h, --help` | Show help message |
| `--version` | Show version information |

### Depth Control Examples

```bash
# Level 1: Current directory only (same as default)
forg -r --depth 1

# Level 2: Current directory + one level of subdirectories
forg -r --depth 2

# Level 3: Current directory + two levels of subdirectories
forg -r --depth 3

# Unlimited depth (processes all subdirectories)
forg -r
```

### Combined Options

```bash
# Recursive with prefix and verbose output
forg -r -p sorted_ -v

# Dry run to preview recursive organization
forg -r -n

# Process 3 levels deep with prefix
forg -r --depth 3 -p archive_
```

## File Categories

forg organizes files into the following categories:

| Category | Extensions |
|----------|------------|
| Images | jpg, jpeg, png, gif, bmp, tiff, svg, webp, ico, psd, ai, eps |
| Videos | mp4, mkv, avi, mov, wmv, flv, webm, m4v, mpeg, mpg, 3gp, rmvb |
| Audio | mp3, wav, flac, aac, ogg, m4a, wma, aiff, mid, midi |
| Documents | pdf, doc, docx, txt, rtf, odt, xls, xlsx, ppt, pptx, csv, md, markdown, log |
| Archives | zip, rar, 7z, tar, gz, bz2, xz, iso, dmg, pkg, deb, rpm |
| Code | cpp, c, h, hpp, py, js, ts, html, htm, css, scss, java, go, rs, rb, php, swift, kt, scala, sh, bash, json, xml, yaml, yml, toml, ini, cfg, conf |
| Executables | exe, app, bin, msi, run, elf, so, dll, dylib |
| Database | sql, db, sqlite, mdb, accdb, frm, ibd |
| Books | epub, mobi, azw, azw3, fb2, djvu, chm |
| Others | Files with unrecognized extensions |

## Skipped Items

The following items are automatically skipped:

- **Hidden files**: Files starting with '.' (e.g., .bashrc)
- **Version control**: .git, .svn, .hg, .bzr directories
- **IDE directories**: .vscode, .idea, .vs
- **Build directories**: build, dist, node_modules, .cache, __pycache__
- **System files**: .DS_Store, Thumbs.db, .Spotlight-V100, .Trashes
- **The forg binary itself**: Will not organize its own executable

## Collision Handling

When a file with the same name already exists in the destination folder, forg automatically renames the new file by appending a counter:

```
file.jpg           # Original
file_1.jpg         # First duplicate
file_2.jpg         # Second duplicate
```

If more than 1000 duplicates exist, a timestamp-based name is used as a fallback.

## Output Examples

### Normal Output

```
Organizing files in: /home/user/downloads
Using prefix: sorted_

  ✓ document.pdf -> Documents
  ✓ photo.jpg -> Images
  ✓ song.mp3 -> Audio

Organization Complete!
Category            Files
------------------------------
Audio               1
Documents           1
Images              1
------------------------------
Total               3
```

### Verbose Output

```
Organizing files in: /home/user/downloads
Using prefix: sorted_
  Moving: document.pdf
    From: /home/user/downloads/document.pdf
    To:   /home/user/downloads/sorted_Documents/document.pdf
  ✓ document.pdf -> sorted_Documents
```

## Man Page

A manual page is included for detailed reference:

```bash
man forg
```

To view the man page without installation, you can use:

```bash
man ./forg.1
```

## Building from Source

### Debug Build

```bash
g++ -std=c++17 -g -Wall -Wextra -o forg main.cpp
```

### Release Build (Optimized)

```bash
g++ -std=c++17 -O2 -Wall -Wextra -o forg main.cpp
```

### Static Build

```bash
g++ -std=c++17 -O2 -static -s -o forg main.cpp
```

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.

## License

This project is open source and available under the MIT License.

## Author

LotusOS-Core

## Version

Current version: 1.0.0

## Contact

For questions or issues, please visit the project repository.
