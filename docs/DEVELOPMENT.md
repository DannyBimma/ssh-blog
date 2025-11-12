# Development Guide

This document provides instructions for developing and testing the SSH Blog viewer locally.

## Prerequisites

### Required Tools
- GCC (or compatible C compiler with C99 support)
- Make
- ncurses development libraries
- Git

### Installing Dependencies

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt install build-essential libncurses5-dev libncursesw5-dev git
```

#### macOS
```bash
brew install ncurses
```

#### Fedora/RHEL
```bash
sudo dnf install gcc make ncurses-devel git
```

## Building the Project

### Clone the Repository
```bash
git clone https://github.com/DannyBimma/ssh-blog.git
cd ssh-blog
```

### Build
```bash
make
```

This will:
- Compile all source files in `src/`
- Create object files in `obj/`
- Build the executable `bin/blog_viewer`

### Build Targets

- `make` or `make all` - Build the blog viewer
- `make clean` - Remove build artifacts
- `make debug` - Build with debug symbols (no optimization)
- `make run` - Build and run the program
- `make install` - Install to `/usr/local/bin` (requires sudo)
- `make uninstall` - Remove from `/usr/local/bin`
- `make help` - Show available targets

## Running Locally

### Basic Usage
```bash
./bin/blog_viewer
```

This will load blog entries from the default `./blog_entries` directory.

### Custom Blog Directory
```bash
./bin/blog_viewer /path/to/blog/entries
```

### Quick Test
```bash
make run
```

## Project Structure

```
ssh-blog/
├── src/               # Source code
│   ├── blog.h         # Core data structures
│   ├── blog_loader.h  # Blog loading functions
│   ├── blog_loader.c
│   ├── ui.h           # Terminal UI
│   ├── ui.c
│   ├── navigation.h   # Input handling
│   ├── navigation.c
│   └── main.c         # Entry point
├── blog_entries/      # Sample blog posts
├── bin/               # Compiled binary (gitignored)
├── obj/               # Object files (gitignored)
├── tests/             # Unit tests (future)
├── scripts/           # Helper scripts
├── docs/              # Documentation
├── Makefile           # Build configuration
└── README.md
```

## Utility Scripts

The `scripts/` directory contains helpful utilities for blog management:

### Add New Blog Entry
```bash
./scripts/add_blog_entry.sh
```
Interactive script that:
- Prompts for blog title and date
- Auto-generates URL-friendly slug
- Creates properly formatted blog entry file
- Opens in your preferred editor ($EDITOR)
- Validates the entry format

### List All Entries
```bash
./scripts/list_entries.sh
```
Displays all blog entries with:
- Title and date
- Filename
- Line count
- Sorted by date (newest first)

### Deploy to Server
```bash
./scripts/deploy.sh user@your-server.com
```
Automated deployment script that:
- Builds the blog viewer
- Uploads binary and blog entries to server
- Installs to /usr/local/bin
- Provides SSH configuration instructions

## Creating Blog Entries

Blog entries are plain text files with a simple format:

### File Naming
```
YYYYMMDD_slug-name.txt
```

Examples:
- `20250112_my-first-post.txt`
- `20250115_building-in-c.txt`

### File Format
```
TITLE: Your Blog Post Title
DATE: YYYY-MM-DD
---
Your blog content goes here.

You can have multiple paragraphs.

Code can be shown with indentation:

    int main() {
        printf("Hello, World!\n");
        return 0;
    }

And so on...
```

### Example Entry
```
TITLE: Getting Started with C Programming
DATE: 2025-01-12
---
C is a powerful systems programming language.

Here's a simple program:

    #include <stdio.h>

    int main() {
        printf("Hello, World!\n");
        return 0;
    }

Compile it with: gcc hello.c -o hello
```

## Controls

Once the blog viewer is running:

### Entry Navigation
- **K** - Navigate to next blog entry
- **L** - Navigate to previous blog entry
- **:b** or **b** - Show list of all blog entries
- **:number** - Jump to entry number (e.g., `:3` for third entry)

### Scrolling & Pagination
- **j** or **↓** - Scroll down one line
- **k** or **↑** - Scroll up one line
- **Ctrl+F** or **Page Down** - Scroll down one page
- **Ctrl+B** or **Page Up** - Scroll up one page
- **g** or **Home** - Jump to top of current entry
- **G** or **End** - Jump to bottom of current entry

### Search
- **:/query** - Search for "query" in titles and content
- **n** - Navigate to next search result
- **N** - Navigate to previous search result

### Other
- **:q** or **q** - Quit

### Visual Features
- **Syntax highlighting** - Code blocks (indented with 4+ spaces) are highlighted in green
- **Search context** - Status line shows active search query and match count
- **Terminal resize** - Automatically adjusts to terminal size changes

## Debugging

### Build with Debug Symbols
```bash
make debug
```

### Run with GDB
```bash
gdb ./bin/blog_viewer
(gdb) run
```

### Check for Memory Leaks
```bash
valgrind --leak-check=full ./bin/blog_viewer
```

### Enable Debug Output
Edit source files and add debug prints:
```c
#ifdef DEBUG
fprintf(stderr, "Debug: loaded %d entries\n", count);
#endif
```

Then build with:
```bash
make debug
```

## Testing

### Manual Testing Checklist

- [ ] Program loads successfully
- [ ] Blog entries are displayed
- [ ] Navigation works (K/L keys)
- [ ] Scrolling works (j/k keys)
- [ ] Command mode works (:q, :b, :/)
- [ ] Search finds entries
- [ ] Blog list displays and is navigable
- [ ] Terminal resize is handled
- [ ] Program exits cleanly

### Test with Different Terminals

Test the blog viewer in various terminal emulators:
- xterm
- gnome-terminal
- iTerm2 (macOS)
- Windows Terminal (WSL)
- tmux/screen sessions

### Test Edge Cases

1. **Empty blog directory**
   ```bash
   mkdir /tmp/empty_blog
   ./bin/blog_viewer /tmp/empty_blog
   ```

2. **Many blog entries**
   ```bash
   # Create 50+ test entries
   for i in {1..50}; do
     echo -e "TITLE: Test Entry $i\nDATE: 2025-01-12\n---\nContent $i" > blog_entries/test_$i.txt
   done
   ./bin/blog_viewer
   ```

3. **Long blog content**
   Create an entry with 500+ lines

4. **Wide terminal**
   Resize terminal to 200+ columns

5. **Narrow terminal**
   Resize terminal to 40 columns

## Common Issues

### ncurses not found
```
Error: ncurses.h: No such file or directory
```
**Solution:** Install ncurses development libraries (see Prerequisites)

### Build fails with linker error
```
undefined reference to `initscr'
```
**Solution:** Make sure `-lncurses` is in LDFLAGS

### Terminal garbled after crash
If the program crashes and leaves your terminal in a bad state:
```bash
reset
```

### No blog entries displayed
- Check that `.txt` files exist in `blog_entries/`
- Verify files have correct format (TITLE:, DATE:, ---)
- Check file permissions are readable

## Code Style

We follow these C coding conventions:

- **Standard:** C99 (ISO/IEC 9899:1999)
- **Indentation:** 4 spaces (no tabs)
- **Line length:** 80-100 characters
- **Comments:** Use `/* */` for block comments, `//` for inline (if C99)
- **Naming:**
  - Functions: `snake_case`
  - Structs: `PascalCase`
  - Constants: `UPPER_SNAKE_CASE`
  - Macros: `UPPER_SNAKE_CASE`

## Contributing

When making changes:

1. Create a feature branch
2. Make your changes
3. Test thoroughly
4. Commit with clear messages
5. Push and create pull request

### Commit Message Format
```
Short summary (50 chars or less)

Detailed explanation if needed:
- What changed
- Why it changed
- Any side effects or considerations
```

## Performance Considerations

- Blog entries are loaded into memory at startup
- Limit: 1000 entries (configurable in `blog.h`)
- Max entry size: 64KB (configurable in `blog.h`)
- Terminal rendering uses ncurses buffering for efficiency

## Future Enhancements

See [BUILD_PLAN.md](../BUILD_PLAN.md) for the full roadmap. Short-term priorities:

- [ ] Syntax highlighting for code blocks
- [ ] Better search with regex support
- [ ] Pagination for very long entries
- [ ] Configuration file support
- [ ] Unit tests

## Getting Help

- Check [BUILD_PLAN.md](../BUILD_PLAN.md) for architecture details
- Review source code comments
- Open an issue on GitHub
- Contact: Danny Bimma

---

Happy coding! 🚀
