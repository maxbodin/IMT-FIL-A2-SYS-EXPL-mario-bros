# Architecture technique — Sextant OS / Plants vs Zombies

## Vue d'ensemble

**Sextant OS** est un kernel x86 bare-metal éducatif (IMT Mines-Télécom). Le projet vise à y implémenter un **Plants vs Zombies** en mode graphique VGA 320×200, 256 couleurs (mode 13h).

---

## Structure des dossiers

```
/
├── hal/                    # Hardware Abstraction Layer (boot Multiboot, ports E/S)
├── sextant/                # Noyau : main.cpp, types.h, interruptions/, memoire/
├── drivers/                # Drivers : Ecran (texte), Clavier, PortSerie, timer
├── vga/                    # Mode graphique VGA 13h (320×200, 256 couleurs)
├── Applications/           # Jeux et apps : SuperPong, Horloge
├── build/                  # Artefacts : sextant.elf, grub.iso, objets .o
├── support/                # Linker script (.lds), licence, sujet PDF
└── .claude/                # Documentation Claude (ce dossier)
```

---

## Composants clés

### HAL (`hal/`)
- `multiboot.S` : point d'entrée assembleur, header Multiboot (GRUB)
- `fonctionsES.h/.cpp` : `inb`/`outb` — lectures/écritures sur ports I/O

### Kernel (`sextant/`)
- `main.cpp` : `Sextant_main()` — init IDT, IRQ, timer 1000 Hz, handlers clavier/timer, puis loop applicative
- `types.h` : types entiers (`ui8_t`…), enum `Couleur` (16 couleurs texte)
- `interruptions/` : IDT (256 entrées), 8259 PIC, wrappers IRQ en ASM, handlers timer et clavier
- `memoire/` : allocateur heap minimal (`new`/`delete`)

### Drivers (`drivers/`)
- `Ecran` : framebuffer texte à 0xB8000, 80×25 colonnes, couleurs, défilement
- `Clavier` : scan codes depuis port 0x60, `getchar()`, `testChar()`
- `timer` : Intel 82C54, fréquence configurable, `getSecondes()`
- `PortSerie` : UART, debug série

### VGA (`vga/`)
- `vga.h/.cpp` : init registres VGA mode 13h (0x13), `set_vga_mode13()`, `clear_vga_screen()`, `plot_square()`, `set_palette_vga()`, `draw_sprite()`
- `sprite.h/.cpp` : tableau palette 256×3 (GTIA Atari), données sprite 32×32
- `sprite.py` : PNG → tableau C (indices palette), quantification couleurs
- `palette.py` : fichiers .pal/.gpl/.act → tableau C

---

### Commandes Makefile
```bash
make          # Compile
make run      # QEMU curses (terminal)
make run_gui  # QEMU GUI
make debug    # GDB + QEMU
make clean    # Nettoie les artefacts
```

---

## Mode graphique VGA 13h

- Résolution : **320×200 pixels**, **256 couleurs**
- Framebuffer : adresse `0xA0000`, 64 000 octets
- Palette : 256 triplets RGB 6-bit chargés via port DAC (0x3C8/0x3C9)
- Sprite : tableau `unsigned char[]`, couleur 255 = transparent
- Visualisation QEMU : VNC port 5900 (`make run_gui` ou option `-vnc :0`)

---

## Pratiques de développement

- Pas d'OS sous-jacent : tout est bare-metal, pas de stdlib
- Pas d'exceptions C++, pas de RTTI
- Mémoire statique ou allocateur minimal `memoire/`
- Les handlers d'interruption doivent être courts (pas de blocage)
- Tester avec QEMU avant de flasher (si applicable)
