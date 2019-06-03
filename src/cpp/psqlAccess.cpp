#include "cpp/psqlAccess.h"


/******************************************************************************
 *
 * This is the source file to get psql access to the fuze experiment
 *
 ******************************************************************************/


/******************************************************************************
 * Function: getRecentShotNumbers
 * Inputs: int
 * Returns: gsl_vector *
 * Description: Get the most recent shot numbers and return them as a
 * gsl vector long in descending order
 ******************************************************************************/

gsl_vector_long *getRecentShotNumbers(int shotNums) {

  gsl_vector_long *nullVec = 0;
  gsl_vector_long *shotNumbers = gsl_vector_long_alloc(shotNums);
  std::ostringstream oss;
  int numRet;

  const std::string PSQL_CONNINFO = "dbname = fuzelogbook "
    "hostaddr = 10.10.10.240 "
    "user = csa "
    "password = csapsql";

  try {

    pqxx::connection conn(PSQL_CONNINFO);

    if (conn.is_open()) {
      std::cout << "Opened database successfully: " << conn.dbname() << std::endl;
    } else {
      std::cout << "Can't open database" << std::endl;
      return nullVec;
    }

    /* Create SQL statement */
    oss << "SELECT shotnumber FROM shots ORDER BY shotnumber DESC LIMIT " << shotNums;
    std::string sql = oss.str();

    /* Create a non-transactional object. */
    pqxx::nontransaction nonTran(conn);
      
    /* Execute SQL query */
    pqxx::result res(nonTran.exec(sql));
    
    /* Make sure there are enough entries */
    numRet = res.end() - res.begin();

    if (shotNums > numRet ) {
      shotNums = numRet;
    }

    /* List down all the records */
    for (pqxx::result::const_iterator cIter = res.begin(); 
	 cIter-res.begin() != shotNums ; ++cIter) {
      gsl_vector_long_set(shotNumbers, cIter-res.begin(), cIter[0].as<int>());
    }

    conn.disconnect();

  } catch (const std::exception &exp) {
    std::cerr << exp.what() << std::endl;
    return nullVec;
  }

  
  return shotNumbers;

}


/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/

static bool testPsqlGetShotNumbers();
static bool testGetRecentShotNumbers();

int testPsqlAccess() {

  if (!testGetRecentShotNumbers()) {
    std::cout << "Get recent shotnumber psql test Failed\n";
    return -1;
  }

  /* This avoids the warning */
  if (0) {
    testPsqlGetShotNumbers();
  }

  std::cout << "All Psql tests passed\n";

  return 0;

}

static bool testPsqlGetShotNumbers() {

  const std::string PSQL_CONNINFO = "dbname = fuzelogbook "
    "hostaddr = 10.10.10.240 "
    "user = csa "
    "password = csapsql";

  try {

    pqxx::connection conn(PSQL_CONNINFO);

    if (conn.is_open()) {
      std::cout << "Opened database successfully: " << conn.dbname() << std::endl;
    } else {
      std::cout << "Can't open database" << std::endl;
      return false;
    }

    /* Create SQL statement */
    std::string sql = "SELECT shotnumber FROM shots WHERE shotnumber>190528001 " 
      "AND spectroscopyenabled=1";

    /* Create a non-transactional object. */
    pqxx::nontransaction nonTran(conn);
      
    /* Execute SQL query */
    pqxx::result res(nonTran.exec(sql));
      
    /* List down all the records */
    for (pqxx::result::const_iterator cIter = res.begin(); cIter != res.end(); ++cIter) {
      std::cout << "ShotNumber = " << cIter[0].as<int>() << std::endl;
    }
    std::cout << "Operation done successfully" << std::endl;

    conn.disconnect ();

  } catch (const std::exception &exp) {
    std::cerr << exp.what() << std::endl;
    return false;
  }

  
  return true;

}

/******************************************************************************
 * Function: getRecentShotNumbers
 * Inputs: int
 * Returns: gsl_vector *
 * Description: Get the most recent shot numbers and return them as a
 * gsl vector
 ******************************************************************************/

static bool testGetRecentShotNumbers() {

  int num = 20;
  gsl_vector_long *test = getRecentShotNumbers(num);

  if (num == (int) (test->size)) {
    return true;
  } else {
    return false;
  }

}
