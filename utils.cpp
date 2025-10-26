#include "utils.hpp"

namespace utils {

int64_t filetime_to_time_t(std::filesystem::file_time_type tp)
{
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(
        tp - std::filesystem::file_time_type::clock::now() + system_clock::now()
        );
    return (int64_t)system_clock::to_time_t(sctp);
}

std::filesystem::file_time_type time_t_to_filetime(int64_t t)
{
    using namespace std::chrono;
    auto sctp = system_clock::from_time_t((time_t)t);
    return std::filesystem::file_time_type::clock::now() + (sctp - system_clock::now());
}

uint32_t perms_to_bits(std::filesystem::perms p){
    using P = std::filesystem::perms;
    uint32_t b=0;
    auto on=[&](P bit,int pos){ if ((p & bit) != P::none) b|=(1u<<pos); };
    on(P::owner_read,0); on(P::owner_write,1); on(P::owner_exec,2);
    on(P::group_read,3); on(P::group_write,4); on(P::group_exec,5);
    on(P::others_read,6);on(P::others_write,7);on(P::others_exec,8);
    return b;
}

std::filesystem::perms bits_to_perms(uint32_t b){
    using P = std::filesystem::perms;
    auto on=[&](int pos,P bit){ return (b & (1u<<pos)) ? bit : P::none; };
    P p = P::none;
    p |= on(0,P::owner_read)|on(1,P::owner_write)|on(2,P::owner_exec);
    p |= on(3,P::group_read)|on(4,P::group_write)|on(5,P::group_exec);
    p |= on(6,P::others_read)|on(7,P::others_write)|on(8,P::others_exec);
    return p;
}

} // utils
