#include "PatternSelect.h"

#include "../../Patterns/PatternBuilder.h"
#include "../../Patterns/PatternArgs.h"
#include "../../Patterns/Pattern.h"
#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Random/Random.h"
#include "../../Menus/Menus.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

PatternSelect::PatternSelect(const RGBColor &col, bool advanced) :
  Menu(col, advanced),
  m_origSet(),
  m_curSet(),
  m_advMode(PATTERN_DOPS, m_curSet)
{
}

PatternSelect::~PatternSelect()
{
}

bool PatternSelect::init()
{
  if (!Menu::init()) {
    return false;
  }
  if (!m_pCurMode) {
    return false;
  }
  // reset current mode
  m_pCurMode->init();
  if (m_advanced) {
    // copy out the colorset so we can walk it with colorset apis
    m_origSet = m_pCurMode->getColorset();
    // copy current mode but clear the colorset, keep pattern and params
    m_advMode = *m_pCurMode;
  }
  DEBUG_LOG("Entered pattern select");
  return true;
}

Menu::MenuAction PatternSelect::run()
{
  MenuAction result = Menu::run();
  if (result != MENU_CONTINUE) {
    return result;
  }
  if (m_advanced) {
    if ((Time::getCurtime() % 256) == 0 && m_curSet.numColors() != 0) {
      // check first color for expiry
      if (!m_curSet.get(0).raw()) {
        m_curSet.removeColor(0);
      }
      // scale down the brightness of each color in the set
      for (uint8_t i = 0; i < m_curSet.numColors(); ++i) {
        m_curSet[i].adjustBrightness(15);
      }
      m_advMode.setColorset(m_curSet);
      m_advMode.init();
    }
    m_advMode.play();
    return MENU_CONTINUE;
  }
  // run the current mode
  m_pCurMode->play();
  // show selections
  Menus::showSelection();
  return MENU_CONTINUE;
}

void PatternSelect::onShortClick()
{
  if (m_advanced) {
    m_curSet.addColor(m_origSet.getNext());
    m_advMode.setColorset(m_curSet);
    m_advMode.init();
    return;
  }
  LedPos srcLed = LED_MULTI;
  if (!m_pCurMode->isMultiLed()) {
    srcLed = mapGetFirstLed(m_targetLeds);
  }
  PatternID newID = (PatternID)(m_pCurMode->getPatternID(srcLed) + 1);
  PatternID endList = PATTERN_SINGLE_LAST;
  PatternID beginList = PATTERN_SINGLE_FIRST;
#if VORTEX_SLIM == 0
  if (m_targetLeds == MAP_LED_ALL || m_targetLeds == MAP_LED(LED_MULTI)) {
    endList = PATTERN_MULTI_LAST;
  }
  if (m_targetLeds == MAP_LED(LED_MULTI)) {
    beginList = PATTERN_MULTI_FIRST;
  }
#endif
  if (newID > endList || newID < beginList) {
    newID = beginList;
  }
  // set the new pattern id
  if (isMultiLedPatternID(newID)) {
    m_pCurMode->setPattern(newID);
  } else {
    m_pCurMode->setPatternMap(m_targetLeds, newID);
  }
  m_pCurMode->init();
  DEBUG_LOGF("Iterated to pattern id %d", newID);
}

void PatternSelect::onLongClick()
{
  // only save if it's not advanced
  leaveMenu(!m_advanced);
}
