#!/bin/bash
#
# list_entries.sh - List all blog entries with metadata
#
# Usage: ./scripts/list_entries.sh

# Default blog directory
BLOG_DIR="${BLOG_DIR:-./blog_entries}"

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${GREEN}SSH Blog - Entry List${NC}"
echo "================================"
echo ""

# Check if blog directory exists
if [ ! -d "$BLOG_DIR" ]; then
    echo -e "${YELLOW}No blog directory found: $BLOG_DIR${NC}"
    exit 1
fi

# Count entries
ENTRY_COUNT=$(find "$BLOG_DIR" -maxdepth 1 -name "*.txt" -type f | wc -l)

if [ "$ENTRY_COUNT" -eq 0 ]; then
    echo "No blog entries found in $BLOG_DIR"
    echo ""
    echo "Create your first entry with:"
    echo "  ./scripts/add_blog_entry.sh"
    echo ""
    exit 0
fi

echo -e "${BLUE}Found $ENTRY_COUNT blog entr$([ "$ENTRY_COUNT" -eq 1 ] && echo 'y' || echo 'ies')${NC}"
echo ""

# List entries sorted by date (newest first)
for file in $(find "$BLOG_DIR" -maxdepth 1 -name "*.txt" -type f | sort -r); do
    filename=$(basename "$file")

    # Extract metadata
    title=$(grep "^TITLE:" "$file" | head -1 | sed 's/^TITLE: *//')
    date=$(grep "^DATE:" "$file" | head -1 | sed 's/^DATE: *//')

    # Fallback to filename if metadata not found
    if [ -z "$title" ]; then
        title="$filename"
    fi

    if [ -z "$date" ]; then
        date="Unknown"
    fi

    # Count lines (approximate content length)
    content_lines=$(wc -l < "$file")

    echo -e "${GREEN}●${NC} [$date] $title"
    echo "  File: $filename ($content_lines lines)"
    echo ""
done

echo "To view these entries:"
echo "  make run"
echo ""
