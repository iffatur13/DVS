#include "viewport.h"

#include <stdexcept>

Viewport::Viewport(int zoomFactor, bool printEventDetails) : print(printEventDetails) {
  //initialize our structure to track events
  viewport_events_ = new PolarityEvent * [WIDTH];
  for (int i = 0; i < WIDTH; i++) {
    viewport_events_ [i] = new PolarityEvent [HEIGHT];
  }
  SetupWindow(zoomFactor);
}

void Viewport::SetupWindow(int zoomFactor) {
  cv::namedWindow (window_name_, CV_WINDOW_NORMAL);
  cv::resizeWindow (window_name_, WIDTH*zoomFactor, HEIGHT*zoomFactor);
  cv::Mat frame_initial = cv::Mat::zeros(HEIGHT, WIDTH, CV_8UC3);
  cv::imshow(window_name_, frame_initial);
}

Viewport::~Viewport() {
  // deallocate storage for event stream
  for (int i = 0; i < WIDTH; i++) {
    delete [] viewport_events_ [i];
  }
  delete [] viewport_events_;
}

// display the given frame in the window
void Viewport::Show(const cv::Mat& frame) const {
  cv::imshow(window_name_, frame);
}

bool Viewport::ShouldQuit() {
  char c = cv::waitKey(1);
  if (c == 27) {
    cv::destroyWindow(window_name_);
    return true;
  }
  return false;
}

void Viewport::ProcessPacketContainer(const caerEventPacketContainer& packet_container) {

  cv::Mat frame = cv::Mat::zeros(HEIGHT, WIDTH, CV_8UC3);

  // returns reference to an EventPacket with a certain type or NULL if none found
  caerEventPacketHeader packet_header = caerEventPacketContainerGetEventPacket(packet_container, POLARITY_EVENT);

  if (packet_header != NULL) {
    caerPolarityEventPacket polarity_event_packet = (caerPolarityEventPacket) packet_header;


    // returns current index 'caerPolarityIteratorCounter' of type 'int32_t'
    // returns current event in 'caerPolarityIteratorElement' of type 'caerPolarityEvent'
    // alternatively: can use CAER_POLARITY_ITERATOR_VALID_START(POLARITY_PACKET)
    // and the closing CAER_POLARITY_ITERATOR_VALID_END
    CAER_POLARITY_ITERATOR_ALL_START(polarity_event_packet)

      int64_t ts = caerPolarityEventGetTimestamp64(caerPolarityIteratorElement, polarity_event_packet);
    uint16_t x = caerPolarityEventGetX(caerPolarityIteratorElement);
    uint16_t y = caerPolarityEventGetY(caerPolarityIteratorElement);
    bool pol = caerPolarityEventGetPolarity(caerPolarityIteratorElement);
    StoreEvent(ts, x, y, pol);

    // handle the event
    //DrawPolarityEventMatchOnly(x, y, pol, frame);
    DrawPolarityEventPoint(x, y, pol, frame);
    //DrawPolarityEventCircle(x, y, pol, frame);

    // invalidate the current event
    if (caerPolarityEventIsValid(caerPolarityIteratorElement)) {
      caerPolarityEventInvalidate(caerPolarityIteratorElement, polarity_event_packet);
    }

    // closing brace macro }
    CAER_POLARITY_ITERATOR_ALL_END
      }

  // free the packet container
  caerEventPacketContainerFree(packet_container);
  // display the frame with events from packet container drawn
  Show(frame);
}


// void Viewport::ProcessPacketContainer(const caerEventPacketContainer& packet_container) {

//   cv::Mat frame = cv::Mat::zeros(HEIGHT, WIDTH, CV_8UC3);

//   int32_t packet_num =
//     caerEventPacketContainerGetEventPacketsNumber(packet_container);
//   if (print) {
//     std::cout << "Got event container with " << packet_num
//               << " packets (allocated)" << std::endl;
//   }

//   for (int32_t i = 0; i < packet_num; i++) {
//     caerEventPacketHeader packet_header =
//       caerEventPacketContainerGetEventPacket(packet_container, i);

//     // skip empty packets
//     if (!PrintEventPacketHeaderInfo(packet_header, i)) { continue; }

//     // Packet 0 is always the special events packet for DVS128, while
//     // packet i is the polarity events packet.
//     if (i == POLARITY_EVENT) {
//       caerPolarityEventPacket polarity_event_packet =
// 	(caerPolarityEventPacket) packet_header;

//       int total_num_events = caerEventPacketHeaderGetEventNumber(packet_header);
//       for (int j = 0; j < total_num_events; j++) {

//         caerPolarityEvent current_event =
//           caerPolarityEventPacketGetEvent(polarity_event_packet, j);

// 	// 32 bit event timestamp, in microseconds
// 	// may wrap around, either handle the special 'TIMESTAMP_WRAP' event
// 	// when this happens, or use the 64 bit timestamp that never wraps around

// 	int64_t ts = caerPolarityEventGetTimestamp64(current_event,
// 						     polarity_event_packet);
//         //int32_t ts = caerPolarityEventGetTimestamp(current_event);

// 	// note that the xy origin (0, 0) is in the lower left of the screen
//         uint16_t x = caerPolarityEventGetX(current_event);
//         uint16_t y = caerPolarityEventGetY(current_event);
//         bool pol = caerPolarityEventGetPolarity(current_event);

//         StoreEvent(ts, x, y, pol);

//         // handle the event
//         //DrawPolarityEventMatchOnly(x, y, pol, frame);
//         DrawPolarityEventPoint(x, y, pol, frame);
//         //DrawPolarityEventCircle(x, y, pol, frame);

// 	// invalidate the current event
// 	if (caerPolarityEventIsValid(current_event)) {
// 	  caerPolarityEventInvalidate(current_event, polarity_event_packet);
// 	}
//       }
//     }

//   }

//   // free the packet container
//   caerEventPacketContainerFree(packet_container);
//   // display the frame with events from packet container drawn
//   Show(frame);
// }

void Viewport::DrawPolarityEventPoint(uint16_t x, uint16_t y, bool polarity,
				      cv::Mat& frame) {

  if (polarity) {
    frame.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 255, 0);
  } else {
    frame.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 255);
  }
}

void Viewport::DrawPolarityEventCircle(uint16_t x, uint16_t y, bool polarity,
                                       cv::Mat& frame) {
  cv::Scalar color;
  if (polarity) { color = GREEN; }
  else { color = RED; }
  int radius = 1;
  cv::circle(frame, cv::Point(x, y), radius, color);
}


void Viewport::DrawPolarityEventMatchOnly(uint16_t x, uint16_t y, bool polarity,
                                          cv::Mat& frame) {
  int64_t time_stamp = viewport_events_[x][y].getBack(polarity);
  if (time_stamp == -1) { return; } // error

  int final_x = -1, final_y = -1;
  int64_t min_t = 9999999999; // this is the 'max' time

  for (int dx = -1; dx < 2; dx++) {
    for (int dy = -1; dy < 2; dy++) {

      if (dx == 0 && dy == 0) continue;

      if (InBound (x+dx, y+dy) &&
          HasMatchingPolarity (x+dx, y+dy, polarity)) {

        // if difference between time_stamp and other timestamp is below
        // the current 'min' time difference
        int64_t curr_t = time_stamp -
          viewport_events_[x+dx][y+dy].getBack(polarity);
        if (curr_t < min_t) {
          // set new values
          min_t = curr_t;
          final_x = x + dx;
          final_y = y + dy;
        }
      }
    }
  }

  // ensure at least one was found
  if (final_x >= 0 || final_y >= 0) {
    // final_x and final_y are the coordinates of the most recent match


    // min_t is the time step between the two events; smaller min_t = faster movement
    //char color_intensity = GetColorValue(min_t);
    int color_intensity = 255;

    // draw a line between the two points
    cv::line (frame, cv::Point (x, y), cv::Point (final_x, final_y),
              polarity ? cv::Scalar (0, color_intensity, 0) :
	      cv::Scalar (0, 0, color_intensity),
              /*thickness*/ 1, /*lineType*/ cv::LINE_8, /*shift*/ 0);
    if (print) {
      std::cout << "line from (" << x << ", " << y << ") to (" << final_x << ", "
                << final_y << ")" << std::endl;
    }
  }
}

// takes the time step (in microseconds) between the two events of interest
// returns a value that is higher, when the time step shorter
// char Viewport::GetColorValue(int64_t time_step) const {
//   double time_seconds = static_cast<double>(time_step) * 1e-6;
//   if (time_seconds < 1.0) // within one second
//     return 255;
//   else if (time_seconds < 2.0)
//     return 150;
//   else
//     return 80;
// }

bool Viewport::InBound(int x, int y) const {
  if (x >= WIDTH  || x < 0 ||
      y >= HEIGHT || y < 0) {
    return false;
  }
  return true;
}

bool Viewport::HasMatchingPolarity (uint16_t x, uint16_t y, bool polarity) const {
  return (!viewport_events_[x][y].isEmpty(polarity));
}

void Viewport::StoreEvent(int64_t time, uint16_t x, uint16_t y, bool polarity) {
  viewport_events_[x][y].addEvent(polarity, time);
}

bool Viewport::PrintEventPacketHeaderInfo(const caerEventPacketHeader& packet_header,
					  int packet_index) const {
  if (packet_header == NULL) {
    if (print) {
      std::cout << "Packet " << packet_index << " is empty (not present)." << std::endl;
    }
    return false;
  }
  if (print) {
    std::cout << "Packet " << packet_index
              << " of type " << caerEventPacketHeaderGetEventType(packet_header)
              << " -> size is " << caerEventPacketHeaderGetEventNumber(packet_header) << "." << std::endl;
  }
  return true;
}
