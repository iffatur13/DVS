#ifndef EVENT_H
#define EVENT_H

class Event {
 public:
  Event (int time_stamp) {
    time_stamp_ = time_stamp;
  }
  int time_stamp_;
};

#endif
