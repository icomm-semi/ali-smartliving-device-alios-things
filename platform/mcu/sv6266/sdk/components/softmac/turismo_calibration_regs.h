#include <soc_types.h>

ssv_cabrio_reg rf_calibration_result[]={
        /* Sx , 1*/       
        {0xCCB0A464,0},

        /* RX DC , 21*/
        //ADR_WF_DCOC_IDAC_Register1~ADR_WF_DCOC_IDAC_Register21
        {0xCCB0A494,0},            
        {0xCCB0A498,0},        
        {0xCCB0A49C,0},        
        {0xCCB0A4A0,0},        
        {0xCCB0A4A4,0},                
        {0xCCB0A4A8,0},                
        {0xCCB0A4AC,0},
        {0xCCB0A4B0,0},                
        {0xCCB0A4B4,0}, 
        {0xCCB0A4B8,0},                
        {0xCCB0A4BC,0},                
        {0xCCB0A4C0,0},                        
        {0xCCB0A4C4,0},
        {0xCCB0A4C8,0},                                
        {0xCCB0A4CC,0},                                
        {0xCCB0A4D0,0},                                        
        {0xCCB0A4D4,0},                                                
        {0xCCB0A4D8,0},                                                
        {0xCCB0A4DC,0},                                                
        {0xCCB0A4E0,0},        
        {0xCCB0A4E4,0},        
        //To Do
        //ADR_5G_DCOC_IDAC_Register1~ADR_5G_DCOC_IDAC_Register21
        {0xCCB0A5A8,0},            
        {0xCCB0A5AC,0},            
        {0xCCB0A5B0,0},            
        {0xCCB0A5B4,0},            
        {0xCCB0A5B8,0},            
        {0xCCB0A5BC,0},            
        {0xCCB0A5C0,0},            
        {0xCCB0A5C4,0},            
        {0xCCB0A5C8,0},            
        {0xCCB0A5CC,0},            
        {0xCCB0A5D0,0},            
        {0xCCB0A5D4,0},            
        {0xCCB0A5D8,0},            
        {0xCCB0A5DC,0},            
        {0xCCB0A5E0,0},            
        {0xCCB0A5E4,0},            
        {0xCCB0A5E8,0},            
        {0xCCB0A5EC,0},            
        {0xCCB0A5F0,0},            
        {0xCCB0A5F4,0},            
        {0xCCB0A5F8,0},            

        /* RX RC , 2*/
        //ADR_WF_H20_RX_FILTER_REGISTER
        {0xCCB0A410,0},        
        //ADR_WF_H40_RX_FILTER_REGISTER
        {0XCCB0A414,0},        

        /* TX DC , 2*/    
        //ADR_WF_TX_DAC_REGISTER
        {0xCCB0A450,0},
        //ADR_5G_TX_DAC_REGISTER
        {0xCCB0A578,0},

        /* TX RX IQ ,5*/
        //ADR_TRX_IQ_COMP_2G
        {0xCCB0A820,0}, 
        //ADR_TRX_IQ_COMP_5G_0
        {0xCCB0A824,0}, 
        //ADR_TRX_IQ_COMP_5G_1
        {0xCCB0A828,0}, 
        //ADR_TRX_IQ_COMP_5G_2
        {0xCCB0A82C,0}, 
        //ADR_TRX_IQ_COMP_5G_3
        {0xCCB0A830,0}, 

        /* PADPD ,22*/
        //ADR_MODE_REGISTER
        {0xCCB0A400,0},
        //ADR_5G_TX_GAIN_PAFB_CONTROL
        {0xCCB0A634,0},
        //ADR_WIFI_PHY_COMMON_BB_SCALE_REG_0~ADR_WIFI_PHY_COMMON_BB_SCALE_REG_3
        {0xCCB0A180,0},        
        {0xCCB0A184,0},
        {0xCCB0A188,0},        
        {0xCCB0A18C,0},
        //ADR_WIFI_PADPD_2G_CONTROL_REG
        {0xCCB0AD1C,0},        
        //ADR_WIFI_PADPD_5G_BB_GAIN_REG        
        {0xCCB0ADA8,0},                
        //ADR_WIFI_PADPD_2G_BB_GAIN_REG        
        {0xCCB0ADAC,0},
        //ADR_WIFI_PADPD_2G_GAIN_REG0~ADR_WIFI_PADPD_2G_GAIN_REGC
        {0xCCB0AD20,0},
        {0xCCB0AD24,0},        
        {0xCCB0AD28,0},        
        //{0xCCB0AD2C,0}, //this register doesn't exist         
        {0xCCB0AD30,0},        
        {0xCCB0AD34,0},        
        {0xCCB0AD38,0},        
        {0xCCB0AD3C,0},        
        {0xCCB0AD40,0},        
        {0xCCB0AD44,0},        
        {0xCCB0AD48,0},        
        {0xCCB0AD4C,0},        
        {0xCCB0AD50,0},
        {0xCCB0AD54,0},       
        //ToDo: ADR_WIFI_PADPD_5100_GAIN_REG0~ADR_WIFI_PADPD_5100_GAIN_REGC
        //0xCCB0A900 ~ 0xCCB0A930
        {0xCCB0A900,0},        
        {0xCCB0A904,0},        
        {0xCCB0A908,0},        
        {0xCCB0A90C,0},        
        {0xCCB0A910,0},        
        {0xCCB0A914,0},        
        {0xCCB0A918,0},        
        {0xCCB0A91C,0},        
        {0xCCB0A920,0},        
        {0xCCB0A924,0},        
        {0xCCB0A928,0},        
        {0xCCB0A92C,0},        
        {0xCCB0A930,0},        
        
        //ToDo: ADR_WIFI_PADPD_5200_GAIN_REG0~ADR_WIFI_PADPD_5200_GAIN_REGC        
        //0xCCB0A980 ~ 0xCCB0A9B0
        {0xCCB0A980,0},        
        {0xCCB0A984,0},        
        {0xCCB0A988,0},        
        {0xCCB0A98C,0},        
        {0xCCB0A990,0},        
        {0xCCB0A994,0},        
        {0xCCB0A998,0},        
        {0xCCB0A99C,0},        
        {0xCCB0A9A0,0},        
        {0xCCB0A9A4,0},        
        {0xCCB0A9A8,0},        
        {0xCCB0A9AC,0},        
        {0xCCB0A9B0,0},        
        
        //ToDo: ADR_WIFI_PADPD_5300_GAIN_REG0~ADR_WIFI_PADPD_5300_GAIN_REGC                
        //0xCCB0AA00 ~ 0xCCB0AA30        
        {0xCCB0AA00,0},        
        {0xCCB0AA04,0},        
        {0xCCB0AA08,0},        
        {0xCCB0AA0C,0},        
        {0xCCB0AA10,0},        
        {0xCCB0AA14,0},        
        {0xCCB0AA18,0},        
        {0xCCB0AA1C,0},        
        {0xCCB0AA20,0},        
        {0xCCB0AA24,0},        
        {0xCCB0AA28,0},        
        {0xCCB0AA2C,0},        
        {0xCCB0AA30,0},        
        
        //ToDo: ADR_WIFI_PADPD_5400_GAIN_REG0~ADR_WIFI_PADPD_5400_GAIN_REGC       
        //0xCCB0AA80 ~ 0xCCB0AAB0        
        {0xCCB0AA80,0},        
        {0xCCB0AA84,0},        
        {0xCCB0AA88,0},        
        {0xCCB0AA8C,0},        
        {0xCCB0AA90,0},        
        {0xCCB0AA94,0},        
        {0xCCB0AA98,0},        
        {0xCCB0AA9C,0},        
        {0xCCB0AAA0,0},        
        {0xCCB0AAA4,0},        
        {0xCCB0AAA8,0},        
        {0xCCB0AAAC,0},        
        {0xCCB0AAB0,0}        
};

