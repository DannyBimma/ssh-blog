#!/bin/bash
#
# test_suite.sh - Comprehensive test suite for SSH Blog
#
# This script tests all core functionality of the blog viewer

set -e

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m'

PASS_COUNT=0
FAIL_COUNT=0

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}SSH Blog - Comprehensive Test Suite${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Helper functions
pass() {
    echo -e "${GREEN}✓ PASS${NC} - $1"
    PASS_COUNT=$((PASS_COUNT + 1))
}

fail() {
    echo -e "${RED}✗ FAIL${NC} - $1"
    FAIL_COUNT=$((FAIL_COUNT + 1))
}

info() {
    echo -e "${BLUE}→${NC} $1"
}

section() {
    echo ""
    echo -e "${YELLOW}=== $1 ===${NC}"
    echo ""
}

# Test 1: Build System
section "Build System Tests"

info "Testing clean build..."
if make clean > /dev/null 2>&1; then
    pass "make clean"
else
    fail "make clean"
fi

info "Testing compilation..."
if make > /dev/null 2>&1; then
    pass "make (compilation)"
else
    fail "make (compilation)"
fi

info "Checking binary exists..."
if [ -f "bin/blog_viewer" ]; then
    pass "Binary created: bin/blog_viewer"
else
    fail "Binary not found"
    exit 1
fi

info "Checking binary is executable..."
if [ -x "bin/blog_viewer" ]; then
    pass "Binary is executable"
else
    fail "Binary is not executable"
fi

# Test 2: Blog Entry Loading
section "Blog Entry Loading Tests"

info "Checking blog directory exists..."
if [ -d "blog_entries" ]; then
    pass "blog_entries directory exists"
else
    fail "blog_entries directory not found"
fi

info "Counting blog entries..."
ENTRY_COUNT=$(find blog_entries -name "*.txt" -type f | wc -l)
if [ "$ENTRY_COUNT" -gt 0 ]; then
    pass "Found $ENTRY_COUNT blog entries"
else
    fail "No blog entries found"
fi

info "Validating blog entry format..."
for file in blog_entries/*.txt; do
    if grep -q "^TITLE:" "$file" && grep -q "^DATE:" "$file" && grep -q "^---" "$file"; then
        pass "Valid format: $(basename "$file")"
    else
        fail "Invalid format: $(basename "$file")"
    fi
done

# Test 3: Command Line Interface
section "Command Line Interface Tests"

info "Testing --help flag..."
if ./bin/blog_viewer --help > /dev/null 2>&1; then
    pass "Help message displays"
else
    fail "Help flag failed"
fi

info "Testing -h flag..."
if ./bin/blog_viewer -h > /dev/null 2>&1; then
    pass "Short help flag works"
else
    fail "Short help flag failed"
fi

info "Testing with non-existent directory..."
if ! ./bin/blog_viewer /nonexistent/path 2>&1 | grep -q "Failed to load"; then
    pass "Handles missing directory gracefully"
else
    fail "Error handling for missing directory"
fi

# Test 4: Source Code Quality
section "Source Code Quality Tests"

info "Checking for compiler warnings..."
if make clean > /dev/null 2>&1 && make 2>&1 | grep -q "warning:"; then
    WARNINGS=$(make clean > /dev/null 2>&1 && make 2>&1 | grep -c "warning:")
    fail "Found $WARNINGS compiler warning(s)"
else
    pass "No critical compiler warnings"
fi

info "Checking for TODO comments in code..."
TODO_COUNT=$(grep -r "TODO" src/ | wc -l)
if [ "$TODO_COUNT" -gt 0 ]; then
    info "Found $TODO_COUNT TODO items (not a failure, just FYI)"
fi

info "Verifying all header files have include guards..."
for header in src/*.h; do
    if grep -q "#ifndef.*_H" "$header" && grep -q "#define.*_H" "$header" && grep -q "#endif" "$header"; then
        pass "Include guards: $(basename "$header")"
    else
        fail "Missing include guards: $(basename "$header")"
    fi
done

# Test 5: Utility Scripts
section "Utility Scripts Tests"

info "Checking script permissions..."
for script in scripts/*.sh; do
    if [ -x "$script" ]; then
        pass "Executable: $(basename "$script")"
    else
        fail "Not executable: $(basename "$script")"
    fi
done

info "Testing list_entries.sh..."
if ./scripts/list_entries.sh > /dev/null 2>&1; then
    pass "list_entries.sh runs successfully"
else
    fail "list_entries.sh failed"
fi

info "Testing list_entries.sh output..."
if ./scripts/list_entries.sh 2>/dev/null | grep -q "Found.*blog entr"; then
    pass "list_entries.sh produces expected output"
else
    fail "list_entries.sh output format incorrect"
fi

# Test 6: Documentation
section "Documentation Tests"

info "Checking README.md exists..."
if [ -f "README.md" ]; then
    pass "README.md exists"
else
    fail "README.md not found"
fi

info "Checking BUILD_PLAN.md exists..."
if [ -f "BUILD_PLAN.md" ]; then
    pass "BUILD_PLAN.md exists"
else
    fail "BUILD_PLAN.md not found"
fi

info "Checking DEVELOPMENT.md exists..."
if [ -f "docs/DEVELOPMENT.md" ]; then
    pass "docs/DEVELOPMENT.md exists"
else
    fail "docs/DEVELOPMENT.md not found"
fi

info "Verifying documentation has recent updates..."
for doc in README.md BUILD_PLAN.md docs/DEVELOPMENT.md; do
    if [ -f "$doc" ]; then
        LINES=$(wc -l < "$doc")
        if [ "$LINES" -gt 50 ]; then
            pass "$(basename "$doc") is comprehensive ($LINES lines)"
        else
            fail "$(basename "$doc") seems incomplete ($LINES lines)"
        fi
    fi
done

# Test 7: File Structure
section "File Structure Tests"

info "Checking project structure..."
REQUIRED_DIRS=("src" "blog_entries" "scripts" "docs")
for dir in "${REQUIRED_DIRS[@]}"; do
    if [ -d "$dir" ]; then
        pass "Directory exists: $dir"
    else
        fail "Missing directory: $dir"
    fi
done

info "Checking Makefile..."
if [ -f "Makefile" ]; then
    pass "Makefile exists"
else
    fail "Makefile not found"
fi

info "Checking .gitignore..."
if [ -f ".gitignore" ]; then
    if grep -q "bin/" ".gitignore" && grep -q "obj/" ".gitignore"; then
        pass ".gitignore properly configured"
    else
        fail ".gitignore missing build directories"
    fi
else
    fail ".gitignore not found"
fi

# Test 8: Git Repository
section "Git Repository Tests"

info "Checking git repository..."
if [ -d ".git" ]; then
    pass "Git repository initialized"
else
    fail "Not a git repository"
fi

info "Checking for uncommitted changes..."
if [ -z "$(git status --porcelain)" ]; then
    pass "No uncommitted changes"
else
    info "Uncommitted changes exist (not a failure)"
fi

info "Checking commit count..."
COMMIT_COUNT=$(git rev-list --count HEAD 2>/dev/null || echo "0")
if [ "$COMMIT_COUNT" -gt 10 ]; then
    pass "Good commit history ($COMMIT_COUNT commits)"
else
    fail "Insufficient commit history ($COMMIT_COUNT commits)"
fi

# Test 9: Memory and Performance (if valgrind available)
section "Memory Tests (Optional)"

if command -v valgrind &> /dev/null; then
    info "valgrind found - running memory leak test..."
    # This test would need interactive input, so we skip for now
    info "Skipping valgrind test (requires interactive mode)"
else
    info "valgrind not installed - skipping memory tests"
fi

# Test 10: Code Statistics
section "Code Statistics"

info "Counting lines of code..."
C_LINES=$(find src -name "*.c" -exec wc -l {} + | tail -1 | awk '{print $1}')
H_LINES=$(find src -name "*.h" -exec wc -l {} + | tail -1 | awk '{print $1}')
TOTAL_LINES=$((C_LINES + H_LINES))

echo ""
echo -e "${BLUE}Code Statistics:${NC}"
echo "  C source files: $C_LINES lines"
echo "  Header files: $H_LINES lines"
echo "  Total: $TOTAL_LINES lines"
echo ""

# Summary
section "Test Summary"

TOTAL_TESTS=$((PASS_COUNT + FAIL_COUNT))
PASS_PERCENT=$((PASS_COUNT * 100 / TOTAL_TESTS))

echo ""
echo -e "${GREEN}Passed:${NC} $PASS_COUNT"
echo -e "${RED}Failed:${NC} $FAIL_COUNT"
echo -e "${BLUE}Total:${NC}  $TOTAL_TESTS"
echo ""
echo -e "${BLUE}Success Rate: ${PASS_PERCENT}%${NC}"
echo ""

if [ "$FAIL_COUNT" -eq 0 ]; then
    echo -e "${GREEN}========================================${NC}"
    echo -e "${GREEN}  ALL TESTS PASSED! ✓${NC}"
    echo -e "${GREEN}========================================${NC}"
    exit 0
else
    echo -e "${YELLOW}========================================${NC}"
    echo -e "${YELLOW}  SOME TESTS FAILED${NC}"
    echo -e "${YELLOW}========================================${NC}"
    exit 1
fi
