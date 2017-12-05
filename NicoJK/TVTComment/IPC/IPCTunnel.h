#pragma once
#include "IPCProtocolStream.h"
#include "IPCMessage/IIPCMessage.h"
#include "../win32filestream.h"
#include <memory>
#include <exception>
#include <atomic>

namespace TVTComment
{

	class IPCTunnelConnectError :public std::exception
	{
	private:
		std::string what_arg;
	public:
		explicit IPCTunnelConnectError(const std::string &what_arg):what_arg(what_arg){}

		virtual const char *what() const override { return what_arg.c_str(); }
	};

	//�v���Z�X�ԒʐM�g���l��
	//Connect����������O��Send��Receive���ĂԂƖ���`����
	//Send��Receive���̂̍ē��͂ł��Ȃ����ʃX���b�h����Send��Receive�𓯎��ɌĂ��OK
	class IPCTunnel
	{
	private:
		IPCProtocolStream upstream;
		IPCProtocolStream downstream;
		std::atomic<HANDLE> connectThread;
		std::atomic<HANDLE> sendThread;
		std::atomic<HANDLE> receiveThread;

	public:
		IPCTunnel(const std::wstring &sendPipeName,const std::wstring &receivePipeName);
		void Connect();
		void Send(const IIPCMessage &msg);
		std::unique_ptr<IIPCMessage> Receive();
		void Cancel() noexcept;//��������Connect,Send,Receive���L�����Z�����Astd::system_error�ŋA�点��
		~IPCTunnel() noexcept;//Cancel���Ă񂾌�
	};
}