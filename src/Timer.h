#ifndef TIMER_H_
#define TIMER_H_

#include <raylib.h>

typedef void (*TimeoutCallback)(void);

typedef struct {
  float currentTime;
  float maxTime;
  int loopTimer;
  int isRunning;
  TimeoutCallback timeoutCallback;
} Timer;

Timer TimerCreate(float maxTime, int loopTimer, TimeoutCallback timeout) {
  return (Timer){.currentTime = 0.0f,
                 .maxTime = maxTime,
                 .loopTimer = loopTimer,
                 .isRunning = 0,
                 .timeoutCallback = timeout};
}

void TimerStart(Timer *timer) {
  timer->currentTime = 0.0f;
  timer->isRunning = 1;
}

void TimerEnd(Timer *timer) {
  timer->currentTime = 0.0f;
  timer->isRunning = 0;
}

void TimerUpdate(Timer *timer) {
  if (!timer->isRunning) {
    timer->currentTime = 0.0f;
    return;
  }
  timer->currentTime += GetFrameTime();
  if (timer->currentTime >= timer->maxTime) {
    timer->currentTime = 0.0f;
    timer->timeoutCallback();
    if (!timer->loopTimer) {
      timer->isRunning = 0;
    }
  }
}

#endif
