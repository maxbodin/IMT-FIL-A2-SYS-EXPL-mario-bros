# Branche : 1-setup-vga

## Objectif
Mise en place du support VGA mode 13h (320×200, 256 couleurs) pour préparer le rendu graphique du jeu Plants vs Zombies.

## Commits
| Hash | Message |
|------|---------|
| `c953564` | add VGA folder |
| `e78249a` | Adding base files |
| `caa09a8` | first commit |

## Travail réalisé

### Ajout du dossier `vga/`
- `vga.h/.cpp` : driver VGA mode 13h
  - `set_vga_mode13()` : initialisation des registres VGA (Misc, Sequencer, CRTC, GC, Attribute)
  - `clear_vga_screen(color)` : remplissage écran
  - `plot_square(x, y, size, color)` : rectangle plein
  - `set_palette_vga(palette[256][3])` : chargement palette DAC
  - `draw_sprite(data, w, h, dstX, dstY)` : rendu sprite avec transparence (couleur 255)
- `sprite.h/.cpp` : palette Atari GTIA 256 couleurs + données sprite 32×32 d'exemple
- `sprite.py` : convertisseur PNG → tableau C (quantification couleurs)
- `palette.py` : convertisseur .pal/.gpl/.act → tableau C
- `atari-8-bit-family-gtia.pal` : palette de référence
- `README.md` (vga/) : guide d'intégration VGA

## État
- Setup VGA fonctionnel (à valider dans QEMU)
- Pas encore intégré dans `main.cpp`
- Prochain ticket : intégration VGA dans le main et début de la game loop PvZ

## Points d'attention
- Le mode VGA 13h désactive le mode texte : il faut basculer **avant** d'utiliser `Ecran`
- La palette est 6-bit (0–63) malgré les valeurs 0–255 dans les fichiers PNG
- QEMU : utiliser `make run_gui` ou VNC pour voir l'output graphique (pas de rendu en mode curses)
