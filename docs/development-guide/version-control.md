# Version Control

## Git Workflow

**Initialize Repository (if not done):**
```bash
git init
git add .
git commit -m "Initial commit"
```

**Typical Workflow:**
```bash
# Create feature branch
git checkout -b feature/new-mode

# Make changes, test
# ...

# Commit changes
git add .
git commit -m "Add new encoder mode"

# Merge back to main
git checkout main
git merge feature/new-mode
```

## .gitignore

**Already Configured:**
- `.pio/` - Build artifacts
- `.vscode/` - IDE settings
- Compiled binaries
