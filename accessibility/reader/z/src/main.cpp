
#include "application"

using namespace std;


int main() {
    Application app;

    app.run(fopen("text", "r"));
}
