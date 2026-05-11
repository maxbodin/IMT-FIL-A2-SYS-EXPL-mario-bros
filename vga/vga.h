#ifndef VGA_H
#define VGA_H
extern volatile unsigned char *video;

void set_vga_mode13(void);
void clear_vga_screen(char color);
void plot_square(int x, int y, int size, unsigned char color);

void set_palette_vga(unsigned char palette_vga[256][3]);
void draw_sprite(const unsigned char* sprite, int w, int h, int dstX, int dstY);
void draw_sprite_scaled(const unsigned char* sprite, int srcW, int srcH, int dstX, int dstY, int dstW, int dstH);
void draw_number(int n, int x, int y, unsigned char color, int scale);
void draw_hp_bar(int x, int y, int w, int hp, int maxHp);
#endif