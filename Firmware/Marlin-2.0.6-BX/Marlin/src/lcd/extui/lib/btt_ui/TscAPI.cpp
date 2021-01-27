#include "TscAPI.h"
#include "../../../../MarlinCore.h"

extern "C" void syncWriteToL1Cache(char ch);

Terminal terminalAck;

size_t Terminal::write(uint8_t c)
{
  // if (marlin_state == MF_INITIALIZING) return 1;
  syncWriteToL1Cache(c);
  return 1;
}

int Terminal::available(void)
{
  return 1;
}

int Terminal::peek(void)
{
  return -1;
}

int Terminal::read(void)
{
  return 0;
}
void Terminal::flush()
{
}