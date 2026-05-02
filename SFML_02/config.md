### Window W H FL FS
- width -> int
- height -> int
- Frame Limit -> int
- FullScreen -> int (1: fullscreen - 2: windowed)

---

### Font F S R G B
- Font File -> std::string (no spaces)
- Font Size -> int 
- RGB -> int, int, int

---

### Player SR CR S FR FG FB OR OG OB OT V
- Shape Radius -> int
- Collision Radius -> int
- Speed -> float
- Fill Color -> FR,FG,FB -> int, int, int
- Outline Color -> OR,OG,OB -> int, int, int
- Outline Thickness -> OT -> int
- Shape Vertices -> V -> int

---

### Enemy SR CR SMIN SMAX OR OG OB OT VMIN VMAX L SI
- Shape Radius -> int
- Collision Radius -> int
- Min / Max Speed -> SMIN, SMAX -> float, float
- Outline Color -> OR,OG,OB -> int, int, int
- Outline Thickness -> OT -> int
- Min/Max Vertices -> VMIN, VMAX -> int, int
- Lifespan -> L -> int
- Spawn Interval -> SP -> int

---

### Bullet SR CR S FR FG FB OR OG OB OT V L
- Shape Radius -> int
- Collision Radius -> int
- Speed -> float
- Fill Color -> FR, FG, FB -> int, int, int
- Outline Color -> OR, OG, OB -> int, int, int
- Outline Thickness -> OT -> int
- Shape Vertices -> V -> int
- LifeSpan -> L -> int



