// WlanHostedNetwork.h

#pragma once
#pragma unmanaged
#include<Windows.h>
#include<wlanapi.h>
#include"PeerMember.h"
HANDLE hWLAN;
DWORD WlanVersion;
WLAN_HOSTED_NETWORK_STATUS NetStatus;
#pragma managed
using namespace System;

namespace WlanHostedNetwork {

	public ref class WlanHostedNetwork :IDisposable
	{
	public:
		WlanHostedNetwork()
		{
			auto retv = WlanOpenHandle(2, nullptr, &WlanVersion, &hWLAN);
			ThrowIfFailed(retv);
			if (HostedNetworkState == gcnew String(L"已激活"))
			{
				_IsEnabled = true;
				_IsStarted = true;
			}
		}
		~WlanHostedNetwork()
		{
			WlanCloseHandle(hWLAN, nullptr);
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
		property String^ Dot11PhyType
		{
			String^ get()
			{
				RefreshStatus();
				String^ ErrStr;
				switch (NetStatus.dot11PhyType)
				{
				case DOT11_PHY_TYPE::dot11_phy_type_unknown:
					ErrStr = gcnew String(L"dot11_phy_type_unknown");
					break;
				case DOT11_PHY_TYPE::dot11_phy_type_fhss:
					ErrStr = gcnew String(L"dot11_phy_type_fhss");
					break;
				case DOT11_PHY_TYPE::dot11_phy_type_dsss:
					ErrStr = gcnew String(L"dot11_phy_type_dsss");
					break;
				case DOT11_PHY_TYPE::dot11_phy_type_irbaseband:
					ErrStr = gcnew String(L"dot11_phy_type_irbaseband");
					break;
				case DOT11_PHY_TYPE::dot11_phy_type_ofdm:
					ErrStr = gcnew String(L"dot11_phy_type_ofdm");
					break;
				case DOT11_PHY_TYPE::dot11_phy_type_hrdsss:
					ErrStr = gcnew String(L"dot11_phy_type_hrdsss");
					break;
				case DOT11_PHY_TYPE::dot11_phy_type_erp:
					ErrStr = gcnew String(L"dot11_phy_type_erp");
					break;
				case DOT11_PHY_TYPE::dot11_phy_type_ht:
					ErrStr = gcnew String(L"dot11_phy_type_ht");
					break;
				case DOT11_PHY_TYPE::dot11_phy_type_vht:
					ErrStr = gcnew String(L"dot11_phy_type_vht");
					break;
				case DOT11_PHY_TYPE::dot11_phy_type_IHV_start:
					ErrStr = gcnew String(L"dot11_phy_type_IHV_start");
					break;
				case DOT11_PHY_TYPE::dot11_phy_type_IHV_end:
					ErrStr = gcnew String(L"dot11_phy_type_IHV_end");
					break;
				default:
					ErrStr = "";
				}
				return ErrStr;
			}
		}
		property UInt32 NumberOfPeers
		{
			UInt32 get()
			{
				RefreshStatus();
				return NetStatus.dwNumberOfPeers;
			}
		}
		property String^ HostedNetworkState
		{
			String^ get()
			{
				RefreshStatus();
				switch (NetStatus.HostedNetworkState)
				{
				case WLAN_HOSTED_NETWORK_STATE::wlan_hosted_network_active:
					return gcnew String(L"已激活");
				case WLAN_HOSTED_NETWORK_STATE::wlan_hosted_network_idle:
					return gcnew String(L"空闲");
				default:
					return gcnew String(L"不可用");
				}
			}
		}
		property Guid IPDeviceID
		{
			Guid get()
			{
				RefreshStatus();
				array<unsigned char>^ chs = gcnew array<unsigned char>(8);
				for (size_t i = 0; i < 8; i++)
					chs[i] = NetStatus.IPDeviceID.Data4[i];
				return Guid(NetStatus.IPDeviceID.Data1, NetStatus.IPDeviceID.Data2, NetStatus.IPDeviceID.Data3, chs);
			}
		}
		property array<PeerMember^>^ PeerMembers
		{
			array<PeerMember^>^ get()
			{
				RefreshStatus();
				array<PeerMember^>^ chs = gcnew array<PeerMember^>(NetStatus.dwNumberOfPeers);
				for (size_t i = 0; i < NetStatus.dwNumberOfPeers; i++)
					chs[i] = gcnew PeerMember(NetStatus.PeerList[i]);
				return chs;
			}
		}
		property UInt32 ChannelFrequency
		{
			UInt32 get()
			{
				RefreshStatus();
				return NetStatus.ulChannelFrequency;
			}
		}
		property String^ HostedNetworkBSSID
		{
			String^ get()
			{
				RefreshStatus();
				return PeerMember::MacToString(NetStatus.wlanHostedNetworkBSSID);
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
	protected:
		virtual void RefreshStatus()
		{
			PWLAN_HOSTED_NETWORK_STATUS NetStat = nullptr;
			auto stat = WlanHostedNetworkQueryStatus(hWLAN, &NetStat, nullptr);
			ThrowIfFailed(stat);
			NetStatus = *NetStat;
			WlanFreeMemory(NetStat);
		}
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
		}
		char* WcharToChar(const wchar_t* wp)
		{
			char *m_char;
			int len = WideCharToMultiByte(CP_ACP, 0, wp, wcslen(wp), NULL, 0, NULL, NULL);
			m_char = new char[len + 1];
			WideCharToMultiByte(CP_ACP, 0, wp, wcslen(wp), m_char, len, NULL, NULL);
			m_char[len] = '\0';
			return m_char;
		}
		wchar_t* ToUnmanagedString(String^ s)
		{
			return (wchar_t*)(System::Runtime::InteropServices::Marshal::UnsafeAddrOfPinnedArrayElement(s->ToCharArray(), 0).ToPointer());
		}
		void SetKEY(String^ key)
		{
			unsigned char* chkey = reinterpret_cast<unsigned char*>(WcharToChar(ToUnmanagedString(key)));
			PWLAN_HOSTED_NETWORK_REASON dwFailedReason = new WLAN_HOSTED_NETWORK_REASON();
			DWORD dwResult = 0;
			dwResult = WlanHostedNetworkSetSecondaryKey(hWLAN, key->Length + 1, chkey, true, false, dwFailedReason, nullptr);
			delete[] chkey;
			ThrowForFailReasonAndDelete(dwFailedReason);
			ThrowIfFailed(dwResult);
			_Key = key;
		}
		void SetSSID(String^ ssidname)
		{
			char* chname = WcharToChar(ToUnmanagedString(ssidname));
			PWLAN_HOSTED_NETWORK_REASON dwFailedReason = new WLAN_HOSTED_NETWORK_REASON();
			WLAN_HOSTED_NETWORK_CONNECTION_SETTINGS cfg = { 0 };
			memcpy(cfg.hostedNetworkSSID.ucSSID, chname, ssidname->Length);
			cfg.hostedNetworkSSID.uSSIDLength = ssidname->Length;
			cfg.dwMaxNumberOfPeers = _MaxPeer;
			DWORD dwResult = WlanHostedNetworkSetProperty(hWLAN, wlan_hosted_network_opcode_connection_settings,
				sizeof(WLAN_HOSTED_NETWORK_CONNECTION_SETTINGS), &cfg, dwFailedReason, nullptr);
			delete[] chname;
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
		bool _IsEnabled;
		bool _IsStarted;
		int _MaxPeer = 20;
	};
}
