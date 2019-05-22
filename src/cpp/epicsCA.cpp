#include "cpp/epicsCA.h"

/******************************************************************************
 *
 * This is the source file to provide a library and template to monitor pv's
 * using the EPICS channel access library
 *
 ******************************************************************************/

/******************************************************************************
 * Example Usage:
 * https://epics.anl.gov/base/R7-0/1-docs/CAref.html
 *
 ******************************************************************************/



/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/

static int testEpicsCAGet();

int testEpicsCA() {

  if (!testEpicsCAGet()) {
    std::cout << "Test EPICS Caget test Failed\n";
    return -1;
  }

  std::cout << "All epicsCA.cpp tests passed\n";

  return 0;

}

static int testEpicsCAGet() {

  double data;

  /* Think this is an EPICS data type. Channel ID */
  chid mychid;

  std::string pvName = "FuZE:DataServer:ShotNumber";

  /* 
   * SEVCHK is a macro to check the return status 
   * If ca_disable_preemptive_callback is specified then additional threads are not 
   * allowed to join the CA context using ca_context_attach() because allowing other 
   * threads to join implies that CA callbacks will be called preemptively from more 
   * than one thread.
   * Should be called once from each thread prior to making any of the other Channel Access calls
   */
  SEVCHK(ca_context_create(ca_disable_preemptive_callback),"ca_context_create");

  /* 
   * Creating the channel 
   * This function creates a CA channel. The CA client library will attempt to establish and 
   * maintain a virtual circuit between the caller's application and a named process variable in 
   * a CA server. Each call to ca_create_channel() allocates resources in the CA client library 
   * and potentially also a CA server. The function ca_clear_channel() is used to release these 
   * resources. If successful, the routine writes a channel identifier into the user's variable 
   * of type "chid". This identifier can be used with any channel access call that operates on 
   * a channel.
   * int ca_create_channel (const char *PVNAME,
   *   caCh *USERFUNC, void *PUSER,
   *   capri PRIORITY, chid *PCHID );
   * USERFUNC is the call back function on change
   * PUSER = void pointer. Storage associated with specified channel. Just set to NULL
   * PRIORITY = 0 = lowest, 99 = highest. Priority level of dispatch within the server.
   * always use the same number for multiple channels, or you could use more resources
   * PCHID = If successfull, will point to a channel identifier. EPICS data type.
   */
  SEVCHK(ca_create_channel(pvName.c_str(),NULL,NULL,10,&mychid),"ca_create_channel failure");

  /*
   * This function flushes the send buffer and then blocks until outstanding ca_get() requests 
   * complete, and until channels created specifying null connection handler function pointers 
   * connect for the first time.
   * The main argument is Timeout: Specifies the time out interval. 
   * A TIMEOUT interval of zero specifies forever.
   * If no ca_get() or connection state change events are outstanding then ca_pend_io() will 
   * flush the send buffer and return immediately without processing any outstanding channel 
   * access background activities.
   */
  SEVCHK(ca_pend_io(5.0),"ca_pend_io failure");

  /*
   *
   */
  SEVCHK(ca_get(DBR_DOUBLE,mychid,(void *)&data),"ca_get failure");

  SEVCHK(ca_pend_io(5.0),"ca_pend_io failure");

  // data should have value
  std::cout << "Shot number: " << std::setprecision(9) << data << "\n";

  if ( false ) {
    std::cout << "EPICS CA Get Test Failed\n";
    return -1;
  }

  return 1;

}
