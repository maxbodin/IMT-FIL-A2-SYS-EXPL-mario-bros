# IMT-FIL-A2-SYS-EXPL-plants-vs-zombies

### Génerer un sprite
``` bash
python3 vga/sprite.py assets/sprites/sources/peashooter.png -p vga/atari-8-bit-family-gtia.pal 
```

Adapter uniquement le chemin vers le png (la palette est commune à tous les sprites).

Les fichiers cpp de chaque sprite sont générés dans `assets/sprites/sources`. Il faut les copy/paste dans `Applications/PlantsVsZombies/sprites`