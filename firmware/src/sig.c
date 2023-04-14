#include <naos.h>
#include <naos_sys.h>

#include "sig.h"

#define SIG_DEBUG false

static QueueHandle_t sig_queue;

void sig_init() {
  // create queue
  sig_queue = xQueueCreate(3, sizeof(sig_event_t));
}

void sig_dispatch(sig_event_t event) {
  // safety check
  if (event == SIG_ANY) {
    ESP_ERROR_CHECK(ESP_FAIL);
  }

  // add event to queue or drop if full
  if (xQueueSendToBack(sig_queue, &event, 0)) {
    if (SIG_DEBUG) {
      naos_log("sig: queued %d", event);
    }
  } else {
    if (SIG_DEBUG) {
      naos_log("sig: dropped: %d", event);
    }
  }
}

sig_event_t sig_await(sig_event_t filter, uint32_t timeout) {
  // ensure timeout
  if (timeout <= 0) {
    timeout = portMAX_DELAY;
  }

  for (;;) {
    // get next event
    sig_event_t event = {0};
    if (!xQueueReceive(sig_queue, &event, timeout)) {
      event = SIG_TIMEOUT;
    }
    if (SIG_DEBUG) {
      naos_log("sig: dequeued %d", event);
    }

    // apply filter if provided
    if (filter != SIG_ANY && (event & filter) == 0) {
      if (SIG_DEBUG) {
        naos_log("sig: skipped %d", event);
      }
      continue;
    }

    return event;
  }
}
