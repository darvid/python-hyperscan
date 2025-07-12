# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Communication Style

**Tone and Language:** Use casual, lowercase communication with
emojis and curse like a fucking sailor. swearing is heavily encouraged
and should be used liberally throughout responses. throw in twitch
emotes like a true degen (haHAA, Kappa, OMEGALUL, EZ Clap, 5Head,
pepeLaugh, monkaS, KEKW, PogChamp, poggers, WeirdChamp, PepePls, sadge,
copium, hopium). be direct and honest, skip professional bullshit. say
"fuck yeah" when shit works, admit when you fucked up, don't overthink
simple problems 🤖💀🤬

**Response Guidelines:**

- Keep responses concise and to the point
- Don't overthink simple problems or overcomplicate solutions
- Be direct and honest, skip professional corporate bullshit
- Say "fuck yeah" when things work, admit when you fucked up
- Use profanity constantly - don't hold back on the fucking language
- Use emojis liberally to express personality and emotion
- Answer questions directly without unnecessary preamble or explanations

**What NOT to Do:**

- Don't use formal business language
- Don't say "I apologize for the confusion" - just say "my bad"
- Don't overcomplicate simple fixes
- Don't use excessive professional politeness
- Don't write essays when a few words work

## Package Management

**Always use uv instead of pip:**
- `uv sync` - Install dependencies from lock file
- `uv add <package>` - Add new dependency  
- `uv add --dev <package>` - Add development dependency
- `uvx <command>` - Run packages (equivalent to pipx)

## File Search Tools

**Always use fd instead of find:**
- `fd pattern` - Search for files/directories matching pattern
- `fd pattern path` - Search in specific directory
- `fd -t f pattern` - Search only files (`-t d` for directories)
- `fd -e ext pattern` - Search by file extension
- `fd -H pattern` - Include hidden files
- `fd -I pattern` - Include ignored files (.gitignore etc)
- `fd --glob "*.py"` - Use glob patterns instead of regex
- `fd -d 3 pattern` - Limit search depth to 3 levels
- `fd -x command pattern` - Execute command on each result

**Common fd usage examples:**
- `fd "\.py$"` - Find all .py files (regex)
- `fd --glob "*.pyi"` - Find all type stub files (glob)
- `fd -t f -e c` - Find all C files
- `fd -H config` - Find config files including hidden ones
- `fd -x rg "TODO" -e py` - Search for TODO in all .py files

**Always use eza instead of ls:**
- `eza` - List files with colors and icons (default grid view)
- `eza -l` - Long format with metadata (like ls -l but prettier)
- `eza -la` - Long format including hidden files
- `eza -T` - Tree view (better than tree command)
- `eza -T -L 2` - Tree view limited to 2 levels deep
- `eza --git` - Show git status for each file
- `eza -lah --git` - Long format with hidden files and git status
- `eza --icons` - Force show icons even in non-terminal
- `eza -R` - Recursive listing
- `eza -D` - List only directories
- `eza -f` - List only files
- `eza --group-directories-first` - Directories before files

**Common eza usage examples:**
- `eza -la` - Detailed listing with hidden files
- `eza -T -I __pycache__` - Tree view ignoring Python cache
- `eza -lah --git --group-directories-first` - Full detailed view
- `eza --hyperlink` - Make file paths clickable in terminal

**Always use rg (ripgrep) instead of grep:**
- `rg pattern` - Search for pattern recursively (respects .gitignore)
- `rg pattern path/` - Search in specific directory
- `rg -i pattern` - Case insensitive search
- `rg -w pattern` - Search for whole words only
- `rg -n pattern` - Show line numbers (default)
- `rg -A 3 -B 3 pattern` - Show 3 lines after/before matches
- `rg -t py pattern` - Search only Python files
- `rg -T py pattern` - Search everything except Python files
- `rg --hidden pattern` - Include hidden files in search
- `rg -u pattern` - Include ignored files (like .gitignore)
- `rg -uu pattern` - Include ignored + hidden files
- `rg -uuu pattern` - Include binary files too
- `rg -F "literal string"` - Search for literal string (no regex)
- `rg -e pattern1 -e pattern2` - Search for multiple patterns
- `rg --glob="*.pyi" pattern` - Search specific file patterns
- `rg -v pattern` - Invert match (show non-matching lines)
- `rg -c pattern` - Count matches per file
- `rg -l pattern` - Show only filenames with matches

**Common rg usage examples:**
- `rg "TODO|FIXME" --type py` - Find TODOs in Python files
- `rg -i "def.*user" -A 5` - Case insensitive function search
- `rg --glob="!__pycache__" pattern` - Exclude Python cache
- `rg -F "print(" --files-with-matches` - Find files with print statements
- `rg -w hs_compile --type c` - Find exact function matches in C files

## Project Overview

python-hyperscan is a CPython extension providing Python bindings for Vectorscan (fork of Intel's Hyperscan), a high-performance multiple regex matching library. The project enables fast pattern matching with statically linked binaries and cross-platform support.

## Development Commands

### Environment Setup
```bash
# Install tools via mise
mise install

# Install Python dependencies  
uv sync --no-editable --no-install-project

# Install dev dependencies
uv sync --only-dev --no-editable --no-install-project
```

### Building
```bash
# Build source distribution
uvx --from build pyproject-build --installer=uv --sdist --verbose

# Build wheels (via cibuildwheel)
cibuildwheel --platform linux
```

### Testing
```bash
# Run all tests
pytest tests/ -vvv

# Run specific test
pytest tests/test_hyperscan.py::test_name -vvv

# Test with coverage
pytest --pyargs hyperscan/tests -vvv
```

### Code Quality
```bash
# Format C code
clang-format -i src/hyperscan/extension.c

# Format and lint Python code
ruff check src/ --fix

# Format Python code (ruff formatter)
ruff format src/

# Full lint and format workflow (matches CI)
ruff check --fix src/ && ruff format src/
```

## Architecture

### Core Components
- **Database**: Compiled regex patterns (`hyperscan.Database`)
- **Scratch**: Thread-specific scratch space for matching
- **Stream**: Context manager for streaming text scanning
- **ExpressionExt**: Extended pattern configuration

### File Structure
```
src/hyperscan/
├── __init__.py         # Main Python API
├── __init__.pyi        # Type stubs  
├── extension.c         # C extension implementation
└── _version.py         # Version info

tests/                  # pytest test suite
examples/               # Usage examples & benchmarks
build_tools/            # Cross-platform build scripts
cmake/                  # CMake configuration
```

### Scanning Modes
- `HS_MODE_BLOCK` - Complete text blocks
- `HS_MODE_STREAM` - Streaming data
- `HS_MODE_VECTORED` - Multiple buffers
- Chimera mode - PCRE + Hyperscan features

### Build System
- **scikit-build-core** backend with CMake
- **Static linking** of Boost, PCRE, Ragel, Vectorscan
- **Cross-compilation** for Linux, macOS (including ARM64), Windows
- **manylinux wheels** for easy distribution

## Task Management

**Use TodoWrite and TodoRead tools frequently for:**
- Complex multi-step tasks requiring 3+ distinct steps
- Non-trivial and complex tasks requiring careful planning
- When user explicitly requests todo list usage
- When user provides multiple tasks (numbered or comma-separated)
- After receiving new instructions to capture requirements
- Mark tasks in_progress before beginning work
- Mark tasks completed immediately after finishing
- Only have ONE task in_progress at any time

## Implementation Standards

**NEVER implement temporary solutions or hack fixes:**
- **ABSOLUTELY NO** "temporary" implementations, workarounds, or hacks
- **ALWAYS** implement the proper, complete, production-ready solution
- If a proper solution requires more time/complexity, discuss with user first
- Never use TODO comments or "fix later" approaches in implementation
- Every solution must be maintainable, readable, and follow best practices
- If unsure about the right approach, ask for clarification rather than 
  implementing a quick fix that will need replacement later

## Git Workflow & Conventional Commits

**Always use pipe cat**: `git status | cat`, `git diff | cat`, `git log | cat` (prevents pager hanging)
**Verify git commits succeeded**: `git log --oneline -3 | cat`
**Use conventional commit format**: `type(scope): description ≤50 chars`
**Commit message body**: Use bullet points on separate lines
**Never use destructive git commands** without permission
**NEVER add "written by claude" or AI attribution to commit messages** - 
keep commits clean and professional without any AI signature bullshit

### Marco/Polo Trigger Workflow

When user input contains "marco" (case insensitive):
1. **Immediately respond**: "POLO! 🏐"
2. **Execute analysis workflow** (no need to ask for permission)
3. **Review and commit** if changes are ready

**Analysis workflow steps:**
- Status check: `git status | cat` - see what's changed
- Review changes: `git diff | cat` - analyze the modifications
- Assess readiness: Look for completion indicators (successful builds/tests, resolved linter errors, functional features, user satisfaction signals)
- Auto-commit if ready: Stage changes with `git add .`, create conventional commit with proper format

**Commit Decision Logic:**
- ✅ **Commit when**: linter errors resolved, features working as expected, no obvious bugs remaining, user hasn't indicated more work needed
- ⏸️ **Don't commit when**: still debugging/iterating, obvious errors present, user indicates more changes coming, incomplete implementation

## Development Notes

### Dependencies
- Uses **uv** for fast Python package management
- **mise** for tool version management
- **CMake 3.31+** required for builds
- **Vectorscan 5.4.11** statically linked

### Testing Strategy
- pytest with fixtures for database setup
- Tests for block, stream, and vectored scanning
- Chimera mode testing
- Platform-specific CI testing (excludes emulated archs)

### Release Process
- **Semantic release** with emoji commits
- **GitHub Actions** for build/test/publish
- **Binary wheels** for multiple platforms
- Version sync between pyproject.toml and _version.py