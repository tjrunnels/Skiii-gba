#include "sound.h"
#include <tonc.h>

/*
 * Skiitheme as GBA PSG, transcribed from the WAV.
 * 159 BPM (file tempo tag). Square 1 = melody, square 2 = bass,
 * wave = mid ostinato, noise = 8th-note snare with the bass.
 * Loop from bar 2 after the G ring.
 */

#define REST 0
#define PK(n, o) ((u8)(1 + (((int)(o) + 2) * 12) + (n)))

#define C2  PK(NOTE_C,  -2)
#define G2  PK(NOTE_G,  -2)
#define B2  PK(NOTE_B,  -2)
#define Bb2 PK(NOTE_BES, -2)
#define C3  PK(NOTE_C,  -1)
#define D3  PK(NOTE_D,  -1)
#define G3  PK(NOTE_G,  -1)
#define B3  PK(NOTE_B,  -1)
#define Bb3 PK(NOTE_BES, -1)
#define C4  PK(NOTE_C,   0)
#define D4  PK(NOTE_D,   0)
#define F4  PK(NOTE_F,   0)
#define G4  PK(NOTE_G,   0)
#define G5  PK(NOTE_G,   1)
#define Bb5 PK(NOTE_BES,  1)
#define B5  PK(NOTE_B,   1)
#define C6  PK(NOTE_C,   2)
#define D6  PK(NOTE_D,   2)
#define E6  PK(NOTE_E,   2)
#define F6  PK(NOTE_F,   2)
#define G6  PK(NOTE_G,   2)

#define ACC 2

#define BPM 159
#define SIXTEENTHS_PER_MIN (BPM * 4)
#define FRAMES_PER_MIN 3600
#define STEPS_PER_BAR 16
#define SONG_BARS 21
#define SONG_STEPS (SONG_BARS * STEPS_PER_BAR)
#define LOOP_STEP STEPS_PER_BAR

#define SWAV_VOL50 0x4000
#define B_C  {C3, 2}, {C3, 2}, {C3, 2}, {C3, 2}
#define B_Cl {C2, 2}, {C3, 2}, {C3, 2}, {C3, 2}
#define B_Bb {Bb2, 2}, {Bb2, 2}, {Bb2, 2}, {Bb2, 2}
#define W_C  {G3, 2}, {C4, 2}, {G3, 2}, {C4, 2}
#define W_Bb {Bb3, 2}, {F4, 2}, {Bb3, 2}, {F4, 2}

typedef struct {
	u8 pitch;
	u8 dur;
} Ev;

typedef struct {
	const Ev *ev;
	u16 n;
	u16 i;
	u8 left;
	u8 pitch;
} Chan;

static Chan ch_bass, ch_wave, ch_mel;
static u16 tick_accum;
static u16 song_step;
static u16 melody_rate;
static u8 melody_on;
static u8 melody_vib;

static void unpack(u8 pitch, int *note, int *oct)
{
	u8 p = (u8)(pitch - 1);
	*note = p % 12;
	*oct = (int)(p / 12) - 2;
}

static void play_sqr(int ch, u8 pitch, u8 vol, u16 duty, u8 env_time)
{
	u16 cnt;
	u16 freq;

	if (pitch == REST) {
		cnt = SSQR_ENV_BUILD(0, 0, 0) | duty;
		freq = SFREQ_RESET;
	} else {
		int note, oct;
		unpack(pitch, &note, &oct);
		/* Hardware bit 11: 0 = decrease. TONC SSQR_DEC is inverted. */
		cnt = SSQR_ENV_BUILD(vol, 0, env_time) | duty;
		freq = SFREQ_RESET | SND_RATE(note, oct);
	}

	if (ch == 1) {
		REG_SND1CNT = cnt;
		REG_SND1FREQ = freq;
	} else {
		REG_SND2CNT = cnt;
		REG_SND2FREQ = freq;
	}
}

static void play_bass(u8 pitch)
{
	play_sqr(2, pitch, 12, SSQR_DUTY1_8, 2);
}

static void play_melody(u8 pitch)
{
	if (pitch == REST) {
		melody_on = 0;
		play_sqr(1, REST, 0, SSQR_DUTY1_4, 0);
		return;
	}

	int note, oct;
	unpack(pitch, &note, &oct);
	melody_rate = SND_RATE(note, oct);
	melody_on = 1;
	melody_vib = 0;
	REG_SND1CNT = SSQR_ENV_BUILD(12, 0, 0) | SSQR_DUTY1_4;
	REG_SND1FREQ = SFREQ_RESET | melody_rate;
}

static void play_wave(u8 pitch)
{
	if (pitch == REST) {
		REG_SND3CNT = 0;
		REG_SND3FREQ = SFREQ_RESET;
		return;
	}

	int note, oct;
	unpack(pitch, &note, &oct);
	REG_SND3CNT = SWAV_VOL50;
	REG_SND3FREQ = SFREQ_RESET | SND_RATE(note, oct);
}

static void play_noise(u8 kind)
{
	if (kind == ACC) {
		REG_SND4CNT = SSQR_ENV_BUILD(1, 0, 1);
		REG_SND4FREQ = SFREQ_RESET | (1 << 4) | 1;
	}
}

static void chan_bind(Chan *c, const Ev *ev, u16 n)
{
	c->ev = ev;
	c->n = n;
	c->i = 0;
	c->left = 0;
	c->pitch = REST;
}

static void chan_tick(Chan *c, void (*trig)(u8), int play)
{
	if (c->left == 0) {
		if (c->i >= c->n)
			return;
		c->pitch = c->ev[c->i].pitch;
		c->left = c->ev[c->i].dur;
		c->i++;
		if (play)
			trig(c->pitch);
	}
	if (c->left)
		c->left--;
}

/* ---- song data: one 16th = one duration unit, 16 per bar ---- */

static const Ev bass_ev[] = {
	/*  1 */ {REST, 16},
	/*  2 */ B_C, B_Bb,
	/*  3 */ B_Cl, B_Bb,
	/*  4 */ B_C, B_Bb,
	/*  5 */ B_Cl, B_Bb,
	/*  6 */ B_C, B_Bb,
	/*  7 */ B_Cl, B_Bb,
	/*  8 */ B_C, B_Bb,
	/*  9 */ B_Cl, B_Bb,
	/* 10 */ B_C, B_Bb,
	/* 11 */ B_Cl, B_Bb,
	/* 12 */ B_C, B_Bb,
	/* 13 */ B_Cl, B_Bb,
	/* 14 */ B_C, B_Bb,
	/* 15 */ B_Cl, B_Bb,
	/* 16 */ B_C, B_Bb,
	/* 17 */ B_Cl, B_Bb,
	/* 18 */ {G2, 4}, {G2, 4}, {B2, 2}, {C3, 2}, {D3, 4},
	/* 19 */ {G2, 8}, {B2, 2}, {C3, 2}, {D3, 4},
	/* 20 */ {G2, 4}, {B2, 4}, {C3, 4}, {D3, 4},
	/* 21 */ {G2, 8}, {G2, 8},
};

static const Ev wave_ev[] = {
	/*  1 */ {REST, 16},
	/*  2 */ W_C, W_Bb,
	/*  3 */ W_C, W_Bb,
	/*  4 */ W_C, W_Bb,
	/*  5 */ W_C, W_Bb,
	/*  6 */ W_C, W_Bb,
	/*  7 */ W_C, W_Bb,
	/*  8 */ W_C, W_Bb,
	/*  9 */ W_C, W_Bb,
	/* 10 */ W_C, W_Bb,
	/* 11 */ W_C, W_Bb,
	/* 12 */ W_C, W_Bb,
	/* 13 */ W_C, W_Bb,
	/* 14 */ W_C, W_Bb,
	/* 15 */ {G5, 16},
	/* 16 */ W_C, W_Bb,
	/* 17 */ W_C, W_Bb,
	/* 18 */ {G4, 4}, {G4, 4}, {B3, 2}, {C4, 2}, {D4, 4},
	/* 19 */ {G4, 8}, {B3, 2}, {C4, 2}, {D4, 4},
	/* 20 */ {G4, 4}, {B3, 4}, {C4, 4}, {D4, 4},
	/* 21 */ {C6, 16},
};

static const Ev mel_ev[] = {
	/* 1-5 */ {REST, 80},
	/*  6 */ {G5, 3}, {E6, 2}, {REST, 1}, {E6, 6}, {G5, 4},
	/*  7 */ {E6, 11}, {REST, 1}, {E6, 2}, {F6, 2},
	/*  8 */ {G6, 2}, {E6, 4}, {C6, 5}, {REST, 1}, {Bb5, 4},
	/*  9 */ {C6, 16},
	/* 10 */ {C6, 4}, {REST, 12},
	/* 11 */ {REST, 16},
	/* 12 */ {G5, 2}, {E6, 2}, {C6, 2}, {E6, 6}, {G5, 4},
	/* 13 */ {E6, 10}, {REST, 2}, {E6, 2}, {F6, 2},
	/* 14 */ {E6, 6}, {C6, 5}, {Bb5, 3}, {G5, 2},
	/* 15 */ {G5, 1}, {C6, 1}, {G5, 3}, {C6, 11},
	/* 16 */ {REST, 16},
	/* 17 */ {REST, 16},
	/* 18 */ {G5, 16},
	/* 19 */ {G5, 4}, {B5, 4}, {D6, 8},
	/* 20 */ {D6, 8}, {G5, 8},
	/* 21 */ {G5, 8}, {C6, 8},
};

/* Quarter-note snares only, locked to the bass downbeat. */
static const u8 pat_floor[16] = {
	ACC, 0, 0, 0, ACC, 0, 0, 0, ACC, 0, 0, 0, ACC, 0, 0, 0
};

static void tick_perc(u16 step)
{
	u8 st = (u8)(step & (STEPS_PER_BAR - 1));
	u8 kind = pat_floor[st];

	if (kind)
		play_noise(kind);
}

static void load_wave_ram(void)
{
	REG_SND3SEL = 0;
	REG_WAVE_RAM0 = 0xFFFFFFFF;
	REG_WAVE_RAM1 = 0xFFFFFFFF;
	REG_WAVE_RAM2 = 0x00000000;
	REG_WAVE_RAM3 = 0x00000000;
	REG_SND3SEL = 0x80;
}

static void music_rewind(u16 to_step)
{
	u16 s;

	chan_bind(&ch_bass, bass_ev, sizeof(bass_ev) / sizeof(bass_ev[0]));
	chan_bind(&ch_wave, wave_ev, sizeof(wave_ev) / sizeof(wave_ev[0]));
	chan_bind(&ch_mel, mel_ev, sizeof(mel_ev) / sizeof(mel_ev[0]));

	for (s = 0; s < to_step; s++) {
		chan_tick(&ch_bass, play_bass, 0);
		chan_tick(&ch_wave, play_wave, 0);
		chan_tick(&ch_mel, play_melody, 0);
	}

	play_bass(ch_bass.pitch);
	play_wave(ch_wave.pitch);
	play_melody(ch_mel.pitch);
	song_step = to_step;
}

static void tick_sixteenth(void)
{
	if (song_step >= SONG_STEPS)
		music_rewind(LOOP_STEP);

	tick_perc(song_step);
	chan_tick(&ch_bass, play_bass, 1);
	chan_tick(&ch_wave, play_wave, 1);
	chan_tick(&ch_mel, play_melody, 1);
	song_step++;
}

static void tick_vibrato(void)
{
	static const s8 vib[8] = {0, 1, 2, 1, 0, -1, -2, -1};

	if (!melody_on)
		return;
	melody_vib++;
	REG_SND1FREQ = (u16)(melody_rate + vib[(melody_vib >> 2) & 7]);
}

void init_sound(void)
{
	REG_SNDSTAT = SSTAT_ENABLE;
	REG_SNDDMGCNT = SDMG_BUILD_LR(
		SDMG_SQR1 | SDMG_SQR2 | SDMG_WAVE | SDMG_NOISE, 7);
	REG_SNDDSCNT = SDS_DMG100;

	REG_SND1SWEEP = SSW_OFF;
	REG_SND1CNT = 0;
	REG_SND1FREQ = 0;
	REG_SND2CNT = 0;
	REG_SND2FREQ = 0;
	REG_SND4CNT = 0;
	REG_SND4FREQ = 0;

	load_wave_ram();
	REG_SND3CNT = 0;
	REG_SND3FREQ = 0;

	tick_accum = 0;
	melody_on = 0;
	melody_vib = 0;
	melody_rate = 0;
	music_rewind(0);
}

void loop_music_frame(void)
{
	tick_vibrato();

	tick_accum += SIXTEENTHS_PER_MIN;
	if (tick_accum < FRAMES_PER_MIN)
		return;
	tick_accum -= FRAMES_PER_MIN;
	tick_sixteenth();
}
