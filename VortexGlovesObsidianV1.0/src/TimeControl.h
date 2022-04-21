#ifndef TIME_H
#define TIME_H

class TimeControl
{
  public:
    TimeControl();

    // any kind of time initialization
    bool init();

    // tick the clock forward to millis()
    void tickClock();

    // get the current time with optional finger offset
    uint64_t getCurtime(uint32_t finger = 0);

  private:
    // global curtime
    uint64_t m_curTime;
};

#endif
