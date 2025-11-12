# SSH Blog - Comprehensive Build Plan

This document outlines the complete development and deployment strategy for the SSH-accessible blog project.

---

## Table of Contents

1. [Project Overview](#project-overview)
2. [Architecture](#architecture)
3. [Development Phases](#development-phases)
4. [Detailed Implementation Steps](#detailed-implementation-steps)
5. [Security Considerations](#security-considerations)
6. [Testing Strategy](#testing-strategy)
7. [Deployment Checklist](#deployment-checklist)
8. [Maintenance & Updates](#maintenance--updates)

---

## Project Overview

### Goal
Create an SSH-accessible blog where users connect via `ssh blog.dannybimma.com` and are immediately presented with an interactive, vim-like blog viewer written in C.

### Success Criteria
- Users can SSH into the blog without getting a shell prompt
- Latest blog entry displays immediately upon connection
- Vim-style navigation works flawlessly (L, K, :q, :b, :/)
- System is secure and isolated from the rest of the server
- Blog entries are easy to add/update
- Performance is snappy even on slow connections

---

## Architecture

### System Components

```
┌─────────────────────────────────────────────┐
│         User's Terminal                     │
│         ssh blog.dannybimma.com             │
└─────────────────┬───────────────────────────┘
                  │
                  │ SSH Connection
                  │
┌─────────────────▼───────────────────────────┐
│         VPS Server (Ubuntu/Debian)          │
│  ┌─────────────────────────────────────┐   │
│  │  SSH Daemon (sshd)                  │   │
│  │  - ForceCommand blog_viewer         │   │
│  │  - Dedicated user: blogreader       │   │
│  └─────────────┬───────────────────────┘   │
│                │                             │
│  ┌─────────────▼───────────────────────┐   │
│  │  Blog Viewer (C Program)            │   │
│  │  - Terminal manipulation (ncurses)  │   │
│  │  - Input handling                   │   │
│  │  - Navigation logic                 │   │
│  │  - Search functionality             │   │
│  └─────────────┬───────────────────────┘   │
│                │                             │
│  ┌─────────────▼───────────────────────┐   │
│  │  Blog Data Storage                  │   │
│  │  - /var/blog/entries/               │   │
│  │  - Markdown or plain text files     │   │
│  │  - Metadata (dates, titles)         │   │
│  └─────────────────────────────────────┘   │
└─────────────────────────────────────────────┘
```

### Technology Stack

- **Language**: C (C89/C99 standard)
- **Terminal UI**: ncurses library
- **Server OS**: Ubuntu 22.04 LTS or Debian 12
- **SSH Server**: OpenSSH
- **VPS Provider**: AWS EC2 / DigitalOcean / Linode
- **DNS**: Namecheap (for blog.dannybimma.com subdomain)
- **Version Control**: Git/GitHub

### File Structure

```
ssh-blog/
├── src/
│   ├── main.c              # Entry point
│   ├── viewer.c            # Core blog viewer logic
│   ├── viewer.h
│   ├── navigation.c        # Navigation handling (L, K)
│   ├── navigation.h
│   ├── search.c            # Search functionality (:/)
│   ├── search.h
│   ├── ui.c                # Terminal UI rendering
│   ├── ui.h
│   ├── blog_loader.c       # Load blog entries from disk
│   ├── blog_loader.h
│   └── utils.c             # Utility functions
│       utils.h
├── blog_entries/
│   ├── metadata.txt        # Index of all blog entries
│   ├── 001_first_post.txt
│   ├── 002_second_post.txt
│   └── ...
├── tests/
│   ├── test_navigation.c
│   ├── test_loader.c
│   └── test_search.c
├── scripts/
│   ├── deploy.sh           # Deployment script
│   ├── add_blog_entry.sh   # Helper to add new entries
│   └── setup_server.sh     # Server setup automation
├── docs/
│   ├── BUILD_PLAN.md       # This file
│   ├── DEPLOYMENT.md       # Deployment guide
│   └── WRITING_GUIDE.md    # How to write blog entries
├── Makefile                # Build configuration
├── README.md
└── LICENSE
```

---

## Development Phases

### Phase 1: Local Development & Prototyping (Week 1-2)
**Goal**: Build a working blog viewer that runs locally

- [ ] Set up development environment
- [ ] Create basic C project structure
- [ ] Implement simple blog entry loader
- [ ] Build basic terminal UI with ncurses
- [ ] Implement navigation (L/K keys)
- [ ] Implement command mode (:q to quit)
- [ ] Test with sample blog entries

### Phase 2: Advanced Features (Week 2-3)
**Goal**: Add search and blog list functionality

- [ ] Implement blog list view (:b command)
- [ ] Implement search functionality (:/ command)
- [ ] Add proper text wrapping for different terminal sizes
- [ ] Handle terminal resize events
- [ ] Add syntax highlighting for code blocks (optional)
- [ ] Improve error handling

### Phase 3: Server Infrastructure (Week 3-4)
**Goal**: Set up VPS and configure SSH

- [ ] Provision VPS (DigitalOcean/AWS)
- [ ] Harden server security
- [ ] Create dedicated `blogreader` user
- [ ] Configure SSH to run blog viewer
- [ ] Set up DNS subdomain (blog.dannybimma.com)
- [ ] Test SSH connection and ForceCommand

### Phase 4: Deployment & Integration (Week 4-5)
**Goal**: Deploy blog viewer to production

- [ ] Compile blog viewer on server
- [ ] Set up blog entry directory structure
- [ ] Configure permissions and file access
- [ ] Write deployment scripts
- [ ] Create blog entry management workflow
- [ ] Write initial blog entries
- [ ] End-to-end testing

### Phase 5: Polish & Launch (Week 5-6)
**Goal**: Final refinements and public launch

- [ ] Performance optimization
- [ ] Add welcome message/ASCII art
- [ ] Write documentation
- [ ] Security audit
- [ ] Beta testing with friends
- [ ] Public announcement/launch

---

## Detailed Implementation Steps

### 1. Development Environment Setup

#### 1.1 Install Required Tools
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential libncurses5-dev libncursesw5-dev git

# macOS
brew install ncurses
```

#### 1.2 Initialize Project Structure
```bash
mkdir -p ssh-blog/{src,tests,scripts,docs,blog_entries}
cd ssh-blog
git init
```

#### 1.3 Create Makefile
```makefile
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -O2
LDFLAGS = -lncurses

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TARGET = $(BIN_DIR)/blog_viewer

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

install:
	install -m 755 $(TARGET) /usr/local/bin/blog_viewer

.PHONY: all clean install
```

---

### 2. Core Blog Viewer Implementation

#### 2.1 Data Structures

**Blog Entry Structure** (`src/viewer.h`):
```c
#define MAX_TITLE_LEN 256
#define MAX_CONTENT_LEN 65536
#define MAX_ENTRIES 1000

typedef struct {
    int id;
    char title[MAX_TITLE_LEN];
    char date[11];  /* YYYY-MM-DD */
    char* content;
    size_t content_length;
} BlogEntry;

typedef struct {
    BlogEntry entries[MAX_ENTRIES];
    int count;
    int current_index;
} BlogDatabase;
```

#### 2.2 Module Breakdown

**main.c** - Entry point
- Initialize ncurses
- Load blog database
- Enter main event loop
- Clean up and exit

**blog_loader.c** - Blog entry management
- `load_all_entries()`: Read all blog files from disk
- `parse_entry()`: Parse individual blog file
- `get_latest_entry()`: Return most recent entry
- `get_entry_by_id()`: Retrieve specific entry

**ui.c** - Terminal UI rendering
- `draw_blog_entry()`: Render blog entry to screen
- `draw_status_line()`: Show status bar at bottom
- `draw_blog_list()`: Display list of all entries (:b command)
- `handle_resize()`: Respond to terminal size changes
- `word_wrap()`: Wrap text to fit terminal width

**navigation.c** - Navigation logic
- `navigate_next()`: Move to next entry (K)
- `navigate_previous()`: Move to previous entry (L)
- `navigate_to_entry()`: Jump to specific entry
- `handle_key_input()`: Process keyboard input

**search.c** - Search functionality
- `search_entries()`: Search through all entries
- `highlight_search_results()`: Highlight matches
- `next_search_result()`: Navigate through results

**utils.c** - Utility functions
- `strip_whitespace()`: String manipulation
- `file_exists()`: File system checks
- `safe_malloc()`: Memory allocation with error checking

---

#### 2.3 Main Event Loop

```c
/* Pseudocode for main.c */

int main(int argc, char* argv[]) {
    /* Initialize ncurses */
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    /* Load blog database */
    BlogDatabase db;
    load_all_entries(&db, "/var/blog/entries");

    /* Start with latest entry */
    db.current_index = 0;

    /* Main loop */
    int running = 1;
    while (running) {
        /* Draw current entry */
        draw_blog_entry(&db.entries[db.current_index]);

        /* Get user input */
        int ch = getch();

        /* Handle input */
        switch (ch) {
            case 'K':
                navigate_next(&db);
                break;
            case 'L':
                navigate_previous(&db);
                break;
            case ':':
                running = handle_command_mode(&db);
                break;
            /* ... other cases */
        }
    }

    /* Cleanup */
    endwin();
    return 0;
}
```

---

#### 2.4 Command Mode Implementation

```c
/* Handle : commands */
int handle_command_mode(BlogDatabase* db) {
    char command[256];

    /* Show command prompt at bottom */
    move(LINES - 1, 0);
    clrtoeol();
    printw(":");
    echo();
    getnstr(command, sizeof(command) - 1);
    noecho();

    /* Parse command */
    if (strcmp(command, "q") == 0) {
        return 0;  /* Exit */
    }
    else if (strcmp(command, "b") == 0) {
        show_blog_list(db);
    }
    else if (command[0] == '/') {
        search_entries(db, command + 1);
    }

    return 1;  /* Continue running */
}
```

---

### 3. Server Setup & Configuration

#### 3.1 VPS Provisioning

**Recommended Specs**:
- **Provider**: DigitalOcean Droplet / AWS EC2 t2.micro / Linode
- **OS**: Ubuntu 22.04 LTS
- **RAM**: 1GB minimum
- **Storage**: 25GB SSD
- **Cost**: ~$5-6/month

**Initial Setup**:
```bash
# After SSH-ing into fresh VPS as root

# Update system
apt update && apt upgrade -y

# Install essentials
apt install -y build-essential libncurses5-dev git ufw fail2ban

# Create dedicated blog user
useradd -m -s /usr/local/bin/blog_viewer blogreader

# Set a password (or disable password login)
passwd blogreader
```

---

#### 3.2 SSH Configuration

**Edit `/etc/ssh/sshd_config`**:
```bash
# Add at the end of file
Match User blogreader
    ForceCommand /usr/local/bin/blog_viewer
    PermitTTY yes
    X11Forwarding no
    AllowAgentForwarding no
    AllowTcpForwarding no
```

**Restart SSH**:
```bash
systemctl restart sshd
```

**What this does**:
- When user `blogreader` connects via SSH, they automatically run `blog_viewer`
- No shell access is granted
- Forwarding is disabled for security
- TTY is permitted for terminal interaction

---

#### 3.3 DNS Configuration

**At Namecheap**:
1. Log into Namecheap account
2. Go to Domain List → dannybimma.com → Manage
3. Navigate to Advanced DNS
4. Add new A Record:
   - **Host**: `blog`
   - **Value**: `<VPS_IP_ADDRESS>`
   - **TTL**: Automatic

**Verify DNS propagation**:
```bash
dig blog.dannybimma.com
nslookup blog.dannybimma.com
```

Wait up to 24-48 hours for full propagation (usually much faster).

---

#### 3.4 Directory Structure on Server

```bash
# Create blog directory
sudo mkdir -p /var/blog/entries
sudo chown -R blogreader:blogreader /var/blog

# Create admin directory for adding entries
sudo mkdir -p /home/admin/blog_admin
```

**Permissions**:
- `/var/blog/entries/`: Readable by `blogreader`, writable by admin only
- `/usr/local/bin/blog_viewer`: Executable by all, writable by root only

```bash
sudo chmod 755 /usr/local/bin/blog_viewer
sudo chmod 755 /var/blog
sudo chmod 644 /var/blog/entries/*
```

---

### 4. Blog Entry Format

#### 4.1 File Naming Convention
```
YYYYMMDD_slug.txt
```

Examples:
- `20250112_welcome-to-my-blog.txt`
- `20250115_building-ssh-blog.txt`

#### 4.2 Entry File Format

```
TITLE: My First Blog Post
DATE: 2025-01-12
---
This is the content of my blog post.

It can have multiple paragraphs.

Code blocks can be indicated with indentation:

    int main() {
        printf("Hello, World!\n");
        return 0;
    }

And so on...
```

#### 4.3 Metadata File

`/var/blog/entries/metadata.txt`:
```
20250112_welcome-to-my-blog.txt|My First Blog Post|2025-01-12
20250115_building-ssh-blog.txt|Building an SSH Blog|2025-01-15
```

Format: `filename|title|date`

---

### 5. Blog Entry Management

#### 5.1 Adding New Entries

**Script**: `scripts/add_blog_entry.sh`
```bash
#!/bin/bash

BLOG_DIR="/var/blog/entries"
DATE=$(date +%Y%m%d)

echo "Enter blog title:"
read TITLE

echo "Enter slug (URL-friendly):"
read SLUG

FILENAME="${DATE}_${SLUG}.txt"
FILEPATH="${BLOG_DIR}/${FILENAME}"

# Create entry file
cat > "$FILEPATH" << EOF
TITLE: $TITLE
DATE: $(date +%Y-%m-%d)
---
EOF

# Open in editor
${EDITOR:-nano} "$FILEPATH"

# Update metadata
echo "${FILENAME}|${TITLE}|$(date +%Y-%m-%d)" >> "${BLOG_DIR}/metadata.txt"

echo "Blog entry created: $FILENAME"
```

---

### 6. Security Considerations

#### 6.1 User Isolation

- `blogreader` user has no shell access (ForceCommand)
- No write permissions to blog directory
- chroot jail (optional, advanced)
- Resource limits via `/etc/security/limits.conf`

```
blogreader hard nproc 20
blogreader hard nofile 50
blogreader hard cpu 5
```

#### 6.2 SSH Hardening

**In `/etc/ssh/sshd_config`**:
```
# Disable root login
PermitRootLogin no

# Use SSH keys only (optional)
PasswordAuthentication no
PubkeyAuthentication yes

# Limit users who can SSH
AllowUsers youradminuser blogreader

# Change default port (optional)
Port 2222
```

#### 6.3 Firewall Configuration

```bash
# Enable UFW
ufw default deny incoming
ufw default allow outgoing

# Allow SSH
ufw allow 22/tcp  # or your custom port

# Enable firewall
ufw enable
```

#### 6.4 Fail2Ban

Protects against brute force attacks:
```bash
# Install
apt install fail2ban

# Configure
cp /etc/fail2ban/jail.conf /etc/fail2ban/jail.local

# Edit jail.local
[sshd]
enabled = true
maxretry = 3
bantime = 3600

# Restart
systemctl restart fail2ban
```

#### 6.5 Code Security

- **Input validation**: Sanitize all user input
- **Buffer overflow protection**: Use `strncpy`, `snprintf`, bounds checking
- **Compile flags**: `-D_FORTIFY_SOURCE=2 -fstack-protector-strong`
- **No arbitrary file access**: Restrict file loading to `/var/blog/entries`
- **Command injection prevention**: Never use `system()` with user input

---

### 7. Testing Strategy

#### 7.1 Unit Tests

Create test files in `tests/`:

```c
/* tests/test_navigation.c */
#include "../src/navigation.h"
#include <assert.h>

void test_navigate_next() {
    BlogDatabase db = {.count = 5, .current_index = 0};
    navigate_next(&db);
    assert(db.current_index == 1);
}

void test_navigate_wrap() {
    BlogDatabase db = {.count = 5, .current_index = 4};
    navigate_next(&db);
    assert(db.current_index == 0);  /* Should wrap */
}

int main() {
    test_navigate_next();
    test_navigate_wrap();
    printf("All navigation tests passed!\n");
    return 0;
}
```

#### 7.2 Integration Tests

**Test SSH connection**:
```bash
ssh blogreader@blog.dannybimma.com
# Should immediately show blog viewer
```

**Test commands**:
- Press `K` multiple times → navigate through entries
- Press `L` → go back
- Type `:q` → disconnect
- Type `:b` → see blog list
- Type `:/search_term` → search works

#### 7.3 Performance Tests

- Test with 100+ blog entries
- Test on slow network connection
- Test terminal resize handling
- Memory leak testing with Valgrind

```bash
valgrind --leak-check=full ./bin/blog_viewer
```

---

### 8. Deployment Process

#### 8.1 Compilation on Server

```bash
# SSH into VPS as admin user
ssh admin@your-vps-ip

# Clone repository
git clone https://github.com/yourusername/ssh-blog.git
cd ssh-blog

# Build
make clean
make

# Install
sudo make install

# Verify
which blog_viewer
# Should output: /usr/local/bin/blog_viewer
```

#### 8.2 Deploy Blog Entries

```bash
# Copy sample entries
sudo cp blog_entries/* /var/blog/entries/
sudo chown -R blogreader:blogreader /var/blog
sudo chmod 644 /var/blog/entries/*
```

#### 8.3 Deployment Script

**scripts/deploy.sh**:
```bash
#!/bin/bash

set -e

SERVER="your-vps-ip"
USER="admin"

echo "Building blog viewer..."
make clean
make

echo "Uploading to server..."
scp bin/blog_viewer ${USER}@${SERVER}:/tmp/

echo "Installing on server..."
ssh ${USER}@${SERVER} << 'EOF'
    sudo mv /tmp/blog_viewer /usr/local/bin/
    sudo chmod 755 /usr/local/bin/blog_viewer
    sudo systemctl restart sshd
EOF

echo "Deployment complete!"
echo "Test with: ssh blogreader@blog.dannybimma.com"
```

---

### 9. Monitoring & Maintenance

#### 9.1 Log Monitoring

```bash
# SSH login attempts
tail -f /var/log/auth.log

# System logs
journalctl -u ssh -f
```

#### 9.2 Updates

```bash
# Update blog viewer
cd ssh-blog
git pull
make clean && make
sudo make install
sudo systemctl restart sshd
```

#### 9.3 Backup

```bash
# Backup blog entries
tar -czf blog_backup_$(date +%Y%m%d).tar.gz /var/blog/entries

# Automate with cron
0 2 * * * tar -czf /backups/blog_$(date +\%Y\%m\%d).tar.gz /var/blog/entries
```

---

### 10. Future Enhancements

#### Phase 2 Features (Optional)
- [ ] Syntax highlighting for code blocks
- [ ] Image rendering (ASCII art conversion)
- [ ] Tags/categories for blog entries
- [ ] Full-text search with regex
- [ ] RSS feed generation
- [ ] Analytics (track connections, popular posts)
- [ ] Multiple color schemes
- [ ] Pagination for long entries
- [ ] Table of contents for long posts
- [ ] Export to PDF functionality
- [ ] Comments system (via email integration?)

#### Advanced Features
- [ ] Multi-user support (different blogs per user)
- [ ] Internationalization (i18n)
- [ ] Plugin system
- [ ] Live reload during development
- [ ] Web gateway (view blog in browser too)

---

## Deployment Checklist

### Pre-Deployment
- [ ] All features implemented and tested locally
- [ ] Code reviewed for security issues
- [ ] Unit tests pass
- [ ] Memory leaks checked with Valgrind
- [ ] Documentation completed

### VPS Setup
- [ ] VPS provisioned and accessible
- [ ] OS updated and hardened
- [ ] Firewall configured (UFW)
- [ ] Fail2Ban installed and configured
- [ ] `blogreader` user created

### DNS & Networking
- [ ] DNS A record created for blog.dannybimma.com
- [ ] DNS propagated and verified
- [ ] SSH accessible via subdomain

### Application Deployment
- [ ] Blog viewer compiled and installed
- [ ] SSH ForceCommand configured
- [ ] Blog directory created with proper permissions
- [ ] Initial blog entries uploaded
- [ ] Integration tests passed

### Post-Deployment
- [ ] End-to-end testing from external network
- [ ] Performance verified
- [ ] Monitoring set up
- [ ] Backup automation configured
- [ ] Documentation updated with production details

---

## Timeline Estimate

| Phase | Duration | Tasks |
|-------|----------|-------|
| Phase 1: Local Development | 1-2 weeks | Core viewer, navigation, basic UI |
| Phase 2: Advanced Features | 1 week | Search, blog list, polish |
| Phase 3: Server Setup | 3-5 days | VPS, DNS, SSH config |
| Phase 4: Deployment | 2-3 days | Deploy, test, debug |
| Phase 5: Launch | 1 week | Polish, beta test, announce |
| **Total** | **4-6 weeks** | |

---

## Key Milestones

1. **Milestone 1**: Basic blog viewer runs locally ✓
2. **Milestone 2**: All navigation features working ✓
3. **Milestone 3**: VPS configured and accessible ✓
4. **Milestone 4**: Blog viewer running on VPS via SSH ✓
5. **Milestone 5**: DNS configured, accessible via blog.dannybimma.com ✓
6. **Milestone 6**: Public launch ✓

---

## Resources & References

### C Programming
- [ncurses Programming Guide](https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/)
- [Beej's Guide to C Programming](https://beej.us/guide/bgc/)

### SSH Configuration
- [OpenSSH Manual](https://www.openssh.com/manual.html)
- [SSH ForceCommand Documentation](https://man.openbsd.org/sshd_config#ForceCommand)

### Server Administration
- [DigitalOcean Server Setup Tutorials](https://www.digitalocean.com/community/tutorials)
- [Ubuntu Server Guide](https://ubuntu.com/server/docs)

### Security
- [OWASP Secure Coding Practices](https://owasp.org/www-project-secure-coding-practices-quick-reference-guide/)
- [Linux Security Hardening Guide](https://www.cisecurity.org/benchmark/distribution_independent_linux)

---

## Contact & Support

For questions or issues during development:
- Check documentation in `/docs`
- Review GitHub issues
- Contact: Danny Bimma

---

**Last Updated**: 2025-01-12
**Version**: 1.0
**Status**: Planning Phase
