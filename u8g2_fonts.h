/* AGGL font bundle — vendored from U8g2.
 *
 * Provides a curated set of u8g2_font_* byte arrays in BDF format. AGGL's
 * textHandle parses these directly (BDFHeader layout matches U8g2), so callers
 * pass these symbols to AGGL::textHandle just like they would to U8g2's
 * setFont().
 *
 * Unused fonts are stripped at link time via --gc-sections.
 */
#ifndef AGGL_U8G2_FONTS_H
#define AGGL_U8G2_FONTS_H

#include <stdint.h>

/* Original U8g2 places fonts in a dedicated flash section via this macro
 * (for AVR PROGMEM / partition control). On ESP-IDF and other modern
 * targets, const data lands in .rodata (flash) by default, so a no-op
 * keeps the source verbatim from upstream without behavioural change. */
#ifndef U8G2_FONT_SECTION
#define U8G2_FONT_SECTION(name)
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern const uint8_t u8g2_font_6x10_mf[2393];
extern const uint8_t u8g2_font_8x13_mf[3036];
extern const uint8_t u8g2_font_8x13B_mf[3188];
extern const uint8_t u8g2_font_10x20_mf[4194];
extern const uint8_t u8g2_font_open_iconic_all_2x_t[5617];
extern const uint8_t u8g2_font_profont29_mf[7170];
extern const uint8_t u8g2_font_unifont_t_symbols[9015];
extern const uint8_t u8g2_font_helvB08_tr[1028];
extern const uint8_t u8g2_font_helvB10_tr[1333];
extern const uint8_t u8g2_font_helvB12_tr[1509];
extern const uint8_t u8g2_font_helvB14_tr[1697];
extern const uint8_t u8g2_font_helvB18_tr[2252];
extern const uint8_t u8g2_font_helvB24_tr[2965];
extern const uint8_t u8g2_font_helvR08_tr[1041];
extern const uint8_t u8g2_font_helvR10_tr[1289];
extern const uint8_t u8g2_font_helvR12_tr[1465];
extern const uint8_t u8g2_font_helvR14_tr[1698];
extern const uint8_t u8g2_font_helvR18_tr[2240];
extern const uint8_t u8g2_font_helvR24_tr[3160];
extern const uint8_t u8g2_font_inb16_mf[5560];

#ifdef __cplusplus
}
#endif

#endif
