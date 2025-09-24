#pragma once
#include <vector>
#include <cstdint>
#include <optional>
#include <cstring>
#include <arpa/inet.h>
namespace network {
    enum typePacket : uint8_t {
        HAND_SHAKE = 1,
        POSITION = 2,
        POINT = 3
    };
    struct PacketData
    {
        uint8_t type;
        uint16_t seq;
        uint32_t tick;
        std::vector<uint8_t> payload;
    };
    class PacketManager
    {
    public:
        static constexpr size_t HEADER_SIZE = 7; 
        static void floatToBytes(float value, uint8_t data[4]) {

            uint32_t f;
            static_assert(sizeof(float) == 4);
            std::memcpy(&f, &value, 4);
            f = htonl(f);
            std::memcpy(data, &f, 4);
        }
        static float bytesToFloat(const uint8_t data[4]) {
            uint32_t f;
            std::memcpy(&f, data, 4);
            f = ntohl(f);
            float value;
            std::memcpy(&value, &f, 4);
            return value;
        }
        std::vector<uint8_t> serialize(const PacketData& packet) {
            uint16_t payloadSize = static_cast<uint16_t>(packet.payload.size());
            std::vector<uint8_t> buffer;
            buffer.reserve(HEADER_SIZE + payloadSize);
            buffer.push_back(packet.type);

            uint16_t seq_net = htons(packet.seq);
            buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&seq_net), reinterpret_cast<uint8_t*>(&seq_net) + 2);
            uint32_t tick_net = htonl(packet.tick);
            buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&tick_net), reinterpret_cast<uint8_t*>(&tick_net) + 4);
            if (!packet.payload.empty()){
                buffer.insert(buffer.end(), packet.payload.begin(), packet.payload.end());
            }
            return buffer;
        }
        bool deserialize(const uint8_t* data, size_t len, PacketData& packet){
            if (len < HEADER_SIZE) {
                return false; 
            }
            packet.type = data[0];

            uint16_t seq_net;
            std::memcpy(&seq_net, data + 1, 2);
            packet.seq = ntohs(seq_net);

            uint32_t tick_net;
            std::memcpy(&tick_net, data + 3, 4);
            packet.tick = ntohl(tick_net);

            packet.payload.assign(data + HEADER_SIZE, data + len);
            return true;
        }

    };    

    
}