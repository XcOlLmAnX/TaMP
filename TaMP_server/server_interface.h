#include "server_logic.h"

class ServerInterface {
public:
    ServerInterface(int port, ServerLogic& logic);
    ~ServerInterface();

    void run();

private:
    int port;
    int serverSocket;
    ServerLogic& logic;

    void initSocket();
    void handleClient(int clientSocket);
};