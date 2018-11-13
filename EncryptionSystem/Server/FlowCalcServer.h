#pragma once
#include <iostream>
#include <string>



struct FlowComponent
{
private:
	typedef std::string FlowString;

private:
	FlowComponent* PrevFlow;
	FlowComponent* NextFlow;
	FlowString CurrentFlowName;

public:

};

class FlowCalcServer
{
public:
	// FlowCalcServer의 생성자입니다.
	FlowCalcServer();

	// FlowCalcServer의 소멸자입니다.
	~FlowCalcServer();

private:

};

