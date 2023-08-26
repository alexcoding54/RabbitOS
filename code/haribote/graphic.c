/* 关于绘图部分的处理 */

#include "bootpack.h"

void init_palette(void)
{
	static unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,	/*  0:黑 */
		0xff, 0x00, 0x00,	/*  1:梁红 */
		0x00, 0xff, 0x00,	/*  2:亮绿 */
		0xff, 0xff, 0x00,	/*  3:亮黄 */
		0x00, 0x00, 0xff,	/*  4:亮蓝 */
		0xff, 0x00, 0xff,	/*  5:亮紫 */
		0x00, 0xff, 0xff,	/*  6:浅亮蓝 */
		0xff, 0xff, 0xff,	/*  7:白 */
		0xc6, 0xc6, 0xc6,	/*  8:亮灰 */
		0x84, 0x00, 0x00,	/*  9:暗红 */
		0x00, 0x84, 0x00,	/* 10:暗绿 */
		0x84, 0x84, 0x00,	/* 11:暗黄 */
		0x00, 0x00, 0x84,	/* 12:暗青 */
		0x84, 0x00, 0x84,	/* 13:暗紫 */
		0x00, 0x84, 0x84,	/* 14:浅暗蓝 */
		0x84, 0x84, 0x84	/* 15:暗灰 */
	};
	unsigned char table2[216 * 3];
	int r, g, b;
	set_palette(0, 15, table_rgb);
	for (b = 0; b < 6; b++) {
		for (g = 0; g < 6; g++) {
			for (r = 0; r < 6; r++) {
				table2[(r + g * 6 + b * 36) * 3 + 0] = r * 51;
				table2[(r + g * 6 + b * 36) * 3 + 1] = g * 51;
				table2[(r + g * 6 + b * 36) * 3 + 2] = b * 51;
			}
		}
	}
	set_palette(16, 231, table2);
	return;
}

void set_palette(int start, int end, unsigned char *rgb)
{
	int i, eflags;
	eflags = io_load_eflags();	/* 记录中断许可标志的值 */
	io_cli(); 					/* 将中断许可标志置为0,禁止中断 */
	io_out8(0x03c8, start);
	for (i = start; i <= end; i++) {
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);	/* 复原中断许可标志 */
	return;
}

void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1)
{
	int x, y;
	for (y = y0; y <= y1; y++) {
		for (x = x0; x <= x1; x++)
			vram[y * xsize + x] = c;
	}
	return;
}

void init_logo(char *logo)
{
    static char _logo[23][23] = {
        ".......*.......*.......",
        "......*O*.....*O*......",
        ".....*O.O*...*O.O*.....",
        "....*O...O*.*O...O*....",
        "....*O...O***O...O*....",
        "....*O...O*O*O...O*....",
        "....*O...OO.OO...O*....",
        "...*O.............O*...",
        "..*O...............O*..",
        ".*O...QQ.......QQ...O*.",
        "*O...QQQQ.....QQQQ...O*",
        "*O....QQ.......QQ....O*",
        "*O...................O*",
        "*O...................O*",
        "*O...................O*",
        "*O...O...........O...O*",
        ".*O...O.........O...O*.",
        "..*O...O.......O...O*..",
        "...*O...OOOOOOO...O*...",
        "....*O...........O*....",
        ".....*O.........O*.....",
        "......*OOOOOOOOO*......",
        ".......*********......."
    };
    
    int x, y;
    for (y=0; y<23; y++) {
        for (x=0; x<23; x++) {
            if (_logo[y][x] == '.')
                logo[y*23+x] = COL8_C6C6C6;
            if (_logo[y][x] == '*')
                logo[y*23+x] = COL8_FFFFFF;
            if (_logo[y][x] == 'O')
                logo[y*23+x] = COL8_000000;
            if (_logo[y][x] == 'Q')
                logo[y*23+x] = COL8_FF0000;
        }
    }
    return;
}

void init_screen8(char *vram, int x, int y)
{
    int *fat;
    unsigned char c;
    struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;

	boxfill8(vram, x, COL8_C6C6C6,  0,     0, x, 28);
    boxfill8(vram, x, COL8_848484, 0, 28, x, 29);

    fat = (int *) memman_alloc_4k(memman, 4 * 2880);
    file_readfat(fat, (unsigned char *) (ADR_DISKIMG + 0x000200));
    read_picture(fat, vram, x, y);
    memman_free_4k(memman, (int) fat, 4 * 2880);

    return;
}

/*void init_screen8(char *vram, int x, int y)
{
    int *fat;
    unsigned char c;
    struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
    
	boxfill8(vram, x, COL8_008484,  0,     0, x, y );
	boxfill8(vram, x, COL8_C6C6C6,  0,     0, x, 28);
    boxfill8(vram, x, COL8_848484, 0, 28, x, 29);
    


	return;
}*/

void putfont8(char *vram, int xsize, int x, int y, char c, char *font)
{
	int i;
	char *p, d /* data */;
	for (i = 0; i < 16; i++) {
		p = vram + (y + i) * xsize + x;
		d = font[i];
		if ((d & 0x80) != 0) { p[0] = c; }
		if ((d & 0x40) != 0) { p[1] = c; }
		if ((d & 0x20) != 0) { p[2] = c; }
		if ((d & 0x10) != 0) { p[3] = c; }
		if ((d & 0x08) != 0) { p[4] = c; }
		if ((d & 0x04) != 0) { p[5] = c; }
		if ((d & 0x02) != 0) { p[6] = c; }
		if ((d & 0x01) != 0) { p[7] = c; }
	}
	return;
}

void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s)
{
	extern char hankaku[4096];
	/* C语言中，字符串都是以0x00结尾 */
	for (; *s != 0x00; s++) {
		putfont8(vram, xsize, x, y, c, hankaku + *s * 16);
		x += 8;
	}
	return;
}

void init_mouse_cursor8(char *mouse, char bc)
/* 鼠标的数据准备（16x16） */
{
	static char cursor[16][16] = {
		"**************..",
		"*OOOOOOOOOOO*...",
		"*OOOOOOOOOO*....",
		"*OOOOOOOOO*.....",
		"*OOOOOOOO*......",
		"*OOOOOOO*.......",
		"*OOOOOOO*.......",
		"*OOOOOOOO*......",
		"*OOOO**OOO*.....",
		"*OOO*..*OOO*....",
		"*OO*....*OOO*...",
		"*O*......*OOO*..",
		"**........*OOO*.",
		"*..........*OOO*",
		"............*OO*",
		".............***"
	};
	int x, y;

	for (y = 0; y < 16; y++) {
		for (x = 0; x < 16; x++) {
			if (cursor[y][x] == '*') {
				mouse[y * 16 + x] = COL8_FFFFFF;
			}
			if (cursor[y][x] == 'O') {
				mouse[y * 16 + x] = COL8_000000;
			}
			if (cursor[y][x] == '.') {
				mouse[y * 16 + x] = bc;
			}
		}
	}
	return;
}

void putblock8_8(char *vram, int vxsize, int pxsize,
	int pysize, int px0, int py0, char *buf, int bxsize)
{
	int x, y;
	for (y = 0; y < pysize; y++) {
		for (x = 0; x < pxsize; x++) {
			vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
		}
	}
	return;
}


int read_picture(int *fat, short *vram, int x, int y)
{
    int i, j, x0, y0, fsize, info[4];
    unsigned char *filebuf, r, g, b;
    struct RGB *picbuf;
    struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
    struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
    struct FILEINFO *finfo;
    struct DLL_STRPICENV *env;
    finfo = file_search("desktop1.jpg", (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
    if (finfo == 0) {
        return -1;
    }
    fsize = finfo->size;
    filebuf = (unsigned char *) memman_alloc_4k(memman, fsize);
    filebuf = file_loadfile2(finfo->clustno, &fsize, fat);
    env = (struct DLL_STRPICENV *) memman_alloc_4k(memman, sizeof(struct DLL_STRPICENV));
    info_JPEG(env, info, fsize, filebuf);
    picbuf = (struct RGB *) memman_alloc_4k(memman, info[2] * info[3] * sizeof(struct RGB));
    decode0_JPEG(env, fsize, filebuf, 4, (unsigned char *) picbuf, 0);
    x0 = (int) ((x - info[2]) / 2);
    y0 = (int) ((y - info[3]) / 2);
        for (i = 0; i < info[3]; i++) {
        for (j = 0; j < info[2]; j++) {
            r = picbuf[i * info[2] + j].r;
            g = picbuf[i * info[2] + j].g;
            b = picbuf[i * info[2] + j].b;
            vram[(y0 + i) * x + (x0 + j)] = rgb2pal(r, g, b, j, i, binfo->vmode);
        }
    }
    memman_free_4k(memman, (int) filebuf, fsize);
    memman_free_4k(memman, (int) picbuf , info[2] * info[3] * sizeof(struct RGB));
    memman_free_4k(memman, (int) env , sizeof(struct DLL_STRPICENV));
    return 0;
}
unsigned short rgb2pal(int r, int g, int b, int x, int y, int cb)
{
    if (cb == 8) {
        static int table[4] = { 3, 1, 0, 2 };
        int i;
        x &= 1; /* 是偶数还是奇数呢？ */
        y &= 1;
        i = table[x + y * 2];
        r = (r * 21) / 256;
        g = (g * 21) / 256;
        b = (b * 21) / 256;
        r = (r + i) / 4;
        g = (g + i) / 4;
        b = (b + i) / 4;
        return((unsigned short) (16 + r + g * 6 + b * 36));
    } else {
        return((unsigned short) (((r << 8) & 0xf800) | ((g << 3) & 0x07e0) | (b >> 3)));
    }
}