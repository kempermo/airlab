#include <esp_random.h>

#include "stm.h"
#include "sns.h"

stm_entry_t stm_entries[] = {
    /* Exclaims */
    {
        .text = "Willkommen im Air Lab! Sieh dich doch einmal um. Berühren ist erlaubt!",
        .mood = STM_HAPPY,
        .exclaim = true,
        .action = STM_FROM_INTRO,
    },
    {
        .text = "Schmuuhhh, mir wird ganz schwindelig...",
        .mood = STM_COLD,
        .exclaim = true,
        .co2_min = 4000,
    },
    {
        .text = "Mir ist übel!",
        .mood = STM_ANGRY1,
        .exclaim = true,
        .co2_min = 3000,
    },
    {
        .text = "Ich kann hier kaum atmen!",
        .mood = STM_ANGRY2,
        .exclaim = true,
        .co2_min = 2000,
        .co2_max = 3000,
    },
    {
        .text = "Uuuu... ich bin müde.",
        .mood = STM_STANDING,
        .exclaim = true,
        .co2_min = 1700,
        .co2_max = 2000,
    },
    {
        .text = "Jo, ich kann mich voll nicht konzentrieren.",
        .mood = STM_ANGRY2,
        .exclaim = true,
        .co2_min = 1400,
        .co2_max = 1700,
    },
    {
        .text = "Boa ey, ist das kalt hier!",
        .mood = STM_COLD,
        .exclaim = true,
        .tmp_max = 10,
    },
    {
        .text = "Sag mal, sind wir im Regenwald?",
        .mood = STM_ANGRY1,
        .exclaim = true,
        .tmp_min = 25,
        .hum_min = 70,
    },
    {
        .text = "Ahem, mega trocken hier!",
        .mood = STM_COLD,
        .exclaim = true,
        .hum_max = 30,
    },
    {
        .text = "Cool, du hast deine Messung abgeschlossen!",
        .mood = STM_HAPPY,
        .exclaim = true,
        .action = STM_COMP_MEASUREMENT,
    },
    {
        .text = "Die Analyse ist der Anfang der Erkenntnis.",
        .mood = STM_POINTING,
        .exclaim = true,
        .action = STM_FROM_ANALYSIS,
    },
    {
        .text = "Super, du hast gerade deine erste Messung gestartet!",
        .mood = STM_POINTING,
        .exclaim = true,
        .action = STM_START_FIRST_MEASUREMENT,
    },
    /* Fun Facts */
    {
        .text = "Ahhh... Ich liebe frische Luft!",
        .mood = STM_HAPPY,
        .co2_max = 600,
    },
    {
        .text = "Die Luft hier ist jetzt richtig nice!",
        .mood = STM_HAPPY,
        .co2_max = 600,
        .hum_min = 40,
        .hum_max = 60,
    },
    {
        .text = "Warme Luft kann mehr Feuchtigkeit auf- nehmen als kalte Luft.",
        .mood = STM_POINTING,
    },
    {
        .text = "Pro Tag atmen wir 10'000 bis 20'000 Liter Luft.",
        .mood = STM_POINTING,
    },
    {
        .text = "Die Luft ist ein Gemisch aus vielen verschiedenen Gasen.",
        .mood = STM_POINTING,
    },
    {
        .text = "78% der Luft ist Stickstoff.",
        .mood = STM_POINTING,
    },
    {
        .text = "21% der Luft ist Sauerstoff.",
        .mood = STM_POINTING,
    },
    {
        .text = "Achtung! Luftschad- stoffe können dich krank machen!",
        .mood = STM_POINTING,
    },
    {
        .text = "1% der Luft sind ganz viele verschiedene Spurengase.",
        .mood = STM_POINTING,
    },
    {
        .text = "Klug ist jener, der Schweres einfach sagt.",
        .mood = STM_POINTING,
    },
    {
        .text = "Auch Bakterien und Viren fliegen in der Luft herum!",
        .mood = STM_POINTING,
    },
    {
        .text = "Pro Tag atmest du ca. 2'500 Liter CO2 aus!",
        .mood = STM_POINTING,
    },
    {
        .text = "Ein Kubikmeter CO2 wiegt 1.98 kg.",
        .mood = STM_POINTING,
    },
    {
        .text = "Ich bin am rechnen...",
        .mood = STM_WORKING,
        .action = STM_FROM_MEASUREMENT,
    },
    {
        .text = "Hmmm...",
        .mood = STM_WORKING,
        .action = STM_FROM_MEASUREMENT,
    },
    {
        .text = "Ich mache gerade eine wichtige Messung...",
        .mood = STM_WORKING,
        .action = STM_FROM_MEASUREMENT,
    },
    {
        .text = "(RH + 454) x 10^3/0.544 = ?",
        .mood = STM_WORKING,
        .action = STM_FROM_MEASUREMENT,
    },
};

size_t stm_num = sizeof(stm_entries) / sizeof(stm_entry_t);

stm_entry_t* stm_get(size_t i) { return i < stm_num ? &stm_entries[i] : NULL; }

stm_entry_t* stm_query(bool exclaim, stm_action_t action) {
  // get sensor data
  sns_state_t sensor = sns_get();

  // de/select and count entries
  int selected = 0;
  for (size_t i = 0; i < stm_num; i++) {
    // get entry
    stm_entry_t* entry = &stm_entries[i];

    // set selection
    entry->selected = true;

    // check exclaim
    if (entry->exclaim != exclaim) {
      entry->selected = false;
    }

    // check action
    if (entry->action != 0 && action != entry->action) {
      entry->selected = false;
    }

    // check co2
    if (entry->co2_min != 0 && (!sensor.ok || sensor.co2 < entry->co2_min)) {
      entry->selected = false;
    } else if (entry->co2_max != 0 && (!sensor.ok || sensor.co2 > entry->co2_max)) {
      entry->selected = false;
    }

    // check temperature
    if (entry->tmp_min != 0 && (!sensor.ok || sensor.tmp < entry->tmp_min)) {
      entry->selected = false;
    } else if (entry->tmp_max != 0 && (!sensor.ok || sensor.tmp > entry->tmp_max)) {
      entry->selected = false;
    }

    // check humidity
    if (entry->hum_min != 0 && (!sensor.ok || sensor.hum < entry->hum_min)) {
      entry->selected = false;
    } else if (entry->hum_max != 0 && (!sensor.ok || sensor.hum > entry->hum_max)) {
      entry->selected = false;
    }

    // increment if selected
    if (entry->selected) {
      selected++;
    }
  }

  // check selected
  if (selected == 0) {
    return NULL;
  }

  // choose entry randomly
  selected = (int)esp_random() % selected;

  // find and return entry
  for (int i = 0; i < stm_num; i++) {
    stm_entry_t* entry = &stm_entries[i];
    if (entry->selected) {
      selected--;
      if (selected < 0) {
        return entry;
      }
    }
  }

  return NULL;
}
