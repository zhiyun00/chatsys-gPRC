#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <grpcpp/grpcpp.h> // 引入头文件

// protobuf生成的头文件，包含两个信息和应用的头文件
#include "msg.grpc.pb.h"
#include "msg.pb.h"

// 这是通用工具
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class MsgServiceClient {
    public:
        MsgServiceClient(std::shared_ptr<Channel> channel)
            : stub_(MsgService::NewStub(channel)) {}


        void GetMsg(std::string username) {

            std::shared_ptr<grpc::ClientReaderWriter<Message, Message>> stream(
                stub_->GetMsg(&context));

            std::thread read_thread([this, &stream](std::string user) {
                Message response;
                while (stream->Read(&response)) {
                    if (response.type() != 2 or response.user() != user) {
                        std::cout << response.user() << ":" << response.msg() << std::endl;
                    }    
                }
            }, username);

            Message request;
            std::string msg;
            while (true) {
                std::getline(std::cin, msg);
                request.set_type(2);
                request.set_user(username);
                request.set_msg(msg);
                stream->Write(request);
            }   
            read_thread.join();
        }

    private:
        std::unique_ptr<MsgService::Stub> stub_;
        //客户端上下文。它可以用来传递额外的信息
        //服务器和/或调整某些RPC行为。
        ClientContext context;
};

int main(int argc, char** argv) {
    MsgServiceClient z_msg(grpc::CreateChannel(
        "localhost:50051", grpc::InsecureChannelCredentials()));
    std::string username;
    std::cout << "Enter the username:";
    std::cin >> username;
    z_msg.GetMsg(username);
    return 0;
}
