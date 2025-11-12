# SSH Blog

An unconventional tech blog/portfolio that users can access and read directly in their terminal via SSH. Instead of a traditional web-based blog, readers connect using `ssh blog.dannybimma.com` and are immediately presented with an interactive, vim-like blog reading experience.

## Concept

This project reimagines the blog as a terminal-first experience. When users SSH into the blog, they don't get a shell prompt - instead, they're dropped directly into a custom C program that displays blog entries with vim-inspired navigation.

## Features

### Core Functionality
- **Direct Blog Access**: Users connect via `ssh blog.dannybimma.com` and immediately see the latest blog entry
- **Custom SSH Experience**: No shell access - SSH configured to run a dedicated blog viewer program
- **C Implementation**: Blog viewer written in pure C (C89/C99 standard) for performance and portability

### Interactive Navigation (Vim-style)

#### Entry Navigation
- **`K`**: Navigate to the next blog entry
- **`L`**: Navigate to the previous blog entry
- **`:b`** or **`b`**: Display a list of all available blog entries

#### Scrolling & Pagination
- **`j`** or **`↓`**: Scroll down one line
- **`k`** or **`↑`**: Scroll up one line
- **`Ctrl+F`** or **`Page Down`**: Scroll down one page
- **`Ctrl+B`** or **`Page Up`**: Scroll up one page
- **`g`** or **`Home`**: Jump to top of current entry
- **`G`** or **`End`**: Jump to bottom of current entry

#### Search
- **`:/query`**: Search for "query" across all blog entries
- **`n`**: Navigate to next search result
- **`N`**: Navigate to previous search result

#### Other
- **`:q`** or **`q`**: Quit the blog viewer (disconnect)

### Technical Stack
- **Language**: C (C89/C99 standard)
- **Hosting**: VPS (AWS EC2 / DigitalOcean Droplet)
- **Domain**: Subdomain `blog.dannybimma.com` via Namecheap DNS
- **Access**: Dedicated read-only user for blog readers

## Project Goals

1. Create a unique, memorable portfolio piece that stands out
2. Demonstrate low-level systems programming skills with C
3. Showcase understanding of SSH configuration and server administration
4. Build an accessible, terminal-friendly reading experience
5. Prove that not everything needs to be web-based

## Target Audience

- Developers and tech enthusiasts who appreciate terminal interfaces
- Anyone interested in unconventional web projects
- Potential employers looking for creative portfolio pieces

## Why SSH?

- **Ubiquity**: SSH is available on virtually every Unix-like system
- **Security**: Built-in encryption and authentication
- **Nostalgia**: Reminiscent of early internet BBS systems
- **Accessibility**: Works over low-bandwidth connections
- **Uniqueness**: Stands out in a sea of traditional blogs

## Development Status

✅ **Phase 1 Complete**: Core blog viewer implementation
✅ **Phase 2 Complete**: Advanced features (search, pagination, syntax highlighting)
🚧 **Phase 3 In Progress**: Server infrastructure and deployment

See [BUILD_PLAN.md](BUILD_PLAN.md) for the detailed implementation roadmap.

### Current Features
- ✅ Full vim-style navigation
- ✅ Syntax highlighting for code blocks
- ✅ Advanced search with multiple results
- ✅ Pagination controls (Page Up/Down, Home/End)
- ✅ Interactive blog list
- ✅ Terminal resize handling
- ✅ Helper scripts for blog management

## Quick Start (Future)

Once deployed, readers will be able to access the blog with:

```bash
ssh blog.dannybimma.com
```

No password required - the blog will be publicly accessible via SSH key authentication or simple password-less access for a dedicated read-only user.

## Quick Start (Local Development)

### Build and Run
```bash
# Clone the repository
git clone https://github.com/DannyBimma/ssh-blog.git
cd ssh-blog

# Build the blog viewer
make

# Run locally
./bin/blog_viewer
```

### Create a New Blog Entry
```bash
# Interactive blog entry creator
./scripts/add_blog_entry.sh
```

### List All Entries
```bash
# View all blog entries with metadata
./scripts/list_entries.sh
```

### Deploy to Server
```bash
# Deploy blog viewer to your VPS
./scripts/deploy.sh user@your-server.com
```

See [docs/DEVELOPMENT.md](docs/DEVELOPMENT.md) for detailed development instructions.

## Author

Danny Bimma
- Website: [www.dannybimma.com](https://www.dannybimma.com)

## License

See [LICENSE](LICENSE) file for details.
