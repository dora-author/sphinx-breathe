#include <stdio.h>
#include <iostream>
#include <map>
#include <memory>
#include <chrono>

#include "utils/log_utils.h"
#include "utils/string_utils.h"
#include "include/common/request_item.h"
#include "morai_grpc_server.h"


int main()
{
	MCOMM_LOG("Start...1");
	auto myServer = new morai::comm::grpc::MoraiGrpcServer();

	MCOMM_LOG("call run server");
	myServer->RunServer("0.0.0.0", 7789);
	MCOMM_LOG(morai::comm::StringUtil::Format("my server state : %s", myServer->IsRunning() ? "Running" : "Stopped"));
	
	MCOMM_LOG("Press a key to continue...");

	uint64_t tick = 0;
	do 
	{
		auto request = myServer->GetRequestItem();
		if (request != nullptr)
		{
			auto real_request = std::static_pointer_cast<morai::comm::RequestItem>(request);

			// process request
			MCOMM_LOG(morai::comm::StringUtil::Format("[%s] rpc called, %lld", real_request->GetType(), tick));

			// std::this_thread::sleep_for(std::chrono::seconds(3));
			real_request->SetResponse(std::make_shared<uint64_t>(tick));

			tick++;
		}
	}
	while (std::cin.get() != '\n');

	MCOMM_LOG("call stop server\n");
	myServer->StopServer();
	MCOMM_LOG(morai::comm::StringUtil::Format("my server state : %s\n", myServer->IsRunning() ? "Running" : "Stopped"));

	delete myServer;
	myServer = nullptr;

	return 0;
}