#include <crow.h>
#include <pqxx/pqxx>

int main(int argc, char **argv)
{
    crow::SimpleApp app;

    // Base home route
    CROW_ROUTE(app, "/")
    ([]()
     { 
        auto page = crow::mustache::load_text("index.html");
        return page; });

    // Dynamic route
    CROW_ROUTE(app, "/message/<string>")
    ([](std::string name)
     { 
        auto index = crow::mustache::load("name.html");
        crow::mustache::context ctx({{"person", name}});
        return index.render(ctx); });

    CROW_ROUTE(app, "/add/<int>/<int>")
    ([](int a, int b)
     { return std::to_string(a + b); });

    CROW_ROUTE(app, "/db")
    (
        []()
        {
            try
            {
                pqxx::connection conn{"postgresql://localhost/DoctorKey?user=postgres&password=1234"};
                pqxx::work tx{conn};

                pqxx::row r = tx.exec1(
                    "select * from \"Users\" limit 1");

                tx.commit();

                return r[0].as<std::string>();
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << std::endl;
                return std::string("Error");
            }
        });

    app.port(1808)
        .multithreaded()
        .run();

    std::cout << "Hello world! God bless you!" << '\n';
    return 0;
}
