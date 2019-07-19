#ifndef _KEELOQ_H_
#define _KEELOQ_H_

/* 加密 
 * buf：明文，也作为加密后的密文输出缓冲
 * buf_len：明文长度，需要4个字节对齐，否则加密失败
 * key：密钥，8个字节
 * nlf：非线性表，4个字节
 * return：0成功，-1失败
*/
int keeLoq_encrypt(unsigned char *buf,int buf_len,unsigned char const key[],unsigned char const nlf[]);

/* 解密 
 * buf：密文，也作为解密后的明文输出缓冲
 * buf_len：密文长度，需要4个字节对齐，否则解密失败
 * key：密钥，8个字节
 * nlf：非线性表，4个字节
 * return：0成功，-1失败
*/
int keeLoq_decrypt(unsigned char *buf,int buf_len,unsigned char const key[],unsigned char const nlf[]);

#endif
