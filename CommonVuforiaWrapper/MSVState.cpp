#include "MSVState.h"

int MSVState::glWidth = 0;
int MSVState::glHeight = 0;
bool MSVState::portrait = false;

void
MSVState::setGLViewSize(int w, int h)
{
  MSVState::glWidth  = w;
  MSVState::glHeight = h;
}

void
MSVState::getGLViewSize(int *w, int *h)
{
  *w = MSVState::glWidth;
  *h = MSVState::glHeight;
}

void
MSVState::setPortrait(bool p)
{
  MSVState::portrait = p;
}

bool
MSVState::isPortrait()
{
  return MSVState::portrait;
}
