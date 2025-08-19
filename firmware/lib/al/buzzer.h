#ifndef AL_BUZZER_H
#define AL_BUZZER_H

/**
 * Plays a single "click" sound.
 */
void al_buzzer_click();

/**
 * Plays a tone at the specified frequency for the specified duration.
 *
 * @param hz The frequency of the tone to play.
 * @param ms The duration of the tone to play.
 * @param wait If true, waits for the tone to finish before returning.
 */
void al_buzzer_beep(int hz, int ms, bool wait);

#endif  // AL_BUZZER_H
