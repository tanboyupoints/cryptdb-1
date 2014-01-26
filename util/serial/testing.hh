#include <string>

#define COLOR_END       "\033[0m"
#define GREEN_BEGIN     "\033[1;92m"
#define RED_BEGIN       "\033[1;31m"
#define PURPLE_BEGIN    "\033[1;35m"

static std::string
niceBool(bool b)
{
    return b ? (GREEN_BEGIN "success" COLOR_END)
             : (RED_BEGIN "failure" COLOR_END);
}

#define TEST(name)                                                  \
{                                                                   \
    std::cout << std::left << std::setw(30) << #name ": "           \
              << niceBool(name()) << std::endl;                     \
}
