#!/usr/bin/env python3
"""
png_to_c_with_palette.py: Convert une image PNG en tableaux C pour VGA,
avec quantization automatique pour les modes autres que 'P',
et option de charger une palette externe JASC-PAL ou GIMP-GPL.

Usage:
    python3 png_to_c_with_palette.py [-p PALETTE_FILE] <image.png>

Génère :
  - #define SPRITE_WIDTH  <width>
  - #define SPRITE_HEIGHT <height>
  - unsigned char palette_vga[256][3] = {{...}};
  - unsigned char sprite_data[SPRITE_WIDTH*SPRITE_HEIGHT] = {{...}};
"""
import sys
import re
from PIL import Image
import argparse

def parse_jasc_pal(path):
    """Parse un fichier JASC-PAL (.pal) en liste de 256 tuples RGB."""
    with open(path, 'r') as f:
        lines = f.read().splitlines()
    # Format: JASC-PAL / version / 256 / R G B...
    header = lines[:3]
    count = int(header[2])
    data = []
    for line in lines[3:3+count]:
        parts = line.strip().split()
        if len(parts) >= 3:
            data.append(tuple(map(int, parts[:3])))
    # Complète si <256
    data += [(0,0,0)] * max(0, 256 - len(data))
    return data[:256]


def parse_gimp_gpl(path):
    """Parse un fichier GIMP-GPL (.gpl) en liste de 256 tuples RGB."""
    data = []
    with open(path, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#') or re.match(r'GIMP Palette', line) or line.startswith('Name:') or line.startswith('Columns:'):
                continue
            parts = line.split()
            if len(parts) >= 3:
                data.append(tuple(map(int, parts[:3])))
    data += [(0,0,0)] * max(0, 256 - len(data))
    return data[:256]


def parse_cpp_pal(path):
    """Parse un fichier shared_palette.cpp (VGA 6-bit) en liste de 256 tuples RGB 8-bit."""
    import re as _re
    data = []
    with open(path, 'r') as f:
        content = f.read()
    for m in _re.finditer(r'\{(\d+),\s*(\d+),\s*(\d+)\}', content):
        r6, g6, b6 = int(m.group(1)), int(m.group(2)), int(m.group(3))
        data.append(((r6 * 255) // 63, (g6 * 255) // 63, (b6 * 255) // 63))
    data += [(0, 0, 0)] * max(0, 256 - len(data))
    return data[:256]


def rgb_to_vga(r, g, b):
    """Convertit RGB 8-bit (0–255) en VGA 6-bit (0–63)."""
    return (r * 63) // 255, (g * 63) // 255, (b * 63) // 255


def build_palette_image(palette):
    """Construit une image PIL mode 'P' 1x256 avec la palette donnée."""
    pal_img = Image.new('P', (1, 1))
    flat = []
    for (r,g,b) in palette:
        flat.extend([r, g, b])
    pal_img.putpalette(flat)
    return pal_img


def png_to_c(path, external_palette=None,
             array_name="sprite_data",
             width_name="SPRITE_WIDTH",
             height_name="SPRITE_HEIGHT",
             palette_name="palette_vga",
             out_dir=None):
    im = Image.open(path)

    # Chargement et application de la palette externe si fournie
    if external_palette:
        ext = external_palette.lower()
        if ext.endswith('.pal'):
            pal = parse_jasc_pal(external_palette)
        elif ext.endswith('.gpl'):
            pal = parse_gimp_gpl(external_palette)
        elif ext.endswith('.cpp') or ext.endswith('.h'):
            pal = parse_cpp_pal(external_palette)
        else:
            print(f"[!] Format de palette non supporté: {external_palette}", file=sys.stderr)
            sys.exit(1)
        # Quantize l'image sur cette palette
        pal_img = build_palette_image(pal)
        im = im.convert('RGB').quantize(palette=pal_img)
    else:
        # Si pas P, quantize en 256 couleurs automatiques
        if im.mode != 'P':
            print(f"[i] Mode actuel: {im.mode}. Conversion en 256 couleurs indexées via quantize...", file=sys.stderr)
            im = im.convert('RGB').quantize(colors=256)
        # Charge palette depuis l'image
        raw_pal = im.getpalette() or []
        raw_pal = raw_pal[:768] + [0] * max(0, 768 - len(raw_pal))
        pal = [(raw_pal[i], raw_pal[i+1], raw_pal[i+2]) for i in range(0, 768, 3)]

    # Conversion en VGA 6-bit
    vga_pal = [rgb_to_vga(r, g, b) for (r, g, b) in pal]

    # Dimensions et données pixels
    w, h = im.size
    data = list(im.getdata())

    # Alpha-aware transparency: index 0 is reserved for transparent pixels.
    # Remap non-transparent pixels that quantized to index 0 → index 1.
    orig = Image.open(path)
    if orig.mode in ('RGBA', 'LA', 'PA'):
        alpha = list(orig.convert('RGBA').split()[3].getdata())
        for i in range(len(data)):
            if alpha[i] < 128:
                data[i] = 0    # transparent
            elif data[i] == 0:
                data[i] = 1    # remap black to near-black
    else:
        # No alpha channel — remap all index 0 to index 1
        for i in range(len(data)):
            if data[i] == 0:
                data[i] = 1

    # Génération du code C
    print(f"// Généré par png_to_c_with_palette.py à partir de '{path}'", file=sys.stdout)
    print(f"#define {width_name}  {w}", file=sys.stdout)
    print(f"#define {height_name} {h}\n", file=sys.stdout)

    # Palette VGA
    print(f"unsigned char {palette_name}[256][3] = {{", file=sys.stdout)
    for r, g, b in vga_pal:
        print(f"    {{{r}, {g}, {b}}},", file=sys.stdout)
    print("};\n", file=sys.stdout)

    # Sprite data
    print(f"unsigned char {array_name}[{width_name}*{height_name}] = {{", file=sys.stdout)
    for i, pix in enumerate(data):
        sep = "," if i < len(data)-1 else ""
        end = "\n" if (i % 16 == 15) else " "
        sys.stdout.write(f"{pix:3d}{sep}{end}")
    print("};", file=sys.stdout)
    
    import os
    base = os.path.splitext(os.path.basename(path))[0]
    if out_dir is None:
        out_dir = os.path.dirname(path)
    guard = base.upper() + "_SPRITE_H"
    header_path = os.path.join(out_dir, base + "_sprite.h")
    source_path = os.path.join(out_dir, base + "_sprite.cpp")

    # Fichier .h avec include guard
    with open(header_path, 'w') as f:
        f.write(f"#ifndef {guard}\n#define {guard}\n\n")
        f.write(f"#define {width_name}  {w}\n")
        f.write(f"#define {height_name} {h}\n\n")
        f.write(f"extern unsigned char {palette_name}[256][3];\n")
        f.write(f"extern unsigned char {array_name}[{width_name}*{height_name}];\n")
        f.write(f"\n#endif\n")
    print(f"[i] Header généré : {header_path}", file=sys.stderr)

    # Fichier .cpp avec les données
    with open(source_path, 'w') as f:
        f.write(f"#include \"{os.path.basename(header_path)}\"\n\n")
        f.write(f"unsigned char {palette_name}[256][3] = {{\n")
        for r, g, b in vga_pal:
            f.write(f"    {{{r}, {g}, {b}}},\n")
        f.write("};\n\n")
        f.write(f"unsigned char {array_name}[{width_name}*{height_name}] = {{\n")
        for i, pix in enumerate(data):
            sep = "," if i < len(data)-1 else ""
            end = "\n" if (i % 16 == 15) else " "
            f.write(f"{pix:3d}{sep}{end}")
        f.write("};\n")
    print(f"[i] Source généré  : {source_path}", file=sys.stderr)

def dir_to_c(dirpath, external_palette=None, out_dir=None):
    """Convertit un dossier de PNGs numérotés en tableau d'animation C."""
    import os, re

    pngs = sorted(
        [f for f in os.listdir(dirpath) if f.lower().endswith('.png') and 'full' not in f.lower() and 'all' not in f.lower()],
        key=lambda f: int(re.search(r'(\d+)', f).group(1)) if re.search(r'(\d+)', f) else 0
    )
    if not pngs:
        print(f"[!] Aucun PNG trouvé dans {dirpath}", file=sys.stderr)
        sys.exit(1)

    base = os.path.basename(dirpath.rstrip('/\\'))
    if out_dir is None:
        out_dir = dirpath

    # Charger la palette externe une fois
    if external_palette:
        ext = external_palette.lower()
        if ext.endswith('.pal'):
            pal = parse_jasc_pal(external_palette)
        elif ext.endswith('.gpl'):
            pal = parse_gimp_gpl(external_palette)
        elif ext.endswith('.cpp') or ext.endswith('.h'):
            pal = parse_cpp_pal(external_palette)
        else:
            print(f"[!] Format non supporté: {external_palette}", file=sys.stderr)
            sys.exit(1)
        pal_img = build_palette_image(pal)
    else:
        pal = None

    frames = []
    width = height = None

    # First pass: determine max dimensions
    max_w = max_h = 0
    for png in pngs:
        path = os.path.join(dirpath, png)
        im = Image.open(path)
        w, h = im.size
        if w > max_w: max_w = w
        if h > max_h: max_h = h

    for png in pngs:
        path = os.path.join(dirpath, png)
        im = Image.open(path)

        # Pad to max dimensions (center horizontally, align bottom)
        w, h = im.size
        if w != max_w or h != max_h:
            padded = Image.new(im.mode, (max_w, max_h), (0, 0, 0, 0) if im.mode == 'RGBA' else (0, 0, 0))
            offset_x = (max_w - w) // 2
            offset_y = max_h - h
            padded.paste(im, (offset_x, offset_y))
            im = padded

        if pal:
            im_q = im.convert('RGB').quantize(palette=pal_img)
        else:
            if im.mode != 'P':
                im_q = im.convert('RGB').quantize(colors=256)
            else:
                im_q = im
        w, h = im_q.size
        if width is None:
            width, height = w, h
        elif (w, h) != (width, height):
            print(f"[!] {png}: taille {w}x{h} != {width}x{height}", file=sys.stderr)
            sys.exit(1)
        frame_data = list(im_q.getdata())

        # Alpha-aware transparency: index 0 = transparent, remap opaque index 0 → 1
        if im.mode in ('RGBA', 'LA', 'PA'):
            alpha = list(im.convert('RGBA').split()[3].getdata())
            for i in range(len(frame_data)):
                if alpha[i] < 128:
                    frame_data[i] = 0
                elif frame_data[i] == 0:
                    frame_data[i] = 1
        else:
            for i in range(len(frame_data)):
                if frame_data[i] == 0:
                    frame_data[i] = 1

        frames.append(frame_data)
        print(f"[i] Frame {len(frames)-1}: {png} ({w}x{h})", file=sys.stderr)

    nb = len(frames)
    guard     = base.upper() + "_ANIM_H"
    width_def = base.upper() + "_WIDTH"
    height_def= base.upper() + "_HEIGHT"
    frames_def= base.upper() + "_FRAMES"
    array_name= base + "_frames"

    # Écriture du .h
    header_path = os.path.join(out_dir, base + "_sprite.h")
    with open(header_path, 'w') as f:
        f.write(f"#ifndef {guard}\n#define {guard}\n\n")
        f.write(f"#define {width_def}  {width}\n")
        f.write(f"#define {height_def} {height}\n")
        f.write(f"#define {frames_def} {nb}\n\n")
        f.write(f"extern unsigned char {array_name}[{frames_def}][{width_def}*{height_def}];\n")
        f.write(f"\n#endif\n")
    print(f"[i] Header généré : {header_path}", file=sys.stderr)

    # Écriture du .cpp
    source_path = os.path.join(out_dir, base + "_sprite.cpp")
    with open(source_path, 'w') as f:
        f.write(f'#include "{base}_sprite.h"\n\n')
        f.write(f"unsigned char {array_name}[{frames_def}][{width_def}*{height_def}] = {{\n")
        for fi, frame in enumerate(frames):
            f.write(f"    {{ // frame {fi}\n    ")
            for i, pix in enumerate(frame):
                sep = "," if i < len(frame)-1 else ""
                end = "\n    " if (i % 16 == 15) else " "
                f.write(f"{pix:3d}{sep}{end}")
            f.write("},\n")
        f.write("};\n")
    print(f"[i] Source généré  : {source_path}", file=sys.stderr)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Convert PNG(s) to C arrays for VGA.")
    parser.add_argument('-p', '--palette', help="Fichier de palette externe (.pal ou .gpl)")
    parser.add_argument('-o', '--output', help="Dossier de sortie pour les fichiers générés (défaut: même dossier que l'entrée)")
    parser.add_argument('image', help="Chemin vers une image PNG ou un dossier de PNGs numérotés")
    args = parser.parse_args()

    import os

    out_dir = args.output
    if out_dir:
        os.makedirs(out_dir, exist_ok=True)

    if os.path.isdir(args.image):
        dir_to_c(args.image, external_palette=args.palette, out_dir=out_dir)
    else:
        base = os.path.splitext(os.path.basename(args.image))[0]
        src_dir = out_dir if out_dir else os.path.dirname(args.image)
        png_to_c(
            args.image,
            external_palette=args.palette,
            array_name=f"{base}_sprite_data",
            width_name=f"{base.upper()}_WIDTH",
            height_name=f"{base.upper()}_HEIGHT",
            palette_name=f"{base}_palette",
            out_dir=src_dir,
        )
