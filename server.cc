#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h> // 引入头文件

// protobuf生成的头文件，包含两个信息和应用的头文件
#include "msg.grpc.pb.h"
#include "msg.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

class MyMsgService final : public MsgService::Service {
  public:
    Status GetMsg(ServerContext* context,
                  grpc::ServerReaderWriter<Message, Message>* stream) override {

      Message request, response;
    
      if (stream->Read(&request)) {
        std::string username = request.user();
        AddClient(stream, username, "joined the chat");
      }
    
      while (stream->Read(&request)) {
        response.set_type(request.type());
        response.set_user(request.user());
        response.set_msg(request.msg());
        BroadcastMessage(response);
      }
    
      return Status::OK;
    }

  private:
    using StreamMap = std::unordered_set<grpc::ServerReaderWriter<Message, Message>*>;
    StreamMap clients_;
    void BroadcastMessage(const Message& message) {
      for (auto& client : clients_) {
        client->Write(message);
      }
    }
    void AddClient(grpc::ServerReaderWriter<Message, Message>* client,
                    const std::string& join_user, std::string join_message) {
      clients_.insert(client);
      Message join_response;
      join_response.set_type(1);
      join_response.set_user(join_user);
      join_response.set_msg(join_message);
      BroadcastMessage(join_response);
    }
};

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  MyMsgService service;

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

int main(int argc, char** argv) {
  RunServer();
  return 0;
}
