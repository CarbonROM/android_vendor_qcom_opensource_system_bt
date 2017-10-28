/******************************************************************************
 *
 *  Copyright (C) 2006-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  This is the private interface file for the BTA Java I/F
 *
 ******************************************************************************/
#ifndef BTA_JV_INT_H
#define BTA_JV_INT_H

#include "bta_api.h"
#include "bta_jv_api.h"
#include "bta_sys.h"
#include "port_api.h"
#include "osi/include/alarm.h"
#include "rfcdefs.h"

/*****************************************************************************
 *  Constants
 ****************************************************************************/

enum {
  /* these events are handled by the state machine */
  BTA_JV_API_ENABLE_EVT = BTA_SYS_EVT_START(BTA_ID_JV),
  BTA_JV_API_DISABLE_EVT,
  BTA_JV_API_GET_CHANNEL_EVT,
  BTA_JV_API_FREE_SCN_EVT,
  BTA_JV_API_START_DISCOVERY_EVT,
  BTA_JV_API_CREATE_RECORD_EVT,
  BTA_JV_API_DELETE_RECORD_EVT,
  BTA_JV_API_L2CAP_CONNECT_EVT,
  BTA_JV_API_L2CAP_CLOSE_EVT,
  BTA_JV_API_L2CAP_START_SERVER_EVT,
  BTA_JV_API_L2CAP_STOP_SERVER_EVT,
  BTA_JV_API_L2CAP_READ_EVT,
  BTA_JV_API_RFCOMM_CONNECT_EVT,
  BTA_JV_API_RFCOMM_CLOSE_EVT,
  BTA_JV_API_RFCOMM_START_SERVER_EVT,
  BTA_JV_API_RFCOMM_STOP_SERVER_EVT,
  BTA_JV_API_RFCOMM_WRITE_EVT,
  BTA_JV_API_SET_PM_PROFILE_EVT,
  BTA_JV_API_PM_STATE_CHANGE_EVT,
  BTA_JV_API_L2CAP_CONNECT_LE_EVT,
  BTA_JV_API_L2CAP_START_SERVER_LE_EVT,
  BTA_JV_API_L2CAP_STOP_SERVER_LE_EVT,
  BTA_JV_API_L2CAP_WRITE_FIXED_EVT,
  BTA_JV_API_L2CAP_CLOSE_FIXED_EVT,
  BTA_JV_MAX_INT_EVT
};

#ifndef BTA_JV_RFC_EV_MASK
#define BTA_JV_RFC_EV_MASK \
  (PORT_EV_RXCHAR | PORT_EV_TXEMPTY | PORT_EV_FC | PORT_EV_FCS)
#endif

/* data type for BTA_JV_API_ENABLE_EVT */
typedef struct {
  BT_HDR hdr;
  tBTA_JV_DM_CBACK* p_cback;
} tBTA_JV_API_ENABLE;

/* data type for BTA_JV_API_START_DISCOVERY_EVT */
typedef struct {
  BT_HDR hdr;
  RawAddress bd_addr;
  uint16_t num_uuid;
  bluetooth::Uuid uuid_list[BTA_JV_MAX_UUIDS];
  uint16_t num_attr;
  uint16_t attr_list[BTA_JV_MAX_ATTRS];
  uint32_t rfcomm_slot_id;
} tBTA_JV_API_START_DISCOVERY;

enum {
  BTA_JV_PM_FREE_ST = 0, /* empty PM slot */
  BTA_JV_PM_IDLE_ST,
  BTA_JV_PM_BUSY_ST
};

/* BTA JV PM control block */
typedef struct {
  uint32_t handle;      /* The connection handle */
  uint8_t state;        /* state: see above enum */
  tBTA_JV_PM_ID app_id; /* JV app specific id indicating power table to use */
  RawAddress peer_bd_addr; /* Peer BD address */
  alarm_t *idle_timer;  /* intermediate idle timer for paricular scb */
  bool cong;            /* TRUE, if congested */
} tBTA_JV_PM_CB;

enum {
  BTA_JV_ST_NONE = 0,
  BTA_JV_ST_CL_OPENING,
  BTA_JV_ST_CL_OPEN,
  BTA_JV_ST_CL_CLOSING,
  BTA_JV_ST_SR_LISTEN,
  BTA_JV_ST_SR_OPEN,
  BTA_JV_ST_SR_CLOSING
};
typedef uint8_t tBTA_JV_STATE;
#define BTA_JV_ST_CL_MAX BTA_JV_ST_CL_CLOSING
/* JV L2CAP control block */
typedef struct {
  tBTA_JV_L2CAP_CBACK* p_cback; /* the callback function */
  uint16_t psm;                 /* the psm used for this server connection */
  tBTA_JV_STATE state;          /* the state of this control block */
  tBTA_SERVICE_ID sec_id;       /* service id */
  uint32_t handle; /* the handle reported to java app (same as gap handle) */
  bool cong;       /* true, if congested */
  tBTA_JV_PM_CB* p_pm_cb; /* ptr to pm control block, NULL: unused */
  uint32_t l2cap_socket_id;
} tBTA_JV_L2C_CB;

#define BTA_JV_RFC_HDL_MASK 0xFF
#define BTA_JV_RFCOMM_MASK 0x80
#define BTA_JV_ALL_APP_ID 0xFF
#define BTA_JV_RFC_HDL_TO_SIDX(r) (((r)&0xFF00) >> 8)
#define BTA_JV_RFC_H_S_TO_HDL(h, s) ((h) | ((s) << 8))

/* port control block */
typedef struct {
  uint32_t handle;      /* the rfcomm session handle at jv */
  uint16_t port_handle; /* port handle */
  tBTA_JV_STATE state;  /* the state of this control block */
  uint8_t max_sess;     /* max sessions */
  uint32_t rfcomm_slot_id;
  bool cong;              /* true, if congested */
  tBTA_JV_PM_CB* p_pm_cb; /* ptr to pm control block, NULL: unused */
} tBTA_JV_PCB;

/* JV RFCOMM control block */
typedef struct {
  tBTA_JV_RFCOMM_CBACK* p_cback; /* the callback function */
  uint16_t rfc_hdl[BTA_JV_MAX_RFC_SR_SESSION];
  tBTA_SERVICE_ID sec_id; /* service id */
  uint8_t handle;         /* index: the handle reported to java app */
  uint8_t scn;            /* the scn of the server */
  uint8_t max_sess;       /* max sessions */
  int curr_sess;          /* current sessions count*/
} tBTA_JV_RFC_CB;

/* data type for BTA_JV_API_L2CAP_CONNECT_EVT & BTA_JV_API_L2CAP_CONNECT_LE_EVT
 */
typedef struct {
  BT_HDR hdr;
  int32_t type; /* One of BTA_JV_CONN_TYPE_ */
  tBTA_SEC sec_mask;
  tBTA_JV_ROLE role;
  union {
    uint16_t remote_psm;
    uint16_t remote_chan;
  };
  uint16_t rx_mtu;
  RawAddress peer_bd_addr;
  int32_t has_cfg;
  tL2CAP_CFG_INFO cfg;
  int32_t has_ertm_info;
  tL2CAP_ERTM_INFO ertm_info;
  tBTA_JV_L2CAP_CBACK* p_cback;
  uint32_t l2cap_socket_id;
} tBTA_JV_API_L2CAP_CONNECT;

/* data type for BTA_JV_API_L2CAP_SERVER_EVT */
typedef struct {
  BT_HDR hdr;
  int32_t type; /* One of BTA_JV_CONN_TYPE_ */
  tBTA_SEC sec_mask;
  tBTA_JV_ROLE role;
  union {
    uint16_t local_psm;
    uint16_t local_chan;
  };
  uint16_t rx_mtu;
  int32_t has_cfg;
  tL2CAP_CFG_INFO cfg;
  int32_t has_ertm_info;
  tL2CAP_ERTM_INFO ertm_info;
  tBTA_JV_L2CAP_CBACK* p_cback;
  uint32_t l2cap_socket_id;
} tBTA_JV_API_L2CAP_SERVER;

/* data type for BTA_JV_API_L2CAP_CLOSE_EVT */
typedef struct {
  BT_HDR hdr;
  uint32_t handle;
  tBTA_JV_L2C_CB* p_cb;
} tBTA_JV_API_L2CAP_CLOSE;

/* data type for BTA_JV_API_L2CAP_READ_EVT */
typedef struct {
  BT_HDR hdr;
  uint32_t handle;
  uint32_t req_id;
  tBTA_JV_L2CAP_CBACK* p_cback;
  uint8_t* p_data;
  uint16_t len;
  uint32_t l2cap_socket_id;
} tBTA_JV_API_L2CAP_READ;

/* data type for BTA_JV_API_L2CAP_WRITE_FIXED_EVT */
typedef struct {
  BT_HDR hdr;
  uint16_t channel;
  RawAddress addr;
  uint32_t req_id;
  tBTA_JV_L2CAP_CBACK* p_cback;
  uint8_t* p_data;
  uint16_t len;
  uint32_t user_id;
} tBTA_JV_API_L2CAP_WRITE_FIXED;

/* data type for BTA_JV_API_RFCOMM_CONNECT_EVT */
typedef struct {
  BT_HDR hdr;
  tBTA_SEC sec_mask;
  tBTA_JV_ROLE role;
  uint8_t remote_scn;
  RawAddress peer_bd_addr;
  tBTA_JV_RFCOMM_CBACK* p_cback;
  uint32_t rfcomm_slot_id;
} tBTA_JV_API_RFCOMM_CONNECT;

/* data type for BTA_JV_API_RFCOMM_SERVER_EVT */
typedef struct {
  BT_HDR hdr;
  tBTA_SEC sec_mask;
  tBTA_JV_ROLE role;
  uint8_t local_scn;
  uint8_t max_session;
  uint32_t handle;
  tBTA_JV_RFCOMM_CBACK* p_cback;
  uint32_t rfcomm_slot_id;
} tBTA_JV_API_RFCOMM_SERVER;

/* data type for BTA_JV_API_SET_PM_PROFILE_EVT */
typedef struct {
  BT_HDR hdr;
  uint32_t handle;
  tBTA_JV_PM_ID app_id;
  tBTA_JV_CONN_STATE init_st;
} tBTA_JV_API_SET_PM_PROFILE;

/* data type for BTA_JV_API_PM_STATE_CHANGE_EVT */
typedef struct {
  BT_HDR hdr;
  tBTA_JV_PM_CB* p_cb;
  tBTA_JV_CONN_STATE state;
} tBTA_JV_API_PM_STATE_CHANGE;

/* data type for BTA_JV_API_RFCOMM_WRITE_EVT */
typedef struct {
  BT_HDR hdr;
  uint32_t handle;
  uint32_t req_id;
  uint8_t* p_data;
  int len;
  tBTA_JV_RFC_CB* p_cb;
  tBTA_JV_PCB* p_pcb;
} tBTA_JV_API_RFCOMM_WRITE;

/* data type for BTA_JV_API_RFCOMM_CLOSE_EVT */
typedef struct {
  BT_HDR hdr;
  uint32_t handle;
  tBTA_JV_RFC_CB* p_cb;
  tBTA_JV_PCB* p_pcb;
  uint32_t rfcomm_slot_id;
} tBTA_JV_API_RFCOMM_CLOSE;

/* data type for BTA_JV_API_CREATE_RECORD_EVT */
typedef struct {
  BT_HDR hdr;
  uint32_t rfcomm_slot_id;
} tBTA_JV_API_CREATE_RECORD;

/* data type for BTA_JV_API_ADD_ATTRIBUTE_EVT */
typedef struct {
  BT_HDR hdr;
  uint32_t handle;
  uint16_t attr_id;
  uint8_t* p_value;
  int32_t value_size;
} tBTA_JV_API_ADD_ATTRIBUTE;

/* data type for BTA_JV_API_FREE_SCN_EVT */
typedef struct {
  BT_HDR hdr;
  int32_t type; /* One of BTA_JV_CONN_TYPE_ */
  uint16_t scn;
} tBTA_JV_API_FREE_CHANNEL;

/* data type for BTA_JV_API_ALLOC_CHANNEL_EVT */
typedef struct {
  BT_HDR hdr;
  int32_t type;    /* One of BTA_JV_CONN_TYPE_ */
  int32_t channel; /* optionally request a specific channel */
  uint32_t l2cap_socket_id;
  uint32_t rfcomm_slot_id;
} tBTA_JV_API_ALLOC_CHANNEL;
/* union of all data types */
typedef union {
  /* GKI event buffer header */
  BT_HDR hdr;
  tBTA_JV_API_ENABLE enable;
  tBTA_JV_API_START_DISCOVERY start_discovery;
  tBTA_JV_API_ALLOC_CHANNEL alloc_channel;
  tBTA_JV_API_FREE_CHANNEL free_channel;
  tBTA_JV_API_CREATE_RECORD create_record;
  tBTA_JV_API_ADD_ATTRIBUTE add_attr;
  tBTA_JV_API_L2CAP_CONNECT l2cap_connect;
  tBTA_JV_API_L2CAP_READ l2cap_read;
  tBTA_JV_API_L2CAP_CLOSE l2cap_close;
  tBTA_JV_API_L2CAP_SERVER l2cap_server;
  tBTA_JV_API_RFCOMM_CONNECT rfcomm_connect;
  tBTA_JV_API_RFCOMM_WRITE rfcomm_write;
  tBTA_JV_API_SET_PM_PROFILE set_pm;
  tBTA_JV_API_PM_STATE_CHANGE change_pm_state;
  tBTA_JV_API_RFCOMM_CLOSE rfcomm_close;
  tBTA_JV_API_RFCOMM_SERVER rfcomm_server;
  tBTA_JV_API_L2CAP_WRITE_FIXED l2cap_write_fixed;
} tBTA_JV_MSG;

/* JV control block */
typedef struct {
  /* the SDP handle reported to JV user is the (index + 1) to sdp_handle[].
   * if sdp_handle[i]==0, it's not used.
   * otherwise sdp_handle[i] is the stack SDP handle. */
  uint32_t sdp_handle[BTA_JV_MAX_SDP_REC]; /* SDP records created */
  uint8_t* p_sel_raw_data; /* the raw data of last service select */
  tBTA_JV_DM_CBACK* p_dm_cback;
  tBTA_JV_L2C_CB l2c_cb[BTA_JV_MAX_L2C_CONN]; /* index is GAP handle (index) */
  tBTA_JV_RFC_CB rfc_cb[BTA_JV_MAX_RFC_CONN];
  tBTA_JV_PCB port_cb[MAX_RFC_PORTS];          /* index of this array is
                                                  the port_handle, */
  uint8_t sec_id[BTA_JV_NUM_SERVICE_ID];       /* service ID */
  bool scn[BTA_JV_MAX_SCN];                    /* SCN allocated by java */
  uint16_t free_psm_list[BTA_JV_MAX_L2C_CONN]; /* PSMs freed by java
                                                (can be reused) */
  uint8_t sdp_active;                          /* see BTA_JV_SDP_ACT_* */
  bluetooth::Uuid uuid;                   /* current uuid of sdp discovery*/
  tBTA_JV_PM_CB pm_cb[BTA_JV_PM_MAX_NUM]; /* PM on a per JV handle bases */
} tBTA_JV_CB;

enum {
  BTA_JV_SDP_ACT_NONE = 0,
  BTA_JV_SDP_ACT_YES,   /* waiting for SDP result */
  BTA_JV_SDP_ACT_CANCEL /* waiting for cancel complete */
};

/* JV control block */
extern tBTA_JV_CB bta_jv_cb;

/* config struct */
extern tBTA_JV_CFG* p_bta_jv_cfg;

extern bool bta_jv_sm_execute(BT_HDR* p_msg);

extern void bta_jv_enable(tBTA_JV_MSG* p_data);
extern void bta_jv_disable(tBTA_JV_MSG* p_data);
extern void bta_jv_get_channel_id(tBTA_JV_MSG* p_data);
extern void bta_jv_free_scn(tBTA_JV_MSG* p_data);
extern void bta_jv_start_discovery(tBTA_JV_MSG* p_data);
extern void bta_jv_create_record(tBTA_JV_MSG* p_data);
extern void bta_jv_delete_record(tBTA_JV_MSG* p_data);
extern void bta_jv_l2cap_connect(tBTA_JV_MSG* p_data);
extern void bta_jv_l2cap_close(tBTA_JV_MSG* p_data);
extern void bta_jv_l2cap_start_server(tBTA_JV_MSG* p_data);
extern void bta_jv_l2cap_stop_server(tBTA_JV_MSG* p_data);
extern void bta_jv_l2cap_read(tBTA_JV_MSG* p_data);
extern void bta_jv_l2cap_write(uint32_t handle, uint32_t req_id,
                               uint8_t* p_data, uint16_t len, uint32_t user_id,
                               tBTA_JV_L2C_CB* p_cb);
extern void bta_jv_rfcomm_connect(tBTA_JV_MSG* p_data);
extern void bta_jv_rfcomm_close(tBTA_JV_MSG* p_data);
extern void bta_jv_rfcomm_start_server(tBTA_JV_MSG* p_data);
extern void bta_jv_rfcomm_stop_server(tBTA_JV_MSG* p_data);
extern void bta_jv_rfcomm_read(tBTA_JV_MSG* p_data);
extern void bta_jv_rfcomm_write(tBTA_JV_MSG* p_data);
extern void bta_jv_set_pm_profile(tBTA_JV_MSG* p_data);
extern void bta_jv_change_pm_state(tBTA_JV_MSG* p_data);
extern void bta_jv_l2cap_connect_le(tBTA_JV_MSG* p_data);
extern void bta_jv_l2cap_start_server_le(tBTA_JV_MSG* p_data);
extern void bta_jv_l2cap_stop_server_le(tBTA_JV_MSG* p_data);
extern void bta_jv_l2cap_write_fixed(tBTA_JV_MSG* p_data);
extern void bta_jv_l2cap_close_fixed(tBTA_JV_MSG* p_data);
extern void bta_jv_idle_timeout_handler(void *tle);

#endif /* BTA_JV_INT_H */
