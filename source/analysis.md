I analyzed `skiitheme.wav` from the waveform, spectrogram, onset grid, and per-band pitch tracking. It is a **mono chiptune-style** loop (pulse/square + noise), not three literal hardware files — but the mix clearly has the three layers you described.

**File:** Matthew Mellusi, dated 2016-12-30 · 33.23s · 44.1 kHz · mono

---

## Tempo, meter, key

| | |
|---|---|
| **BPM** | **~157** (measured **156.6**; half-time feel **~78**) |
| **Meter** | 4/4 |
| **Quarter note** | ~0.383 s |
| **Key** | **C minor** (center of gravity C / Bb), later a **G** turnaround |
| **Length** | ~22 bars (bar 1 is pickup/intro) |
| **Downbeat 1** | ~0.00s · **bar 2 / first pitched downbeat** ~**1.52s** |

Intro is almost exactly **1 bar** of percussion (4 beats × 0.383s ≈ 1.53s), then the pitched channels enter. That lock is why 157 BPM is the right grid.

---

## Form (when each channel enters)

```
Bar 1        0.00–1.52s   PERCUSSION only (noise hits)
Bars 2–5     1.52–7.65s   + BASS / RHYTHM ostinato
Bars 6–16    7.65–24.5s   + MELODY
Bars 17–20   24.5–30.6s   G-major walk-up / climax
Bars 21–22   30.6–33.2s   bass drops; melody rings out
```

That matches what you heard: percussion → rhythmic layer → melody.

---

## Channel 1 — Percussion (noise)

Timbre: **broadband noise**, energy mostly **2–6 kHz**. Classic Game Boy / NES *noise channel* hats, not a sampled acoustic kit. No pitched snare; the “kick” feeling later is mostly the **bass note attacks**.

**Bar 1** (solo): four **quarter-note** noise hits on beats 1–2–3–4  
(~0.04, 0.42, 0.81, 1.18s)

**From bar 2 on:** busy 8th/16th noise bed with **accents on the quarter notes**. Rough 16th pattern (X = accent, x = lighter, . = rest):

```
Bar 1:  X... X... X... X...     (quarters only)
Bar 2:  Xxxx Xxxx Xxxx XxxX
Bar 3+: XxxX XxxX XxxX XxxX     (accents on beat + the "a" of each beat)
```

Think: closed-hat 16ths with a four-on-the-floor accent. It runs under the whole tune until the last ~2 seconds, when everything thins out.

---

## Channel 2 — Rhythm / bass ostinato (enters bar 2)

This is the “rhythmic” part. Two things locked together:

1. **Bass fundamentals** (octave-jumping pulse wave, ~58–132 Hz)
2. **Mid chord tones** (~F3–C4) stabbing the same harmony

### Harmony loop (bars 2–17)

A **2-beat C, 2-beat Bb** vamp:

```
||: C  | C  | Bb | Bb :||
```

### Bass line (octave written as C2 / C3)

C2 = 65 Hz, C3 = 131 Hz, Bb1 = 58 Hz, Bb2 = 117 Hz.

Core cell (repeats with small variations):

```
Bar  2:  C3        C3        Bb2       Bb1
Bar  3:  C2        C3        Bb2       Bb2
Bar  4:  C3        C3        Bb2       Bb2
Bar  5:  C3        C3        Bb1       Bb1
   ...same idea through bar 17...
```

It often **punches the low octave on the downbeat** (C2 or Bb1) then answers on C3/Bb2. Very typical 8-bit bass writing.

### Rhythm/harmony notes on top of that bass

On **C bars:** C3, C4, G5-ish chord tone  
On **Bb bars:** Bb3, F3, F4  

So the mid voice is basically:

```
C bars:  C3 / C4  eighth-ish pulse
Bb bars: F3 / Bb3 / F4  eighth-ish pulse
```

Not a walking inner melody — an **ostinato pump** outlining **Cm** (C–Eb–G, though Eb is weaker in the mix) and **Bb** (Bb–D–F).

---

## Channel 3 — Melody (enters bar 6, ~7.65s)

Higher pulse wave. Long held notes, mostly **C5–E6**. Contour from the spectrogram + CQT (nearest chromatic pitch):

**Phrase A — first hook (bars 6–10)**

```
Bar 6:  E6  (whole)
Bar 7:  E6  (whole)          ← the long whistle-y note you can see ~1318 Hz
Bar 8:  E6  →  C6
Bar 9:  C6  (whole)
Bar 10: C6  then drops toward C5 / Bb4
```

**Phrase A′ (bars 12–16)** — same shape an octave mix of C5 + E6:

```
Bar 12: C5  (with E6 coming back in)
Bar 13: E6  held over the C/Bb vamp
Bar 14: E5 → F5 → G5          (climb)
Bar 15: G5 + C6  (stacked)
Bar 16: C5 / C6
```

**Phrase B — G turnaround (bars 17–20)**

Bass leaves the C–Bb vamp and walks **G–B–C–D** (G major). Melody sits on G-major chord tones:

```
Bar 17 beat 4:  bass → G2     melody → G5
Bar 18:  G2  | B2  | C3  | G2     melody G5 / B4 / D5
Bar 19:  G2  | G2  | C3–D3 | G2   melody B4 / D5
Bar 20:  G2  | B2  | C3  | D3     melody D5 / G5  (peak)
```

**Tag (bars 21–22)**  
Bass energy collapses. Melody rings **G5 + C6** (and a hint of C7 harmonic) to the end. Feels like an open Cm/G freeze, not a hard cadence.

Simplified hook in scale degrees (C minor):

```
5  (E6)   held
3  (C6)   held
then later  5–6–1  (E–F–G) into the G-major lift
```

---

## Bar-by-bar lead sheet (compressed)

`B` = bass · `R` = rhythm chord · `M` = melody · `P` = perc

```
Bar 1   P: X   X   X   X          (noise quarters)
        B/R/M: tacet

Bar 2   B: C3      C3      Bb2     Bb1
        R: C               Bb
        M: (thin C5/Bb4 pad only)

Bar 3   B: C2      C3      Bb2     Bb2
        R: C               Bb

Bar 4   B: C3      C3      Bb2     Bb2
        R: C               Bb

Bar 5   B: C3      C3      Bb1     Bb1
        R: C               Bb

Bar 6   B: C3      C3      Bb2     Bb2
        M: E6 ----------------------------

Bar 7   B: C3      C3      Bb2     Bb2
        M: E6 ----------------------------

Bar 8   B: C3      C3      Bb2     C3
        M: E6 -------- C6 --------

Bar 9   B: C3      Bb1     Bb1     C3
        M: C6 ----------------------------

Bar 10  B: C3      Bb2     Bb1     C2
        M: C6 ---- (resolves down)

Bars 11–13  same C|C|Bb|Bb vamp; melody returns to E6 / C5

Bar 14  B: C3 | Bb2 | Bb1 | C2
        M: E5    F5    G5     (ascent)

Bar 15  B: C3 | Bb2 | Bb2 | C2
        M: G5 + C6  held

Bar 16  B: C2 | Bb  | Bb2 | C3
        M: C5 / C6

Bar 17  B: C2 | Bb1 | Bb  | G2     ← pivot
        M: C5 / G5

Bar 18  B: G2 | B2 | C3 | G2
        M: G5 / B4 / D5

Bar 19  B: G2 | G2 | C3–D3 | G2
        M: B4 / D5

Bar 20  B: G2 | B2 | C3 | D3
        M: D5 / G5

Bar 21–22
        B: fades
        M: G5 + C6  ring-out
        P: thins
```

---

## What the three channels *are*, musically

| Channel | Register | Role | Timbre |
|---|---|---|---|
| 1 Percussion | 2–6 kHz noise | Timekeeper | LFSR / chip noise |
| 2 Rhythm + bass | Bb1–C4 | 2-bar **C–C–Bb–Bb** ostinato | Pulse/square, strong odd harmonics |
| 3 Melody | C5–G6 | Long held notes, then G-major climb | Brighter pulse |

Because it is **one rendered mono mix** of bright pulse waves, harmonics stack (C2 produces energy at C3, G3, C4…). Automatic note-picking therefore sometimes reports the octave above the real fundamental. The bass fundamentals and the long melody notes (E6, C6, G5) are the reliable ones.

---

## Other useful details

- **Not 3 literal WAV channels** — one mixed mono file. Isolation above is by frequency + onset, not by stems.
- **Sound:** 8-bit / early-game pulse + noise. That matches a 2016 “old game song.”
- **Loop point:** it does not cleanly loop in this 33s bounce; it **intros, vamps, then cadences and rings out**.
- If you originally wrote this in a tracker (Famitracker, LSDJ, OpenMPT, etc.), the grid you want is **156–157 BPM, 4/4, 1 bar perc count-in**.

If you want, I can turn this into a MIDI file on that 156.6 grid (bass + a simplified melody + noise-as-hihat) so you can drop it into a DAW and correct the inner rhythm notes by ear.