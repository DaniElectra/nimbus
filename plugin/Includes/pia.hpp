#pragma once
#include <3ds.h>

namespace nn {

namespace pia {

namespace common {

struct InetAddress {
    u8 m_ipAddress[4];
    u16 m_port;
    u8 _padding[2];
};
static_assert(sizeof(InetAddress) == 0x8, "sizeof(InetAddress) != 0x8");

struct StationAddress {
    u32 vtable;
    InetAddress m_inetAddress;
    u16 m_extensionID;
    u8 _padding[2];
};
static_assert(sizeof(StationAddress) == 0x10, "sizeof(StationAddress) != 0x10");

// The 3dbrew RecursiveLock is implemented the same way as the CriticalSection, so this should work
using CriticalSection = RecursiveLock;

}

namespace transport {

// Main class is stubbed, only implementing subtypes for now
struct Station {
    struct IdentificationToken {
        u8 m_token[0x20]; // ASCII
    };
    static_assert(sizeof(IdentificationToken) == 0x20, "sizeof(IdentificationToken) != 0x20");

    struct PlayerName {
        char16_t m_name[16 + 1]; // 16 characters + NULL terminator
        u8 m_length; // Number of characters
        u8 m_language;
    };
    static_assert(sizeof(PlayerName) == 0x24, "sizeof(PlayerName) != 0x24");

    struct IdentificationInfo {
        IdentificationToken m_identificationToken;
        PlayerName m_playerName;
        u8 m_platformID;
        u8 _padding[3];
        u32 m_ackID;
    };
    static_assert(sizeof(IdentificationInfo) == 0x4C, "sizeof(IdentificationInfo) != 0x4C");
};

struct StationLocation {
    u32 vtable;
    common::StationAddress m_stationAddress;
    u32 m_constantID; // PID
    u32 m_variableID; // CID
    u32 m_serviceVariableID; // RVCID
    u8 m_urlType;
    u8 m_nexStreamID;
    u8 m_nexStreamType;
    u8 m_natMapping;
    u8 m_natFiltering;
    u8 m_natFlags;
    u8 m_probeinit;
    u8 _padding;
};
static_assert(sizeof(StationLocation) == 0x28, "sizeof(StationLocation) != 0x28");

struct StationConnectionInfo {
    u32 vtable;
    StationLocation m_publicLocation;
    StationLocation m_privateLocation;
};
static_assert(sizeof(StationConnectionInfo) == 0x54, "sizeof(StationConnectionInfo) != 0x54");

struct StationConnectionInfoTable {
    u32 vtable;
    StationConnectionInfo m_localConnectionInfo;
    StationConnectionInfo m_remoteConnectionInfo;
    common::StationAddress m_stationAddress; // TODO - Who does this station belong to???
    StationConnectionInfo* m_participantConnectionInfos;
    u32 m_participantStations[12]; // This is actually a nn::pia::transport::Station*[12], but we'll leave it stubbed
    u32 m_participantSlotCount; // This represents the number of slots initialized, not the actual number of participants connected
    common::CriticalSection m_cs;
};
static_assert(sizeof(StationConnectionInfoTable) == 0x100, "sizeof(StationConnectionInfoTable) != 0x100");

struct IdentificationInfoTable {
    u32 vtable;
    Station::IdentificationInfo m_localIdentificationInfo;
    Station::IdentificationInfo* m_participantIdentificationInfos;
    u32 m_participantStations[12]; // This is actually a nn::pia::transport::Station*[12], but we'll leave it stubbed
    u32 m_participantSlotCount; // This represents the number of slots initialized, not the actual number of participants connected
    common::CriticalSection m_cs;
};
static_assert(sizeof(IdentificationInfoTable) == 0x94, "sizeof(IdentificationInfoTable) != 0x94");

}

}

}
