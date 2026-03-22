#pragma once
// ghostmoon UI Catalog: KnobStylesStandard — Standard knob styles #1-31
#include "KnobStyles.h"

namespace gm {
namespace knobs {

// ============================================================
// 1. Dot Ring Knob (Neon/Futuristic)
// No solid body. Thin outer ring with a glowing dot that orbits
// the circumference. Ultra-minimal.
// ============================================================

void drawDotRingKnob(juce::Graphics &g, int x, int y, int width,
                             int height, float sliderPos,
                             float rotaryStartAngle, float rotaryEndAngle,
                             juce::Slider &slider,
                             juce::Colour accent = juce::Colour(0xffff44aa));


// ============================================================
// 2. Bronze Knob (Warm/Luxury)
// Warm metallic gradient body with cream pointer line.
// Subtle bronze arc indicator. Boutique hardware feel.
// ============================================================

void drawBronzeKnob(juce::Graphics &g, int x, int y, int width,
                            int height, float sliderPos,
                            float rotaryStartAngle, float rotaryEndAngle,
                            juce::Slider &slider,
                            juce::Colour accent = juce::Colour(0xffcc9955));



// ============================================================
// 3. LED Ladder Knob
// Dark flat body with segmented LED-style arc indicator.
// Individual lit segments instead of a continuous arc.
// ============================================================

void drawLedLadderKnob(juce::Graphics &g, int x, int y, int width,
                               int height, float sliderPos,
                               float rotaryStartAngle, float rotaryEndAngle,
                               juce::Slider &slider,
                               juce::Colour accent = juce::Colour(0xff00dddd));


// ============================================================
// 4. Glass Knob
// Semi-transparent body with colored backlight glow.
// Pointer line visible through the glass surface.
// ============================================================

void drawGlassKnob(juce::Graphics &g, int x, int y, int width,
                           int height, float sliderPos,
                           float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider &slider,
                           juce::Colour accent = juce::Colour(0xff4488ff));


// ============================================================
// 5. Minimal Knob
// Ultra-flat disc with just a thin pointer line. No arc,
// no glow, no texture. Pure simplicity.
// ============================================================

void drawMinimalKnob(juce::Graphics &g, int x, int y, int width,
                             int height, float sliderPos,
                             float rotaryStartAngle, float rotaryEndAngle,
                             juce::Slider &slider,
                             juce::Colour accent = juce::Colour(0xffcccccc));


// ============================================================
// 6. Outline Knob
// No fill — just a thick circle outline and a tick mark.
// Skeletal, airy, modern.
// ============================================================

void drawOutlineKnob(juce::Graphics &g, int x, int y, int width,
                             int height, float sliderPos,
                             float rotaryStartAngle, float rotaryEndAngle,
                             juce::Slider &slider,
                             juce::Colour accent = juce::Colour(0xff888899));


// ============================================================
// 7. Neon Arc Knob
// Dark flat body with a gradient arc that transitions from
// one neon color to another (teal -> magenta by default).
// ============================================================

void drawNeonArcKnob(juce::Graphics &g, int x, int y, int width,
                              int height, float sliderPos,
                              float rotaryStartAngle, float rotaryEndAngle,
                              juce::Slider &slider,
                              juce::Colour accentStart = juce::Colour(0xff00ddcc),
                              juce::Colour accentEnd = juce::Colour(0xffdd44ff));


// ============================================================
// 8. Knurled Knob
// Tick marks around the edge simulate machined grip texture.
// White dot marker for position indication.
// ============================================================

void drawKnurledKnob(juce::Graphics &g, int x, int y, int width,
                              int height, float sliderPos,
                              float rotaryStartAngle, float rotaryEndAngle,
                              juce::Slider &slider,
                              juce::Colour accent = juce::Colour(0xff00cccc));


// ============================================================
// 9. Ivory Knob
// Light body with copper accents. Inverted palette — stands
// out against dark plugin backgrounds.
// ============================================================

void drawIvoryKnob(juce::Graphics &g, int x, int y, int width,
                           int height, float sliderPos,
                           float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider &slider,
                           juce::Colour accent = juce::Colour(0xffbb7744));


// ============================================================
// 10. Concentric Knob
// Multiple thin concentric rings at different radii.
// Glowing dot orbits the outermost ring.
// ============================================================

void drawConcentricKnob(juce::Graphics &g, int x, int y, int width,
                                int height, float sliderPos,
                                float rotaryStartAngle, float rotaryEndAngle,
                                juce::Slider &slider,
                                juce::Colour accent = juce::Colour(0xff44ddaa));


// ============================================================
// Knobs 11-15: Dark Metallic family extensions
// ============================================================

// 11. Brushed Steel — horizontal brush strokes + recessed groove pointer
void drawBrushedSteelKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent = juce::Colour(0xff66aacc));


// 12. Carbon Fiber — crosshatch texture + orange accent line
void drawCarbonFiberKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent = juce::Colour(0xffee6622));


// 13. Chrome Cap — floating convex chrome cap with deep ring shadow
void drawChromeCapKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent = juce::Colour(0xff00ccee));


// 14. Gunmetal — laser-etched scale marks around perimeter
void drawGunmetalKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent = juce::Colour(0xff55ccaa));


// 15. Rose Gold — polished warm pink-gold + hairline pointer
void drawRoseGoldKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent = juce::Colour(0xffdd8899));


// 16. Walnut — dark wood-like texture + brass center dot
void drawWalnutKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent = juce::Colour(0xffbb8844));


// 17. Ceramic — white porcelain body + gold pointer
void drawCeramicKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent = juce::Colour(0xffccaa44));


// 18. Sapphire — deep blue body + silver pointer
void drawSapphireKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent = juce::Colour(0xff4488dd));


// 19. Wireframe — just outlined circles + electric blue pointer
void drawWireframeKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent = juce::Colour(0xff00aaff));


// 20. Gradient Fill — body fills with a purple-to-teal gradient as value increases
void drawGradientFillKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour fillStart = juce::Colour(0xff6633cc),
    juce::Colour fillEnd = juce::Colour(0xff22ccaa));


// 21. Hex — hexagonal shape + neon edge, hex-following value arc
void drawHexKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent = juce::Colour(0xff44ffaa));


// 22. Mercury — liquid metal reflective dome
void drawMercuryKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent = juce::Colour(0xff88bbcc));


// 23. Circuit — PCB trace aesthetic with right-angle grooves
void drawCircuitKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent = juce::Colour(0xff33cc55));


// 24. Floating Disc — magnetic levitation look with gap ring
void drawFloatingDiscKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent = juce::Colour(0xff7766ee));


// 25. Holographic — rainbow shimmer surface (hue shifts with value)
void drawHolographicKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour /*accent*/ = juce::Colour(0xffcccccc));


// 26. OLED Band — wraparound display strip with colored fill
void drawOledBandKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent = juce::Colour(0xff22ddff));


// 27. Pinstripe — fine radial lines from center + colored dot
void drawPinstripeKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent = juce::Colour(0xffee4444));


// 28. Sunken — recessed body with raised pointer ridge
void drawSunkenKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent = juce::Colour(0xffddaa33));


// 29. Dual Arc — two arcs (inner value, outer accent) at different radii
void drawDualArcKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour inner = juce::Colour(0xff00cccc),
    juce::Colour outer = juce::Colour(0xffcc44ff));


// 30. Target — bullseye rings + crosshair pointer
void drawTargetKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent = juce::Colour(0xffff4444));


// ============================================================
// Knob 31: Radar — radar screen aesthetic
// ============================================================

// 31. Radar — dark scope display with sweeping beam + range rings
void drawRadarKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent = juce::Colour(0xff22cc44));


} // namespace knobs
} // namespace gm