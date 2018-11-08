#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <pistache/endpoint.h>


using namespace Pistache;

struct HelloHandler : public Http::Handler {
    HTTP_PROTOTYPE(HelloHandler)

    void onRequest(const Http::Request& request, Http::ResponseWriter writer) {
        writer.send(Http::Code::Ok, "Hello, World!");
    }
};

int main(int argc, char **argv) {
    
    int opt;
    int maxmem = 100;
    int portnum = 2505;
    while ((opt = getopt(argc, argv, "mt:")) != -1) {
        switch (opt) {
            case 'm':
                // todo: fix this!
                maxmem = atoi(optarg);
                break;
            case 't':
                portnum = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-m max cache memory] [-t port number]\n", argv[0]);
                   exit(EXIT_FAILURE);
               }
    }

    // check that everything was set properly
    std::cout << "maxmem: " << maxmem << ", portnum: " << portnum << "\n";

    Http::listenAndServe<HelloHandler>("*:9080");
}