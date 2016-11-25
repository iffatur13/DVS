// #ifndef WIDTH
// #define WIDTH 190
// #endif

// #ifndef HEIGHT
// #define HEIGHT 180
// #endif

#include <libcaer/libcaer.h>
#include <libcaer/devices/davis.h>

#include <cstdio>
#include <csignal>
#include <atomic>
#include <iostream>

// was having issues with name collision between libcaer and Mat
#ifdef TYPE_MASK
#undef TYPE_MASK
#endif

#include <opencv2/core/mat.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "aqueue.h"
#include "polarity_event.h"
#include "viewport.h"

using namespace std;

// must be declared here
static atomic_bool globalShutdown(false);
static void globalShutdownSignalHandler(int signal) {
  // Simply set the running flag to false on SIGTERM and SIGINT (CTRL+C)
  // for global shutdown.
  if (signal == SIGTERM || signal == SIGINT) {
    globalShutdown.store(true);
  }
}

// setup signal handler
bool initSignalHandler();

// caer device biases
void initBiases (caerDeviceHandle&);

// grabs connected device info, prints relevant information, and sets the default
// configuration before using the device (not done automatically)
void getDeviceInfo (caerDeviceHandle&);

int main () {

  // takes factor by which window dimensions will be scaled and whether
  // per-event details will be output to standard output
  Viewport view(2, false);

  // some initialization
  if (!initSignalHandler()) {
    return (EXIT_FAILURE);
  }
  // Open a DAVIS, give it a device ID of 1, and don't care about USB bus or
  // SN restrictions.
  caerDeviceHandle davis_handle = caerDeviceOpen(1, CAER_DEVICE_DAVIS_FX2,
                                                 0, 0, NULL);
  if (davis_handle == NULL) {
    return (EXIT_FAILURE);
  }

  // grabs device info and sets default configuration
  getDeviceInfo(davis_handle);

  // setup biases, increase bandwidth
  initBiases(davis_handle);

  // ************************ //
  // MAIN LOOP
  // ************************ //
  while (!globalShutdown.load(memory_order_relaxed)) {

    // grab a packet container
    caerEventPacketContainer packetContainer = caerDeviceDataGet(davis_handle);
    if (packetContainer == NULL) {
      continue; // Skip if nothing there.
    }

    view.ProcessPacketContainer(packetContainer);

    if (view.ShouldQuit()) {
      break;
    }

  }

  caerDeviceDataStop(davis_handle);
  caerDeviceClose(&davis_handle);

  for (int i = 0; i < 256; i++) {
    cout << "hist[" << i << "] = " << hist[i] << ", bits = " << __builtin_popcount(i) << endl;
  }

  return (EXIT_SUCCESS);
}


//returns true on success, false on failure
bool initSignalHandler() {
  // Install signal handler for global shutdown.
  struct sigaction shutdownAction;

  shutdownAction.sa_handler = &globalShutdownSignalHandler;
  shutdownAction.sa_flags = 0;
  sigemptyset(&shutdownAction.sa_mask);
  sigaddset(&shutdownAction.sa_mask, SIGTERM);
  sigaddset(&shutdownAction.sa_mask, SIGINT);

  if (sigaction(SIGTERM, &shutdownAction, NULL) == -1) {
    caerLog(CAER_LOG_CRITICAL, "ShutdownAction",
	    "Failed to set signal handler for SIGTERM. Error: %d.", errno);
    return false;
    //return (EXIT_FAILURE);
  }

  if (sigaction(SIGINT, &shutdownAction, NULL) == -1) {
    caerLog(CAER_LOG_CRITICAL, "ShutdownAction",
	    "Failed to set signal handler for SIGINT. Error: %d.", errno);
    return false;
    //return (EXIT_FAILURE);
  }
  return true;
}

void initBiases(caerDeviceHandle& davis_handle) {
  // Tweak some biases, to increase bandwidth in this case.
  struct caer_bias_coarsefine coarseFineBias;

  coarseFineBias.coarseValue = 2;
  coarseFineBias.fineValue = 116;
  coarseFineBias.enabled = true;
  coarseFineBias.sexN = false;
  coarseFineBias.typeNormal = true;
  coarseFineBias.currentLevelNormal = true;
  caerDeviceConfigSet(davis_handle, DAVIS_CONFIG_BIAS, DAVIS240_CONFIG_BIAS_PRBP,
                      caerBiasCoarseFineGenerate(coarseFineBias));

  coarseFineBias.coarseValue = 1;
  coarseFineBias.fineValue = 33;
  coarseFineBias.enabled = true;
  coarseFineBias.sexN = false;
  coarseFineBias.typeNormal = true;
  coarseFineBias.currentLevelNormal = true;
  caerDeviceConfigSet(davis_handle, DAVIS_CONFIG_BIAS, DAVIS240_CONFIG_BIAS_PRSFBP,
                      caerBiasCoarseFineGenerate(coarseFineBias));

  // Let's verify they really changed!
  uint32_t prBias, prsfBias;
  caerDeviceConfigGet(davis_handle,
		      DAVIS_CONFIG_BIAS,
		      DAVIS240_CONFIG_BIAS_PRBP,
		      &prBias);

  caerDeviceConfigGet(davis_handle,
		      DAVIS_CONFIG_BIAS,
		      DAVIS240_CONFIG_BIAS_PRSFBP,
		      &prsfBias);

  printf("New bias values --- PR-coarse: %d, PR-fine: %d, PRSF-coarse: %d, PRSF-fine: %d.\n",
         caerBiasCoarseFineParse(prBias).coarseValue,
	 caerBiasCoarseFineParse(prBias).fineValue,
         caerBiasCoarseFineParse(prsfBias).coarseValue,
	 caerBiasCoarseFineParse(prsfBias).fineValue);

  // Now let's get start getting some data from the device. We just loop,
  // no notification needed.
  caerDeviceDataStart(davis_handle, NULL, NULL, NULL, NULL, NULL);

  // Let's turn on blocking data-get mode to avoid wasting resources.
  caerDeviceConfigSet(davis_handle,
		      CAER_HOST_CONFIG_DATAEXCHANGE,
		      CAER_HOST_CONFIG_DATAEXCHANGE_BLOCKING,
		      true);
}


void getDeviceInfo (caerDeviceHandle& davis_handle) {
  // get device info
  struct caer_davis_info davis_info = caerDavisInfoGet(davis_handle);

  // print device information
  printf("%s --- ID: %d, Master: %d, DVS X: %d, DVS Y: %d, Logic: %d.\n",
         davis_info.deviceString,
	 davis_info.deviceID,
	 davis_info.deviceIsMaster,
         davis_info.dvsSizeX,
	 davis_info.dvsSizeY,
	 davis_info.logicVersion);

  // Send the default configuration before using the device (not done automatically)
  caerDeviceSendDefaultConfig(davis_handle);
}
