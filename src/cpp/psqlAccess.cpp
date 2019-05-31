#include "cpp/psqlAccess.h"


/******************************************************************************
 *
 * This is the source file to get psql access to the fuze experiment
 *
 ******************************************************************************/



/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/

static bool testPsqlGetShotNumbers();

int testPsqlAccess() {

  if (!testPsqlGetShotNumbers()) {
    std::cout << "Get shotnumber psql test Failed\n";
    return -1;
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
    std::string sql = "SELECT shotnumber FROM shots WHERE shotnumber>190528001";

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
