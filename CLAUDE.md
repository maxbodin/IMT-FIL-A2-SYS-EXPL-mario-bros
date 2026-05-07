# Project Context

This is a C++ project with multiple applications under `Applications/`.
Current focus: PlantsVsZombies game with sprite-based rendering.

## Documentation

Before starting any task, read the relevant docs in `.claude/`:

- **`.claude/ARCHITECTURE.md`** — Read FIRST for any task involving 
  code structure, module relationships, or new features. Contains the 
  overall system design.
- **`.claude/DROITS.md`** — Read if the task touches permissions, 
  access control, or licensing concerns.
- **`.claude/branch-1-setup-vga.md`** — Read if working on VGA setup, 
  display initialization, or graphics output.
- **`.claude/README.md`** — General project overview, read if you need 
  high-level context.

## Workflow

1. Identify which `.claude/*.md` files are relevant to the user's request
2. Read ONLY those files (don't read all of them)
3. Then proceed with the task

## Code conventions

- C++ with `.cpp`/`.h` split
- Sprites live in `Applications/<AppName>/sprites/`
- Shared utilities at the application root (e.g., `Entity.cpp`, `Grid.cpp`)

## Important

- Don't explore the repo with grep/glob before checking `.claude/` docs first
- Ask before making architectural changes