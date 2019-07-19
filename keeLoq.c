#include <stdio.h>
#include <string.h>

#define bit(x,n)                        (( ((x)[(n) >> 0x3]) & ( 1 << ((n) & 0x7)) )?1:0) 
#define nlf_calc(x,a,b,c,d,e)           (bit(x,a)+(bit(x,b)<<1)+(bit(x,c)<<2)+(bit(x,d)<<3)+(bit(x,e)<<4))

/* 数组按位右移位一位，带进位 
 * buf：数组指针
 * buf_len：数组长度
 * carry：进位值
*/
static __attribute__((always_inline)) inline void array_bit_rrc(unsigned char buf[],unsigned char buf_len,unsigned char carry)
{
    unsigned char temp;
    for(int i=buf_len-1;i>=0;i--) {
        temp=buf[i];
        buf[i]=buf[i]>>1;
        if(carry!=0) {
            buf[i] |= 0x80;
        }
        carry = temp&0x01;
    }
}

/* 数组按位左移位一位，带进位 
 * buf：数组指针
 * buf_len：数组长度
 * carry：进位值
*/
static __attribute__((always_inline)) inline void array_bit_rlc(unsigned char buf[],unsigned char buf_len,unsigned char carry)
{
    unsigned char temp;
    for(int i=0;i<buf_len;i++) {
        temp=buf[i];
        buf[i]=buf[i]<<1;
        if(carry!=0){
            buf[i] |= 0x01;
        }
        carry = temp&0x80;
    }
}

/* 加密 
 * buf：明文，也作为加密后的密文输出缓冲
 * buf_len：明文长度，需要4个字节对齐，否则加密失败
 * key：密钥，8个字节
 * nlf：非线性表，4个字节
 * return：0成功，-1失败
*/
int keeLoq_encrypt(unsigned char *buf,int buf_len,unsigned char const key[],unsigned char const nlf[])
{
    if(buf_len & 0x03) {
        return -1;
    }

    while(buf_len) {
        /*最少32轮，确保每一个bit都加密过，需要与解密轮数保持一致，推荐528 */
        for (int i = 0; i < 32; i++) {
            array_bit_rrc(buf,4,bit(nlf,nlf_calc(buf,1,9,20,26,31))^bit(buf, 16)^bit(buf, 0)^bit(key, i&0x3f));
        }
        buf += 4;
        buf_len -= 4;
    }
    return buf_len;
}

/* 解密 
 * buf：密文，也作为解密后的明文输出缓冲
 * buf_len：密文长度，需要4个字节对齐，否则解密失败
 * key：密钥，8个字节
 * nlf：非线性表，4个字节
 * return：0成功，-1失败
*/
int keeLoq_decrypt(unsigned char *buf,int buf_len,unsigned char const key[],unsigned char const nlf[])
{
    if(buf_len & 0x03) {
        return -1;
    }
    while(buf_len) {
        /*最少32轮，确保每一个bit都加密过，需要与加密轮数保持一致，推荐528 */
        for (int i = 32; i >0; i--) {
            array_bit_rlc(buf,4,bit(nlf,nlf_calc(buf,0,8,19,25,30))^bit(buf, 15)^bit(buf, 31)^bit(key, (i-1)&0x3f));
        }
        buf += 4;
        buf_len -= 4;
    }
    return buf_len;
}

/* 测试 */
int main()
{
    const unsigned char key[8]={0X09,0X87,0X65,0X43,0X21,0XAB,0XCD,0XEF};
    const unsigned char nlf[4]={0x01,0XA8,0X1F,0XA9};
    char test[100] = "hello world, HA~ HA~ HA~ ";

    if(0 != keeLoq_encrypt(test,sizeof(test),key,nlf)) {
        printf("encrypt error\r\n");
    }else {
        printf("encrypt success: %s \r\n",test);
        if(0 != keeLoq_decrypt(test,sizeof(test),key,nlf)) {
            printf("decrypt error\r\n");
        }else {
            printf("decrypt success: %s \r\n",test);
        }
    }
}
