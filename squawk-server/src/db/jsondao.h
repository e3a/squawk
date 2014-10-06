#ifndef JSONDAO_H
#define JSONDAO_H

#include "http.h"
#include "../squawkconfig.h"
#include "sqlite3database.h"

#include "log4cxx/logger.h"

namespace squawk {
namespace db {

class JsonDao {
public:
    JsonDao(squawk::SquawkConfig * config);
    JsonDao() {};
    virtual ~JsonDao();

    void get_albums(::http::HttpRequest & request, ::http::HttpResponse & response);
    void get_artists(::http::HttpRequest & request, ::http::HttpResponse & response);

private:
    static log4cxx::LoggerPtr logger;
    Sqlite3Database * db;
};
}}
#endif // JSONDAO_H
