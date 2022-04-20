#include "Mode.h"
#include "Pattern.h"

#include "LedControl.h"

Mode::Mode() :
  m_pPattern(nullptr),
  m_pColorset(nullptr)
{
}

Mode::Mode(Pattern *pat, Colorset *set) :
  m_pPattern(pat),
  m_pColorset(set)
{
}

void Mode::play(LedControl *ledControl)
{
  if (!m_pPattern || !m_pColorset) {
    return;
  }
  // play the curren pattern with current color set
  m_pPattern->play(ledControl, m_pColorset);
}
