#ifndef COM_H
#define COM_H

void com_init();
bool com_started();
void com_online();

void com_log(const char *, size_t len);

#endif  // COM_H
