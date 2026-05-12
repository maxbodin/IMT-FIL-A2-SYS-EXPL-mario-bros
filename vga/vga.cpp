#include "vga.h"

volatile unsigned char *video = (unsigned char *)0xA0000;
/* I/O port helpers (you must have I/O privileges!) */
static inline void outb(unsigned short port, unsigned char val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}
static inline unsigned char inb(unsigned short port) {
    unsigned char val;
    asm volatile ("inb %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

/* VGA register values for mode 0x13 (320×200, 256 colors) */
static const unsigned char vga_misc    = 0x63;
static const unsigned char vga_seq[5]  = { 0x03,0x01,0x0F,0x00,0x0E };
static const unsigned char vga_crtc[25]= {
    0x5F,0x4F,0x50,0x82,0x54,0x80,0xBF,0x1F,
    0x00,0x41,0x00,0x00,0x00,0x00,0x00,0x00,
    0x9C,0x0E,0x8F,0x28,0x1F,0x96,0xB9,0xA3,0xFF
};
static const unsigned char vga_gc[9]   = { 
    //0x00,0x00,0x10,0x0E,0x02,0x0F,0x00,0x00,0x00 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
	0xFF    
};
static const unsigned char vga_attr[21]= {
    /* palette 0–15 */ 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
                       0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
    /* overscan, etc */ 0x41,0x00,0x0F,0x00,0x00
};


void set_vga_mode13(void) {
    int i;

    /* 1) Miscellaneous Output */
    outb(0x03C2, vga_misc);

    /* 2) Sequencer registers */
    for (i = 0; i < 5; i++) {
        outb(0x03C4, i);
        outb(0x03C5, vga_seq[i]);
    }

    /* 3) CRTC registers */
    for (i = 0; i < 25; i++) {
        outb(0x03D4, i);
        outb(0x03D5, vga_crtc[i]);
    }

    /* 4) Graphics Controller registers */
    for (i = 0; i < 9; i++) {
        outb(0x03CE, i);
        outb(0x03CF, vga_gc[i]);
    }

    /* 5) Attribute Controller registers
     *    Reset flip‑flop by reading 0x3DA before each write.
     */
    for (i = 0; i < 21; i++) {
        inb(0x03DA);
        outb(0x03C0, i);
        outb(0x03C0, vga_attr[i]);
    }

    /* Unblank display: re‑reset flip‑flop and set bit 5 */
    inb(0x03DA);
    outb(0x03C0, 0x20);
}

void clear_vga_screen(char color) {

    int i;

    for (i = 0; i < 320 * 200; i++) {
        video[i] = color;
    }
}

void plot_square(int x, int y, int size, unsigned char color) {
    for (int row = 0; row < size; row++) {
        int py = y + row;
        if (py < 0 || py >= 200) continue;
        for (int col = 0; col < size; col++) {
            int px = x + col;
            if (px < 0 || px >= 320) continue;
            video[py * 320 + px] = color;
        }
    }
}

void plot_palette(int x, int y, int size) {
    int row, col;
    for (row = 0; row < 16; row++) {
        int base = (y + row) * 320 + x;
        for (col = 0; col < 16; col++) {
            plot_square(x+col*size,y+row*size,size,row*16+col);
        }
    }
}


void set_palette_vga(unsigned char palette_vga[256][3]) {
    // Sélection de l'entrée 0
    outb(0x3C8, 0);
    // Envoi des 256 triplets VGA (6-bit)
    for(int i = 0; i < 256; i++) {
        outb(0x3C9, palette_vga[i][0]);
        outb(0x3C9, palette_vga[i][1]);
        outb(0x3C9, palette_vga[i][2]);
    }
}
// 3x5 bitmap font for digits 0-9 (bit 2 = left, bit 0 = right)
static const unsigned char digit_font[10][5] = {
    {0b111,0b101,0b101,0b101,0b111}, // 0
    {0b010,0b110,0b010,0b010,0b111}, // 1
    {0b111,0b001,0b111,0b100,0b111}, // 2
    {0b111,0b001,0b011,0b001,0b111}, // 3
    {0b101,0b101,0b111,0b001,0b001}, // 4
    {0b111,0b100,0b111,0b001,0b111}, // 5
    {0b111,0b100,0b111,0b101,0b111}, // 6
    {0b111,0b001,0b001,0b001,0b001}, // 7
    {0b111,0b101,0b111,0b101,0b111}, // 8
    {0b111,0b101,0b111,0b001,0b111}, // 9
};

void draw_digit(int d, int x, int y, unsigned char color, int scale) {
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 3; col++) {
            if (digit_font[d][row] & (1 << (2 - col)))
                plot_square(x + col * scale, y + row * scale, scale, color);
        }
    }
}

void draw_number(int n, int x, int y, unsigned char color, int scale) {
    if (n < 0) n = 0;
    char buf[6];
    int len = 0;
    if (n == 0) { buf[len++] = 0; }
    else { int tmp = n; while (tmp > 0) { buf[len++] = tmp % 10; tmp /= 10; } }
    for (int i = 0; i < len; i++) {
        int digit = buf[len - 1 - i];
        draw_digit(digit, x + i * (3 * scale + scale), y, color, scale);
    }
}

// 3x5 bitmap font for a small set of letters (only those actually used)
static const unsigned char letter_font[26][5] = {
    {0b000,0b000,0b000,0b000,0b000}, // a
    {0b000,0b000,0b000,0b000,0b000}, // b
    {0b000,0b000,0b000,0b000,0b000}, // c
    {0b000,0b000,0b000,0b000,0b000}, // d
    {0b000,0b000,0b000,0b000,0b000}, // e
    {0b111,0b100,0b110,0b100,0b100}, // f
    {0b000,0b000,0b000,0b000,0b000}, // g
    {0b000,0b000,0b000,0b000,0b000}, // h
    {0b000,0b000,0b000,0b000,0b000}, // i
    {0b000,0b000,0b000,0b000,0b000}, // j
    {0b000,0b000,0b000,0b000,0b000}, // k
    {0b000,0b000,0b000,0b000,0b000}, // l
    {0b000,0b000,0b000,0b000,0b000}, // m
    {0b000,0b000,0b000,0b000,0b000}, // n
    {0b000,0b000,0b000,0b000,0b000}, // o
    {0b111,0b101,0b111,0b100,0b100}, // p
    {0b000,0b000,0b000,0b000,0b000}, // q
    {0b000,0b000,0b000,0b000,0b000}, // r
    {0b011,0b100,0b010,0b001,0b110}, // s
    {0b000,0b000,0b000,0b000,0b000}, // t
    {0b000,0b000,0b000,0b000,0b000}, // u
    {0b000,0b000,0b000,0b000,0b000}, // v
    {0b101,0b101,0b111,0b010,0b010}, // w
    {0b000,0b000,0b000,0b000,0b000}, // x
    {0b000,0b000,0b000,0b000,0b000}, // y
    {0b000,0b000,0b000,0b000,0b000}, // z
};

void draw_char(char c, int x, int y, unsigned char color, int scale) {
    const unsigned char* glyph = 0;
    if (c >= '0' && c <= '9')
        glyph = digit_font[c - '0'];
    else if (c >= 'a' && c <= 'z')
        glyph = letter_font[c - 'a'];
    else if (c >= 'A' && c <= 'Z')
        glyph = letter_font[c - 'A'];
    if (!glyph) return;
    for (int row = 0; row < 5; row++)
        for (int col = 0; col < 3; col++)
            if (glyph[row] & (1 << (2 - col)))
                plot_square(x + col * scale, y + row * scale, scale, color);
}

void draw_text(const char* str, int x, int y, unsigned char color, int scale) {
    int stride = 3 * scale + scale;
    for (int i = 0; str[i]; i++)
        draw_char(str[i], x + i * stride, y, color, scale);
}

// palette: 205=(41,63,44) green, 40=(45,30,10) orange, 50=(23,1,0) red, 1=(4,4,4) dark border
static inline bool vga_safe(int x, int y) {
    return x >= 0 && x < 320 && y >= 0 && y < 200;
}

void draw_hp_bar(int x, int y, int w, int hp, int maxHp) {
    if (maxHp <= 0) return;
    int filled = (hp * w) / maxHp;
    if (filled < 0) filled = 0;
    if (filled > w) filled = w;

    unsigned char fill_color;
    int pct = (hp * 100) / maxHp;
    if (pct <= 10)      fill_color = 50;
    else if (pct <= 30) fill_color = 40;
    else                fill_color = 205;

    for (int col = 0; col < w; col++)
        if (vga_safe(x + col, y - 1))
            video[(y - 1) * 320 + x + col] = 1;
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < w; col++)
            if (vga_safe(x + col, y + row))
                video[(y + row) * 320 + x + col] = (col < filled) ? fill_color : 1;
    }
    for (int col = 0; col < w; col++)
        if (vga_safe(x + col, y + 3))
            video[(y + 3) * 320 + x + col] = 1;
}

// Draw a w×h sprite at (dstX, dstY), skipping color 0, with bounds clipping
void draw_sprite(const unsigned char* sprite,
                 int w, int h,
                 int dstX, int dstY)
{
    int x0 = (dstX < 0) ? -dstX : 0;
    int y0 = (dstY < 0) ? -dstY : 0;
    int x1 = (dstX + w > 320) ? 320 - dstX : w;
    int y1 = (dstY + h > 200) ? 200 - dstY : h;
    for (int yy = y0; yy < y1; ++yy) {
        for (int xx = x0; xx < x1; ++xx) {
            unsigned char c = sprite[yy * w + xx];
            if (c != 0) {
                video[(yy+dstY)*320 + (xx + dstX)] = c;
            }
        }
    }
}

void draw_sprite_scaled(const unsigned char* sprite,
                        int srcW, int srcH,
                        int dstX, int dstY,
                        int dstW, int dstH)
{
    for (int dy = 0; dy < dstH; ++dy) {
        int py = dstY + dy;
        if (py < 0 || py >= 200) continue;
        int sy = dy * srcH / dstH;
        for (int dx = 0; dx < dstW; ++dx) {
            int px = dstX + dx;
            if (px < 0 || px >= 320) continue;
            int sx = dx * srcW / dstW;
            unsigned char c = sprite[sy * srcW + sx];
            if (c != 0) {
                video[py * 320 + px] = c;
            }
        }
    }
}