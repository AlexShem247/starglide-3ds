#ifndef COLORS_H
#define COLORS_H


#define COLOR_BLACK RGB_Color {0, 0, 0, 255}
#define BLACK_TINT RGB_Color {0, 0, 0, 150}
#define COLOR_WHITE RGB_Color {255, 255, 255, 255}
#define COLOR_BLUE RGB_Color {109, 154, 227, 255}

struct RGB_Color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

#endif // COLORS_H
