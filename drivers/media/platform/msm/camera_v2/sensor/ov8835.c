

#include "msm_sensor.h"
#define OV8835_SENSOR_NAME "ov8835"
DEFINE_MSM_MUTEX(ov8835_mut);

static struct msm_sensor_ctrl_t ov8835_s_ctrl;

extern uint16_t module_integrator_id;
extern uint8_t eeprom_buffer[12];

#define CONFIG_MSMB_CAMERA_DEBUG
#undef CDBG
#ifdef CONFIG_MSMB_CAMERA_DEBUG
#define CDBG(fmt, args...) pr_err(fmt, ##args)
#else
#define CDBG(fmt, args...) do { } while (0)
#endif
static struct msm_sensor_power_setting ov8835_power_setting[] = {
    {
		.seq_type = SENSOR_VREG,
		.seq_val = CAM_VIO,
		.config_val = 0,
		.delay = 1,
	},
	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_VANA,
		.config_val = GPIO_OUT_HIGH,
		.delay = 1,
	},
	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_VDIG,
		.config_val = GPIO_OUT_HIGH,
		.delay = 1,
	},
	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_VAF,
		.config_val = GPIO_OUT_HIGH,
		.delay = 5,
	},
	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_STANDBY,
		.config_val = GPIO_OUT_LOW,
		.delay = 1,
	},
	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_RESET,
		.config_val = GPIO_OUT_LOW,
		.delay = 5,
	},
	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_STANDBY,
		.config_val = GPIO_OUT_HIGH,
		.delay = 5,
	},
	
	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_RESET,
		.config_val = GPIO_OUT_HIGH,
		.delay = 10,
	},
	{
		.seq_type = SENSOR_CLK,
		.seq_val = SENSOR_CAM_MCLK,
		.config_val = 24000000,
		.delay = 50,
	},
	{
		.seq_type = SENSOR_I2C_MUX,
		.seq_val = 0,
		.config_val = 0,
		.delay = 5,
	},
};

static struct v4l2_subdev_info ov8835_subdev_info[] = {
	{
		.code   = V4L2_MBUS_FMT_SBGGR10_1X10,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.fmt    = 1,
		.order    = 0,
	},
};

static const struct i2c_device_id ov8835_i2c_id[] = {
	{OV8835_SENSOR_NAME, (kernel_ulong_t)&ov8835_s_ctrl},
	{ }
};

static int32_t msm_ov8835_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	return msm_sensor_i2c_probe(client, id, &ov8835_s_ctrl);
}

static struct i2c_driver ov8835_i2c_driver = {
	.id_table = ov8835_i2c_id,
	.probe  = msm_ov8835_i2c_probe,
	.driver = {
		.name = OV8835_SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client ov8835_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};

static const struct of_device_id ov8835_dt_match[] = {
	{.compatible = "qcom,ov8835", .data = &ov8835_s_ctrl},
	{}
};

MODULE_DEVICE_TABLE(of, ov8835_dt_match);

static struct platform_driver ov8835_platform_driver = {
	.driver = {
		.name = "qcom,ov8835",
		.owner = THIS_MODULE,
		.of_match_table = ov8835_dt_match,
	},
};

static int32_t ov8835_platform_probe(struct platform_device *pdev)
{
	int32_t rc = 0;
	const struct of_device_id *match;
	printk("ov8835_platform_probe zx!\n");
	match = of_match_device(ov8835_dt_match, &pdev->dev);
	rc = msm_sensor_platform_probe(pdev, match->data);

	return rc;
}  
 #if 0
uint16_t ov8835_read_sccb16(uint16_t address, struct msm_sensor_ctrl_t *s_ctrl) 
{
	int32_t rc = 0;
	uint16_t data = 0;

	rc = s_ctrl->sensor_i2c_client->i2c_func_tbl->i2c_read(
			s_ctrl->sensor_i2c_client,
			address,
			&data, MSM_CAMERA_I2C_BYTE_DATA);
	if (rc < 0) {
		return rc;
	}
	return data;
} 

uint16_t ov8835_write_sccb16(uint16_t address, uint16_t data, struct msm_sensor_ctrl_t *s_ctrl) 
{ 
    int32_t rc = 0;
    rc = s_ctrl->sensor_i2c_client->i2c_func_tbl->
        i2c_write(s_ctrl->sensor_i2c_client, address, data, MSM_CAMERA_I2C_BYTE_DATA);
    if (rc < 0) {
        msleep(100);
        rc = s_ctrl->sensor_i2c_client->i2c_func_tbl->
               i2c_write(s_ctrl->sensor_i2c_client, address, data, MSM_CAMERA_I2C_BYTE_DATA);
    }
	return rc;
}         
 
void clear_otp_buffer(struct msm_sensor_ctrl_t *s_ctrl) 
{ 
        int i; 
        
        for (i=0;i<16;i++) { 
        ov8835_write_sccb16(0x3d00 + i, 0x00,s_ctrl); 
        } 
} 
 




int check_otp_wb(uint index, struct msm_sensor_ctrl_t *s_ctrl) 
{ 
        uint  flag; 
        int bank, address; 
        
        bank = 0xc0 | index; 
        ov8835_write_sccb16(0x3d84, bank,s_ctrl); 
        
        ov8835_write_sccb16(0x3d81, 0x01,s_ctrl); 
        msleep(5); 
        
        address = 0x3d00; 
        flag = ov8835_read_sccb16(address,s_ctrl); 
	pr_err("otpwb %s  flag=0x%x",__func__,flag); 

        flag = flag & 0xc0; 
        
        ov8835_write_sccb16(0x3d81, 0x00,s_ctrl); 
        
        clear_otp_buffer(s_ctrl); 
        if (flag==0) { 
        return 0; 
        } 
        else if (flag & 0x80) { 
        return 1; 
        } 
        else { 
        return 2; 
        } 
 
} 
 




int check_otp_lenc(uint index, struct msm_sensor_ctrl_t *s_ctrl) 
{ 
        int flag,  bank; 
        int address; 
        
        bank = 0xc0 | (index*4); 
        ov8835_write_sccb16(0x3d84, bank,s_ctrl); 
        
        ov8835_write_sccb16(0x3d81, 0x01,s_ctrl); 
        msleep(5); 
        
        address = 0x3d00; 
        flag = ov8835_read_sccb16(address,s_ctrl); 
	pr_err("otplenc %s  flag=0x%x",__func__,flag); 
        flag = flag & 0xc0; 
        
        ov8835_write_sccb16(0x3d81, 0x00,s_ctrl); 
        
        clear_otp_buffer(s_ctrl); 
        if (flag==0) { 
        return 0; 
        } 
        else if (flag & 0x80) { 
        return 1; 
        } 
        else { 
        return 2; 
        } 
 
} 
 
 


int read_otp_wb(uint index, struct otp_struct *otp_ptr, struct msm_sensor_ctrl_t *s_ctrl) 
{ 
int  bank,address,temp; 


bank = 0xc0 | index; 
ov8835_write_sccb16(0x3d84, bank,s_ctrl); 

ov8835_write_sccb16(0x3d81, 0x01,s_ctrl); 
msleep(5); 
address = 0x3d00; 
otp_ptr->module_id = ov8835_read_sccb16(address + 1,s_ctrl); 
otp_ptr->lens_id = ov8835_read_sccb16(address + 2,s_ctrl); 
otp_ptr->production_year = ov8835_read_sccb16(address + 3,s_ctrl);
otp_ptr->production_month = ov8835_read_sccb16(address + 4,s_ctrl);
otp_ptr->production_day = ov8835_read_sccb16(address + 5,s_ctrl);

temp=ov8835_read_sccb16(address + 10,s_ctrl); 

otp_ptr->rg_ratio = (ov8835_read_sccb16(address + 6,s_ctrl)<<2)+((temp>>6)&0x03); 
otp_ptr->bg_ratio =(ov8835_read_sccb16(address + 7,s_ctrl)<<2)+((temp>>4)&0x03); 
otp_ptr->light_rg = (ov8835_read_sccb16(address + 8,s_ctrl)<<2)+((temp>>2)&0x03); 
otp_ptr->light_bg = (ov8835_read_sccb16(address + 9,s_ctrl)<<2)+(temp&0x03); 

otp_ptr->user_data[0] = ov8835_read_sccb16(address + 11,s_ctrl); 
otp_ptr->user_data[1] = ov8835_read_sccb16(address + 12,s_ctrl); 
otp_ptr->user_data[2] = ov8835_read_sccb16(address + 13,s_ctrl); 
otp_ptr->user_data[3] = ov8835_read_sccb16(address + 14,s_ctrl); 
otp_ptr->user_data[4] = ov8835_read_sccb16(address + 15,s_ctrl); 

ov8835_write_sccb16(0x3d81, 0x00,s_ctrl); 

clear_otp_buffer(s_ctrl); 

return 0; 
} 
 


int read_otp_lenc(uint index, struct otp_struct *otp_ptr, struct msm_sensor_ctrl_t *s_ctrl) 
{ 
 
int bank, i; 
int address; 

bank = 0xc0 | (index*4); 
ov8835_write_sccb16(0x3d84, bank,s_ctrl); 

ov8835_write_sccb16(0x3d81, 0x01,s_ctrl); 
msleep(5); 
address = 0x3d01; 
for(i=0;i<15;i++) { 
otp_ptr->lenc[i]=ov8835_read_sccb16(address,s_ctrl); 
pr_err("%s  lenc[%d]=0x%x",__func__,i,otp_ptr->lenc[i]); 
address++; 
} 

ov8835_write_sccb16(0x3d81, 0x00,s_ctrl); 

clear_otp_buffer(s_ctrl); 

bank++; 
ov8835_write_sccb16(0x3d84, bank ,s_ctrl); 

ov8835_write_sccb16(0x3d81, 0x01,s_ctrl); 
msleep(5); 
address = 0x3d00; 
for(i=15;i<31;i++) { 
otp_ptr->lenc[i]=ov8835_read_sccb16(address,s_ctrl); 
pr_err("%s  lenc[%d]=0x%x",__func__,i,otp_ptr->lenc[i]); 
address++; 
} 

ov8835_write_sccb16(0x3d81, 0x00,s_ctrl); 

clear_otp_buffer(s_ctrl); 

bank++; 
ov8835_write_sccb16(0x3d84, bank ,s_ctrl); 

ov8835_write_sccb16(0x3d81, 0x01,s_ctrl); 
msleep(5); 
address = 0x3d00; 
for(i=31;i<47;i++) { 
otp_ptr->lenc[i]=ov8835_read_sccb16(address,s_ctrl); 
pr_err("%s  lenc[%d]=0x%x",__func__,i,otp_ptr->lenc[i]); 
address++; 
} 

ov8835_write_sccb16(0x3d81, 0x00,s_ctrl); 

clear_otp_buffer(s_ctrl); 

bank++; 
ov8835_write_sccb16(0x3d84, bank ,s_ctrl); 

ov8835_write_sccb16(0x3d81, 0x01,s_ctrl); 
msleep(5); 
address = 0x3d00; 
for(i=47;i<62;i++) { 
otp_ptr->lenc[i]=ov8835_read_sccb16(address,s_ctrl); 
pr_err("%s  lenc[%d]=0x%x",__func__,i,otp_ptr->lenc[i]); 
address++; 
} 

ov8835_write_sccb16(0x3d81, 0x00,s_ctrl); 

clear_otp_buffer(s_ctrl); 
return 0; 
} 
 




int update_awb_gain(uint32_t R_gain, uint32_t G_gain, uint32_t B_gain, struct msm_sensor_ctrl_t *s_ctrl) 
{ 
pr_err("otpwb %s  R_gain =%x  0x3400=%x",__func__,R_gain,R_gain>>8); 
pr_err("otpwb %s  R_gain =%x  0x3401=%x",__func__,R_gain,R_gain & 0x00ff); 
pr_err("otpwb %s  G_gain =%x  0x3402=%x",__func__,G_gain,G_gain>>8); 
pr_err("otpwb %s  G_gain =%x  0x3403=%x",__func__,G_gain,G_gain & 0x00ff); 
pr_err("otpwb %s  B_gain =%x  0x3404=%x",__func__,B_gain,B_gain>>8); 
pr_err("otpwb %s  B_gain =%x  0x3405=%x",__func__,B_gain,B_gain & 0x00ff); 
 
if (R_gain>0x400) { 
ov8835_write_sccb16(0x3400, R_gain>>8,s_ctrl); 
ov8835_write_sccb16(0x3401, R_gain & 0x00ff,s_ctrl); 
} 
if (G_gain>0x400) { 
ov8835_write_sccb16(0x3402, G_gain>>8,s_ctrl); 
ov8835_write_sccb16(0x3403, G_gain & 0x00ff,s_ctrl); 
} 
if (B_gain>0x400) { 
ov8835_write_sccb16(0x3404, B_gain>>8,s_ctrl); 
ov8835_write_sccb16(0x3405, B_gain & 0x00ff,s_ctrl); 
} 
return 0; 
} 
 
 
 
int update_lenc(struct otp_struct *otp_ptr, struct msm_sensor_ctrl_t *s_ctrl) 
{ 
                int i, temp; 
		   temp= ov8835_read_sccb16(0x5000,s_ctrl); 
                temp = 0x80 | temp; 
                ov8835_write_sccb16(0x5000, temp,s_ctrl); 
                for(i=0;i<62;i++) { 
                ov8835_write_sccb16(0x5800 + i, otp_ptr->lenc[i],s_ctrl); 
                } 
                return 0; 
} 
 

uint RG_Ratio_typical ; 
uint BG_Ratio_typical ; 







int load_otp_wb(struct msm_sensor_ctrl_t *s_ctrl) 
{ 
uint i, temp, otp_index; 
uint32_t  R_gain,G_gain,B_gain,G_gain_R, G_gain_B; 
int rg,bg; 



for(i=1;i<=3;i++) {                             
	temp = check_otp_wb(i,s_ctrl); 
	pr_err("otpwb %s  temp =%d  i=%d",__func__,temp,i); 
	if (temp == 2) { 
		otp_index = i; 
		break; 
	} 
}               
if (i>3) {                                     
pr_err("otpwb %s  i=%d   no valid wb OTP data ",__func__,i); 
return 1; 
} 
read_otp_wb(otp_index, &current_ov8835_otp,s_ctrl); 
 
if(current_ov8835_otp.module_id==0x01){ 

BG_Ratio_typical=0x116; 
RG_Ratio_typical=0x102; 
}else if(current_ov8835_otp.module_id==0x02){ 

BG_Ratio_typical=0x53; 
RG_Ratio_typical=0x55; 
}else if(current_ov8835_otp.module_id==0x06){ 

BG_Ratio_typical=0x53; 
RG_Ratio_typical=0x55; 
}else if(current_ov8835_otp.module_id==0x15){ 

BG_Ratio_typical=0x116; 
RG_Ratio_typical=0x102; 
}else if(current_ov8835_otp.module_id==0x31){ 

BG_Ratio_typical=0x53; 
RG_Ratio_typical=0x55; 
}else{ 

BG_Ratio_typical=0x116; 
RG_Ratio_typical=0x102; 
} 
if(current_ov8835_otp.light_rg==0) { 
	
	rg = current_ov8835_otp.rg_ratio; 
} else { 
	rg = current_ov8835_otp.rg_ratio * ((current_ov8835_otp.light_rg +512) / 1024); 
} 
if(current_ov8835_otp.light_bg==0) { 

	bg = current_ov8835_otp.bg_ratio; 
} else { 
	bg = current_ov8835_otp.bg_ratio * ((current_ov8835_otp.light_bg +512) / 1024); 
} 
 
if(rg==0||bg==0) return 0; 
 


if(bg < BG_Ratio_typical) 
{ 
        if (rg < RG_Ratio_typical){ 
                
                
                G_gain = 0x400; 
                B_gain = 0x400 * BG_Ratio_typical / bg; 
                R_gain = 0x400 * RG_Ratio_typical / rg; 
                } else{ 
                
                
                R_gain = 0x400; 
                G_gain = 0x400 * rg / RG_Ratio_typical; 
                B_gain = G_gain * BG_Ratio_typical / bg; 
                } 
        } else{ 
        
                if (rg < RG_Ratio_typical){ 
                
                
                B_gain = 0x400; 
                G_gain = 0x400 * bg / BG_Ratio_typical; 
                R_gain = G_gain * RG_Ratio_typical / rg; 
                } else{ 
                
                
                G_gain_B = 0x400 * bg / BG_Ratio_typical; 
                G_gain_R = 0x400 * rg / RG_Ratio_typical; 
                if(G_gain_B > G_gain_R ) 
                { 
                B_gain = 0x400; 
                G_gain = G_gain_B; 
                R_gain = G_gain * RG_Ratio_typical / rg; 
                } 
                else 
                { 
                R_gain = 0x400; 
                G_gain = G_gain_R; 
                B_gain = G_gain * BG_Ratio_typical / bg; 
                } 
                } 
} 

	current_ov8835_otp.final_R_gain=R_gain;
	current_ov8835_otp.final_G_gain=G_gain;
	current_ov8835_otp.final_B_gain=B_gain;
	current_ov8835_otp.wbwritten=1;


return 0; 
} 
 



int load_otp_lenc(struct msm_sensor_ctrl_t *s_ctrl) 
{ 
 
uint i, temp, otp_index; 
 

 
for(i=1;i<=3;i++) {         
	temp = check_otp_lenc(i,s_ctrl); 
	if (temp == 2) { 
		pr_err("otplenc %s  temp =%d  i=%d",__func__,temp,i); 
		otp_index = i; 
		break; 
	} 
} 
if (i>3) {     
	
	pr_err("otplenc %s  i=%d   no valid lenc OTP data ",__func__,i); 
	return 1; 
} 
read_otp_lenc(otp_index, &current_ov8835_otp,s_ctrl); 
current_ov8835_otp.lencwritten=1;


return 0; 
} 
 





int check_otp_VCM(int index,int code, struct msm_sensor_ctrl_t *a_ctrl) 
{ 
            int flag,  bank; 
            int address; 
            
            bank = 0xc0 + 16; 
            ov8835_write_sccb16(0x3d84, bank,a_ctrl); 
            
            ov8835_write_sccb16(0x3d81, 0x01,a_ctrl); 
            msleep(5); 
            
            address = 0x3d00 + (index-1)*4+code*2; 
            flag = ov8835_read_sccb16(address,a_ctrl); 
		pr_err("otpVCM %s index =%d , code=%d ,flag=0x%x",__func__,index,code,flag); 
            flag = flag & 0xc0; 
            
            ov8835_write_sccb16(0x3d81, 0x00,a_ctrl); 
            
            clear_otp_buffer(a_ctrl); 
            if (flag==0) { 
            return 0; 
            } 
            else if (flag & 0x80) { 
            return 1; 
            } 
            else { 
            return 2; 
            } 
 
} 




int read_otp_VCM(int index,int code,struct msm_sensor_ctrl_t *a_ctrl) 
{ 
        int vcm, bank; 
        int address; 
        
        bank = 0xc0 + 16; 
        ov8835_write_sccb16(0x3d84, bank,a_ctrl); 
        
        ov8835_write_sccb16(0x3d81, 0x01,a_ctrl); 
        msleep(5); 
        
        address = 0x3d00 + (index-1)*4+code*2; 
        vcm = ov8835_read_sccb16(address,a_ctrl); 
        vcm = (vcm & 0x03) + (ov8835_read_sccb16(address+1,a_ctrl)<<2); 

	  if(code==1){
		current_ov8835_otp.VCM_end=vcm;
	  }else{
		current_ov8835_otp.VCM_start=vcm;
	  }
	  
        
        ov8835_write_sccb16(0x3d81, 0x00,a_ctrl); 
        
        clear_otp_buffer(a_ctrl); 
        return 0; 
 
} 
 
int load_otp_VCM(struct msm_sensor_ctrl_t *a_ctrl) 
{ 
   int i, code,temp, otp_index; 
   for(code=0;code<2;code++) { 
                for(i=1;i<=3;i++) { 
                        temp = check_otp_VCM(i,code,a_ctrl); 
                        pr_err("otpaf %s  temp =%d , i=%d,code=%d",__func__,temp,i,code); 
                        if (temp == 2) { 
                                otp_index = i; 
                                break; 
                        } 
                } 
                if (i>3) { 
                        pr_err("otpaf %s  i=%d   no valid af OTP data ",__func__,i); 
                        return 1; 
                } 
 
                read_otp_VCM(otp_index,code, a_ctrl); 

   }
                return 0; 
} 
 



int update_blc_ratio(struct msm_sensor_ctrl_t *a_ctrl) 
{ 
int K,temp; 


ov8835_write_sccb16(0x3d84, 0xdf,a_ctrl); 


ov8835_write_sccb16(0x3d81, 0x01,a_ctrl); 
msleep(5); 

K  = ov8835_read_sccb16(0x3d0b,a_ctrl); 
if(K!=0){
	if ((K>=0x15) && (K<=0x40)){ 
		
		pr_err("%s  auto load mode ",__func__);
		temp = ov8835_read_sccb16(0x4008,a_ctrl); 
		temp &= 0xfb;
		ov8835_write_sccb16(0x4008, temp,a_ctrl); 

		temp = ov8835_read_sccb16(0x4000,a_ctrl); 
		temp &= 0xf7;
		ov8835_write_sccb16(0x4000, temp,a_ctrl); 
		return 2; 
	}
}

K  = ov8835_read_sccb16(0x3d0a,a_ctrl); 
if ((K>=0x10) && (K<=0x40)){ 
	
	pr_err("%s  manual load mode ",__func__);
	ov8835_write_sccb16(0x4006, K,a_ctrl); 
	
	temp = ov8835_read_sccb16(0x4008,a_ctrl); 
	temp &= 0xfb;
	ov8835_write_sccb16(0x4008, temp,a_ctrl); 

	temp = ov8835_read_sccb16(0x4000,a_ctrl); 
	temp |= 0x08;
	ov8835_write_sccb16(0x4000, temp,a_ctrl); 
	return 1; 
}else{ 
	
	pr_err("%s  set to default ",__func__);
	ov8835_write_sccb16(0x4006, 0x20,a_ctrl); 
	
	temp = ov8835_read_sccb16(0x4008,a_ctrl); 
	temp &= 0xfb;
	ov8835_write_sccb16(0x4008, temp,a_ctrl); 

	temp = ov8835_read_sccb16(0x4000,a_ctrl); 
	temp |= 0x08;
	ov8835_write_sccb16(0x4000, temp,a_ctrl); 
	return 0; 
}

} 

int32_t ov8835_msm_sensor_otp_probe(struct msm_sensor_ctrl_t *s_ctrl)
{
       pr_err("%s: %d\n", __func__, __LINE__);
       ov8835_write_sccb16(0x0100, 0x01,s_ctrl);
	load_otp_wb(s_ctrl);
	
	load_otp_lenc(s_ctrl);

	load_otp_VCM(s_ctrl);
	
	
  eeprom_buffer[0]=current_ov8835_otp.module_id;
  eeprom_buffer[1]=current_ov8835_otp.lens_id;
  eeprom_buffer[8]=current_ov8835_otp.VCM_start>>8;
  eeprom_buffer[9]=current_ov8835_otp.VCM_start;
  eeprom_buffer[10]=current_ov8835_otp.VCM_end>>8;
  eeprom_buffer[11]=current_ov8835_otp.VCM_end;
module_integrator_id=current_ov8835_otp.module_id;
	if(current_ov8835_otp.module_id){
		pr_err("%s  current_ov8835_otp: module id =0x%x\n",__func__,current_ov8835_otp.module_id);
		pr_err("%s  current_ov8835_otp: lens_id =0x%x\n",__func__,current_ov8835_otp.lens_id);
		pr_err("%s  current_ov8835_otp: production_year =%d\n",__func__,current_ov8835_otp.production_year);
		pr_err("%s  current_ov8835_otp: production_month =%d\n",__func__,current_ov8835_otp.production_month);
		pr_err("%s  current_ov8835_otp: production_day =%d\n",__func__,current_ov8835_otp.production_day);
		pr_err("%s  current_ov8835_otp: rg_ratio =0x%x\n",__func__,current_ov8835_otp.rg_ratio);
		pr_err("%s  current_ov8835_otp: bg_ratio =0x%x\n",__func__,current_ov8835_otp.bg_ratio);
		pr_err("%s  current_ov8835_otp: light_rg =0x%x\n",__func__,current_ov8835_otp.light_rg);
		pr_err("%s  current_ov8835_otp: light_bg =0x%x\n",__func__,current_ov8835_otp.light_bg);
		pr_err("%s  current_ov8835_otp: user_data[0] =0x%x\n",__func__,current_ov8835_otp.user_data[0]);
		pr_err("%s  current_ov8835_otp: user_data[1] =0x%x\n",__func__,current_ov8835_otp.user_data[1]);
		pr_err("%s  current_ov8835_otp: user_data[2] =0x%x\n",__func__,current_ov8835_otp.user_data[2]);
		pr_err("%s  current_ov8835_otp: user_data[3] =0x%x\n",__func__,current_ov8835_otp.user_data[3]);
		pr_err("%s  current_ov8835_otp: user_data[4] =0x%x\n",__func__,current_ov8835_otp.user_data[4]);		
		pr_err("%s  current_ov8835_otp: VCM_start =0x%x\n",__func__,current_ov8835_otp.VCM_start);
		pr_err("%s  current_ov8835_otp: VCM_end =0x%x\n",__func__,current_ov8835_otp.VCM_end);	
	}else{
		pr_err("%s  no  otp  module \n",__func__);
	}
	
	return 0;
}

int32_t ov8835_msm_sensor_otp_write(struct msm_sensor_ctrl_t *s_ctrl)
{

	if(current_ov8835_otp.wbwritten){
		pr_err("%s: wb otp is writing\n", __func__);
		update_awb_gain(current_ov8835_otp.final_R_gain, current_ov8835_otp.final_G_gain, current_ov8835_otp.final_B_gain,s_ctrl); 
	}else{
		pr_err("%s: no wb otp \n", __func__);
	}

	if(current_ov8835_otp.lencwritten){
		pr_err("%s: lenc otp is writing\n", __func__);
		update_lenc(&current_ov8835_otp,s_ctrl); 
	}else{
		pr_err("%s: no lenc otp \n", __func__);
	}

	update_blc_ratio(s_ctrl);
	
	return 0;
}
#endif

static int __init ov8835_init_module(void)
{
	int32_t rc = 0;
	pr_info("%s:%d\n", __func__, __LINE__);
	rc = platform_driver_probe(&ov8835_platform_driver,
		ov8835_platform_probe);
	if (!rc)
		return rc;
	pr_err("%s:%d rc %d\n", __func__, __LINE__, rc);
	return i2c_add_driver(&ov8835_i2c_driver);
}

static void __exit ov8835_exit_module(void)
{
	pr_info("%s:%d\n", __func__, __LINE__);
	if (ov8835_s_ctrl.pdev) {
		msm_sensor_free_sensor_data(&ov8835_s_ctrl);
		platform_driver_unregister(&ov8835_platform_driver);
	} else
		i2c_del_driver(&ov8835_i2c_driver);
	return;
}

static struct msm_sensor_fn_t ov8835_sensor_fn_t = {
	.sensor_config = msm_sensor_config,
	.sensor_power_up = msm_sensor_power_up,
	.sensor_power_down = msm_sensor_power_down,
	.sensor_match_id = msm_sensor_match_id,
	
};

static struct msm_sensor_ctrl_t ov8835_s_ctrl = {
	.sensor_i2c_client = &ov8835_sensor_i2c_client,
	.power_setting_array.power_setting = ov8835_power_setting,
	.power_setting_array.size = ARRAY_SIZE(ov8835_power_setting),
	.msm_sensor_mutex = &ov8835_mut,
	.sensor_v4l2_subdev_info = ov8835_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(ov8835_subdev_info),
		.func_tbl = &ov8835_sensor_fn_t,
};

module_init(ov8835_init_module);
module_exit(ov8835_exit_module);
MODULE_DESCRIPTION("ov8835");
MODULE_LICENSE("GPL v2");
