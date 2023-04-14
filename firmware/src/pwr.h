typedef struct {
  float battery;
  bool usb;
  bool fast;
} pwr_state_t;

void pwr_init();

pwr_state_t pwr_get();

void pwr_off();
void pwr_sleep(bool deep);
