#include <cstdio>
#include <unistd.h>
#include "PAL.h"

namespace pal {

   pstatus threadSleep(int mseconds) {
   usleep(mseconds * 1000);
   return PAL_OK;
}

} // End of namespace pal
