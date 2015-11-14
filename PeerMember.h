#pragma once

#pragma unmanaged
#include<Windows.h>
#include<wlanapi.h>
#pragma managed
using namespace System;
namespace WlanHostedNetwork {
	public ref class PeerMember
	{
	public:
		property String^ AuthState
		{
			String^ get()
			{
				return _AuthState;
			}
		}
		property String^ MacAddress
		{
			String^ get()
			{
				return _MacAddress;
			}
		}

		property String^ Vendor
		{
			String^ get()
			{
				return _Vendor;
			}
			void set(String^ value)
			{
				_Vendor = value;
			}
		}
		PeerMember(WLAN_HOSTED_NETWORK_PEER_STATE auth)
		{
			switch (auth.PeerAuthState)
			{
			case WLAN_HOSTED_NETWORK_PEER_AUTH_STATE::wlan_hosted_network_peer_state_authenticated:
				_AuthState = gcnew String(L"已认证");
				break;
			default:
				_AuthState = gcnew String(L"无效");
			}
			_MacAddress = MacToString(auth.PeerMacAddress);
			_Vendor = L"";
		}
		~PeerMember()
		{

		}
		static String^ MacToString(DOT11_MAC_ADDRESS mac)
		{
			auto sb = gcnew Text::StringBuilder();
			for (size_t i = 0; i < 6; i++)
				sb->Append(mac[i].ToString("X2"))->Append("-");
			sb->Remove(sb->Length - 1, 1);
			return sb->ToString();
		}
	private:
		String^ _Vendor;
		String^ _AuthState;
		String^ _MacAddress;
	};
}