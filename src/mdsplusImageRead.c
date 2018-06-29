#include <stdio.h>
#include <mdslib.h>
#include <mdsshr.h>

static int status_ok( int status ) {

  return ( (status & 1) == 1 );

}

static int get_signal_length(const char *signal) {

  int dtype_long = DTYPE_LONG;
  char buf[1024];
  int size;
  int null = 0;
  int idesc = descr(&dtype_long, &size, &null);
  int status;

  /* init buffer */
  memset(buf,0,sizeof(buf));

  /* put SIZE() TDI function around signal name */
  snprintf(buf,sizeof(buf)-1,"SIZE(%s)",signal);

  /* use MdsValue to get the signal length */
  status = MdsValue(buf, &idesc, &null, 0);

  if ( !( (status & 1) == 1 ) ) {

    fprintf(stderr,"Unable to get length of %s.\n",signal);
    return -1;

  }

  /* return signal length */
  return size;

}



int main() {

  printf("Starting...\n");

  /* local vars */
  int connectionStatus;
  int connectionID;
  int shotNumber = 1;
  int size;
  int dtype_float = DTYPE_FLOAT;
  int null = 0;
  int sigDescrShape;
  int sigDescrData;
  int sigDescrDim;
  float *shape,
    *data,
    *dim;
  int numRows,
    numCols,
    numRowsDim,
    numColsDim;
  int ii, jj;
  char *treeName = "my_tree";
  
  connectionID = MdsConnect("localhost");
  if (connectionID == -1) {
    fprintf(stderr, "Connection Failed\n");
    return -1;
  }
  
  connectionStatus = MdsOpen(treeName, &shotNumber);
  if ( !status_ok(connectionStatus) ) {
    fprintf(stderr,"\nError message: %s.\n", MdsGetMsg(connectionStatus));
    MdsDisconnect();
    return -1;
  }
      
  size = get_signal_length("shape(DHI)");
  shape = (float *)malloc(size * sizeof(float));  
  sigDescrShape = descr(&dtype_float, shape, &size, &null);
  connectionStatus = MdsValue("shape(DHI)", &sigDescrShape, &null, 0);
  if ( !status_ok(connectionStatus) ) {
    fprintf(stderr,"\nError message: %s.\n", MdsGetMsg(connectionStatus));
    MdsDisconnect();
    return -1;

  }
  numRows = (int) shape[0];
  numCols = (int) shape[1];
  shape = (float *)malloc(size * sizeof(float));  


  size = get_signal_length("DHI");
  printf("Size DHI: %d\n", size);
  data = (float *)malloc(size * sizeof(float));  
  sigDescrData = descr(&dtype_float, data, &size, &null);
  connectionStatus = MdsValue("DHI", &sigDescrData, &null, 0);
  if ( !status_ok(connectionStatus) ) {
    fprintf(stderr,"\nError message: %s.\n", MdsGetMsg(connectionStatus));
    MdsDisconnect();
    return -1;

  }

  size = get_signal_length("shape(DHI)");
  shape = (float *)malloc(size * sizeof(float));  
  sigDescrShape = descr(&dtype_float, shape, &size, &null);
  connectionStatus = MdsValue("shape(DHI)", &sigDescrShape, &null, 0);
  if ( !status_ok(connectionStatus) ) {
    fprintf(stderr,"\nError message: %s.\n", MdsGetMsg(connectionStatus));
    MdsDisconnect();
    return -1;

  }
  numRows = (int) shape[0];
  numCols = (int) shape[1];
  shape = (float *)malloc(size * sizeof(float));  

  size = get_signal_length("shape(DIM_OF(DHI))");
  shape = (float *)malloc(size * sizeof(float));  
  sigDescrShape = descr(&dtype_float, shape, &size, &null);
  connectionStatus = MdsValue("shape(DIM_OF(DHI))", &sigDescrShape, &null, 0);
  if ( !status_ok(connectionStatus) ) {
    fprintf(stderr,"\nError message: %s.\n", MdsGetMsg(connectionStatus));
    MdsDisconnect();
    return -1;

  }
  numRowsDim = (int) shape[0];
  numColsDim = (int) shape[1];
  
  size = get_signal_length("DIM_OF(DHI)");
  printf("Size DHI DIM: %d\n", size);
  dim = (float *)malloc(size * sizeof(float));  
  sigDescrDim = descr(&dtype_float, dim, &size, &null);
  connectionStatus = MdsValue("DIM_OF(DHI)", &sigDescrDim, &null, 0);
  if ( !status_ok(connectionStatus) ) {
    MdsDisconnect();
    return -1;
  }

  
  /* Closing the tree */
  connectionStatus = MdsClose(treeName, &shotNumber);

  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {
      printf("|%g|\t", data[ii+jj*numRows]);
    }
    printf("\n----------------------------\n");
  }
  printf("**************************\n");
  printf("**************************\n");
  for (ii = 0; ii < numRowsDim; ii++) {
    for (jj = 0; jj < numColsDim; jj++) {
      printf("|%g|\t", dim[ii+jj*numRows]);
    }
    printf("\n----------------------------\n");
  }

  
  printf("Finishing...\n");
  
  return 0;

}


/***********************************
To populate this tree:
fuze@ops:~/Documents/myTree$ export my_tree_path=/home/fuze/Documents/myTree

webertr@fuze2:~/Documents/mdsplusCTest$ mdstcl
TCL> edit my_tree/new
TCL> add node DHI/usage=SIGNAL
TCL> add node DHI:LINE_INT/usage=NUMERIC
TCL> write
TCL> close
TCL> set tree my_tree
TCL> create pulse 1
TCL> quit


webertr@fuze2:~/Documents/mdsplusCTest$ mdstcl 
TCL> set tree my_tree/shot=1
TCL> put DHI "build_signal(DHI:LINE_INT,, [[0,1,2],[4,5,6]])"
TCL> close

webertr@fuze2:~/Documents/mdsplusCTest$ tdic
TDI> treeopen("my_tree", 1)
265388041
TDI> data(DHI)
[[1,2,3], [4,5,6], [7,8,9]]
TDI> data(DIM_OF(DHI))
[[0,1,2], [4,5,6]]
TDI> data(shape(DHI))
[3,3]
TDI> 

Then, to run the server:
webertr@fuze2:~/Documents/mdsplusCTest$ export my_tree_path=/home/webertr/Documents/my_tree
webertr@fuze2:~/Documents/mdsplusCTest$ mdsip -p 8000 -m -h mdsip.hosts


with mdsip.hosts:
/O=Grid/O=National Fusion Collaboratory/OU=MIT/CN=Thomas W. Fredian/Email=twf@psfc.mit.edu | twf
* | MAP_TO_LOCAL
* | webertr


Then, finally, 

gcc -Wall -I/usr/local/mdsplus/include -L/usr/local/mdsplus/lib -lMdsLib -lMdsShr main.c -o run


******************************************************************************/
