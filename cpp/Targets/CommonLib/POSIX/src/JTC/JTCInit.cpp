#include "JTC/JTCInit.h"

namespace JTCInit {

JTCInitialize* g_jtcInitialize = NULL;

void initJTC()
{
   if(!g_jtcInitialize) {
      g_jtcInitialize = new JTCInitialize;      
   }
}

} // end of namespace JTC

