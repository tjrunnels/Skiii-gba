typedef enum {NOTHING, SCORED_POINT, GAME_OVER} FrameResult;

FrameResult process_game_frame(BootReturn bootReturn);
void game_setup(void);