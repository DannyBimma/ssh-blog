#!/bin/bash
#
# deploy.sh - Deploy blog viewer to remote server
#
# Usage: ./scripts/deploy.sh [user@host]

set -e

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${GREEN}SSH Blog - Deployment Script${NC}"
echo "================================"
echo ""

# Get server details
if [ -z "$1" ]; then
    echo -n "Enter server (user@host): "
    read -r SERVER
else
    SERVER="$1"
fi

if [ -z "$SERVER" ]; then
    echo -e "${RED}Error: Server not specified${NC}"
    echo "Usage: $0 [user@host]"
    exit 1
fi

echo -e "${YELLOW}Deploying to: $SERVER${NC}"
echo ""

# Check if binary exists
if [ ! -f "bin/blog_viewer" ]; then
    echo -e "${YELLOW}Binary not found. Building...${NC}"
    make clean
    make
fi

echo -e "${GREEN}Step 1:${NC} Uploading blog viewer binary..."
scp bin/blog_viewer "${SERVER}:/tmp/blog_viewer" || {
    echo -e "${RED}Failed to upload binary${NC}"
    exit 1
}

echo -e "${GREEN}Step 2:${NC} Uploading blog entries..."
ssh "${SERVER}" "mkdir -p /var/blog/entries" || {
    echo -e "${RED}Failed to create blog directory${NC}"
    exit 1
}

scp blog_entries/*.txt "${SERVER}:/var/blog/entries/" 2>/dev/null || {
    echo -e "${YELLOW}Warning: No blog entries to upload${NC}"
}

echo -e "${GREEN}Step 3:${NC} Installing blog viewer..."
ssh "${SERVER}" << 'EOF'
    sudo mv /tmp/blog_viewer /usr/local/bin/blog_viewer
    sudo chmod 755 /usr/local/bin/blog_viewer
    sudo chown root:root /usr/local/bin/blog_viewer
EOF

if [ $? -ne 0 ]; then
    echo -e "${RED}Failed to install blog viewer${NC}"
    exit 1
fi

echo ""
echo -e "${GREEN}✓ Deployment successful!${NC}"
echo ""
echo "Next steps:"
echo ""
echo "1. Configure SSH for the blog user:"
echo "   ssh $SERVER"
echo "   sudo useradd -m -s /usr/local/bin/blog_viewer blogreader"
echo ""
echo "2. Edit /etc/ssh/sshd_config and add:"
echo "   Match User blogreader"
echo "       ForceCommand /usr/local/bin/blog_viewer"
echo "       PermitTTY yes"
echo "       X11Forwarding no"
echo "       AllowAgentForwarding no"
echo "       AllowTcpForwarding no"
echo ""
echo "3. Restart SSH:"
echo "   sudo systemctl restart sshd"
echo ""
echo "4. Test the blog:"
echo "   ssh blogreader@your-server-ip"
echo ""
