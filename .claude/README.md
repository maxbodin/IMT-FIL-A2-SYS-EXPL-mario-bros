# .claude/ — Documentation de travail

Ce dossier contient la documentation interne utilisée par Claude pour maintenir le contexte entre sessions.

## Fichiers

| Fichier | Description |
|---------|-------------|
| [ARCHITECTURE.md](ARCHITECTURE.md) | Description technique complète du projet (structure, composants, build, objectifs) |
| [DROITS.md](DROITS.md) | Droits et restrictions de Claude (git interdit, etc.) |
| [branch-1-setup-vga.md](branch-1-setup-vga.md) | Historique et état de la branche `1-setup-vga` |

## Convention de nommage

- `branch-<id>-<slug>.md` : suivi d'une branche/ticket
- `ARCHITECTURE.md` : doc technique générale (mise à jour continue)
- `DROITS.md` : règles permanentes

## À faire en début de session

1. Lire `ARCHITECTURE.md` pour le contexte technique
2. Lire le fichier de branche correspondant à la branche git courante
3. Respecter `DROITS.md`
