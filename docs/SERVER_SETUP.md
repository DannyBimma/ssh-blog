# SSH Blog - Server Setup Guide

Complete guide for deploying your SSH blog to a production server.

---

## Table of Contents

1. [Choosing a VPS Provider](#choosing-a-vps-provider)
2. [Server Requirements](#server-requirements)
3. [Step-by-Step Setup](#step-by-step-setup)
4. [DNS Configuration](#dns-configuration)
5. [Security Hardening](#security-hardening)
6. [Deployment](#deployment)
7. [Testing](#testing)
8. [Troubleshooting](#troubleshooting)

---

## Choosing a VPS Provider

### Recommended: DigitalOcean

**Best for this project** ⭐

**Why DigitalOcean**:
- Simple, predictable pricing ($6/month for basic droplet)
- Excellent documentation and tutorials
- Easy-to-use control panel
- Fast SSD storage
- One-click SSH access from dashboard
- Free monitoring and alerting
- Snapshots and backups available
- Great for beginners

**Pros**:
- ✅ $6/month for 1GB RAM / 1 vCPU / 25GB SSD
- ✅ Simple web interface
- ✅ Global data centers
- ✅ Excellent uptime (99.99% SLA)
- ✅ Easy SSH key management
- ✅ Built-in firewall

**Cons**:
- ⚠️ No free tier
- ⚠️ Slightly more expensive than competitors

### Alternative: AWS Lightsail

**Good alternative**

**Why Lightsail**:
- Cheaper than EC2 ($3.50/month for smallest instance)
- Simpler than full AWS ecosystem
- Similar to DigitalOcean experience
- Integrates with other AWS services

**Pros**:
- ✅ $3.50/month for 512MB RAM / 1 vCPU / 20GB SSD
- ✅ Part of AWS ecosystem
- ✅ Static IP included
- ✅ Simple interface

**Cons**:
- ⚠️ Still requires AWS account setup
- ⚠️ 512MB may be tight for future growth

### Alternative: Linode (Akamai)

**Solid choice**

**Why Linode**:
- Competitive pricing ($5/month)
- Developer-friendly
- Good documentation
- Now backed by Akamai

**Pros**:
- ✅ $5/month for 1GB RAM / 1 vCPU / 25GB SSD
- ✅ Excellent performance
- ✅ 99.9% uptime SLA
- ✅ Good support

**Cons**:
- ⚠️ Smaller company than AWS/DO

### Not Recommended for This Project

❌ **AWS EC2**: Too complex, billing can be confusing
❌ **Heroku**: Not designed for SSH services
❌ **Vercel/Netlify**: Web-only platforms
❌ **Shared Hosting**: No root access

### Final Recommendation

**Go with DigitalOcean** for the best experience:
- Clear pricing
- Great documentation
- Perfect for SSH blog
- Good for portfolio projects

**Budget Option**: Use AWS Lightsail if cost is primary concern

---

## Server Requirements

### Minimum Specifications

| Resource | Minimum | Recommended | Why |
|----------|---------|-------------|-----|
| RAM | 512MB | 1GB | Blog viewer is lightweight |
| CPU | 1 vCPU | 1 vCPU | Minimal processing needed |
| Storage | 10GB | 25GB SSD | Blog entries + OS |
| Bandwidth | 500GB | 1TB | SSH uses minimal bandwidth |
| OS | Ubuntu 20.04+ | Ubuntu 22.04 LTS | Long-term support |

### Recommended Droplet (DigitalOcean)

**Basic Droplet - $6/month**:
- 1 GB RAM
- 1 vCPU
- 25 GB SSD
- 1 TB Transfer
- Ubuntu 22.04 LTS

This is more than sufficient for an SSH blog with thousands of readers.

---

## Step-by-Step Setup

### Phase 1: Create VPS (DigitalOcean)

#### 1. Sign Up for DigitalOcean

1. Go to https://www.digitalocean.com
2. Create account
3. Add payment method
4. **Optional**: Use referral code for $200 credit (60 days)

#### 2. Create SSH Key (On Your Local Machine)

```bash
# Generate SSH key if you don't have one
ssh-keygen -t ed25519 -C "youremail@example.com"

# Display public key
cat ~/.ssh/id_ed25519.pub
```

Copy the output (starts with `ssh-ed25519...`)

#### 3. Add SSH Key to DigitalOcean

1. In DigitalOcean dashboard: Settings → Security → SSH Keys
2. Click "Add SSH Key"
3. Paste your public key
4. Name it (e.g., "My Laptop")

#### 4. Create Droplet

1. Click "Create" → "Droplets"
2. Choose:
   - **Distribution**: Ubuntu 22.04 LTS
   - **Plan**: Basic ($6/month)
   - **CPU**: Regular (1 GB / 1 vCPU)
   - **Datacenter**: Choose closest to you
   - **Authentication**: SSH Key (select the one you added)
   - **Hostname**: `blog-server` or `dannybimma-blog`
3. Click "Create Droplet"
4. Wait ~60 seconds for creation

#### 5. Note Your Server IP

You'll see the IP address (e.g., `165.227.123.45`)

**Save this IP** - you'll need it for DNS and SSH

---

### Phase 2: Initial Server Setup

#### 1. Connect to Server

```bash
ssh root@YOUR_SERVER_IP
```

#### 2. Update System

```bash
# Update package lists
apt update

# Upgrade all packages
apt upgrade -y

# Install essentials
apt install -y build-essential libncurses5-dev git ufw fail2ban
```

#### 3. Create Admin User (Optional but Recommended)

```bash
# Create user
adduser admin

# Add to sudo group
usermod -aG sudo admin

# Copy SSH keys to new user
rsync --archive --chown=admin:admin ~/.ssh /home/admin
```

#### 4. Configure Firewall

```bash
# Allow SSH
ufw allow 22/tcp

# Enable firewall
ufw enable

# Check status
ufw status
```

**Output should show**:
```
Status: active

To                         Action      From
--                         ------      ----
22/tcp                     ALLOW       Anywhere
```

---

### Phase 3: Create Blog User

This is the user people will SSH into to view your blog.

#### 1. Create User

```bash
# Create user with no password
sudo adduser --disabled-password blogreader

# Or create with password (less secure but simpler)
sudo adduser blogreader
```

#### 2. Set Up Blog Directory

```bash
# Create blog directory
sudo mkdir -p /var/blog/entries

# Set ownership
sudo chown -R blogreader:blogreader /var/blog

# Set permissions (readable by blogreader, writable by admin)
sudo chmod 755 /var/blog
sudo chmod 755 /var/blog/entries
```

---

### Phase 4: Deploy Blog Viewer

You can use the automated script or do it manually:

#### Option A: Automated Deployment (Recommended)

On your **local machine**:

```bash
# Build the project
make clean && make

# Deploy to server
./scripts/deploy.sh root@YOUR_SERVER_IP
```

The script will:
1. Upload blog_viewer binary
2. Upload blog entries
3. Install to /usr/local/bin
4. Set correct permissions

#### Option B: Manual Deployment

On your **local machine**:

```bash
# Build
make clean && make

# Upload binary
scp bin/blog_viewer root@YOUR_SERVER_IP:/tmp/

# Upload blog entries
scp blog_entries/*.txt root@YOUR_SERVER_IP:/tmp/
```

On the **server**:

```bash
# Install binary
sudo mv /tmp/blog_viewer /usr/local/bin/
sudo chmod 755 /usr/local/bin/blog_viewer

# Move blog entries
sudo mv /tmp/*.txt /var/blog/entries/
sudo chown blogreader:blogreader /var/blog/entries/*.txt
sudo chmod 644 /var/blog/entries/*.txt

# Verify
ls -la /usr/local/bin/blog_viewer
ls -la /var/blog/entries/
```

---

### Phase 5: Configure SSH

This is the magic that makes users land in your blog instead of a shell.

#### 1. Edit SSH Configuration

```bash
sudo nano /etc/ssh/sshd_config
```

#### 2. Add Blog User Configuration

**Add at the end of the file**:

```
# SSH Blog Configuration
Match User blogreader
    ForceCommand /usr/local/bin/blog_viewer /var/blog/entries
    PermitTTY yes
    X11Forwarding no
    AllowAgentForwarding no
    AllowTcpForwarding no
    PermitTunnel no
```

**What this does**:
- `ForceCommand`: Runs blog_viewer instead of shell
- `PermitTTY yes`: Allows terminal interaction
- `X11Forwarding no`: Disables X11 (security)
- `AllowAgentForwarding no`: Disables SSH agent forwarding (security)
- `AllowTcpForwarding no`: Disables port forwarding (security)
- `PermitTunnel no`: Disables tunneling (security)

#### 3. Test SSH Configuration

```bash
# Test configuration syntax
sudo sshd -t
```

**If no output**: ✅ Configuration is valid
**If errors**: ❌ Fix syntax errors before continuing

#### 4. Restart SSH

```bash
sudo systemctl restart sshd

# Check status
sudo systemctl status sshd
```

Should show `active (running)`

---

### Phase 6: DNS Configuration (Namecheap)

Now make your blog accessible via `blog.dannybimma.com`

#### 1. Log into Namecheap

1. Go to https://www.namecheap.com
2. Log in
3. Dashboard → Domain List
4. Find `dannybimma.com` → Click "Manage"

#### 2. Add DNS Record

1. Click "Advanced DNS" tab
2. Under "Host Records", click "ADD NEW RECORD"
3. Fill in:
   - **Type**: A Record
   - **Host**: `blog`
   - **Value**: `YOUR_SERVER_IP` (e.g., `165.227.123.45`)
   - **TTL**: Automatic (or 300 for faster propagation)
4. Click ✓ to save

#### 3. Wait for DNS Propagation

DNS changes can take 5 minutes to 48 hours (usually < 1 hour)

**Check propagation**:

```bash
# Check if DNS is working
dig blog.dannybimma.com

# Or
nslookup blog.dannybimma.com
```

You should see your server IP in the response.

---

## Testing

### Test 1: Direct IP Access

```bash
ssh blogreader@YOUR_SERVER_IP
```

**Expected**: Blog viewer launches immediately

### Test 2: Domain Access

After DNS propagates:

```bash
ssh blogreader@blog.dannybimma.com
```

**Expected**: Blog viewer launches immediately

### Test 3: Blog Navigation

Once connected:
- Press `K` to navigate
- Type `:b` to see blog list
- Type `:/test` to search
- Type `:q` to quit

### Test 4: Multiple Connections

Open multiple terminals and connect simultaneously to test concurrent access.

---

## Security Hardening

### 1. Fail2Ban (Brute Force Protection)

```bash
# Install
sudo apt install -y fail2ban

# Configure
sudo cp /etc/fail2ban/jail.conf /etc/fail2ban/jail.local
sudo nano /etc/fail2ban/jail.local
```

**Find and modify**:

```ini
[sshd]
enabled = true
port = ssh
logpath = /var/log/auth.log
maxretry = 3
bantime = 3600
findtime = 600
```

**Restart**:

```bash
sudo systemctl restart fail2ban
sudo systemctl enable fail2ban

# Check status
sudo fail2ban-client status sshd
```

### 2. Disable Root SSH Login

```bash
sudo nano /etc/ssh/sshd_config
```

**Find and change**:

```
PermitRootLogin no
```

**Restart SSH**:

```bash
sudo systemctl restart sshd
```

### 3. Change Default SSH Port (Optional)

Makes it harder for bots to find your SSH service.

```bash
sudo nano /etc/ssh/sshd_config
```

**Find and change**:

```
Port 2222
```

**Update firewall**:

```bash
sudo ufw allow 2222/tcp
sudo ufw delete allow 22/tcp
```

**Restart SSH**:

```bash
sudo systemctl restart sshd
```

**Now connect with**:

```bash
ssh -p 2222 blogreader@blog.dannybimma.com
```

### 4. Set Up Automatic Security Updates

```bash
sudo apt install -y unattended-upgrades
sudo dpkg-reconfigure --priority=low unattended-upgrades
```

Select "Yes" to enable automatic updates.

### 5. Regular Backups

#### Snapshot (DigitalOcean)

1. In DigitalOcean dashboard
2. Select your droplet
3. Click "Snapshots"
4. Create snapshot
5. Schedule weekly snapshots (recommended)

#### Manual Backup

```bash
# Backup blog entries
tar -czf blog_backup_$(date +%Y%m%d).tar.gz /var/blog/entries

# Download to local machine
scp root@blog.dannybimma.com:~/blog_backup_*.tar.gz ./
```

---

## Maintenance

### Adding New Blog Entries

On your **local machine**:

```bash
# Create new entry
./scripts/add_blog_entry.sh

# Commit to git
git add blog_entries/
git commit -m "Add new blog post"

# Deploy to server
scp blog_entries/NEWFILE.txt root@blog.dannybimma.com:/var/blog/entries/
```

Or redeploy everything:

```bash
./scripts/deploy.sh root@blog.dannybimma.com
```

### Updating Blog Viewer

After making changes to the C code:

```bash
# Local: Build
make clean && make

# Local: Deploy
./scripts/deploy.sh root@blog.dannybimma.com

# Server: Restart SSH (if needed)
ssh root@blog.dannybimma.com 'sudo systemctl restart sshd'
```

### Monitoring

```bash
# Check who's connected
who

# Check SSH logs
sudo tail -f /var/log/auth.log

# Check system resources
htop

# Check disk usage
df -h
```

---

## Troubleshooting

### Issue: "Connection refused"

**Cause**: SSH service not running or firewall blocking

**Fix**:

```bash
# Check SSH status
sudo systemctl status sshd

# Start if stopped
sudo systemctl start sshd

# Check firewall
sudo ufw status
```

### Issue: "Permission denied"

**Cause**: SSH key not configured or wrong username

**Fix**:

```bash
# Verify username
ssh blogreader@blog.dannybimma.com

# Check SSH key
cat ~/.ssh/id_ed25519.pub

# Or use password if configured
ssh -o PreferredAuthentications=password blogreader@blog.dannybimma.com
```

### Issue: "Blog viewer not found"

**Cause**: Binary not installed or wrong path

**Fix**:

```bash
# Check if binary exists
ssh root@blog.dannybimma.com 'ls -la /usr/local/bin/blog_viewer'

# Verify SSH config
ssh root@blog.dannybimma.com 'sudo grep ForceCommand /etc/ssh/sshd_config'
```

### Issue: "No blog entries found"

**Cause**: Blog files not uploaded

**Fix**:

```bash
# Check blog directory
ssh root@blog.dannybimma.com 'ls -la /var/blog/entries/'

# Upload entries
scp blog_entries/*.txt root@blog.dannybimma.com:/var/blog/entries/
```

### Issue: DNS not resolving

**Cause**: DNS not propagated or incorrect A record

**Fix**:

```bash
# Check DNS
dig blog.dannybimma.com

# Check Namecheap settings
# Verify A record points to correct IP
```

---

## Cost Estimate

### DigitalOcean (Recommended)

- **Droplet**: $6/month
- **Snapshots** (optional): $1.20/month (1 snapshot of 6GB droplet)
- **Total**: ~$7/month

### AWS Lightsail (Budget Option)

- **Instance**: $3.50/month
- **Total**: $3.50/month

### Domain (Already Owned)

- **dannybimma.com**: Already registered with Namecheap
- **No additional cost**

### Annual Cost

- **DigitalOcean**: ~$84/year
- **AWS Lightsail**: ~$42/year

---

## Next Steps After Deployment

1. ✅ Test from different networks
2. ✅ Share blog URL with friends for testing
3. ✅ Monitor logs for a few days
4. ✅ Set up automated backups
5. ✅ Write your first "real" blog post
6. ✅ Add to your resume/portfolio
7. ✅ Share on social media (Twitter, LinkedIn, HN)

---

## Quick Reference

### Connect to Blog

```bash
ssh blogreader@blog.dannybimma.com
```

### Connect to Server (Admin)

```bash
ssh root@blog.dannybimma.com
# or
ssh admin@blog.dannybimma.com
```

### Deploy Updates

```bash
./scripts/deploy.sh root@blog.dannybimma.com
```

### View Logs

```bash
ssh root@blog.dannybimma.com 'sudo tail -f /var/log/auth.log'
```

---

**Ready to Deploy?** Follow the steps above and your SSH blog will be live!

**Questions?** Check [TROUBLESHOOTING](#troubleshooting) or open a GitHub issue.

---

**Last Updated**: 2025-01-12
**Maintainer**: Danny Bimma
