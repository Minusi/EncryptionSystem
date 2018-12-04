/**********************************************************************
 * NetMessage.cpp는 네트워크 전송에서 사용되는 각종 메시지들을 모아놓은 
 * 컨테이너 파일입니다. 네트워크 상에서 send/recv 되는 모든 플래그 메시
 * 지들은 이곳에서 정의되며 다음과 같은 메시지들이 이곳에 포함되고 있습
 * 니다:
 *
 * CalcClient	- 계산 클라이언트 메시지
 * CalcServer	- 계산 서버 메시지
 * DH			- DH 알고리즘 공개키 공유 메시지
 **********************************************************************/

#include "Server\CalcServer.h"
#include "Client\CalcClient.h"

#include "Encryption\DH\DHEncryption.h"
#include "Encryption\RSA\RSAEncryption.h"




/**********************************************************************
 * CalcClient에서 사용하는 네트워크 플래그 메시지
 **********************************************************************/
std::string const CalcClient::Request::LOGINFLAG("REQUEST LOGIN FLAG");
std::string const CalcClient::Request::USERLOGIN("REQUEST USER LOGIN");
std::string const CalcClient::Request::USERSIGNUP("REQUEST USER SIGNUP");
std::string const CalcClient::Request::INTEGERVALUE("REQUEST INTEGER VALUE");
std::string const CalcClient::Request::DISCONNECT("REQUEST DISCONNECT");
std::string const CalcClient::Request::SYNCENCRYPT("REQUEST SYNC ENCRYPT");



/**********************************************************************
 * CalcServer에서 사용하는 네트워크 플래그 메시지
 **********************************************************************/
std::string const CalcServer::Respond::LoginFlag::NOLOGIN{ "RESPOND NOLOGIN" };
std::string const CalcServer::Respond::LoginFlag::LOGIN{ "RESPOND LOGIN" };

std::string const CalcServer::Respond::Login::MISMATCH("RESPOND MISMATCH");
std::string const CalcServer::Respond::Login::SUCCESS("RESPOND LOGIN SUCCESS");

std::string const CalcServer::Respond::SignUp::IDALREADY("RESPOND ID ALREADY");
std::string const CalcServer::Respond::SignUp::SUCCESS("RESPOND SIGNUP SUCEESS");

std::string const CalcServer::Respond::Calc::SUCCESS("RESPOND CALC SUCCESS");
std::string const CalcServer::Respond::Calc::NONVALID("RESPOND CALC NONVALID");

std::string const CalcServer::Respond::Encrypt::ACCEPT("RESPOND ACCEPT");
std::string const CalcServer::Respond::Encrypt::REJECT("RESPOND REJECT");


/**********************************************************************
 * DH에서 사용하는 네트워크 플래그 메시지
 **********************************************************************/
std::string const DHEncryption::Message::Common::ACCEPT("DH ACCEPT");
std::string const DHEncryption::Message::Common::RECVKEY("DH RECV KEY");
std::string const DHEncryption::Message::Common::INVALIDKEY("DH INVALID KEY");

std::string const DHEncryption::Message::Host::SENDPRIMES("DH SEND PRIMES");
std::string const DHEncryption::Message::Host::SENDHOSTKEY("DH SEND HOST KEY");

std::string const DHEncryption::Message::Guest::INVALIDPRIMES("DH INVALID PRIMES");
std::string const DHEncryption::Message::Guest::RECVPRIMES("DH RECV PRIMES");
std::string const DHEncryption::Message::Guest::SENDGUESTKEY("DH SEND GUEST KEY");



/**********************************************************************
 * RSA에서 사용하는 네트워크 플래그 메시지
 **********************************************************************/
std::string const RSAEncryption::Message::Common::ACCEPT("RSA ACCEPT");
std::string const RSAEncryption::Message::Common::RECVKEY("RSA RECV KEY");
std::string const RSAEncryption::Message::Common::INVALIDKEY("RSA INVALID KEY");

std::string const RSAEncryption::Message::Host::SENDHOSTKEY("RSA SEND HOST KEY");

std::string const RSAEncryption::Message::Guest::SENDGUESTKEY("RSA SEND GUEST KEY");