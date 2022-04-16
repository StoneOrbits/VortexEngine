#include "Mode.h"

Mode::Mode()
{

}

Mode::Mode(Pattern *pat, Colorset *set)
{
}

void Mode::play()
{
  if (!m_pPattern || !m_pColorset) {
    return;
  }
  // play the curren pattern with current color set
  m_pPattern->play(m_pColorset);
}
