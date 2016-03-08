

#include "msm_sensor.h"
#include <linux/proc_fs.h>

#define T4K35_SENSOR_NAME "t4k35"
DEFINE_MSM_MUTEX(t4k35_mut);

static struct msm_sensor_ctrl_t t4k35_s_ctrl;

static int T4k35moduleId = 0;

static struct msm_sensor_power_setting t4k35_power_setting[] = {
	{
		.seq_type = SENSOR_VREG,
		.seq_val = CAM_VIO,
		.config_val = 0,
		.delay = 5,
	},

	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_VANA,
		.config_val = GPIO_OUT_LOW,
		.delay = 5,
	},
	
	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_VANA,
		.config_val = GPIO_OUT_HIGH,
		.delay = 5,
	},
        
	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_VDIG,
		.config_val = GPIO_OUT_LOW,
		.delay = 5,
	},	
	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_VDIG,
		.config_val = GPIO_OUT_HIGH,
		.delay = 5,
	},

	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_VAF,
		.config_val = GPIO_OUT_LOW,
		.delay = 5,
	},	
	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_VAF,
		.config_val = GPIO_OUT_HIGH,
		.delay = 5,
	},
        
	{
		.seq_type = SENSOR_CLK,
		.seq_val = SENSOR_CAM_MCLK,
		.config_val = 24000000,
		.delay = 10,
	},		
	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_STANDBY,
		.config_val = GPIO_OUT_LOW,
		.delay = 10,
	},
	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_STANDBY,
		.config_val = GPIO_OUT_HIGH,
		.delay = 20,
	},
       {
           .seq_type = SENSOR_GPIO,
           .seq_val = SENSOR_GPIO_RESET,
           .config_val = GPIO_OUT_LOW,
           .delay = 10,
       },
	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_RESET,
		.config_val = GPIO_OUT_HIGH,
		.delay = 20,
	},
	{
		.seq_type = SENSOR_I2C_MUX,
		.seq_val = 0,
		.config_val = 0,
		.delay = 0,
	},
};

static struct v4l2_subdev_info t4k35_subdev_info[] = {
	{
		.code   = V4L2_MBUS_FMT_SGRBG10_1X10,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.fmt    = 1,
		.order    = 0,
	},
};

static const struct i2c_device_id t4k35_i2c_id[] = {
	{T4K35_SENSOR_NAME, (kernel_ulong_t)&t4k35_s_ctrl},
	{ }
};

static ssize_t t4k35_camera_id_read_proc(char *page,char **start,off_t off,int count,int *eof,void* data)
{		 	
    int ret;

    if (T4k35moduleId == 6)
    {
        unsigned char *camera_status = "BACK Camera ID: T4K35 Qtech 8M";
        ret = strlen(camera_status);
        sprintf(page,"%s\n",camera_status);
    }
    else if (T4k35moduleId == 21)
    {
        unsigned char *camera_status = "BACK Camera ID: T4K35 Lite-on 8M";
        ret = strlen(camera_status);
        sprintf(page,"%s\n",camera_status);
    }
    else
    {
        unsigned char *camera_status = "BACK Camera ID: T4K35 8M";
        ret = strlen(camera_status);
        sprintf(page,"%s\n",camera_status);
    }
 	 
    return (ret + 1);	
}

static void t4k35_camera_proc_file(void)
{
    struct proc_dir_entry *proc_file  = create_proc_entry("driver/camera_id_back",0644,NULL);
    if(proc_file)
     {
  	     proc_file->read_proc = t4k35_camera_id_read_proc;			
     }	
    else
     {
        printk(KERN_INFO "camera_proc_file error!\r\n");	
     }
}

static int32_t msm_t4k35_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
    
    return msm_sensor_i2c_probe(client, id, &t4k35_s_ctrl);
}

static struct i2c_driver t4k35_i2c_driver = {
	.id_table = t4k35_i2c_id,
	.probe  = msm_t4k35_i2c_probe,
	.driver = {
		.name = T4K35_SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client t4k35_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};

static const struct of_device_id t4k35_dt_match[] = {
	{.compatible = "qcom,t4k35", .data = &t4k35_s_ctrl},
	{}
};

MODULE_DEVICE_TABLE(of, t4k35_dt_match);

static struct platform_driver t4k35_platform_driver = {
	.driver = {
		.name = "qcom,t4k35",
		.owner = THIS_MODULE,
		.of_match_table = t4k35_dt_match,
	},
};

static int32_t t4k35_platform_probe(struct platform_device *pdev)
{
	int32_t rc = 0;
	const struct of_device_id *match;
	match = of_match_device(t4k35_dt_match, &pdev->dev);
	rc = msm_sensor_platform_probe(pdev, match->data);

    if (rc == 0)
    {
        t4k35_camera_proc_file();
    }
       
	return rc;
}

static int __init t4k35_init_module(void)
{
	int32_t rc = 0;
	pr_info("%s:%d\n", __func__, __LINE__);
	rc = platform_driver_probe(&t4k35_platform_driver,
		t4k35_platform_probe);
	if (!rc)
		return rc;
	pr_err("%s:%d rc %d\n", __func__, __LINE__, rc);
	return i2c_add_driver(&t4k35_i2c_driver);
}

static void __exit t4k35_exit_module(void)
{
	pr_info("%s:%d\n", __func__, __LINE__);
	if (t4k35_s_ctrl.pdev) {
		msm_sensor_free_sensor_data(&t4k35_s_ctrl);
		platform_driver_unregister(&t4k35_platform_driver);
	} else
		i2c_del_driver(&t4k35_i2c_driver);
	return;
}

static int32_t t4k35_sensor_i2c_read(struct msm_sensor_ctrl_t *s_ctrl,
	uint16_t addr, uint16_t *data,
	enum msm_camera_i2c_data_type data_type)
{
    int32_t rc = 0;
    rc = s_ctrl->sensor_i2c_client->i2c_func_tbl->i2c_read(
            s_ctrl->sensor_i2c_client,
            addr,
            data, data_type);

    return rc;
}

static int32_t t4k35_sensor_i2c_write(struct msm_sensor_ctrl_t *s_ctrl,
	uint16_t addr, uint16_t data,
	enum msm_camera_i2c_data_type data_type)
{
    int32_t rc = 0;
    rc = s_ctrl->sensor_i2c_client->i2c_func_tbl->
        i2c_write(s_ctrl->sensor_i2c_client, addr, data, data_type);
    
    if (rc < 0) {
        msleep(100);
        rc = s_ctrl->sensor_i2c_client->i2c_func_tbl->
               i2c_write(s_ctrl->sensor_i2c_client, addr, data, data_type);
    }

	return rc;
}

#define SET_T4k35_REG(reg, val) t4k35_sensor_i2c_write(s_ctrl,reg,val,MSM_CAMERA_I2C_BYTE_DATA)
#define GET_T4k35_REG(reg, val) t4k35_sensor_i2c_read(s_ctrl,reg,&val,MSM_CAMERA_I2C_BYTE_DATA)

typedef struct t4k35_otp_struct 
{
  uint8_t LSC[53];              
  uint8_t AWB[8];               
  uint8_t Module_Info[9];
  uint16_t AF_Macro;
  uint16_t AF_Inifity;
} st_t4k35_otp;

#define T4k35_OTP_PSEL 0x3502
#define T4k35_OTP_CTRL 0x3500
#define T4k35_OTP_DATA_BEGIN_ADDR 0x3504
#define T4k35_OTP_DATA_END_ADDR 0x3543

static uint16_t t4k35_otp_data[T4k35_OTP_DATA_END_ADDR - T4k35_OTP_DATA_BEGIN_ADDR + 1] = {0x00};
static uint16_t t4k35_otp_data_backup[T4k35_OTP_DATA_END_ADDR - T4k35_OTP_DATA_BEGIN_ADDR + 1] = {0x00};

static uint16_t t4k35_r_golden_value=0x50;
static uint16_t t4k35_g_golden_value=0x90;
static uint16_t t4k35_b_golden_value=0x5D;
static uint16_t t4k35_af_macro_pos=500;
static uint16_t t4k35_af_inifity_pos=100;

static void t4k35_otp_set_page(struct msm_sensor_ctrl_t *s_ctrl, uint16_t page)
{
   	
    SET_T4k35_REG(T4k35_OTP_PSEL, page);
}
static void t4k35_otp_access(struct msm_sensor_ctrl_t *s_ctrl)
{
	uint16_t reg_val;
	
	GET_T4k35_REG(T4k35_OTP_CTRL, reg_val);
	SET_T4k35_REG(T4k35_OTP_CTRL, reg_val | 0x80);
	usleep(30);
}
static void t4k35_otp_read_enble(struct msm_sensor_ctrl_t *s_ctrl, uint8_t enble)
{
    if (enble)
        SET_T4k35_REG(T4k35_OTP_CTRL, 0x01);
    else
        SET_T4k35_REG(T4k35_OTP_CTRL, 0x00);
}



static int32_t t4k35_otp_read_data(struct msm_sensor_ctrl_t *s_ctrl, uint16_t* otp_data)
{
    uint16_t i = 0;
    
	
    for (i = 0; i <= (T4k35_OTP_DATA_END_ADDR - T4k35_OTP_DATA_BEGIN_ADDR); i++)
	{
        GET_T4k35_REG(T4k35_OTP_DATA_BEGIN_ADDR+i,otp_data[i]);
    }

    return 0;
}

static void t4k35_update_awb(struct msm_sensor_ctrl_t *s_ctrl, struct t4k35_otp_struct *p_otp)
{
  uint16_t rg,bg,r_otp,g_otp,b_otp;

  r_otp=p_otp->AWB[1]+(p_otp->AWB[0]<<8);
  g_otp=(p_otp->AWB[3]+(p_otp->AWB[2]<<8)+p_otp->AWB[5]+(p_otp->AWB[4]<<8))/2;
  b_otp=p_otp->AWB[7]+(p_otp->AWB[6]<<8);
  
  rg = 256*(t4k35_r_golden_value *g_otp)/(r_otp*t4k35_g_golden_value);
  bg = 256*(t4k35_b_golden_value*g_otp)/(b_otp*t4k35_g_golden_value);

  printk("r_golden=0x%x,g_golden=0x%x, b_golden=0x%0x\n", t4k35_r_golden_value,t4k35_g_golden_value,t4k35_b_golden_value);
  printk("r_otp=0x%x,g_opt=0x%x, b_otp=0x%0x\n", r_otp,g_otp,b_otp);
  printk("rg=0x%x, bg=0x%0x\n", rg,bg);

  
  SET_T4k35_REG(0x0212, rg >> 8);
  SET_T4k35_REG(0x0213, rg & 0xff);

  
  SET_T4k35_REG(0x0214, bg >> 8);
  SET_T4k35_REG(0x0215, bg & 0xff);

}

static void t4k35_update_lsc(struct msm_sensor_ctrl_t *s_ctrl, struct t4k35_otp_struct *p_otp)
{
  uint16_t addr;
  int i;

  
  addr = 0x323A;
  SET_T4k35_REG(addr, p_otp->LSC[0]);
  addr = 0x323E;
  for(i = 1; i < 53; i++) 
  {
    
    SET_T4k35_REG(addr++, p_otp->LSC[i]);
  }

  
  SET_T4k35_REG(0x3237,0x80);
}

static int32_t t4k35_otp_init_lsc_awb(struct msm_sensor_ctrl_t *s_ctrl, struct t4k35_otp_struct *p_otp)
{
  int i,j;
  
  uint16_t check_sum=0x00;

  
  for(i = 3; i >= 0; i--) 
  {
  	
  	t4k35_otp_read_enble(s_ctrl, 1);
  	
  	
	t4k35_otp_set_page(s_ctrl, i);
	
    t4k35_otp_access(s_ctrl);

	printk("otp lsc data area data: %d \n", i);
    t4k35_otp_read_data(s_ctrl, t4k35_otp_data);


	
	printk("otp lsc backup area data: %d\n", i + 6);
	
	t4k35_otp_set_page(s_ctrl, i+6);
	
    t4k35_otp_access(s_ctrl);
	
	t4k35_otp_read_data(s_ctrl, t4k35_otp_data_backup);
	
  	t4k35_otp_read_enble(s_ctrl, 0);

	
    for(j = 0; j < 64; j++) 
	{
        t4k35_otp_data[j]=t4k35_otp_data[j]|t4k35_otp_data_backup[j];
    }

	
    if (0 == t4k35_otp_data[0]) 
	{
      continue;
    } 
	else 
	{
	  
	  for(j = 2; j < 64; j++) 
	  {
        check_sum=check_sum+t4k35_otp_data[j];
      }
	  
	  if((check_sum & 0xFF) == t4k35_otp_data[1])
	  {
	  	printk("otp lsc checksum ok!\n");
		for(j=3;j<=55;j++)
		{
			p_otp->LSC[j-3]=t4k35_otp_data[j];
		}
		for(j=56;j<=63;j++)
		{
			p_otp->AWB[j-56]=t4k35_otp_data[j];
		}
		return 0;
	  }
	  else
	  {
		printk("otp lsc checksum error!\n");
		return -1;
	  }
    }
  }

  if (i < 0) 
  {
    return -1;
    printk("No otp lsc data on sensor t4k35\n");
  }
  else 
  {
    return 0;
  }
}


static int32_t t4k35_otp_init_module_info(struct msm_sensor_ctrl_t *s_ctrl, struct t4k35_otp_struct *p_otp)
{
  int i,pos;
  uint16_t check_sum=0x00;

  
  t4k35_otp_read_enble(s_ctrl, 1);
  
  t4k35_otp_set_page(s_ctrl, 4);
  
  t4k35_otp_access(s_ctrl);
  printk("data area data:\n");
  t4k35_otp_read_data(s_ctrl, t4k35_otp_data);


  
  t4k35_otp_set_page(s_ctrl, 10);
  
  t4k35_otp_access(s_ctrl);
  t4k35_otp_read_data(s_ctrl, t4k35_otp_data_backup);
  
  t4k35_otp_read_enble(s_ctrl, 0);		
  
  
  for(i = 0; i < 64; i++) 
  {
	  t4k35_otp_data[i]=t4k35_otp_data[i]|t4k35_otp_data_backup[i];
  }

  
  if(t4k35_otp_data[32])
  {
	  pos=32;
  }
  else if(t4k35_otp_data[0])
  {
  	  pos=0;
  }
  else
  {
  	  printk("otp no module information!\n");
  	  return -1;
  }

  
  for(i = pos+2; i <pos+32; i++) 
  {
     check_sum=check_sum+t4k35_otp_data[i];
  }

  if((check_sum&0xFF)==t4k35_otp_data[pos+1])
  {
        int moduleId = t4k35_otp_data[pos + 6];
        printk("fuyipeng --- T4k35 moduleId :%d !\n", moduleId);
        T4k35moduleId = moduleId;
        
	  	printk("otp module info checksum ok!\n");
		if((t4k35_otp_data[pos+15]==0x00)&&(t4k35_otp_data[pos+16]==0x00)
			&&(t4k35_otp_data[pos+17]==0x00)&&(t4k35_otp_data[pos+18]==0x00)
			&&(t4k35_otp_data[pos+19]==0x00)&&(t4k35_otp_data[pos+20]==0x00)
			&&(t4k35_otp_data[pos+21]==0x00)&&(t4k35_otp_data[pos+22]==0x00))
			return 0;
		
			
		t4k35_r_golden_value=t4k35_otp_data[pos+16]+(t4k35_otp_data[pos+15]<<8);
		t4k35_g_golden_value=(t4k35_otp_data[pos+18]+(t4k35_otp_data[pos+17]<<8)+t4k35_otp_data[pos+20]+(t4k35_otp_data[pos+19]<<8))/2;
		t4k35_b_golden_value=t4k35_otp_data[pos+22]+(t4k35_otp_data[pos+21]<<8);
		return 0;
  }
  else
  {
	printk("otp module info checksum error!\n");
	return -1;
  }

}

static int32_t t4k35_otp_init_af(struct msm_sensor_ctrl_t *s_ctrl)
{
  int i,pos;
  uint16_t check_sum=0x00;

  
  t4k35_otp_read_enble(s_ctrl, 1);
  
  t4k35_otp_set_page(s_ctrl, 5);
  
  t4k35_otp_access(s_ctrl);
  printk("data area data:\n");
  t4k35_otp_read_data(s_ctrl, t4k35_otp_data);


  
  t4k35_otp_set_page(s_ctrl, 11);
  
  t4k35_otp_access(s_ctrl);
  t4k35_otp_read_data(s_ctrl, t4k35_otp_data_backup);
  
  t4k35_otp_read_enble(s_ctrl, 0);		
  
  
  for(i = 0; i < 64; i++) 
  {
	  t4k35_otp_data[i]=t4k35_otp_data[i]|t4k35_otp_data_backup[i];
  }

  
  
  if(t4k35_otp_data[24])
  {
	  pos=24;
  }
  else if(t4k35_otp_data[16])
  {
  	  pos=16;
  }
  else if(t4k35_otp_data[8])
  {
  	  pos=8;
  }
  else if(t4k35_otp_data[0])
  {
  	  pos=0;
  }
  else
  {
  	  printk("no otp macro AF information!\n");
  	  return -1;
  }
  
  
  check_sum=0x00;
  for(i = pos+2; i <pos+8; i++) 
  {
     check_sum=check_sum+t4k35_otp_data[i];
  }
	  
  if((check_sum&0xFF)==t4k35_otp_data[pos+1])
  {
	  	printk("otp macro AF checksum ok!\n");
        s_ctrl->af_otp_macro=(t4k35_otp_data[pos+3]<<8)+t4k35_otp_data[pos+4];
        if(s_ctrl->af_otp_macro==0x00)
        {
           s_ctrl->af_otp_macro=t4k35_af_macro_pos;
        }

  }
  else
  {
	printk("otp macro AF checksum error!\n");
    s_ctrl->af_otp_macro=t4k35_af_macro_pos;
	
  }

  
  
  if(t4k35_otp_data[56])
  {
	  pos=56;
  }
  else if(t4k35_otp_data[48])
  {
  	  pos=48;
  }
  else if(t4k35_otp_data[40])
  {
  	  pos=40;
  }
  else if(t4k35_otp_data[32])
  {
  	  pos=32;
  }
  else
  {
  	  printk("no otp inifity AF information!\n");
  	  return -1;
  }
  
  
  check_sum=0x00;
  for(i = pos+2; i <pos+8; i++) 
  {
     check_sum=check_sum+t4k35_otp_data[i];
  }
	  
  if((check_sum&0xFF)==t4k35_otp_data[pos+1])
  {
	  	printk("otp inifity AF checksum ok!\n");
        s_ctrl->af_otp_inifity=(t4k35_otp_data[pos+4]<<8)+t4k35_otp_data[pos+5];
        if(s_ctrl->af_otp_inifity==0x00)
        {
           s_ctrl->af_otp_inifity=t4k35_af_inifity_pos;
        }

  }
  else
  {
	printk("otp inifity AF checksum error!\n");
    s_ctrl->af_otp_inifity=t4k35_af_inifity_pos;
  }
  return 0;

}

static int32_t t4k35_otp_init_setting(struct msm_sensor_ctrl_t *s_ctrl)
{
    int32_t rc = 0;
	st_t4k35_otp t4k35_data;

	rc=t4k35_otp_init_module_info(s_ctrl, &t4k35_data);
	if(rc==0x00)
	{
		
	}
	
    rc=t4k35_otp_init_lsc_awb(s_ctrl, &t4k35_data);
	if(rc==0x00)
	{
		t4k35_update_lsc(s_ctrl, &t4k35_data);
		t4k35_update_awb(s_ctrl, &t4k35_data);
	}

	t4k35_otp_init_af(s_ctrl);
    
    return rc;
}

static struct msm_sensor_fn_t t4k35_sensor_fn_t = {
	.sensor_config = msm_sensor_config,
	.sensor_power_up = msm_sensor_power_up,
	.sensor_power_down = msm_sensor_power_down,
	.sensor_match_id = msm_sensor_match_id,
	.sensor_set_opt_setting = t4k35_otp_init_setting,
};


static struct msm_sensor_ctrl_t t4k35_s_ctrl = {
	.sensor_i2c_client = &t4k35_sensor_i2c_client,
	.power_setting_array.power_setting = t4k35_power_setting,
	.power_setting_array.size = ARRAY_SIZE(t4k35_power_setting),
	.msm_sensor_mutex = &t4k35_mut,
	.sensor_v4l2_subdev_info = t4k35_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(t4k35_subdev_info),
	.func_tbl = &t4k35_sensor_fn_t,
};

module_init(t4k35_init_module);
module_exit(t4k35_exit_module);
MODULE_DESCRIPTION("t4k35");
MODULE_LICENSE("GPL v2");
