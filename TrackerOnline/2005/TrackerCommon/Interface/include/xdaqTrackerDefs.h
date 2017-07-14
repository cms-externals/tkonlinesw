#ifndef _xdaqTrackerDefs_h
#define _xdaqTrackerDefs_h
// System Definition
#define SYST_TRACKER                             0x1

// Subsystem definition
#define SUB_SYST_TRIGGER                            0x1
#define SUB_SYST_FEC                                0x2
#define SUB_SYST_FED                                0x3
#define SUB_SYST_SENDER                             0x4
#define SUB_SYST_ANALYSIS                           0x5
#define SUB_SYST_SUPERVISOR                         0x6
#define SUB_SYST_DBCS                               0x7

// FED Version 
#define VERS_FEDPMC                             0x30
#define VERS_FED9U                              0x31
#define VERS_FEDEMULATOR                        0x32
#define VERS_TRIGGERFED                         0x33


// Generic FED Commands

#define FED_INIT_CMD                          0x1
#define FED_FAST_INIT_CMD                     0x23
#define FED_START_CMD                         0x2
#define FED_STOP_CMD                          0x3
#define FED_SET_MODE_SCOPE_CMD                0x4
#define FED_SET_MODE_RAW_CMD                  0x5
#define FED_SET_MODE_CLUSTER_CMD              0x6
#define FED_CLEAR_DATA_BUFFER_CMD             0x7
#define FED_STORE_HEADER_CMD                  0x8
#define FED_SET_MODE_PROCESS_CMD              0x9
#define FED_SET_SUPER_MODE_NORMAL_CMD         0xa
#define FED_SET_SUPER_MODE_FAKE_CMD           0xb
#define FED_SET_SUPER_MODE_ZERO_LITE_CMD      0xc
#define FED_SET_SUPER_MODE_FAKE_ZERO_LITE_CMD 0xd
#define FED_LOAD_STRIPS_CMD                   0xe
#define FED_LOAD_FAKE_EVENT_CMD               0xf
#define FED_LOAD_TEMP_SETTINGS_CMD            0x2f
#define FED_SET_INIT_MODE_CMD                 0x2e
#define FED_GENERATE_SOFT_TRIGGER_CMD         0x2d
#define FED_GET_NUMBER_OF_TRIGGER_CMD         0x2c
#define FED_SET_DAQ_REGISTER_CMD              0x2a
#define FED_GET_FED_ID_CMD                     0x2b
#define FED_GET_NUMBER_OF_EVENT_READ_CMD       0x20 
#define FED_TTS_CONNECTION_TEST                0x22 
#define FED_DO_VME_POLLING                    0x24
#define FED_VME_POLLING_DONE                  0x25
#define FED_BACKPRESSURE_ENABLE               0x29
#define FED_BACKPRESSURE_DISABLE              0x34
#define DS_PAUSE                             0x26
#define DS_RESUME                            0x27
#define XTALDAQ_SCAN_CMD                     0x28
#define XTALDAQ_SCAN_POINT_DONE              0x35
#define FED_INCREMENT_DELAY_STEP             0x41
#define FED_SET_DELAY_PARAMETERS             0x42
#define FED_START_DELAY_MAP                  0x43
#define FED_SETOUTOFSYNC                     0x44
// FEDPMC Commands
#define FED_ENABLE_TRIGGER_CMD  0x10
#define FED_DISABLE_TRIGGER_CMD  0x11
#define FED_CLEAN_START_CMD  0x12
#define FED_CLEAN_STOP_CMD  0x13
#define FED_LOOP_CMD  0x14
#define FED_EXIT_LOOP_CMD  0x15
#define FED_RESTART_CMD  0x16
#define FED_PUSH_EVENT_CMD  0x17
#define FED_FIND_DELAY_CMD  0x18
#define FED_SET_DELAY_CMD  0x19
#define FED_FIND_THRESHOLD_CMD  0x1a
#define FED_SET_LOW_THRESHOLD_CMD   0x1b
#define FED_SET_HIGH_THRESHOLD_CMD   0x1c
#define FED_CLEAR_BUFFER_CMD   0x1d
#define FED_GET_STORED_CMD   0x1e
#define FED_SET_SCOPE_MODE_CMD   0x1f
#define FED_IN_FED_BUFFER_CMD  0x20

// FED EMulator Command
#define FED_FAKE_TRIGGER_CMD  0x100

// FED 9U Command
#define FED9U_START_OF_SPILL  0x201
#define FED9U_END_OF_SPILL  0x202
#define FED9U_ADJUST_TTC_CLK_SKEW 0x203 
#define FED9U_ADJUST_CHANNEL_CLK_SKEW 0x204 
#define FED_GET_READOUT_CNT_CMD 0x205
#define FED_FEFPGA_SOFT_RESET_CMD   0x206
#define FED_PURGE_EVENTS_CMD 0x207 

// TSC Version

#define VERS_TSC     0x1
#define VERS_TCW     0x2

// TSC Command
#define TSC_READ_CMD  0x00
#define TSC_WRITE_CMD  0x01

#define TSC_RESET_LATENCY     0x10
#define TSC_CLOCK_MIN         0x20
#define TSC_TRIGGER_COUNT     0x30
#define TSC_TRIGGER_REGISTER  0x40
#define TSC_TRIGGER_FREQUENCY 0x50
#define TSC_FED_LATENCY       0x60
#define TSC_APV_LATENCY       0x70
#define TSC_CALIB_LATENCY     0x80
#define TSC_FED_DELAY_CLOCK   0x90
#define TSC_FED_DELAY_TRIGGER 0xa0
#define TSC_RESET_APV         0xb0
#define TSC_RESET_FED         0xc0
#define TSC_RESET_PLL         0xd0
#define TSC_REENABLE_TRIGGER  0xe0
#define TSC_SOFT_TRIGGER      0xf0
#define TSC_GATE_POSITION     0x100
#define TSC_GATE_WIDTH        0x110
#define TSC_ENABLE_INTERRUPT  0x120
#define TSC_DISABLE_INTERRUPT 0x130
#define TSC_PULSER_DELAY      0x140
#define TSC_REGISTER          0x150
#define TSC_GET_FIFO          0x160
#define TSC_CLEAR_FIFO          0x170
#define TSC_VETO              0x180
#define TSC_CLEAR_VETO        0x190
#define TSC_LASER_LATENCY     0x200
#define TSC_APV2_LATENCY     0x210
#define TSC_APV3_LATENCY     0x220
#define TSC_VOODOO           0x230
#define TSC_TEST_ENABLE      0x240
#define TSC_TEST_DISABLE     0x250
#define TSC_VOODOO_LOCAL     0x260

// GENERIC TRIGGER SUPERVISOR
#define GTS_READ_CMD              0x00
#define GTS_WRITE_CMD             0x01

#define GTS_STOP_TRIGGER          0x10
#define GTS_START_TRIGGER         0X20

#define GTS_RESET_LATENCY         0x30
#define GTS_TRIGGER_LATENCY       0x40
#define GTS_CALIB_LATENCY         0x50
#define GTS_LASER_LATENCY         0x60
#define GTS_RUN_TYPE              0x70
#define GTS_N_TRIGGERS_PER_BURST  0x80
#define GTS_APPLY_CHANGES         0x90


//FEC 
#include "xdaqTrackerFecDefs.h"


// SENDER
#define SENDER_SEND_BUFFER         0x01
#define SENDER_PREALLOCATE_BUFFER  0x02
#define SENDER_CLEAR_BUFFER        0x03
#define SENDER_GET_STORED          0x04

// DBA
#define DBA_GET_NEXT_DBA_CMD    0x10
#define DBA_SET_NEXT_DBA_CMD    0x11
#define DBA_SET_SEGNO_CMD    0x12

#define DBA_DS   0x0
#define DBA_FED9U 0x1
#define DBA_FED   0x2

// ROOT Analyzer
#define  ROOTANALYZER_GET_RECEIVED_CMD                         0x01
#define  ROOTANALYZER_GET_LIST_OF_HISTOGRAMS_CMD      0x02
#define  ROOTANALYZER_GET_HISTOGRAM_CMD                      0x03
// KMUX (sub syst trigger..?)
#define KMUX_SET_ALL               0x01
#define KMUX_SET_CARD               0x02

// SPILL
#define SPILL_START_CMD            0x01
#define SPILL_STOP_CMD            0x02

// Database Cache Server
#define FED_FIRST_ID              50
#define DBCS_FED9U_1              0x1
#define DBCS_FED9U_2              0x2


// Monitoring structure
struct FedInfoStruct
{
  int TriggerSource;
  int BeFpgaReadRoute;
  int Clock;
  int DaqMode;
  int DaqSuperMode;
  int TTSSLINKStatus;
  int SecondBeStatusRegister;
  int FedId;
  int FedHardwareId;
  int ScopeLength;
  int TriggerNumber;
  int QdrFrameCount;
  int QdrTotalFrameCount;
  int QdrBufferLevel;
  int BeStatusRegister;
  // <NAC date="02/11/2009"> added spy event count to monitoring
  int SpyCount;
  // </NAC>
};
struct DsInfoStruct
{
  int received;
  int sent;
};

// Message d'erreur

//SupervisorLoop

//Local EVB
#define ERR_EVB_THREAD_PROBLEM 0x1001

//Timing
#define ERR_ALLPLL_CONFIG_FAILURE 0x1011
#define ERR_PLL_READ_FAILURE 0x1012
#define ERR_PLL_CONFIG_FAILURE 0x1013

//Gain Scan
#define ERR_AOH_CONFIG_FAILURE 0x1021

//Connection
#define ERR_XTOF_AOH_CONFIG_FAILURE 0x1031
#define ERR_XTOF_NODCU_FAILURE 0x1032

//VPSP
#define ERR_VPSP_CONFIG_FAILURE 0x1041

// CALIB
#define ERR_CAL_APV_READ_FAILURE 0x1051
#define ERR_CAL_APV_WRITE_FAILURE 0x1052

//CALIB Scan
#define ERR_CALS_APV_READ_FAILURE 0x1061
#define ERR_CALS_APV_WRITE_FAILURE 0x1062

// Fine Delay
#define ERR_FINE_DCU_MISSING   0x1071
#define ERR_FINE_CON_MISSING 0x1072
#define ERR_FINE_APV_PREDELAY_FAILURE 0x1073
#define ERR_FINE_PLL_WRITE_FAILURE 0x1074

// TrackerManager

#define ERR_TKMG_NO_TRG_APP 0x1101
#define ERR_TKMG_NO_DS_APP  0x1102
#define ERR_TKMG_NO_EVM_APP 0x1103
#define ERR_TKMG_NO_FECHARDID_DATA 0x1104
#define ERR_TKMG_NO_FECHARDID_READ 0x1105
#define ERR_TKMG_ENDRUN_FAILURE    0x1106
#define ERR_TKMG_STARTRUN_FAILURE  0x1107
#define ERR_TKMG_RESUME_DEVICE_FAILURE 0x1108
#define ERR_TKMG_RESUME_DCU_FAILURE    0x1109
#define ERR_TKMG_APV_LATENCY_WRITE_FAILURE 0x110A
#define ERR_TKMG_APV_LATENCY_READ_FAILURE  0x110B
#define ERR_TKMG_DBACCESS_FAILURE          0x110C
#define ERR_TKMG_DEVICE_FACTORY_FAILURE    0x110D
#define ERR_TKMG_DCU_PLL_FAILURE           0x110E
#define ERR_TKMG_DCU_INFO_FAILURE          0x110E
#define ERR_TKMG_FEDID_ACCESS_FAILURE      0x110F
#define ERR_TKMG_TRANSITION_FAILURE        0x1120
#define ERR_TKMG_EXTRA_FRAME               0x1121
#define ERR_TKMG_MISSING_FRAME             0x1122
#define ERR_TKMG_NO_FED_REPLY              0x1123
#define ERR_TKMG_NO_FED_ANSWER             0x1124
#define ERR_TKMG_NO_DS_REPLY               0x1125
#define ERR_TKMG_NO_DS_ANSWER              0x1126
#define ERR_TKMG_APVMODE_FAILURE        0x1127

//DS
#define ERR_DS_NO_NEXT_APP 0x1200
#define ERR_DS_NO_FEDS_IN_CRATE 0x1201
#define ERR_DS_TRANSITION_FAILURE 0x1203

//DATABASE CACHE
#define ERR_DBCACHE_NO_O2O 0x1210
#define ERR_DBCACHE_NO_FEDID 0x1211
#define ERR_DBCACHE_FEDDESC_FAILURE 0x1212
#define ERR_DBCACHE_TRANSITION_FAILURE 0x1213
#endif
