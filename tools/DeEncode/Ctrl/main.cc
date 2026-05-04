#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include "conf/AlignConfig.h"
#include "log/LogHandler.h"
#include "conf/AligndataConfig.h"

typedef char csv_strings[256];
#pragma pack(1)
typedef struct
{
	int8_t  version;
    int8_t  table_id;
    int16_t  entry_size;
    int32_t  ifn;
    int32_t  num_entries;
    uint8_t  reserved[9];
}Ctrldata_head;


typedef struct _Ip
{
    csv_strings  ip_1;
    uint32_t  ip_2;
}Ip;

typedef union _calling_ed
{
	csv_strings  calling_party_id;
	uint32_t  called_party_id;
}Calling_ed;

typedef struct _subscriber_id
{
    uint32_t  subscriber;
    Calling_ed calling_ed;
}Subscriber_id;

typedef struct
{
    uint32_t  index;
    Subscriber_id subscriber_id;
    Ip ip;
    csv_strings  node_id_1;
    csv_strings  node_id_2;
    uint16_t  flags;
    uint8_t  link_type;
    uint64_t  subscriber_id_type;
    uint64_t  session_id;
    uint32_t  locationKey1;
    uint32_t  locationKey2;
    uint32_t  locationKey3;
    uint32_t  locationKey4;
}Ctrldata_keys;

typedef struct
{
    uint32_t   key_table_idx;
    uint32_t   app_id;
    uint32_t   msg_id;
    uint8_t    app_flags;
    uint8_t    info_type;
    uint16_t   flags;
    csv_strings   tran_start_time;
    uint64_t   response_time;
    uint32_t   result_code;
    uint16_t   ext_type;
    uint16_t   ext_len;
    uint64_t   ext_val_or_offset;
}Ctrldata_stats;

bool WriteCtrldata_keys_File(const char* filename)
{	
    FILE* fp = fopen(filename, "w+" );
    Ctrldata_head head =
    {
    1,     //int8_t  version
	1,     //int8_t  table_id;
	0,     //int16_t  entry_size;
	1,     //int32_t  ifn;
    0,     //int32_t  num_entries;
	0xff,  //int8_t  reserved_1;
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff
    };

    Ctrldata_keys body /*={
    15,             //uint32_t  index;
    {//struct 
	    123,  //uint32_t  subscriber;
        //union
        {
            "abc:def:ghi",  //csv_strings  calling_party_id;
            123 //uint32_t  called_party_id;
        //union calling_ed
        }
    //struct subscriber_id
    },
    //union
	{
        "1.1.1.1",      //csv_strings  ip_1
	    2     //uint32_t  ip_2
    //union ip
    },
	"a,d,d,d",      //csv_strings  node_id_1;
	"d,r,t,y",      //csv_strings  node_id_2
	1,              //uint16_t  flags;
	2,              //uint8_t  link_type
	234,            //uint64_t  subscriber_id_type
	78,             //uint64_t  session_id
	10,             //uint32_t  locationKey1
	100,            //uint32_t  locationKey2
    1000,           //uint32_t  locationKey3
	1100            //uint32_t  locationKey4
    }*/;
    body.index = 15;
    body.subscriber_id.subscriber  = 123;
    body.subscriber_id.calling_ed.called_party_id = 123;
    strncpy(body.subscriber_id.calling_ed.calling_party_id, "abc:def:ghi", 256);
    body.ip.ip_2 = 2;
    strncpy(body.ip.ip_1, "1.1.1.1", 256);
    strncpy(body.node_id_1, "a,d,d,d", 256);
    strncpy(body.node_id_2, "d,r,t,y", 256);
    body.flags = 1;
    body.link_type = 2;
    body.subscriber_id_type = 234;
    body.session_id = 78;
    body.locationKey1 = 10;
    body.locationKey2 = 100;
    body.locationKey3 = 1000;
    body.locationKey4 = 1100;

    Ctrldata_keys body1/* ={
    15,             //uint32_t  index;
    {//struct 
	    123,  //uint32_t  subscriber;
        //union
        {
            "abc:def:ghi",  //csv_strings  calling_party_id;
            123 //uint32_t  called_party_id;
        //union calling_ed
        }
    //struct subscriber_id
    },
    //union
	{
        "1.1.1.1",      //csv_strings  ip_1
	    2      //uint32_t  ip_2
    //union ip
    },
	"a,d,d,d",      //csv_strings  node_id_1;
	"d,r,t,y",      //csv_strings  node_id_2
	1,              //uint16_t  flags;
	2,              //uint8_t  link_type
	234,            //uint64_t  subscriber_id_type
	78,             //uint64_t  session_id
	10,             //uint32_t  locationKey1
	100,            //uint32_t  locationKey2
    1000,           //uint32_t  locationKey3
	1100            //uint32_t  locationKey4
    }*/;

    body1.index = 15;
    body1.subscriber_id.subscriber  = 123;
    body1.subscriber_id.calling_ed.called_party_id = 123;
    strncpy(body1.subscriber_id.calling_ed.calling_party_id, "def:abc:ghi", 256);
    body1.ip.ip_2 = 2;
    strncpy(body1.ip.ip_1, "2.2.2.2", 256);
    strncpy(body1.node_id_1, "d,e,f,g", 256);
    strncpy(body1.node_id_2, "a,b,c,d", 256);
    body1.flags = 2;
    body1.link_type = 4;
    body1.subscriber_id_type = 567;
    body1.session_id = 87;
    body1.locationKey1 = 20;
    body1.locationKey2 = 200;
    body1.locationKey3 = 2000;
    body1.locationKey4 = 2200;

    head.entry_size = sizeof(Ctrldata_keys);
    head.num_entries = 2;
    fwrite((char*)&head, sizeof(Ctrldata_head), 1, fp);
    fwrite((char*)&body, sizeof(Ctrldata_keys), 1, fp);
    fwrite((char*)&body1, sizeof(Ctrldata_keys), 1, fp);

    fclose(fp);

    return true;
}

bool WriteCtrldata_stats_File(const char* filename)
{
	
    FILE* fp = fopen(filename, "w+" );
    Ctrldata_head head =
    {
    1,     //int8_t  version
	1,     //int8_t  table_id;
	0,     //int16_t  entry_size;
	1,     //int32_t  ifn;
    0,     //int32_t  num_entries;
	0xff,  //int8_t  reserved_1;
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff
    };

    Ctrldata_stats body =
    {
    18,     //uint32_t   key_table_idx
    88,     //uint32_t   app_id
	99,     //uint32_t   msg_id
	60,     //uint8_t    app_flags
	66,     //uint8_t    info_type
	1,      //uint16_t   flags
    "123:456:789",   //csv_strings   tran_start_time
	789,    //uint64_t   response_time
	80,     //uint32_t   result_code
	90,     //uint16_t   ext_type
	60,     //uint16_t   ext_len
	100     //uint64_t   ext_val_or_offset
    }; 
    Ctrldata_stats body1 =
    {
    18,     //uint32_t   key_table_idx
    88,     //uint32_t   app_id
	99,     //uint32_t   msg_id
	60,     //uint8_t    app_flags
	66,     //uint8_t    info_type
	1,      //uint16_t   flags
    "123:456:789",   //csv_strings   tran_start_time
	789,    //uint64_t   response_time
	80,     //uint32_t   result_code
	90,     //uint16_t   ext_type
	60,     //uint16_t   ext_len
	100     //uint64_t   ext_val_or_offset
    }; 

    head.entry_size = sizeof(Ctrldata_stats);
    head.num_entries = 2;
    fwrite((char*)&head, sizeof(Ctrldata_head), 1, fp);
    fwrite((char*)&body, sizeof(Ctrldata_stats), 1, fp);
    fwrite((char*)&body1, sizeof(Ctrldata_stats), 1, fp);

    fclose(fp);

    return true;
}

int main(int argc, char* argv[])
{
    CAlignConfig::GetInstance()->Setlogpath("./output/log/");
    CLogHandler::GetInstance()->Init(true);
    WriteCtrldata_keys_File("Ctrldata_keys.data");
    WriteCtrldata_stats_File("Ctrldata_stats.data");

	return 0;
}
