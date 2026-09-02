#include <stdio.h>
#include <tonc.h>

void note_play(int note, int octave) {
    REG_SND1FREQ = SFREQ_RESET | SND_RATE(note, octave);
}

void init_sound() {
    // turn sound on
    REG_SNDSTAT= SSTAT_ENABLE;
    // snd1 on left/right ; both full volume
    REG_SNDDMGCNT = SDMG_BUILD_LR(SDMG_SQR1, 7);
    // DMG ratio to 100%
    REG_SNDDSCNT= SDS_DMG100;

    // no sweep
    REG_SND1SWEEP= SSW_OFF;
    // envelope: vol=12, decay, max step time (7) ; 50% duty
    REG_SND1CNT= SSQR_ENV_BUILD(12, 0, 7) | SSQR_DUTY1_2;
    REG_SND1FREQ= 0;
}


int frame_in_loop = 0;
const int BEATS_PER_MIN = 220;
const int FRAMES_PER_BEAT = 60 / (BEATS_PER_MIN / 60);

int beat_index = 0;
const u8 melody[6]= { NOTE_F, 0, NOTE_A, NOTE_B, NOTE_F, NOTE_A, NOTE_B };


void loop_music_frame() {

    // runs once per beat
    if(++frame_in_loop == FRAMES_PER_BEAT) {
        frame_in_loop = 0;

        if(melody[beat_index] != 0){
            note_play(melody[beat_index], 0);
        }
        if(++beat_index == 6) {
            beat_index = 0;
        }
        
    }
}