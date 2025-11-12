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
- **`L` (capital L)**: Navigate to the previous blog entry
- **`K` (capital K)**: Navigate to the next blog entry
- **`:q`**: Quit the blog viewer (disconnect)
- **`:b`**: Display a list of all available blog entries
- **`:/`**: Enter search mode to search across all blog entries

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

This project is currently in the planning and development phase. See [BUILD_PLAN.md](BUILD_PLAN.md) for the detailed implementation roadmap.

## Quick Start (Future)

Once deployed, readers will be able to access the blog with:

```bash
ssh blog.dannybimma.com
```

No password required - the blog will be publicly accessible via SSH key authentication or simple password-less access for a dedicated read-only user.

## Author

Danny Bimma
- Website: [www.dannybimma.com](https://www.dannybimma.com)

## License

See [LICENSE](LICENSE) file for details.
