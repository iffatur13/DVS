#ifndef POLARITY_EVENTS_H
#define POLARITY_EVENTS_H

#include "aqueue.h"

template class AQueue<int>;

class PolarityEvent {
 public:
  AQueue<int64_t> positive_events_; // most recent at front, holds timestamps
  AQueue<int64_t> negative_events_; // most recent at front, holds timestamps

  ~PolarityEvent () {
    positive_events_.clear();
    negative_events_.clear();
  }
  
  void addEvent (bool polarity, int64_t time_stamp) {
    if (polarity) {
      positive_events_.enqueue (time_stamp);
    } else {
      negative_events_.enqueue (time_stamp);
    }
  }

  bool isEmpty (bool positive) {
    if (positive) {
      return positive_events_.empty (); // true if empty
    }
    return negative_events_.empty (); // true if empty
  }

  int64_t getFront (bool polarity) {
    // positive and there is at least one positive event
    if (polarity && !positive_events_.empty ()) {
      return positive_events_.front ();
      // negative and there is at least one negative event
    } else if (!polarity && !negative_events_.empty ()) {
      return negative_events_.front ();
    } 
    return -1;
  }
  
  int64_t getBack (bool polarity) {
    // positive and there is at least one positive event
    if (polarity && !positive_events_.empty()) {
      return positive_events_.back();
      // negative and there is at least one negative event
    } else if (!polarity && !negative_events_.empty()) {
      return negative_events_.back();
    } 
    return -1;
  }
};

#endif
