#include "EncryptionBase.h"

#include <iostream>

#include "Debug\Debug.h"



EncryptionBase::EncryptionBase(INetDelegateComponent* InNetDeleComp)
	: NetComponent(InNetDeleComp)
{
}