// WlanHostedNetwork.h

#pragma once
#pragma unmanaged
#include<Windows.h>
#include<wlanapi.h>
#include"PeerMember.h"
#include "WlanHostedNetworkStatus.h"
HANDLE hWLAN;
DWORD WlanVersion;

#pragma managed
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Collections::ObjectModel;

namespace WlanHostedNetwork 
{
	void WINAPI OnWirelessHostedNetworkMessage(PWLAN_NOTIFICATION_DATA data, PVOID context);
	void ClearItems();
    //WLAN承载网络
	public ref class WlanHostedNetwork :IDisposable
	{
	public:
		WlanHostedNetwork()
		{
			auto retv = WlanOpenHandle(2, nullptr, &WlanVersion, &hWLAN);
			ThrowIfFailed(retv);
			WlanRegisterNotification(hWLAN, WLAN_NOTIFICATION_SOURCE_HNWK, TRUE, OnWirelessHostedNetworkMessage, nullptr, nullptr, nullptr);
			if (CurrentStatus->HostedNetworkState == gcnew String(L"已激活"))
			{
				_IsEnabled = true;
				_IsStarted = true;
			}
		}
		~WlanHostedNetwork()
		{
			WlanCloseHandle(hWLAN, nullptr);
			Dispatcher->Invoke(gcnew Action(ClearItems));
		}
		static property Action<Action^>^ Dispatcher
		{
			Action<Action^>^ get()
			{
				return _runOnUIThread;
			}
			void set(Action<Action^>^ runOnUIThread)
			{
				_runOnUIThread = runOnUIThread;
			}
		}
		IntPtr UnsafeGetHandle()
		{
			return IntPtr(hWLAN);
		}
		property UInt32 WLANVersion
		{
			UInt32 get()
			{
				return WlanVersion;
			}
		}
		property bool IsEnabled
		{
			bool get()
			{
				return _IsEnabled;
			}
			void set(bool value)
			{
				if (value)
				{
					AllowHostedNetWork();
				}
				else
				{
					DisallowHostedNetWork();
				}
			}
		}
		property bool IsHalfHiddenNetwork
		{
			bool get()
			{
				return _IsHalfHiddenNetwork;
			}
			void set(bool value)
			{
				_IsHalfHiddenNetwork = value;
			}
		}
		property bool IsStarted
		{
			bool get()
			{
				return _IsStarted;
			}
			void set(bool value)
			{
				if (value)
				{
					StartHostedNetWork();
				}
				else
				{
					StopHostedNetWork();
				}
			}
		}

		static property ObservableCollection<PeerMember^>^ ConnectedPeerMembers
		{
			ObservableCollection<PeerMember^>^ get()
			{
				return _ConnectedPeerMembers;
			}
		}
		property String^ Key
		{
			String^ get()
			{
				return _Key;
			}
			void set(String^ value)
			{
				SetKEY(value);
			}
		}
		property String^ SSID
		{
			String^ get()
			{
				return _SSID;
			}
			void set(String^ value)
			{
				SetSSID(value);
			}
		}
		property WlanHostedNetworkStatus^ CurrentStatus
		{
			WlanHostedNetworkStatus^ get()
			{
				PWLAN_HOSTED_NETWORK_STATUS netStat = nullptr;
				auto queryStatus = WlanHostedNetworkQueryStatus(hWLAN, &netStat, nullptr);
				ThrowIfFailed(queryStatus);
				auto netStatus = gcnew WlanHostedNetworkStatus(netStat);
				WlanFreeMemory(netStat);
				return netStatus;
			}
		}
	protected:

		void AllowHostedNetWork()
		{
			PWLAN_HOSTED_NETWORK_REASON pFailReason = new WLAN_HOSTED_NETWORK_REASON();
			DWORD dwResult = 0;
			dwResult = WlanHostedNetworkForceStart(hWLAN, pFailReason, nullptr);
			ThrowForFailReasonAndDelete(pFailReason);
			ThrowIfFailed(dwResult);
			_IsEnabled = true;
		}

		void DisallowHostedNetWork()
		{
			PWLAN_HOSTED_NETWORK_REASON pFailReason = new WLAN_HOSTED_NETWORK_REASON();
			DWORD dwResult = 0;
			dwResult = WlanHostedNetworkForceStop(hWLAN, pFailReason, nullptr);
			ThrowForFailReasonAndDelete(pFailReason);
			ThrowIfFailed(dwResult);
			Dispatcher->Invoke(gcnew Action(ClearItems));
			_IsEnabled = false;
		}

		void StartHostedNetWork()
		{
			PWLAN_HOSTED_NETWORK_REASON pFailReason = new WLAN_HOSTED_NETWORK_REASON();
			DWORD dwResult = 0;
			dwResult = WlanHostedNetworkStartUsing(hWLAN, pFailReason, nullptr);
			ThrowForFailReasonAndDelete(pFailReason);
			ThrowIfFailed(dwResult);
			_IsStarted = true;
		}

		void StopHostedNetWork()
		{
			PWLAN_HOSTED_NETWORK_REASON pFailReason = new WLAN_HOSTED_NETWORK_REASON();
			DWORD dwResult = 0;
			dwResult = WlanHostedNetworkStopUsing(hWLAN, pFailReason, nullptr);
			ThrowForFailReasonAndDelete(pFailReason);
			ThrowIfFailed(dwResult);
			Dispatcher->Invoke(gcnew Action(ClearItems));
			_IsStarted = false;
		}

		void ThrowForFailReasonAndDelete(WLAN_HOSTED_NETWORK_REASON* pFailReason)
		{
			if (*pFailReason != WLAN_HOSTED_NETWORK_REASON::wlan_hosted_network_reason_success)
			{
				String^ FailString = gcnew String(L"");
				array<String^>^ reasons = { gcnew String(L"wlan_hosted_network_reason_success"),gcnew String(L"wlan_hosted_network_reason_unspecified"),gcnew String(L"wlan_hosted_network_reason_bad_parameters"),gcnew String(L"wlan_hosted_network_reason_service_shutting_down"),gcnew String(L"wlan_hosted_network_reason_insufficient_resources"),gcnew String(L"wlan_hosted_network_reason_elevation_required"),gcnew String(L"wlan_hosted_network_reason_read_only"),gcnew String(L"wlan_hosted_network_reason_persistence_failed"),gcnew String(L"wlan_hosted_network_reason_crypt_error"),gcnew String(L"wlan_hosted_network_reason_impersonation"),gcnew String(L"wlan_hosted_network_reason_stop_before_start"),gcnew String(L"wlan_hosted_network_reason_interface_available"),gcnew String(L"wlan_hosted_network_reason_interface_unavailable"),gcnew String(L"wlan_hosted_network_reason_miniport_stopped"),gcnew String(L"wlan_hosted_network_reason_miniport_started"),gcnew String(L"wlan_hosted_network_reason_incompatible_connection_started"),gcnew String(L"wlan_hosted_network_reason_incompatible_connection_stopped"),gcnew String(L"wlan_hosted_network_reason_user_action"),gcnew String(L"wlan_hosted_network_reason_client_abort"),gcnew String(L"wlan_hosted_network_reason_ap_start_failed"),gcnew String(L"wlan_hosted_network_reason_peer_arrived"),gcnew String(L"wlan_hosted_network_reason_peer_departed"),gcnew String(L"wlan_hosted_network_reason_peer_timeout"),gcnew String(L"wlan_hosted_network_reason_gp_denied"),gcnew String(L"wlan_hosted_network_reason_service_unavailable"),gcnew String(L"wlan_hosted_network_reason_device_change"),gcnew String(L"wlan_hosted_network_reason_properties_change"),gcnew String(L"wlan_hosted_network_reason_virtual_station_blocking_use"),gcnew String(L"wlan_hosted_network_reason_service_available_on_virtual_station") };
				FailString = reasons[*pFailReason];
				delete pFailReason;
				throw gcnew Net::WebException(L"热点状态异常:" + FailString);
			}
			delete pFailReason;
		}

		void Reactive()
		{
			DWORD dwResult = 0;
			BOOL bIsAllow = false;
			PWLAN_HOSTED_NETWORK_REASON dwFailedReason = new WLAN_HOSTED_NETWORK_REASON;
			dwResult = WlanHostedNetworkSetProperty(hWLAN, wlan_hosted_network_opcode_enable, sizeof(BOOL), &bIsAllow, dwFailedReason, nullptr);
			ThrowForFailReasonAndDelete(dwFailedReason);
			ThrowIfFailed(dwResult);
			Dispatcher->Invoke(gcnew Action(ClearItems));
		}

		unsigned char* Encode(String^ wp, int& length)
		{
			auto chs = System::Text::Encoding::GetEncoding("utf-8")->GetBytes(wp->ToCharArray());
			auto lst = gcnew System::Collections::Generic::List<unsigned char>();
			lst->AddRange(chs);
			lst->Add(0);
			chs = lst->ToArray();
			length = chs->Length;
			return reinterpret_cast<unsigned char*>(System::Runtime::InteropServices::Marshal::UnsafeAddrOfPinnedArrayElement(chs, 0).ToPointer());
		}

		void SetKEY(String^ key)
		{
			int length = 0;
			unsigned char* chkey = Encode(key, length);
			PWLAN_HOSTED_NETWORK_REASON dwFailedReason = new WLAN_HOSTED_NETWORK_REASON();
			DWORD dwResult = 0;
			dwResult = WlanHostedNetworkSetSecondaryKey(hWLAN, length, chkey, true, false, dwFailedReason, nullptr);
			ThrowForFailReasonAndDelete(dwFailedReason);
			ThrowIfFailed(dwResult);
			_Key = key;
		}

		void SetSSID(String^ ssidname)
		{
			int length = 0;
			char* chname = reinterpret_cast<char*>(Encode(ssidname, length));
			PWLAN_HOSTED_NETWORK_REASON dwFailedReason = new WLAN_HOSTED_NETWORK_REASON();
			WLAN_HOSTED_NETWORK_CONNECTION_SETTINGS cfg = { 0 };
			memcpy(cfg.hostedNetworkSSID.ucSSID, chname, length);
			cfg.hostedNetworkSSID.uSSIDLength = length - static_cast<int>(!_IsHalfHiddenNetwork);
			cfg.dwMaxNumberOfPeers = _MaxPeer;
			DWORD dwResult = WlanHostedNetworkSetProperty(hWLAN, wlan_hosted_network_opcode_connection_settings,
				sizeof(WLAN_HOSTED_NETWORK_CONNECTION_SETTINGS), &cfg, dwFailedReason, nullptr);
			ThrowForFailReasonAndDelete(dwFailedReason);
			ThrowIfFailed(dwResult);
			_SSID = ssidname;
		}

		void ThrowIfFailed(UInt32 stat)
		{
			switch (stat)
			{
			case ERROR_SUCCESS:
				break;
			case ERROR_ACCESS_DENIED:
				throw gcnew UnauthorizedAccessException();
			case ERROR_INVALID_HANDLE:
				throw gcnew InvalidOperationException(L"网络热点句柄已关闭");
			case ERROR_INVALID_PARAMETER:
				throw gcnew ArgumentException();
			case ERROR_INVALID_STATE:
				throw gcnew InvalidOperationException(L"网络热点被禁止或者正在关闭");
			case ERROR_NOT_ENOUGH_MEMORY:
				throw gcnew OutOfMemoryException();
			case ERROR_SERVICE_NOT_ACTIVE:
				throw gcnew InvalidOperationException(L"WLAN AutoConfig服务未运行。打开services.msc手动启用它，然后重试。");
			case ERROR_BAD_PROFILE:
				throw gcnew IO::InvalidDataException();
			case ERROR_NOT_SUPPORTED:
				throw gcnew NotSupportedException();
			default:
				throw gcnew Net::WebException(L"RPC调用期间发生异常:" + stat.ToString("X"));
			}
		}
	private:
		String^ _Key = L"";
		String^ _SSID = L"";
		bool _IsHalfHiddenNetwork;
		bool _IsEnabled;
		bool _IsStarted;
		int _MaxPeer = 20;
		static Action<Action^>^ _runOnUIThread;
		static ObservableCollection<PeerMember^>^ _ConnectedPeerMembers=gcnew ObservableCollection<PeerMember^>();
	};

	//下面的内容本应该出现在lambda表达式。由于c++/cli限制只好写到外面了。

	WLAN_HOSTED_NETWORK_PEER_STATE _newState;
	int _removeIndex;
	void ClearItems()
	{
		WlanHostedNetwork::ConnectedPeerMembers->Clear();
	}
	void AddItem()
	{
		WlanHostedNetwork::ConnectedPeerMembers->Add(gcnew PeerMember(_newState));
	}
	void AddItemOnUIThread()
	{
		WlanHostedNetwork::Dispatcher->Invoke(gcnew Action(AddItem));
	}
	void RemoveAt()
	{
		WlanHostedNetwork::ConnectedPeerMembers->RemoveAt(_removeIndex);
	}
	void RemoveItemOnUIThread()
	{
		auto mac = PeerMember::MacToString(_newState.PeerMacAddress);
		int i = 0;
		for each (PeerMember^ member in WlanHostedNetwork::ConnectedPeerMembers)
		{
			if (member->MacAddress == mac)
			{
				_removeIndex = i;
				WlanHostedNetwork::Dispatcher->Invoke(gcnew Action(RemoveAt));
				break;
			}
			i++;
		}
	}
	void WINAPI OnWirelessHostedNetworkMessage(PWLAN_NOTIFICATION_DATA data, PVOID context)
	{
		switch (data->NotificationCode)
		{
		case wlan_hosted_network_peer_state_change:
			if (data->dwDataSize > 0 && data->pData != nullptr)
			{
				auto state = (WLAN_HOSTED_NETWORK_DATA_PEER_STATE_CHANGE*)(data->pData);
				_newState = state->NewState;
				switch (_newState.PeerAuthState)
				{
				case wlan_hosted_network_peer_state_authenticated:
					AddItemOnUIThread();
					break;
				case wlan_hosted_network_peer_state_invalid:
					RemoveItemOnUIThread();
					break;
				default:
					break;
				}
			}
			break;
		default:
			break;
		}
	}
}
