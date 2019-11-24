
import grpc
import server_pb2
import server_pb2_grpc
from google.protobuf import empty_pb2

channel = grpc.insecure_channel("localhost:10003")
stub = server_pb2_grpc.ServerStub(channel)

config = stub.GetConfig(empty_pb2.Empty())
state = stub.GetState(empty_pb2.Empty())
