#include "Packet.h"
#include <cstdint>
#include <vector>
#include <atomic>
#include <chrono>
namespace network {
    class PacketFactory {
        std::atomic<uint16_t> _seq{0};        
        std::chrono::steady_clock::time_point _startTime = std::chrono::steady_clock::now();
        public:
        uint16_t nextSeq() {
            return _seq.fetch_add(1);
        }
        uint32_t currentTick() 
        {
            return static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _startTime).count());
        }
        PacketData createPacket(typePacket type, float x,float y) {
            PacketData packet;
            packet.type = type;
            packet.seq = nextSeq();
            packet.tick = currentTick();
            packet.payload.resize(8);
            PacketManager::floatToBytes(x, packet.payload.data());
            PacketManager::floatToBytes(y, packet.payload.data() + 4);
            return packet;
        }
    };
}