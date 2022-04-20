#include "Pattern.h"

#include "LedControl.h"
#include "Colorset.h"

Pattern::Pattern() :
  m_pColorset(nullptr)
{
}

Pattern::~Pattern()
{
}

bool Pattern::bind(const Colorset *colorset)
{
  if (!colorset) {
    return false;
  }
  m_pColorset = colorset;
  return true;
}
