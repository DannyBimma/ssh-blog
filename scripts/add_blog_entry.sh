#!/bin/bash
#
# add_blog_entry.sh - Helper script to create new blog entries
#
# Usage: ./scripts/add_blog_entry.sh

set -e

# Default blog directory
BLOG_DIR="${BLOG_DIR:-./blog_entries}"

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${GREEN}SSH Blog - New Entry Creator${NC}"
echo "================================"
echo ""

# Get blog title
echo -n "Enter blog title: "
read -r TITLE

if [ -z "$TITLE" ]; then
    echo -e "${RED}Error: Title cannot be empty${NC}"
    exit 1
fi

# Get slug (URL-friendly name)
echo ""
echo -n "Enter slug (URL-friendly name, e.g., 'my-first-post'): "
read -r SLUG

if [ -z "$SLUG" ]; then
    # Auto-generate slug from title
    SLUG=$(echo "$TITLE" | tr '[:upper:]' '[:lower:]' | tr ' ' '-' | tr -cd '[:alnum:]-')
    echo -e "${YELLOW}Auto-generated slug: $SLUG${NC}"
fi

# Get date (default to today)
DATE=$(date +%Y-%m-%d)
echo ""
echo -n "Enter date (YYYY-MM-DD) [default: $DATE]: "
read -r USER_DATE

if [ -n "$USER_DATE" ]; then
    DATE="$USER_DATE"
fi

# Generate filename
FILENAME="${DATE//-/}_${SLUG}.txt"
FILEPATH="${BLOG_DIR}/${FILENAME}"

# Check if file already exists
if [ -f "$FILEPATH" ]; then
    echo -e "${RED}Error: File already exists: $FILEPATH${NC}"
    echo -n "Overwrite? (y/N): "
    read -r OVERWRITE
    if [ "$OVERWRITE" != "y" ] && [ "$OVERWRITE" != "Y" ]; then
        echo "Aborted."
        exit 1
    fi
fi

# Create blog directory if it doesn't exist
mkdir -p "$BLOG_DIR"

# Create the blog entry file with template
cat > "$FILEPATH" << EOF
TITLE: $TITLE
DATE: $DATE
---


EOF

echo ""
echo -e "${GREEN}Created: $FILEPATH${NC}"
echo ""
echo "Opening in editor..."
echo ""

# Open in user's preferred editor
EDITOR="${EDITOR:-nano}"
"$EDITOR" "$FILEPATH"

# Verify the file has content
if [ ! -s "$FILEPATH" ]; then
    echo -e "${YELLOW}Warning: File is empty. Entry not saved.${NC}"
    rm "$FILEPATH"
    exit 1
fi

echo ""
echo -e "${GREEN}✓ Blog entry created successfully!${NC}"
echo ""
echo "File: $FILEPATH"
echo ""
echo "To view your blog:"
echo "  make run"
echo ""
echo "Or:"
echo "  ./bin/blog_viewer"
echo ""
