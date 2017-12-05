#pragma once
#include "../TVTestPlugin.h"
#include "../CommentWindow.h"
#include "../Util.h"
#include "IPC/IPCTunnel.h"
#include "ChannelInfo.h"
#include <vector>
#include <locale>
#include <codecvt>
#include <memory>
#include <atomic>
#include <thread>
#include <ppltasks.h>

#ifndef UNICODE
#error Character set must be Unicode!
#endif

namespace TVTComment
{
	enum class TVTCommentCommand
	{
		ShowWindow,
	};

	//TVTComment�̍ŏ�ʃN���X
	//���̃N���X�̃C���X�^���X�̐������Ԃ͐����E�B���h�E�̂�蒷���K�v������
	//�G���[���Ƀv���O�C���𖳌������邱�Ƃ�����
	class TVTComment
	{
	private:
		TVTest::CTVTestApp *tvtest;
		CCommentWindow *commentWindow;
		HWND dialog;

		std::time_t lastTOT;
		uint16_t lastEventId;

		int errorCount;//��M�G���[���N������
		static constexpr int FETALERROR_COUNT = 10;//errorCount�����̒l�ȏ�ɂȂ�ƃv���O�C���𖳌�������

		std::atomic<bool> isClosing;//ClosingIPCMessage���󂯎��v���O�C������悤�Ƃ��Ă���

		std::atomic<bool> isConnected;
		std::unique_ptr<IPCTunnel> ipcTunnel;

		std::vector<ChannelInfo> channelList;

		concurrency::cancellation_token_source cancel;//����TVTComment�N���X�̃C���X�^���X��delete����Ƃ��Ɏg��
		concurrency::task<void> asyncTask;

		std::wstring collectExePath;//�N������EXE�̃p�X

		static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf8_utf16_conv;

	private:
		void doConnect();
		void receiveLoop();
		void processReceivedMessage(const IIPCMessage &msg);
		bool sendMessage(const IIPCMessage &msg);

		static std::time_t SystemTimeToUnixTime(const SYSTEMTIME &time);
		void sendCurrentChannelIPCMessage(const TVTest::ChannelInfo &ci,const TVTest::ProgramInfo &pi);

	public:
		static constexpr int WM_USERINTERACTIONREQUEST = WM_APP + 1001;//�_�C�A���O�{�b�N�X�̕\���Ȃǃ��[�U�[�ւ̃��b�Z�[�W��\������
		static constexpr int WM_DISABLEPLUGIN = WM_APP + 1002;//�v���O�C���𖳌�������i�ʃX���b�h���疳��������Ƃ��Ɏg���j
		static constexpr int WM_ONCHANNELLISTCHANGE = WM_APP + 1003;//�����o�֐�OnChannelListChange���Ăԁi�ʃX���b�h���瑗��Ƃ��Ɏg���j
		static constexpr int WM_ONCHANNELSELECTIONCHANGE = WM_APP + 1004;//�����o�֐�OnChannelSelectionChange���Ăԁi�ʃX���b�h���瑗��Ƃ��Ɏg���j
		static constexpr int WM_SETCHATOPACITY = WM_APP + 1005;//�R�����g���ߓx��ݒ肷�� wParam�ɓ��ߓx��n��(0~255)

		enum class UserInteractionRequestType{ConnectSucceed,ConnectFail,InvalidMessage,ReceiveError,SendError, FetalErrorInTask};

	public:
		bool IsConnected() const;
		TVTComment(TVTest::CTVTestApp *tvtest,CCommentWindow *commentWindow,const std::wstring &collectExePath);
		INT_PTR DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		void OnChannelListChange();
		void OnChannelSelectionChange();
		void OnForward(std::time_t tot);//TOT�����̍X�V�Ԋu���ׂ����Ԋu�ŌĂ�
		void OnCommandInvoked(TVTCommentCommand command);
		~TVTComment() noexcept;
	};
}