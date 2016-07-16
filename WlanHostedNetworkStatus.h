#pragma once
#include<wlanapi.h>
using namespace System;

namespace WlanHostedNetwork
{
	public ref class WlanHostedNetworkStatus
	{
	internal:
		WlanHostedNetworkStatus(WLAN_HOSTED_NETWORK_STATUS* NetStatus)
		{
			_HostedNetworkBSSID = PeerMember::MacToString(NetStatus->wlanHostedNetworkBSSID);
			_ChannelFrequency = NetStatus->ulChannelFrequency;
			_PeerMembers = gcnew array<PeerMember^>(NetStatus->dwNumberOfPeers);
			for (size_t i = 0; i < NetStatus->dwNumberOfPeers; i++)
				_PeerMembers[i] = gcnew PeerMember(NetStatus->PeerList[i]);
			array<unsigned char>^ chs = gcnew array<unsigned char>(8);
			for (size_t i = 0; i < 8; i++)
				chs[i] = NetStatus->IPDeviceID.Data4[i];
			_IPDeviceID = Guid(NetStatus->IPDeviceID.Data1, NetStatus->IPDeviceID.Data2, NetStatus->IPDeviceID.Data3, chs);
			switch (NetStatus->HostedNetworkState)
			{
			case WLAN_HOSTED_NETWORK_STATE::wlan_hosted_network_active:
				_HostedNetworkState = gcnew String(L"已激活");
				break;
			case WLAN_HOSTED_NETWORK_STATE::wlan_hosted_network_idle:
				_HostedNetworkState = gcnew String(L"空闲");
				break;
			default:
				_HostedNetworkState = gcnew String(L"不可用");
				break;
			}
			_NumberOfPeers= NetStatus->dwNumberOfPeers;
			switch (NetStatus->dot11PhyType)
			{
			case DOT11_PHY_TYPE::dot11_phy_type_unknown:
				_Dot11PhyType = gcnew String(L"未知类型");
				break;
			case DOT11_PHY_TYPE::dot11_phy_type_fhss:
				_Dot11PhyType = gcnew String(L"跳频扩展频谱(蓝牙)");
				break;
			case DOT11_PHY_TYPE::dot11_phy_type_dsss:
				_Dot11PhyType = gcnew String(L"直接序列扩频");
				break;
			case DOT11_PHY_TYPE::dot11_phy_type_irbaseband:
				_Dot11PhyType = gcnew String(L"红外基带");
				break;
			case DOT11_PHY_TYPE::dot11_phy_type_ofdm:
				_Dot11PhyType = gcnew String(L"正交频分复用(802.11a)");
				break;
			case DOT11_PHY_TYPE::dot11_phy_type_hrdsss:
				_Dot11PhyType = gcnew String(L"高频直接序列扩频");
				break;
			case DOT11_PHY_TYPE::dot11_phy_type_erp:
				_Dot11PhyType = gcnew String(L"扩展频率(802.11g)");
				break;
			case DOT11_PHY_TYPE::dot11_phy_type_ht:
				_Dot11PhyType = gcnew String(L"高吞吐(802.11n)");
				break;
			case DOT11_PHY_TYPE::dot11_phy_type_vht:
				_Dot11PhyType = gcnew String(L"超高吞吐(802.11ac)");
				break;
			case DOT11_PHY_TYPE::dot11_phy_type_IHV_start:
				_Dot11PhyType = gcnew String(L"dot11_phy_type_IHV_start");
				break;
			case DOT11_PHY_TYPE::dot11_phy_type_IHV_end:
				_Dot11PhyType = gcnew String(L"dot11_phy_type_IHV_end");
				break;
			default:
				_Dot11PhyType = "";
				break;
			}
		}
	public:
		~WlanHostedNetworkStatus()
		{
		}
		property String^ HostedNetworkState
		{
			String^ get()
			{
				return _HostedNetworkState;
			}
		}
		property Guid IPDeviceID
		{
			Guid get()
			{
				return _IPDeviceID;
			}
		}
		property String^ HostedNetworkBSSID
		{
			String^ get()
			{
				return _HostedNetworkBSSID;
			}
		}
		property String^ Dot11PhyType
		{
			String^ get()
			{
				return _Dot11PhyType;
			}
		}
		property UInt32 ChannelFrequency
		{
			UInt32 get()
			{
				return _ChannelFrequency;
			}
		}
		property int NumberOfPeers
		{
			int get()
			{
				return _NumberOfPeers;
			}
		}
		property array<PeerMember^>^ PeerMembers
		{
			array<PeerMember^>^ get()
			{
				return _PeerMembers;
			}
		}
	private:
		String^ _HostedNetworkState;
		Guid _IPDeviceID;
		String^ _HostedNetworkBSSID;
		String^ _Dot11PhyType;
		UInt32 _ChannelFrequency;
		int _NumberOfPeers;
		array<PeerMember^>^ _PeerMembers;
	};
}