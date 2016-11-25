#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <string>
#include <iostream>

#include <cstdio>
#include <csignal>
#include <atomic>

#include <libcaer/libcaer.h>
#include <libcaer/devices/davis.h>

// was having issues with name collision between libcaer and Mat
#ifdef TYPE_MASK
#undef TYPE_MASK
#endif


#include <opencv2/core/mat.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "aqueue.h"
#include "polarity_event.h"

class Viewport {

 public:
  Viewport (int zoomFactor=1, bool printEventDetails=true);
  ~Viewport();

  void ProcessPacketContainer(const caerEventPacketContainer&);
  void DrawPolarityEventPoint(uint16_t, uint16_t, bool, cv::Mat&);
  void DrawPolarityEventCircle(uint16_t, uint16_t, bool, cv::Mat&);
  void DrawPolarityEventMatchOnly(uint16_t, uint16_t, bool, cv::Mat&);
  void DrawPolarityEventByDirection(uint16_t, uint16_t, bool, cv::Mat&);
  void DrawPolarityEventBitMapping(uint16_t, uint16_t, bool, cv::Mat&);
  void Show(const cv::Mat&) const;
  bool ShouldQuit();

 private:
  const int WIDTH = 190;
  const int HEIGHT = 180;
  const cv::Scalar GREEN = cv::Scalar(0, 255, 0);
  const cv::Scalar RED = cv::Scalar(0, 0, 255);
  const std::string window_name_ = "main frame";


  PolarityEvent **viewport_events_;
  bool print;

  void SetupWindow(int);
  void StoreEvent(int64_t, uint16_t, uint16_t, bool);
  bool PrintEventPacketHeaderInfo(const caerEventPacketHeader&, int) const;
  bool InBound (int, int) const;
  bool HasMatchingPolarity (uint16_t, uint16_t, bool) const;

  //char GetColorValue(int64_t) const;
};

extern int hist[256];

#endif
