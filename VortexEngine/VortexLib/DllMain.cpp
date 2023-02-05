#include <Windows.h>

#include "EngineWrapper.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  // Perform actions based on the reason for calling.
  switch (fdwReason) {
  case DLL_PROCESS_ATTACH:
    VEngine::init();
    break;
  case DLL_THREAD_ATTACH:
    break;
  case DLL_THREAD_DETACH:
    break;
  case DLL_PROCESS_DETACH:
    VEngine::cleanup();
    break;
  }
  return TRUE;  // Successful DLL_PROCESS_ATTACH.
}