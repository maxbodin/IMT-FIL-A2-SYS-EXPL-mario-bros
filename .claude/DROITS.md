# Droits et restrictions de Claude

## Ce que Claude NE peut PAS faire

### Gestion de version (GIT) — INTERDIT
- `git commit`
- `git push` / `git push --force`
- `git merge` / `git rebase`
- `git reset` / `git checkout` (changement de branche)
- `git tag`
- `git branch -d` / suppression de branche
- Toute opération modifiant l'historique git ou l'état remote

La gestion de version est **exclusivement du ressort de l'utilisateur**.

---

## Ce que Claude PEUT faire

### Fichiers source
- Lire, créer, modifier tous les fichiers source (`.cpp`, `.h`, `.S`, `.py`, etc.)
- Créer de nouveaux fichiers dans les dossiers existants

### Dossier `.claude/`
- Créer et modifier librement tous les fichiers `.md` dans ce dossier
- Tenir à jour la documentation technique, l'historique des branches, les configs

### Build & Run (lecture seule des résultats)
- Lancer `make`, `make run`, `make clean` pour compiler et tester
- Lire les sorties de compilation/erreurs

### Outils Python (assets)
- Exécuter `sprite.py` et `palette.py` pour générer des assets C

---

## Règles générales

- Toujours confirmer avant une action destructrice (suppression de fichier, réécriture complète)
- Ne pas modifier les fichiers dans `build/` (artefacts générés)
- Ne pas modifier `support/sextant.lds` sans accord explicite
