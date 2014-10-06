#include "jsondao.h"
#include "squawk-utils.h"

namespace squawk {
namespace db {

log4cxx::LoggerPtr JsonDao::logger(log4cxx::Logger::getLogger("squawk.db.JsonDao"));

JsonDao::JsonDao(squawk::SquawkConfig * config) {
    db = new Sqlite3Database();
    db->open(config->string_value(CONFIG_DATABASE_FILE));
}
JsonDao::~JsonDao() {}
void JsonDao::get_albums(::http::HttpRequest & request, ::http::HttpResponse & response) {
    std::cout << "get albums:\n" << request << std::endl;
    Sqlite3Statement * stmt = NULL;
    Sqlite3Statement * stmt_artist = NULL;
    try {
      stmt = db->prepare_statement("select name, genre, year, ROWID from tbl_cds_albums");
      stmt_artist = db->prepare_statement(
                  "select artist.ROWID, artist.name from tbl_cds_artists artist "
                  "JOIN tbl_cds_artists_albums m ON artist.ROWID = m.artist_id "
                  "where m.album_id=?");

      response << "[";
//      int step_status;
      bool first_album = true;
      while( stmt->step() ) {
//      do {
//          step_status = stmt->step();
//          if(step_status == SQLITE_ROW) {
              if( first_album ){
                  first_album = false;
              } else response << ", ";
              response << "{\"name\":\"" << squawk::utils::string::escape_json(stmt->get_string(0)) << "\", \"genre\":\"" << squawk::utils::string::escape_json(stmt->get_string(1));
              response << "\", \"year\":\"" << squawk::utils::string::escape_json(stmt->get_string(2)) << "\", \"id\":" << std::to_string(stmt->get_int(3)) << ", \"artists\":[";

//        int step_status_artist;
        stmt_artist->bind_int(1, stmt->get_int(3));
        bool first_artist = true;
        while( stmt_artist->step() ) {
//        do {
//            step_status_artist = stmt_artist->step();
//            if(step_status_artist == SQLITE_ROW) {
                if( first_artist ){
                    first_artist = false;
                } else response << ", ";

                response << "{\"id\":" << std::to_string(stmt_artist->get_int(0)) << ",  \"name\":\"" << squawk::utils::string::escape_json(stmt_artist->get_string(1)) << "\"}";
//            }
        } // while(step_status_artist != SQLITE_DONE);
        stmt_artist->reset();
        response << "]}";
//          }
      } // while(step_status != SQLITE_DONE);

      response << "]";

      stmt->reset();
      db->release_statement(stmt_artist);
      db->release_statement(stmt);

    } catch(DaoException & e) {
      LOG4CXX_FATAL(logger, "Can not get albums, Exception:" << e.code() << "-> " << e.what());
      if(stmt != NULL) db->release_statement(stmt);
      throw;
    } catch( ... ) {
        LOG4CXX_FATAL(logger, "Other Excpeption in get_albums.");
        throw;
    }
}
void JsonDao::get_artists(::http::HttpRequest & request, ::http::HttpResponse & response) {
    Sqlite3Statement * stmt = NULL;
    try {
        stmt = db->prepare_statement("select ROWID, name, letter from tbl_cds_artists order by name");
//        int step_status;
        response << "[";
        bool first_artist = true;
//        do {
//            step_status = stmt->step();
//            if(step_status == SQLITE_ROW) {
        while( stmt->step() ) {
                if( first_artist ){
                    first_artist = false;
                } else response << ", ";
                response << "{\"id\":" << std::to_string(stmt->get_int(0)) << ", \"name\":\"" << squawk::utils::string::escape_json(stmt->get_string(1)) <<
                            "\", \"letter\":\"" << stmt->get_string(2) << "\"}";
//            }
        } // while(step_status != SQLITE_DONE);
        stmt->reset();
        db->release_statement(stmt);
        response << "]";
    } catch(DaoException & e) {
        LOG4CXX_FATAL(logger, "Can not get artists, Exception:" << e.code() << "-> " << e.what());
        if(stmt != NULL) db->release_statement(stmt);
        throw;
    } catch( ... ) {
        LOG4CXX_FATAL(logger, "Other Excpeption in get_artists.");
        throw;
    }
}

}}
