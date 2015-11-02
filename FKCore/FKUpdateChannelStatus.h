#ifndef FKUPDATECHANNELSTATUS
#define FKUPDATECHANNELSTATUS

//Flags used to determine update channel status

namespace FKUpdateChannelStatus{
    //done progress
    const qint8 startLoading  = 0b00000001;
    const qint8 finishLoading = 0b00000010;
    const qint8 copied        = 0b00000100;
    const qint8 deleted       = 0b00001000;
    const qint8 updated       = 0b00010000;
    //required actions
    const qint8 needDelete    = 0b00100000;
    const qint8 needUpdate    = 0b01000000;

    const qint8 avaliable     = 0b10000000;
    const qint8 actual        = 0b00000000;
}

#endif // FKUPDATECHANNELSTATUS

