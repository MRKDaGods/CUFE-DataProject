#pragma once

namespace ui {
	enum Color {
		COLOR_FG_BLACK = 0x0000,
		COLOR_FG_DARK_BLUE = 0x0001,
		COLOR_FG_DARK_GREEN = 0x0002,
		COLOR_FG_DARK_CYAN = 0x0003,
		COLOR_FG_DARK_RED = 0x0004,
		COLOR_FG_DARK_MAGENTA = 0x0005,
		COLOR_FG_DARK_YELLOW = 0x0006,
		COLOR_FG_GREY = 0x0007,
		COLOR_FG_DARK_GREY = 0x0008,
		COLOR_FG_BLUE = 0x0009,
		COLOR_FG_GREEN = 0x000A,
		COLOR_FG_CYAN = 0x000B,
		COLOR_FG_RED = 0x000C,
		COLOR_FG_MAGENTA = 0x000D,
		COLOR_FG_YELLOW = 0x000E,
		COLOR_FG_WHITE = 0x000F,
		COLOR_BG_BLACK = 0x0000,
		COLOR_BG_DARK_BLUE = 0x0010,
		COLOR_BG_DARK_GREEN = 0x0020,
		COLOR_BG_DARK_CYAN = 0x0030,
		COLOR_BG_DARK_RED = 0x0040,
		COLOR_BG_DARK_MAGENTA = 0x0050,
		COLOR_BG_DARK_YELLOW = 0x0060,
		COLOR_BG_GREY = 0x0070,
		COLOR_BG_DARK_GREY = 0x0080,
		COLOR_BG_BLUE = 0x0090,
		COLOR_BG_GREEN = 0x00A0,
		COLOR_BG_CYAN = 0x00B0,
		COLOR_BG_RED = 0x00C0,
		COLOR_BG_MAGENTA = 0x00D0,
		COLOR_BG_YELLOW = 0x00E0,
		COLOR_BG_WHITE = 0x00F0
	};

	/// <summary>
	/// Bitwise or operator
	/// </summary>
	inline Color operator|(Color left, Color right) {
		return (Color)((int)left | (int)right);
	}

	inline Color operator^(Color left, int right) {
		return (Color)((int)left ^ right);
	}
}