#include "../../inc/MarlinConfig.h"

extern "C" void wtd_update()
{
  watchdog_refresh();
}
