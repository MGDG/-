/*用于二维码数据的压缩*/


static const char *ss="0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_@";

/*16进制字符转数值，错误返回-1*/
static int my_ishex(char ch)
{
	if(ch >= '0' && ch <= '9') {
		return (ch-'0');
    }
	else if(ch >= 'a' && ch <= 'z') {
		return (ch-'a'+10);
    }
	else if(ch >= 'A' && ch <= 'Z') {
		return (ch-'A'+10);
    }
	return -1;
}

/*64进制字符串转成16进制字符串*/
int base64_to_hex16(const char *base64_buf,size_t base64_buf_len, char *hex16_buf, size_t hex16_buf_len)
{
    if(base64_buf==NULL || hex16_buf==NULL || base64_buf_len==0 || hex16_buf_len==0) {
        return 0;
    }

    size_t need_len = ((base64_buf_len/2)*3) + ((base64_buf_len%2)+1) + 1;
    if(hex16_buf_len < need_len) {
        printf("out buf len not enough\n");
        return 0;
    }
    
    int pos = 0;
    size_t j = 0;
    int index = -1;
    uint8_t pre_temp_char = 0;
    
    /*bit左对齐，从前往后转换，不足的bit位补0*/
    for(size_t i=0; i<base64_buf_len; i++) {
        char *p = strchr(ss,base64_buf[i]);
        if(p != NULL) {
            /*字符在字符串中的位置*/
            index = (p-ss);
        }
        else {
            index = -1;
        }

        if((index < 0) || (index > 63) ) {
            printf("not base64 char(%02X), index(%d)\n",base64_buf[i],i);
            return 0;
        }
        
        uint8_t temp = ((uint8_t)index)&0x3F;
        if(pos == 0) {
            /*取前4个bit转16进制字符*/
            hex16_buf[j++] = ss[((temp>>2)&0x0F)];
            pre_temp_char = temp;
            pos = 1;
        }
        else if(pos == 1) {
            uint8_t temp1 = ((pre_temp_char<<2)&0x0C) | ((temp>>4)&0x03);
            hex16_buf[j++] = ss[temp1];
            hex16_buf[j++] = ss[(temp&0x0F)];
            pos = 0;
        }
    }

    int left = base64_buf_len%4;
    if(left == 2) {
        /*说明最后4个bit是转换的时候填充的0，需要去掉最后一位*/
        j--;
    }
    else if(left == 1) {
        /*还有两个bit没转换*/
        uint8_t temp1 = ((pre_temp_char<<2)&0x0C);
        hex16_buf[j++] = ss[temp1];
    }
    
    /*填充结束符*/
    hex16_buf[j] = 0x00;
    return j;
}

/*16进制字符串转64进制字符串*/
int hex16_to_base64(const char *hex16_buf, size_t hex16_buf_len,char *base64_buf,size_t base64_buf_len)
{
	if(base64_buf==NULL || hex16_buf==NULL || base64_buf_len==0 || hex16_buf_len==0) {
		return 0;
    }
	size_t need_len = ((hex16_buf_len/3)*2) + (hex16_buf_len%3) + 1;
	if(base64_buf_len < need_len) {
		return 0;
	}

	int pos = 0;
	size_t j = 0;
	unsigned char pre_value = 0;
	/*bit左对齐，从前往后转换，不足的bit位补0*/
	for(size_t i=0; i<hex16_buf_len; i++) {
		int value = my_ishex(hex16_buf[i]);
		if(value < 0 || value > 15) {
			return 0;
		}
		switch(pos) {
			case 0: {
				/*填充前4bit*/
				pre_value = ((value<<2)&0x3C);
				pos = 1;
			}break;

			case 1: {
				/*填充剩余的2bit*/
				pre_value |= ((value>>2)&0x03);
				base64_buf[j++] = ss[pre_value];
				pre_value = value;
				pos = 2;
			}break;

			case 2: {
				pre_value = ((pre_value<<4)&0x30);
				pre_value |= (value&0x0F);
				base64_buf[j++] = ss[pre_value];
				pos = 0;
			}break;

			default:return 0;
		}
	}

	if(pos == 1) {
		/*还有4个bit没转换*/
		base64_buf[j++] = ss[pre_value];
	}
	else if(pos == 2) {
		/*还有2个bit没转换成64位的*/
		pre_value = (pre_value<<4)&0x30;
		base64_buf[j++] = ss[pre_value];
	}
	base64_buf[j] = 0x00;
	return j;
}

void test(void)
{
	int len = 0;

	char buf[] = {0x00,0x00,0x12,0x34,0x56,0x34,0x56,0x78,0xac,0xf1,0x69,0xfa,0x78,0xac,0xf1,0x69,0xfa,0x78,0x78,0x12,0x34,0x56,0x78,0xac,0xf1,0x69,0xfa,0x78,0x78,0x12,0x34,0x56,0x78,0xac,0xf1,0x69,0xfa,0x78,0x78,0x12,0x34};
//	char buf[] = {0x78,0xf1,0xaf,0x55,0x1c};
//	char buf[] = {0x78,0xaf,0x55,0x1c};
//	char buf[] = {0x78,0xaf,0x1c};
//	char buf[] = {0x4d,0x5a};
//	char buf[] = {0x78};
	
	char base_buf[100] = {0};
	char byte_buf[100] = {0};
	char hex16_buf[100] = {0};

	for(int i=0;i<20000;i++) {
		/*42个字节，压缩，20000次耗时128毫秒，平均0.0064ms/次*/
		len = byte_to_base64(buf,sizeof(buf),base_buf,sizeof(base_buf));
	}

	printf( "len:%d,%s\r\n", len,base_buf); 

	for(int i=0;i<20000;i++) {
		/*解压缩，20000次耗时1614毫秒，平均0.08ms/次*/
		len = base64_to_byte(base_buf,strlen(base_buf),byte_buf,sizeof(byte_buf));
	}

	len = base64_to_hex16(base_buf,strlen(base_buf),hex16_buf,sizeof(hex16_buf));
	
	printf("len:%d,%s\r\n", len,hex16_buf); 
}

