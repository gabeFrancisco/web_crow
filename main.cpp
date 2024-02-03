#include <crow.h>
#include <pqxx/pqxx>

int main(int argc, char **argv)
{
    crow::SimpleApp app;
    std::string cstr = "postgresql://localhost/webcrow?user=postgres&password=1234";
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
        [&cstr]()
        {
            try
            {
                pqxx::connection conn{cstr};
                pqxx::work w{conn};
                pqxx::row r = w.exec1(
                    "select * from users limit 1");

                w.commit();

                auto page = crow::mustache::load("db.html");
                crow::mustache::context ctx({{"user_id", r[0].as<std::string>()}});
                return page.render(ctx);
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << std::endl;
                auto page = crow::mustache::load("error.html");
                return page.render();
            }
        });

    CROW_ROUTE(app, "/db").methods(crow::HTTPMethod::POST)([&cstr](const crow::request& req)
        {
            try{
                pqxx::connection conn{cstr};
                pqxx::work w{conn};

                auto params = req.get_body_params();
                
                std::string name = params.get("name");
                std::string qte = params.get("qte");
                std::string price = params.get("price");
                std::string userId = params.get("userId");

                w.exec0("insert into products(name, qte, price, userId) values ('" + name + "'," + qte + "," + price + "," + 
                    userId + ")");
                w.commit();

                auto page = crow::mustache::load("db.html");
                crow::mustache::context ctx({{"message", "New product added!"}});
                return page.render(ctx);
            }
            catch(const std::exception &e){
                std::cerr << e.what() << std::endl;
                auto page = crow::mustache::load("error.html");
                return page.render();
            }
        });

    app.port(1808)
        .multithreaded()
        .run();

    std::cout << "Hello world! God bless you!" << '\n';
    return 0;
}
