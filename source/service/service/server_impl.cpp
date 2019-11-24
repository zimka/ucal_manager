//
// Created by igor on 19.11.2019.
//

#include <server.pb.h>
#include <server.grpc.pb.h>

#include <grpcpp/server_builder.h>

#include <runtime/state_machine.h>
#include <runtime/core.h>
#include <common/keys.h>
#include <common/logger.h>
#include <common/exceptions.h>

#include "server_impl.h"

// TODO: shall we use AsyncService?

namespace service {
    class ServerImpl : public Server::Service {
    public:
        ~ServerImpl() override = default;
        ::grpc::Status GetConfig(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::JsonMsg* response) override;
        ::grpc::Status GetPlan(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::grpc::ServerWriter< ::BlockMsg>* writer) override;
        ::grpc::Status GetData(::grpc::ServerContext* context, const ::TimeStampMsg* request, ::grpc::ServerWriter< ::FrameMsg>* writer) override;
        ::grpc::Status GetState(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::StateMsg* response) override;
        ::grpc::Status SetPlan(::grpc::ServerContext* context, ::grpc::ServerReader< ::BlockMsg>* reader, ::OkMsg* response) override;
        ::grpc::Status SetConfig(::grpc::ServerContext* context, const ::JsonMsg* request, ::OkMsg* response) override;
        ::grpc::Status RunNext(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::OkMsg* response) override;
        ::grpc::Status Stop(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::OkMsg* response) override;
        ::grpc::Status GetLogs(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::LogMsg* response) override;
        ::grpc::Status DropLogs(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::OkMsg* response) override;

    private:
        runtime::StateMachine machine;
    };

    template <class F, class ... Args>
    ::grpc::Status catcherDecorator(F function, Args ... args) {
        try {
            return function(args...);
        } catch (common::StateViolationError& err) {
            return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, err.what());
        } catch (common::InputError& err) {
            return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, err.what());
        } catch (common::FatalError& err) {
            return grpc::Status(grpc::StatusCode::INTERNAL, err.what());
        } catch (const std::exception& e) {
            std::cout << "FATAL: " << e.what() << std::endl;
            return grpc::Status(grpc::StatusCode::UNKNOWN, "Something really unexpected happened");
        }
    }

    ::grpc::Status ServerImpl::GetConfig(::grpc::ServerContext* context, const ::google::protobuf::Empty* request,
                                         ::JsonMsg* response)
    {
        return catcherDecorator([response, this] {
            response->set_json(machine.getConfig().dump());
            return ::grpc::Status::OK;
        });
    }

    ::grpc::Status ServerImpl::GetPlan(::grpc::ServerContext* context, const ::google::protobuf::Empty* request,
                                       ::grpc::ServerWriter<::BlockMsg>* writer)
    {
        return catcherDecorator([this, writer] {
            for (auto& block : machine.getPlan()) {
                BlockMsg msg;
                msg.set_block_len_tu(block.block_len_tu);
                msg.set_read_step_tu(block.read_step_tu);
                msg.set_write_step_tu(block.write_step_tu);
                for (auto& entry : block.voltage_0) {
                    msg.add_voltage_0(entry);
                }
                for (auto& entry : block.voltage_1) {
                    msg.add_voltage_1(entry);
                }
                writer->Write(msg);
            }
            return ::grpc::Status::OK;
        });
    }

    ::grpc::Status ServerImpl::GetData(::grpc::ServerContext* context, const ::TimeStampMsg* request,
                                       ::grpc::ServerWriter<::FrameMsg>* writer)
    {
        return catcherDecorator([request, writer, this] {
            auto& storage = machine.getData();
            auto begin = storage.afterTs(common::TimeStamp {request->step(), request->count()});
            std::for_each(begin, storage.end(), [writer] (storage::Frame const& frame) {
                FrameMsg message;
                message.set_size(frame.size());
                auto timestamp = frame.getTs();
                TimeStampMsg* timeMsg = message.mutable_ts();
                timeMsg->set_step(timestamp.step);
                timeMsg->set_count(timestamp.count);
                auto* data = message.mutable_data();
                for (auto& key : common::SignalKey::_values()) {
                    if (key == +common::SignalKey::Undefined) {
                        continue;
                    }
                    auto& signalData = frame[key];
                    FrameMsg_SignalData signalDataMsg;
                    for (auto& value : signalData) {
                        signalDataMsg.add_data(value);
                    }
                    data->insert({key._to_string(), signalDataMsg});
                }
                writer->Write(message);
            });
            return grpc::Status::OK;
        });
    }

    ::grpc::Status ServerImpl::GetState(::grpc::ServerContext* context, const ::google::protobuf::Empty* request,
                                        ::StateMsg* response)
    {
        return catcherDecorator([this, response] {
            auto state = machine.getState();
            response->set_code(state);
            response->set_name(state._to_string());
            return grpc::Status::OK;
        });
    }

    ::grpc::Status ServerImpl::SetPlan(::grpc::ServerContext* context, ::grpc::ServerReader<::BlockMsg>* reader,
                                       ::OkMsg* response)
    {
        return catcherDecorator([this, reader] {
            BlockMsg message;
            runtime::Plan plan;
            while (reader->Read(&message)) {
                plan.emplace_back(runtime::Block {
                        message.write_step_tu(),
                        message.block_len_tu(),
                        message.read_step_tu(),
                });
                auto& block = plan.back();
                int size = message.voltage_0_size();
                block.voltage_0.reserve(size);
                for (int i = 0; i < size; ++i) {
                    block.voltage_0.push_back(message.voltage_0(i));
                }
                size = message.voltage_1_size();
                for (int i = 0; i < size; ++i) {
                    block.voltage_1.push_back(message.voltage_1(i));
                }
            }
            machine.setPlan(std::move(plan));
            return grpc::Status::OK;
        });
    }

    ::grpc::Status ServerImpl::SetConfig(::grpc::ServerContext* context, const ::JsonMsg* request, ::OkMsg* response)
    {
        return catcherDecorator([request, this] {
            json config (request->json());
            machine.setConfig(config);
            return grpc::Status::OK;
        });
    }

    ::grpc::Status ServerImpl::RunNext(::grpc::ServerContext* context, const ::google::protobuf::Empty* request,
                                       ::OkMsg* response)
    {
        return catcherDecorator([request, this] {
            machine.runNext();
            return grpc::Status::OK;
        });
    }

    ::grpc::Status ServerImpl::Stop(::grpc::ServerContext* context, const ::google::protobuf::Empty* request,
                                    ::OkMsg* response)
    {
        return catcherDecorator([this] {
            machine.stop();
            return grpc::Status::OK;
        });
    }

    ::grpc::Status ServerImpl::GetLogs(::grpc::ServerContext* context, const ::google::protobuf::Empty* request,
                                       ::LogMsg* response)
    {
        return catcherDecorator([response] {
            auto logger = common::createLogger("log.txt"); // FIXME: acquire logger from machine;
            auto lines = logger->getLines();
            for (auto& line : lines) {
                response->add_row(std::move(line));
            }
            return grpc::Status::OK;
        });
    }

    ::grpc::Status ServerImpl::DropLogs(::grpc::ServerContext* context, const ::google::protobuf::Empty* request,
                                        ::OkMsg* response)
    {
        return catcherDecorator([] {
            auto logger = common::createLogger("log.txt"); // FIXME: acquire logger from machine;
            logger->clean();
            return grpc::Status::OK;
        });
    }

    void RunService (std::string const& address_port) {

        grpc::ServerBuilder builder;
        ServerImpl service;

        builder.AddListeningPort(address_port, grpc::InsecureServerCredentials());
        builder.RegisterService(&service);
        std::unique_ptr<grpc::Server> serverPtr(builder.BuildAndStart());
        std::cout << "Server listening on " << address_port << std::endl;
        serverPtr->Wait();
    }
}
