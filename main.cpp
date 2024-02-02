#include <crow.h>

int main(int argc, char **argv)
{
    crow::SimpleApp app;
    CROW_ROUTE(app, "/")
    ([]()
    { 
        auto page = crow::mustache::load_text("index.html");
        return page; 
    });

    app.port(1808)
        .multithreaded()
        .run();

    std::cout << "Hello world! God bless you!" << '\n';
    return 0;
}
