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
 * Also, see tests below.
 *
 ******************************************************************************/

/******************************************************************************
 * Function: monitorPVsWithCallback
 * Inputs: std::string[], epicsCBFuncPointer[]
 * Returns: int
 * Description: Monitor a series of PV (Enum) names passed, and attached callback 
 * functions with value changes. Enum is 16 bit short. chtype = long
 * mbbo is 16 bit short = DBR_ENUM
 * bo is 16 bit short = DBR_ENUM
 * ao is double = DBR_DOUBLE; 
 * longout is long = DBR_LONG
 ******************************************************************************/

int monitorPVsWithCallback(std::string pvNames[], epicsCBFuncPointer cbFunc[],
			   chtype channelType[], const int length) {

  int ii;

  chid childArray[length];
  int priority = 10;
  
  /* 
   * Creating channel with no additional threads allowed to join 
   * Callled once from each thread prior to making Channel Access calls
   */
  SEVCHK(ca_context_create(ca_disable_preemptive_callback),"ca_context_create");

  for (ii = 0; ii < length; ii++) {

    /* 
     * Creates a CA channel. No connnection callback specified
     */
    SEVCHK(ca_create_channel(pvNames[ii].c_str(),NULL,NULL,priority,&childArray[ii]), 
	   "ca_create_channel");

    std::cout << "Monitoring " << pvNames[ii] << "\n";

    /* 
     * Registering callback with the created child for a "significant" change
     * DBR_LONG = channel type. DBE_VALUE = is a mask to for events to select for
     * significant value changes
     */ 
    SEVCHK(ca_create_subscription(channelType[ii],1,childArray[ii], DBE_VALUE,cbFunc[ii],
				  NULL,NULL), "ca_create_subscription");

  }

  /* Should never return from this call. Now monitoring PV's */
  SEVCHK(ca_pend_event(0.0),"ca_pend_event");

  return 1;  

}


/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/

static int testEpicsCAGet();
static int testEpicsCAMonitorCallBack();
static int testMonitorPVsWithCallback();


static void printChidInfo(chid chid, std::string message) {

  std::cout << "\n" << message << "\n";

  printf("pv: %s  type(%d) nelements(%ld) host(%s)",
	 ca_name(chid),ca_field_type(chid),ca_element_count(chid),
	 ca_host_name(chid));
  printf(" read(%d) write(%d) state(%d)\n",
	 ca_read_access(chid),ca_write_access(chid),ca_state(chid));

}

static void exceptionCallback(struct exception_handler_args args) {

  chid	chid = args.chid;
  long	stat = args.stat; /* Channel access status code*/
 
  std::string noname = "corn dogs";
  std::string channel;
  channel = (chid ? ca_name(chid) : noname);

  if(chid) printChidInfo(chid,"exceptionCallback");
  printf("exceptionCallback stat %s channel %s\n",
	 ca_message(stat),channel.c_str());
}

static void eventCallbackLong(struct event_handler_args eha) {

    chid	chid = eha.chid;

    if(eha.status!=ECA_NORMAL) {
	printChidInfo(chid,"eventCallback");
    } else {
      //char	*pdata = (char *)eha.dbr;
      //printf("Event Callback: %s = %s\n",ca_name(eha.chid),pdata);
      long *pdata = (long *)eha.dbr;
      printf("Event Callback: %s = %ld\n",ca_name(eha.chid),*pdata);
    }
}

static void eventCallbackDouble(struct event_handler_args eha) {

    chid	chid = eha.chid;

    if(eha.status!=ECA_NORMAL) {
	printChidInfo(chid,"eventCallback");
    } else {
      //char	*pdata = (char *)eha.dbr;
      //printf("Event Callback: %s = %s\n",ca_name(eha.chid),pdata);
      double *pdata = (double *)eha.dbr;
      printf("Event Callback Double: %s = %f\n",ca_name(eha.chid),*pdata);
    }
}

static void eventCallbackEnum(struct event_handler_args eha) {

    chid	chid = eha.chid;

    if(eha.status!=ECA_NORMAL) {
	printChidInfo(chid,"eventCallback");
    } else {
      //char	*pdata = (char *)eha.dbr;
      //printf("Event Callback: %s = %s\n",ca_name(eha.chid),pdata);
      short *pdata = (short *)eha.dbr;
      printf("Event Callback Enum: %s = %d\n",ca_name(eha.chid),*pdata);
    }
}


int testEpicsCA() {

  if (!testMonitorPVsWithCallback()) {
    std::cout << "Test EPICS camonitor long callback test Failed\n";
    return -1;
  }

  if (!testEpicsCAGet()) {
    std::cout << "Test EPICS Caget test Failed\n";
    return -1;
  }

  if (!testEpicsCAMonitorCallBack()) {
    std::cout << "Test EPICS camonitor callback test Failed\n";
    return -1;
  }

  std::cout << "All epicsCA.cpp tests passed\n";

  return 0;

}

static int testMonitorPVsWithCallback() {

  const int SIZE = 9;
  std::string pvNames[SIZE] = {"FuZE:ControlPLC:InnerParkerBatteryVoltage", 
			       "FuZE:ControlPLC:Holography:Z", 
			       "FuZE:ControlPLC:Holography:Y",
			       "FuZE:ControlPLC:InnerGasValveType", 
			       "FuZE:ControlPLC:ThyristorBank:Enabled",
			       "FuZE:ControlPLC:InnerValveGasType",
			       "FuZE:ControlPLC:PSQLError", 
			       "FuZE:ControlPLC:InnerParkerDial", 
			       "FuZE:ControlPLC:MDSplusError"};

  epicsCBFuncPointer cbFuncs[SIZE] = {eventCallbackDouble, eventCallbackDouble, 
				      eventCallbackDouble,
				      eventCallbackEnum, eventCallbackEnum, eventCallbackEnum,
				      eventCallbackLong, eventCallbackLong, eventCallbackLong};

  chtype channelType[SIZE] = {DBR_DOUBLE, DBR_DOUBLE, DBR_DOUBLE, 
			      DBR_ENUM, DBR_ENUM, DBR_ENUM,
			      DBR_LONG, DBR_LONG, DBR_LONG};

  monitorPVsWithCallback(pvNames, cbFuncs, channelType, SIZE);

  return 1;

}


static int testEpicsCAGet() {

  double data;

  /* Think this is an EPICS data type. Channel ID */
  chid mychid;

  std::string pvName = "FuZE:DataServer:ShotNumber";
  int priority = 10;

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
  SEVCHK(ca_create_channel(pvName.c_str(),NULL,NULL,priority,&mychid),"ca_create_channel failure");

  /*
   * This function flushes the send buffer and then blocks until outstanding ca_get() requests 
   * complete, and until channels created specifying null connection handler function pointers 
   * connect for the first time.
   * The main argument is Timeout: Specifies the time out interval. 
   * 0 specifies to wait forever
   * A TIMEOUT interval of zero specifies forever.
   * If no ca_get() or connection state change events are outstanding then ca_pend_io() will 
   * flush the send buffer and return immediately without processing any outstanding channel 
   * access background activities.
   */
  SEVCHK(ca_pend_io(5.0),"ca_pend_io failure");

  /*
   * When ca_get() or ca_array_get() are invoked the returned channel value can't be assumed to 
   * be stable in the application supplied buffer until after ECA_NORMAL is returned from 
   * ca_pend_io(). If a connection is lost outstanding ca get requests are not automatically 
   * reissued following reconnect.
   */
  SEVCHK(ca_get(DBR_DOUBLE,mychid,(void *)&data),"ca_get failure");

  SEVCHK(ca_pend_io(5.0),"ca_pend_io failure");

  // data should have value. Need to do a ca_pend_io first
  std::cout << "Shot number: " << std::setprecision(9) << data << "\n";

  if ( false ) {
    std::cout << "EPICS CA Get Test Failed\n";
    return -1;
  }

  return 1;

}


static int testEpicsCAMonitorCallBack() {

  /*
   * See the file: 
   * /usr/local/epics/base-3.15.3/src/template/base/top/caClientApp/caMonitor.c
   * for details
   */
  std::string pvName = "FuZE:DataServer:ShotNumber";
  chid mychid;
  int priority = 10;
  
  /* Creating channel with no additional threads allowed to join */
  SEVCHK(ca_context_create(ca_disable_preemptive_callback),"ca_context_create");

  /* Replace the currently installed CA context global exception handler callback. */
  SEVCHK(ca_add_exception_event(exceptionCallback,NULL),
	 "ca_add_exception_event");

  /* Creating the channel with no connection call back */
  SEVCHK(ca_create_channel(pvName.c_str(),NULL,NULL,priority,&mychid),
	 "ca_create_channel");

  /* 
   * Register a state change subscription and specify a callback function to be invoked whenever 
   * the process variable undergoes significant state changes. A significant change can be a 
   * change in the process variable's value, alarm status, or alarm severity. In the process 
   * control function block database the deadband field determines the magnitude of a significant 
   * change for the process variable's value. Each call to this function consumes resources in 
   * the client library and potentially a CA server until one of ca_clear_channel() or 
   * ca_clear_subscription() is called.
   * int ca_create_subscription ( chtype TYPE, unsigned long COUNT,
   *     chid CHID, unsigned long MASK,
   *     caEventCallBackFunc USERFUNC, void *USERARG, 
   *     evid *PEVID );
   * TYPE = the type of channel (DBR_STRING, DBR_LONG) 
   * COUNT = The element count to be read
   * CHID = Channel identifier
   * MASK = A mask with bits set for each of the event trigger types requested. 
   * The event trigger mask must be a bitwise or of one or more of the following constants.
   * DBE_VALUE - Trigger events when the channel value exceeds the monitor dead band
   * ...
   * USERFUNC = function that is the callback 
   * typedef void ( caEventCallBackFunc ) (struct event_handler_args);
   * USERARG = pointer sized variable retained and passed back to user callback function
   * PEVID = This is a pointer to user supplied event id which is overwritten if successful. 
   * This event id can later be used to clear a specific event. This option may be omitted by 
   * passing a null pointer.
   */
  SEVCHK(ca_create_subscription(DBR_LONG,1,mychid, DBE_VALUE,eventCallbackLong,
				NULL,NULL),
	 "ca_create_subscription");

  /* 
   * Should never return from following call. Why? 0 specifies to wait forever.
   * When ca_pend_event() is invoked the send buffer is flushed and CA background 
   * activity is processed for TIMEOUT seconds.
   * The ca_pend_event() function will not return before the specified timeout expires and all 
   * unfinished channel access labor has been processed, and unlike ca_pend_io() returning from 
   * the function does not indicate anything about the status of pending IO requests.
   */
  SEVCHK(ca_pend_event(0.0),"ca_pend_event");

  if ( false ) {
    std::cout << "EPICS CA Monitor Callback Test Failed\n";
    return -1;
  }

  return 1;


}
